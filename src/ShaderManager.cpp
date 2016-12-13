#include "ShaderManager.hpp"
#include "gl_core_4_5.h"
#include "GLFW/glfw3.h"
#include "Shader.hpp"
#include "ShadingProgram.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <istream>
#include <iterator>
#include <stdexcept>
#include <system_error>
#include <unordered_map>
#include <utility>

#if defined(__has_include)
	#if __has_include("experimental/filesystem")
	#define HAS_FILESYSTEM
	#define HAS_EXPERIMENTAL_FILESYSTEM
	#elif __has_include("filesystem")
	#define HAS_FILESYSTEM
	#define HAS_STD_FILESYSTEM
	#endif
#elif defined(__cpp_lib_experimental_filesystem)
#define HAS_FILESYSTEM
#define HAS_EXPERIMENTAL_FILESYSTEM
#elif defined(_MSC_VER) && _MSC_VER >= 1700
#define HAS_FILESYSTEM
#define HAS_EXPERIMENTAL_FILESYSTEM
#endif

#if defined(HAS_STD_FILESYSTEM)
#include <filesystem>
#define FILESYSTEM_NAMESPACE std::filesystem
#elif defined(HAS_EXPERIMENTAL_FILESYSTEM)
#include <experimental/filesystem>
#define FILESYSTEM_NAMESPACE std::experimental::filesystem
#else
#define FILESYSTEM_NAMESPACE std
#endif

std::string GlProj::Utilities::NormalisePath(const std::string& path)
{
	using namespace FILESYSTEM_NAMESPACE;
#if defined(HAS_FILESYSTEM)
	return canonical(path).u8string();
#else
	return path;
#endif
}

using namespace GlProj::Utilities;

namespace GlProj
{
	namespace Graphics
	{
		using GlProj::Utilities::LocalWeakPtr;
		using GlProj::Utilities::make_localshared;

		class ShaderManager
		{
			std::unordered_map<std::string, LocalWeakPtr<Shader>> registeredShaders;
		public:
			LocalSharedPtr<Shader> RegisterShader(GLenum type, GLuint handle, const std::string& name)
			{
				auto newPtr = make_localshared<Shader>(type, handle);
				auto inserted = registeredShaders.insert({name, newPtr});
				if(!inserted.second)
				{
					(*inserted.first).second = newPtr;
				}
				return std::move(newPtr);
			}
			LocalSharedPtr<Shader> FindByName(const std::string& name) const
			{
				auto found = registeredShaders.find(name);
				if(found == registeredShaders.end())
				{
					return nullptr;
				}

				return found->second.lock();
			}

			void CleanUpDangling()
			{
				auto begin = registeredShaders.begin();

				while(begin != registeredShaders.end())
				{
					if(!begin->second.expired())
					{
						++begin;
						continue;
					}
					begin = registeredShaders.erase(begin);
				}
			}
		};

		ShaderManager* GetShaderManager()
		{
			static ShaderManager instance;
			return &instance;
		}
		LocalSharedPtr<Shader> LoadShader(ShaderManager* manager, GLenum shaderType, const std::string& path, bool replace)
		{
			if(!replace)
			{
				auto ptr = FindCachedShaderByPath(manager, path);
				if(ptr != nullptr)
				{
					return std::move(ptr);
				}
			}

			auto shaderFile = std::ifstream(path);
			auto errnum = errno;
			if(!shaderFile.is_open())
			{
				std::string errMessage = "Shader file could not be opened.\n";
				errMessage += "File: "; 
				errMessage += path + '\n';
				errMessage += "Error: ";
				errMessage += std::error_code(errnum, std::system_category()).message();
				errMessage += '\n';

				throw std::runtime_error(errMessage);
			}
			shaderFile.exceptions(std::ios_base::badbit);
			shaderFile.imbue(std::locale::classic());
			shaderFile.unsetf(std::ios_base::skipws);

			std::string shaderSource{ std::istream_iterator<char>(shaderFile),
										std::istream_iterator<char>() };

			shaderFile.close();

			auto shaderID = glCreateShader(shaderType);
			auto source = shaderSource.data();
			auto sourceSize = GLint(shaderSource.size());
			glShaderSource(shaderID, 1, &source, &sourceSize);
			glCompileShader(shaderID);

			GLint compileStatus;
			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
			if (compileStatus != GL_TRUE)
			{
				GLint logLength;
				glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
				auto log = std::make_unique<char[]>(logLength);
				glGetShaderInfoLog(shaderID, logLength, nullptr, log.get());
				std::string err = "Shader Compile Error.\n";
				err += "File: " + path + '\n';
				err += log.get();

				glDeleteShader(shaderID);
				throw std::runtime_error(err);
			}

			return manager->RegisterShader(shaderType, shaderID, NormalisePath(path));
		}
		LocalSharedPtr<Shader> RegisterShader(ShaderManager* manager, GLenum type, GLuint handle, const std::string& name, bool replace)
		{
			if (!replace)
			{
				auto ptr = FindCachedShaderByName(manager, name);
				if (ptr != nullptr)
				{
					return std::move(ptr);
				}
			}

			return manager->RegisterShader(type, handle, name);
		}

		LocalSharedPtr<Shader> FindCachedShaderByPath(const ShaderManager* manager, const std::string& path)
		{
			auto canonicalPath = NormalisePath(path);

			return manager->FindByName(canonicalPath);
		}
		LocalSharedPtr<Shader> FindCachedShaderByName(const ShaderManager* manager, const std::string& name)
		{
			return manager->FindByName(name);
		}
		void ReleaseUnused(ShaderManager* manager)
		{
			manager->CleanUpDangling();
		}
		
		LocalSharedPtr<ShadingProgram> GenerateProgram()
		{
			return make_localshared<ShadingProgram>(glCreateProgram());
		}
		void AttachShader(ShadingProgram* program, Shader* shader)
		{
			glAttachShader(program->GetHandle(), shader->GetHandle());
		}
		void DetachShader(ShadingProgram* program, Shader* shader)
		{
			glDetachShader(program->GetHandle(), shader->GetHandle());
		}
		void LinkProgram(ShadingProgram* program)
		{
			auto handle = program->GetHandle();
			glLinkProgram(handle);

			GLint linkStatus;
			glGetProgramiv(handle, GL_LINK_STATUS, &linkStatus);
			if (linkStatus != GL_TRUE)
			{
				GLint logLength;
				glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logLength);
				auto logBuffer = std::make_unique<char[]>(logLength);
				glGetProgramInfoLog(handle, logLength, nullptr, logBuffer.get());
				std::string err = "Shader Program Linking Error.\n";
				err += "Error: ";
				err += logBuffer.get();
				throw std::runtime_error(err);
			}

			program->FetchProgramInfo();
		}
	}
}

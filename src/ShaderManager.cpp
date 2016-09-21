#include "ShaderManager.hpp"
#include "gl_core_4_1.h"
#include "GLFW/glfw3.h"
#include "Shader.hpp"
#include "ShadingProgram.hpp"
#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#if !defined(__GNUG__) && !defined(__clang__)
#include <experimental/filesystem>
#endif
std::string NormalisePath(const std::string& path)
{
#if defined(__GNUG__) || defined(__clang__)
	return path;
#else
	return std::experimental::filesystem::canonical(path).u8string();
#endif
}


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

			FILE* shaderFile;

			auto err = fopen_s(&shaderFile, path.c_str(), "r");
			if(err != 0)
			{
				std::string errMessage = "Shader file could not be opened.\n";
				errMessage += "File: "; 
				errMessage += path + '\n';
				errMessage += "Error: ";
				errMessage += std::strerror(err);
				errMessage += '\n';

				throw std::runtime_error(errMessage);
			}
			std::unique_ptr<FILE, void(*)(FILE*)> fileHandle(shaderFile, [](FILE* f) { fclose(f); });

			auto begin = ftell(fileHandle.get());
			if(fseek(fileHandle.get(), 0, SEEK_END) != 0)
			{
				std::string errMessage = "Unknown error occurred while seeking in shader file.\n";
				errMessage += "File: ";
				errMessage += path + '\n';
				throw std::runtime_error(errMessage);
			}
			auto end = ftell(fileHandle.get());
			auto fileSize = end - begin;

			auto sourceBuffer = std::make_unique<char[]>(fileSize);

			if(fseek(fileHandle.get(), 0, SEEK_SET) != 0)
			{
				std::string errMessage = "Unknown error occurred while seeking in shader file.\n";
				errMessage += "File: ";
				errMessage += path + '\n';
				throw std::runtime_error(errMessage);
			}

			auto totalRead = fread(sourceBuffer.get(), sizeof(char), fileSize, fileHandle.get());
			if(totalRead == 0)
			{
				std::string errMessage = "Unknown error occurred while reading shader file.\n";
				errMessage += "File: ";
				errMessage += path + '\n';
				throw std::runtime_error(errMessage);
			}

			auto shaderID = glCreateShader(shaderType);
			auto source = sourceBuffer.get();
			auto sourceSize = GLint(totalRead);
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

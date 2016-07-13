#include "ShadingProgram.hpp"
#include "GLFW\glfw3.h"
#include "Shader.hpp"
#include <algorithm>
#include <stdexcept>

namespace GlProj
{
	namespace Graphics
	{
		bool operator==(const ShadingProgram& x, const ShadingProgram& y) noexcept
		{
			return x.programHandle == y.programHandle;
		}
		bool operator!=(const ShadingProgram& x, const ShadingProgram& y) noexcept
		{
			return !(x == y);
		}
		ShadingProgram::ShadingProgram(ShadingProgram&& x) noexcept
			: programHandle(x.programHandle)
		{
			x.programHandle = invalidHandle;
		}
		ShadingProgram & ShadingProgram::operator=(ShadingProgram&& x) noexcept
		{
			if (this != &x)
			{
				if (programHandle != invalidHandle)
				{
					glDeleteProgram(programHandle);
				}
				programHandle = x.programHandle;
				x.programHandle = invalidHandle;
			}

			return *this;
		}
		ShadingProgram::~ShadingProgram()
		{
			if (programHandle != invalidHandle)
			{
				glDeleteProgram(programHandle);
			}
		}
		ShadingProgram::ShadingProgram(GLuint handle) noexcept
			: programHandle(handle)
		{}
		GLuint ShadingProgram::GetHandle() const noexcept
		{
			return programHandle;
		}
		/*void ShadingProgram::BindShader(const std::shared_ptr<Shader>& shader)
		{
			if (std::find_if(boundShaders.begin(), boundShaders.end(), [&shader](const auto& x)
			{
				return *x == *shader;
			}) != boundShaders.end())
			{
				return;
			}
			glAttachShader(programHandle, shader->GetHandle());
			boundShaders.push_back(shader);
		}
		void ShadingProgram::DetachShader(const Shader& shader)
		{
			auto pos = std::find_if(boundShaders.begin(), boundShaders.end(), [&shader](const auto& x)
			{
				return *x == shader;
			});
			if (pos == boundShaders.end())
			{
				return;
			}
			glDetachShader(programHandle, pos->get()->GetHandle());
			boundShaders.erase(pos);
		}
		void ShadingProgram::LinkProgram()
		{
			glLinkProgram(programHandle);

			GLint linkStatus;
			glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
			if (linkStatus != GL_TRUE)
			{
				GLint logLength;
				glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLength);
				auto logBuffer = std::make_unique<char[]>(logLength);
				glGetProgramInfoLog(programHandle, logLength, nullptr, logBuffer.get());
				std::string err = "Shader Program Linking Error.\n";
				err += "Error: ";
				err += logBuffer.get();
				throw std::runtime_error(err);
			}
		}*/
	}
}
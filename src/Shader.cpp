#include "Shader.hpp"
#include "gl_core_4_5.h"
#include "GLFW/glfw3.h"

namespace GlProj
{
	namespace Graphics
	{
		bool operator==(const Shader& x, const Shader& y) noexcept
		{
			return x.shaderHandle == y.shaderHandle && x.shaderType == y.shaderType;
		}
		bool operator!=(const Shader& x, const Shader& y) noexcept
		{
			return !(x == y);
		}
		Shader::Shader(GLenum type, GLuint handle) noexcept
			: shaderHandle(handle)
			, shaderType(type)
		{}
		Shader::Shader(Shader&& x) noexcept
			: shaderHandle(x.shaderHandle)
			, shaderType(x.shaderType)
		{
			x.shaderHandle = invalidHandle;
		}
		Shader& Shader::operator=(Shader&& x) noexcept
		{
			if(this != &x)
			{
				if(shaderHandle != invalidHandle)
				{
					glDeleteShader(shaderHandle);
				}
				shaderHandle = x.shaderHandle;
				shaderType = x.shaderType;
				x.shaderHandle = invalidHandle;
			}

			return *this;
		}
		Shader::~Shader()
		{
			if(shaderHandle != invalidHandle)
			{
				glDeleteShader(shaderHandle);
			}
		}
		GLuint Shader::GetHandle() const noexcept
		{
			return shaderHandle;
		}
		GLenum Shader::GetType() const noexcept
		{
			return shaderType;
		}
	}
}

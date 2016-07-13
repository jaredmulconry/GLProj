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
	}
}
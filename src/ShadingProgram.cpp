#include "ShadingProgram.hpp"
#include "gl_core_4_1.h"
#include "GLFW\glfw3.h"
#include "Shader.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

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
			, attributes(std::move(x.attributes))
			, uniforms(std::move(x.uniforms))
		{
			x.programHandle = invalidHandle;
		}
		ShadingProgram& ShadingProgram::operator=(ShadingProgram&& x) noexcept
		{
			if (this != &x)
			{
				if (programHandle != invalidHandle)
				{
					glDeleteProgram(programHandle);
				}
				programHandle = x.programHandle;
				attributes = std::move(x.attributes);
				uniforms = std::move(x.uniforms);
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
		void ShadingProgram::FetchProgramInfo()
		{
			//TODO: Query for vertex attribute and uniform information
		}
	}
}
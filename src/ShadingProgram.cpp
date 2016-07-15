#include "ShadingProgram.hpp"
#include "gl_core_4_1.h"
#include "GLFW\glfw3.h"
#include "Shader.hpp"
#include <algorithm>
#include <memory>
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
		void ShadingProgram::Bind() const noexcept
		{
			glUseProgram(GetHandle());
		}
		void ShadingProgram::FetchProgramInfo()
		{
			attributes.clear();
			uniforms.clear();

			//TODO: Query for vertex attribute and uniform information
			GLint attributeCount;
			glGetProgramiv(GetHandle(), GL_ACTIVE_ATTRIBUTES, &attributeCount);
			attributes.reserve(attributeCount);
			GLint nameLengthMax;
			glGetProgramiv(GetHandle(), GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nameLengthMax);
			auto nameBuf = std::make_unique<char[]>(nameLengthMax);

			for (GLint i = 0; i < attributeCount; ++i)
			{
				nameBuf[0] = '\0';
				GLint size;
				GLenum type;
				glGetActiveAttrib(GetHandle(), GLuint(i), nameLengthMax, nullptr, &size, &type, nameBuf.get());
				GLint loc = glGetAttribLocation(GetHandle(), nameBuf.get());
				attributes.push_back(VertexAttribute{nameBuf.get(), type, loc, size});
			}

			GLint uniformCount;
			glGetProgramiv(GetHandle(), GL_ACTIVE_UNIFORMS, &uniformCount);
			uniforms.reserve(uniformCount);

			glGetProgramiv(GetHandle(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameLengthMax);
			nameBuf = std::make_unique<char[]>(nameLengthMax);

			for (GLint i = 0; i < uniformCount; ++i)
			{
				nameBuf[0] = '\0';
				GLint size;
				GLenum type;
				GLint loc;
				auto idx = GLuint(i);
				glGetActiveUniformsiv(GetHandle(), 1, &idx, GL_UNIFORM_SIZE, &size);
				glGetActiveUniformsiv(GetHandle(), 1, &idx, GL_UNIFORM_TYPE, &size);
				glGetActiveUniformName(GetHandle(), idx, nameLengthMax, nullptr, nameBuf.get());
				glGetUniformLocation(GetHandle(), nameBuf.get());

				uniforms.push_back(UniformInformation{nameBuf.get(), type, loc, size});
			}
		}
	}
}
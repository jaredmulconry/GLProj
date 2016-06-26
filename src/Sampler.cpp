#include "Sampler.hpp"
#include "gl_core_4_1.h"
#include "GLFW\glfw3.h"

namespace GlProj
{
	namespace Graphics
	{
		Sampler::Sampler() noexcept
		{
			glGenSamplers(1, &samplerHandle);
		}
		Sampler::~Sampler()
		{
			if (samplerHandle != invalidHandle)
			{
				glDeleteSamplers(1, &samplerHandle);
			}
		}
		GLuint Sampler::GetHandle() const noexcept
		{
			return samplerHandle;
		}

		template<>
		void Sampler::GetParameter(GLenum name, GLint& value)
		{
			glGetSamplerParameteriv(samplerHandle, name, &value);
		}
		template<>
		void Sampler::GetParameter(GLenum name, GLfloat& value)
		{
			glGetSamplerParameterfv(samplerHandle, name, &value);
		}
		template<>
		void Sampler::GetParameter(GLenum name, GLint* values)
		{
			glGetSamplerParameteriv(samplerHandle, name, values);
		}
		template<>
		void Sampler::GetParameter(GLenum name, GLfloat* values)
		{
			glGetSamplerParameterfv(samplerHandle, name, values);
		}


		template<>
		void Sampler::SetParameter(GLenum name, GLint value)
		{
			glSamplerParameteri(samplerHandle, name, value);
		}
		template<>
		void Sampler::SetParameter(GLenum name, GLfloat value)
		{
			glSamplerParameterf(samplerHandle, name, value);
		}
		template<>
		void Sampler::SetParameter(GLenum name, GLint* values)
		{
			SetParameter(name, const_cast<const GLint*>(values));
		}
		template<>
		void Sampler::SetParameter(GLenum name, const GLint* values)
		{
			glSamplerParameteriv(samplerHandle, name, values);
		}
		template<>
		void Sampler::SetParameter(GLenum name, GLfloat* values)
		{
			SetParameter(name, const_cast<const GLfloat*>(values));
		}
		template<>
		void Sampler::SetParameter(GLenum name, const GLfloat* values)
		{
			glSamplerParameterfv(samplerHandle, name, values);
		}
	}
}
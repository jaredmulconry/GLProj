#include "Sampler.hpp"
#include "gl_core_4_5.h"
#include "GLFW/glfw3.h"

namespace GlProj
{
	namespace Graphics
	{
		Sampler::Sampler() noexcept
		{
			glGenSamplers(1, &samplerHandle);
		}
		Sampler::Sampler(Sampler&& x) noexcept
			:samplerHandle(x.samplerHandle)
		{
			x.samplerHandle = invalidHandle;
		}
		Sampler& Sampler::operator=(Sampler&& x) noexcept
		{
			if (this != &x)
			{
				if (samplerHandle != invalidHandle)
				{
					glDeleteSamplers(1, &samplerHandle);
				}
				samplerHandle = x.samplerHandle;
				x.samplerHandle = invalidHandle;
			}

			return *this;
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

		void Sampler::Bind(GLuint unit) const noexcept
		{
			glBindSampler(unit, samplerHandle);
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
		void Sampler::SetParameter(GLenum name, const GLint* values)
		{
			glSamplerParameteriv(samplerHandle, name, values);
		}
		template<>
		void Sampler::SetParameter(GLenum name, const GLfloat* values)
		{
			glSamplerParameterfv(samplerHandle, name, values);
		}
		bool operator==(const Sampler& x, const Sampler& y) noexcept
		{
			return x.samplerHandle == y.samplerHandle;
		}
		bool operator!=(const Sampler& x, const Sampler& y) noexcept
		{
			return !(x == y);
		}
	}
}

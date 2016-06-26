#pragma once
#include "OpenGLTypes.hpp"

namespace GlProj
{
	namespace Graphics
	{
		class Sampler
		{
			GLuint samplerHandle = invalidHandle;
		public:
			static const constexpr GLuint invalidHandle = GLuint(-1);

			Sampler() noexcept;
			Sampler(const Sampler&) = delete;
			Sampler(Sampler&&) noexcept;
			Sampler& operator=(Sampler&&) noexcept;
			~Sampler();

			GLuint GetHandle() const noexcept;

			template<typename T>
			void GetParameter(GLenum, T&);
			template<typename T>
			void GetParameter(GLenum, T*);
			template<typename T>
			void SetParameter(GLenum, T);
		};
	}
}
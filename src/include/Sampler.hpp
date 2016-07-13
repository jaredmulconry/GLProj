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

			friend bool operator==(const Sampler&, const Sampler&) noexcept;
			friend bool operator!=(const Sampler&, const Sampler&) noexcept;

			Sampler() noexcept;
			Sampler(const Sampler&) = delete;
			Sampler(Sampler&&) noexcept;
			Sampler& operator=(Sampler&&) noexcept;
			~Sampler();

			GLuint GetHandle() const noexcept;

			void Bind(GLuint) const noexcept;

			template<typename T>
			void GetParameter(GLenum, T&);
			template<typename T>
			void GetParameter(GLenum, T*);
			template<typename T>
			void SetParameter(GLenum, T);
		};
	}
}
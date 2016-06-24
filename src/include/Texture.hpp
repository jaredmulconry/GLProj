#pragma once
#include "OpenGLTypes.hpp"

namespace GlProj
{
	namespace Graphics
	{
		class TextureManager;

		class Texture
		{
			GLuint textureHandle = invalidHandle;
		public:
			static const constexpr GLuint invalidHandle = GLuint(-1);
			Texture() noexcept = default;
			Texture(const Texture&) = delete;
			Texture(Texture&&) noexcept;
			Texture& operator=(Texture&&) noexcept;
			~Texture();
			explicit Texture(GLuint) noexcept;

			GLuint GetHandle() const noexcept;
		};
	}
}
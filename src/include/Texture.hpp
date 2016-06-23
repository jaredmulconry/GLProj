#pragma once
#include "OpenGLTypes.hpp"

namespace GlProj
{
	namespace Graphics
	{
		class Texture
		{
			GLuint textureHandle = invalidHandle;
			explicit Texture(GLuint) noexcept;
		public:
			friend class TextureManager;

			static const constexpr GLuint invalidHandle = GLuint(-1);
			Texture() noexcept = default;
			Texture(const Texture&) = delete;
			Texture(Texture&&) noexcept;
			Texture& operator=(Texture&&) noexcept;
			~Texture();

			GLuint GetHandle() const noexcept;
		};
	}
}
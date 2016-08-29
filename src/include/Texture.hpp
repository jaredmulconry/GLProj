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
			GLenum textureType;
		public:
			static const constexpr GLuint invalidHandle = GLuint(-1);

			friend bool operator==(const Texture&, const Texture&) noexcept;
			friend bool operator!=(const Texture&, const Texture&) noexcept;

			Texture() noexcept = default;
			Texture(const Texture&) = delete;
			Texture(Texture&&) noexcept;
			Texture& operator=(Texture&&) noexcept;
			~Texture();
			Texture(GLenum, GLuint) noexcept;

			GLuint GetHandle() const noexcept;
			GLenum GetType() const noexcept;

			void Bind() const noexcept;
		};
	}
}
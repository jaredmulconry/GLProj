#include "Texture.hpp"
#include "gl_core_4_1.h"
#include "GLFW/glfw3.h"

namespace GlProj
{
	namespace Graphics
	{
		Texture::Texture(Texture&& o) noexcept
			: textureHandle(o.textureHandle)
			, textureType(o.textureType)
		{
			o.textureHandle = invalidHandle;
		}
		Texture& Texture::operator=(Texture&& o) noexcept
		{
			if (&o != this)
			{
				if (textureHandle != invalidHandle)
				{
					glDeleteTextures(1, &textureHandle);
				}
				textureHandle = o.textureHandle;
				textureType = o.textureType;
				o.textureHandle = invalidHandle;
			}
			return *this;
		}
		Texture::~Texture()
		{
			if (textureHandle != invalidHandle)
			{
				glDeleteTextures(1, &textureHandle);
			}
		}
		Texture::Texture(GLenum type, GLuint handle) noexcept
			: textureHandle(handle)
			, textureType(type)
		{
		}
		GLuint Texture::GetHandle() const noexcept
		{
			return textureHandle;
		}
		GLenum Texture::GetType() const noexcept
		{
			return textureType;
		}
		void Texture::Bind() const noexcept
		{
			glBindTexture(textureType, textureHandle);
		}
		bool operator==(const Texture& x, const Texture& y) noexcept
		{
			return x.textureHandle == y.textureHandle;
		}
		bool operator!=(const Texture& x, const Texture& y) noexcept
		{
			return !(x == y);
		}
	}
}

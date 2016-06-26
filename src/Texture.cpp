#include "Texture.hpp"
#include "gl_core_4_1.h"
#include "GLFW\glfw3.h"

namespace GlProj
{
	namespace Graphics
	{
		Texture::Texture(Texture&& o) noexcept
			: textureHandle(o.textureHandle)
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
		Texture::Texture(GLuint handle) noexcept
			: textureHandle(handle)
		{
		}
		GLuint Texture::GetHandle() const noexcept
		{
			return textureHandle;
		}
	}
}
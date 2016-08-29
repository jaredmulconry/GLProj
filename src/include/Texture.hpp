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
			int refCount = 1;
		public:
			static const constexpr GLuint invalidHandle = GLuint(-1);
			using intrusively_counted = void;

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

			///Required for Increments
			void IncrementLocalPtr() noexcept
			{
				++refCount;
			}
			///Required for Decrements
			void DecrementLocalPtr() noexcept
			{
				--refCount;
			}
			///Required for access to the use count
			long LocalUseCount() const noexcept
			{
				return refCount;
			}
		};
	}
}
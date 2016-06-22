#pragma once
#include <gl\GL.h>

namespace GlProj
{
	namespace Graphics
	{
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

			GLuint handle() const noexcept;
		};
	}
}
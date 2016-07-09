#pragma once
#include "gl_core_4_1.h"

namespace GlProj
{
	namespace Graphics
	{
		class MeshArrayBuffer
		{
			GLuint arrayBufferHandle = invalidHandle;
		public:
			static const constexpr GLuint invalidHandle = GLuint(-1);

			MeshArrayBuffer() noexcept;
			MeshArrayBuffer(MeshArrayBuffer&&) noexcept;
			MeshArrayBuffer& operator=(MeshArrayBuffer&&) noexcept;
			~MeshArrayBuffer();

			GLuint GetHandle() const noexcept;
			GLenum GetType() const noexcept;

			void Bind() const noexcept;
		};
	}
}
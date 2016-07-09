#include "MeshArrayBuffer.hpp"
#include "GLFW\glfw3.h"

namespace GlProj
{
	namespace Graphics
	{
		MeshArrayBuffer::MeshArrayBuffer() noexcept
		{
			glGenBuffers(1, &arrayBufferHandle);
			glBindBuffer(GetType(), GetHandle());
		}
		MeshArrayBuffer::MeshArrayBuffer(MeshArrayBuffer&& x) noexcept
			:arrayBufferHandle(x.arrayBufferHandle)
		{
			x.arrayBufferHandle = invalidHandle;
		}
		MeshArrayBuffer& MeshArrayBuffer::operator=(MeshArrayBuffer&& x) noexcept
		{
			if (this != &x)
			{
				if (arrayBufferHandle != invalidHandle)
				{
					glDeleteBuffers(1, &arrayBufferHandle);
				}
				arrayBufferHandle = x.arrayBufferHandle;

				x.arrayBufferHandle = invalidHandle;
			}

			return *this;
		}
		MeshArrayBuffer::~MeshArrayBuffer()
		{
			if (arrayBufferHandle != invalidHandle)
			{
				glDeleteBuffers(1, &arrayBufferHandle);
			}
		}
		GLuint MeshArrayBuffer::GetHandle() const noexcept
		{
			return arrayBufferHandle;
		}
		GLenum MeshArrayBuffer::GetType() const noexcept
		{
			return GL_ARRAY_BUFFER;
		}
		void MeshArrayBuffer::Bind() const noexcept
		{
			glBindBuffer(GetType(), GetHandle());
		}
	}
}
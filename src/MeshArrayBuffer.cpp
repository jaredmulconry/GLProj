#include "MeshArrayBuffer.hpp"
#include "GLFW/glfw3.h"

namespace GlProj
{
	namespace Graphics
	{
		MeshArrayBuffer::MeshArrayBuffer() noexcept
		{
			glGenVertexArrays(1, &arrayBufferHandle);
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
					glDeleteVertexArrays(1, &arrayBufferHandle);
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
				glDeleteVertexArrays(1, &arrayBufferHandle);
			}
		}
		GLuint MeshArrayBuffer::GetHandle() const noexcept
		{
			return arrayBufferHandle;
		}
		GLenum MeshArrayBuffer::GetType() const noexcept
		{
			return GL_VERTEX_ARRAY;
		}
		void MeshArrayBuffer::Bind() const noexcept
		{
			glBindVertexArray(GetHandle());
		}
		void MeshArrayBuffer::UnBind() noexcept
		{
			glBindVertexArray(0);
		}
		bool operator==(const MeshArrayBuffer& x, const MeshArrayBuffer& y) noexcept
		{
			return x.arrayBufferHandle == y.arrayBufferHandle;
		}
		bool operator!=(const MeshArrayBuffer& x, const MeshArrayBuffer& y) noexcept
		{
			return !(x == y);
		}
	}
}

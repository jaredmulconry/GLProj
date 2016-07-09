#include "MeshDataBuffer.hpp"
#include "GLFW\glfw3.h"

namespace GlProj
{
	namespace Graphics
	{
		MeshDataBuffer::MeshDataBuffer(GLenum bufferType, GLsizeiptr dataSize, const GLvoid* data, GLenum usage)
			:bufferType(bufferType)
		{
			glGenBuffers(1, &meshDataHandle);
			glBindBuffer(bufferType, meshDataHandle);

			glBufferData(bufferType, dataSize, data, usage);
		}
		MeshDataBuffer::MeshDataBuffer(MeshDataBuffer&& x) noexcept
			: meshDataHandle(x.meshDataHandle)
			, bufferType(x.bufferType)
		{
			x.meshDataHandle = invalidHandle;
		}
		MeshDataBuffer& MeshDataBuffer::operator=(MeshDataBuffer&& x) noexcept
		{
			if (this != &x)
			{
				if (meshDataHandle != invalidHandle)
				{
					glDeleteBuffers(1, &meshDataHandle);
				}
				meshDataHandle = x.meshDataHandle;
				bufferType = x.bufferType;

				x.meshDataHandle = invalidHandle;
			}

			return *this;
		}
		MeshDataBuffer::~MeshDataBuffer()
		{
			if (meshDataHandle != invalidHandle)
			{
				glDeleteBuffers(1, &meshDataHandle);
			}
		}
		GLuint MeshDataBuffer::GetHandle() const noexcept
		{
			return meshDataHandle;
		}
		GLenum MeshDataBuffer::GetType() const noexcept
		{
			return bufferType;
		}
		void MeshDataBuffer::UpdateData(GLintptr offset, GLsizeiptr dataSize, const GLvoid * data) const noexcept
		{
			glBufferSubData(GetType(), offset, dataSize, data);
		}
		void* MeshDataBuffer::MapBuffer(GLintptr offset, GLsizeiptr dataSize, GLbitfield access) const noexcept
		{
			return glMapBufferRange(GetType(), offset, dataSize, access);
		}
		void MeshDataBuffer::UnmapBuffer() const noexcept
		{
			glUnmapBuffer(GetType());
		}
		void MeshDataBuffer::Bind() const noexcept
		{
			glBindBuffer(GetType(), GetHandle());
		}
		void MeshDataBuffer::BindBase(GLuint index) const noexcept
		{
			glBindBufferBase(GetType(), index, GetHandle());
		}
		void MeshDataBuffer::BindRange(GLuint index, GLintptr offset, GLsizeiptr size) const noexcept
		{
			glBindBufferRange(GetType(), index, GetHandle(), offset, size);
		}
	}
}
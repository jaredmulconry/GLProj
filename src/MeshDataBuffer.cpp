#include "MeshDataBuffer.hpp"
#include "GLFW/glfw3.h"

namespace GlProj
{
	namespace Graphics
	{
		MeshDataBuffer::MeshDataBuffer(BufferType bufferType, GLsizeiptr dataSize, const GLvoid* data, 
										GLenum dataType, GLint elemsPerVert, BufferUsage usage)
			: bufferType(GLenum(bufferType))
			, dataType(dataType)
			, elementsPerVertex(elemsPerVert)
		{
			glGenBuffers(1, &meshDataHandle);
			glBindBuffer(this->bufferType, meshDataHandle);

			glBufferData(this->bufferType, dataSize, data, GLenum(usage));
		}
		MeshDataBuffer::MeshDataBuffer(MeshDataBuffer&& x) noexcept
			: meshDataHandle(x.meshDataHandle)
			, bufferType(x.bufferType)
			, dataType(x.dataType)
			, elementsPerVertex(x.elementsPerVertex)
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
				dataType = x.dataType;
				elementsPerVertex = x.elementsPerVertex;

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
		BufferType MeshDataBuffer::GetBufferType() const noexcept
		{
			return BufferType(bufferType);
		}
		GLenum MeshDataBuffer::GetDataType() const noexcept
		{
			return dataType;
		}
		GLint MeshDataBuffer::GetElementsPerVertex() const noexcept
		{
			return elementsPerVertex;
		}
		void MeshDataBuffer::UpdateData(GLintptr offset, GLsizeiptr dataSize, const GLvoid * data) const noexcept
		{
			glBufferSubData(bufferType, offset, dataSize, data);
		}
		void* MeshDataBuffer::MapBuffer(GLintptr offset, GLsizeiptr dataSize, GLbitfield access) const noexcept
		{
			return glMapBufferRange(bufferType, offset, dataSize, access);
		}
		void MeshDataBuffer::UnmapBuffer() const noexcept
		{
			glUnmapBuffer(bufferType);
		}
		void MeshDataBuffer::Bind() const noexcept
		{
			glBindBuffer(bufferType, GetHandle());
		}
		void MeshDataBuffer::BindBase(GLuint index) const noexcept
		{
			glBindBufferBase(bufferType, index, GetHandle());
		}
		void MeshDataBuffer::BindRange(GLuint index, GLintptr offset, GLsizeiptr size) const noexcept
		{
			glBindBufferRange(bufferType, index, GetHandle(), offset, size);
		}
		bool operator==(const MeshDataBuffer& x, const MeshDataBuffer& y) noexcept
		{
			return x.meshDataHandle == y.meshDataHandle;
		}
		bool operator!=(const MeshDataBuffer& x, const MeshDataBuffer& y) noexcept
		{
			return !(x == y);
		}
	}
}

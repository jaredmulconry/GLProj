#include "MeshDataBuffer.hpp"
#include "GLFW/glfw3.h"

namespace GlProj
{
	namespace Graphics
	{
		MeshDataBuffer::MeshDataBuffer(GLenum bufferType, GLsizeiptr dataSize, const GLvoid* data, GLenum dataType, GLint elemsPerVert, GLenum usage)
			: bufferType(bufferType)
			, dataType(dataType)
			, elementsPerVertex(elemsPerVert)
		{
			glGenBuffers(1, &meshDataHandle);
			glBindBuffer(bufferType, meshDataHandle);

			glBufferData(bufferType, dataSize, data, usage);
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
		GLenum MeshDataBuffer::GetBufferType() const noexcept
		{
			return bufferType;
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
			glBufferSubData(GetBufferType(), offset, dataSize, data);
		}
		void* MeshDataBuffer::MapBuffer(GLintptr offset, GLsizeiptr dataSize, GLbitfield access) const noexcept
		{
			return glMapBufferRange(GetBufferType(), offset, dataSize, access);
		}
		void MeshDataBuffer::UnmapBuffer() const noexcept
		{
			glUnmapBuffer(GetBufferType());
		}
		void MeshDataBuffer::Bind() const noexcept
		{
			glBindBuffer(GetBufferType(), GetHandle());
		}
		void MeshDataBuffer::BindBase(GLuint index) const noexcept
		{
			glBindBufferBase(GetBufferType(), index, GetHandle());
		}
		void MeshDataBuffer::BindRange(GLuint index, GLintptr offset, GLsizeiptr size) const noexcept
		{
			glBindBufferRange(GetBufferType(), index, GetHandle(), offset, size);
		}
		bool operator==(const MeshDataBuffer& x, const MeshDataBuffer& y) noexcept
		{
			return x.meshDataHandle == y.meshDataHandle && x.bufferType == y.bufferType;
		}
		bool operator!=(const MeshDataBuffer& x, const MeshDataBuffer& y) noexcept
		{
			return !(x == y);
		}
	}
}

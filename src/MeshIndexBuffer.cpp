#include "MeshIndexBuffer.hpp"
#include "assimp\mesh.h"
#include "assimp\scene.h"
#include <vector>

namespace GlProj
{
	namespace Graphics
	{
		MeshIndexBuffer::MeshIndexBuffer(GLsizeiptr dataSize, const GLvoid* data, GLenum usage)
		{
			glGenBuffers(1, &indexDataHandle);
			glBindBuffer(GetType(), GetHandle());

			glBufferData(GetType(), dataSize, data, usage);
		}
		MeshIndexBuffer::MeshIndexBuffer(GLsizeiptr size, const aiFace* faces, GLenum usage)
		{
			glGenBuffers(1, &indexDataHandle);
			glBindBuffer(GetType(), GetHandle());

			std::vector<unsigned int> meshIndices;
			meshIndices.reserve(size * faces->mNumIndices);

			for(int i = 0; i < size; ++i)
			{
				meshIndices.insert(meshIndices.cend(), faces[i].mIndices, faces[i].mIndices + faces[i].mNumIndices);
			}

			glBufferData(GetType(), meshIndices.size() * sizeof(unsigned int), meshIndices.data(), usage);
		}
		MeshIndexBuffer::MeshIndexBuffer(MeshIndexBuffer&& x) noexcept
			: indexDataHandle(x.indexDataHandle)
		{
			x.indexDataHandle = invalidHandle;
		}
		MeshIndexBuffer& MeshIndexBuffer::operator=(MeshIndexBuffer&& x) noexcept
		{
			if (this != &x)
			{
				if (indexDataHandle != invalidHandle)
				{
					glDeleteBuffers(1, &indexDataHandle);
				}
				indexDataHandle = x.indexDataHandle;

				x.indexDataHandle = invalidHandle;
			}

			return *this;
		}
		MeshIndexBuffer::~MeshIndexBuffer()
		{
			if (indexDataHandle != invalidHandle)
			{
				glDeleteBuffers(1, &indexDataHandle);
			}
		}
		GLuint MeshIndexBuffer::GetHandle() const noexcept
		{
			return indexDataHandle;
		}
		GLenum MeshIndexBuffer::GetType() const noexcept
		{
			return GL_ELEMENT_ARRAY_BUFFER;
		}
		void MeshIndexBuffer::UpdateData(GLintptr offset, GLsizeiptr dataSize, const GLvoid* data) const noexcept
		{
			glBufferSubData(GetType(), offset, dataSize, data);
		}
		void* MeshIndexBuffer::MapBuffer(GLintptr offset, GLsizeiptr dataSize, GLbitfield access) const noexcept
		{
			return glMapBufferRange(GetType(), offset, dataSize, access);
		}
		void MeshIndexBuffer::UnmapBuffer() const noexcept
		{
			glUnmapBuffer(GetType());
		}
		void MeshIndexBuffer::Bind() const noexcept
		{
			glBindBuffer(GetType(), GetHandle());
		}
		void MeshIndexBuffer::BindBase(GLuint index) const noexcept
		{
			glBindBufferBase(GetType(), index, GetHandle());
		}
		void MeshIndexBuffer::BindRange(GLuint index, GLintptr offset, GLsizeiptr size) const noexcept
		{
			glBindBufferRange(GetType(), index, GetHandle(), offset, size);
		}
	}
}
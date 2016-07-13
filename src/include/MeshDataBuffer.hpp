#pragma once
#include "gl_core_4_1.h"

namespace GlProj
{
	namespace Graphics
	{
		class MeshDataBuffer
		{
			GLuint meshDataHandle = invalidHandle;
			GLenum bufferType;
		public:
			static const constexpr GLuint invalidHandle = GLuint(-1);

			friend bool operator==(const MeshDataBuffer&, const MeshDataBuffer&) noexcept;
			friend bool operator!=(const MeshDataBuffer&, const MeshDataBuffer&) noexcept;

			MeshDataBuffer() noexcept = default;
			MeshDataBuffer(GLenum bufferType, GLsizeiptr dataSize, const GLvoid* data, GLenum usage);
			MeshDataBuffer(MeshDataBuffer&&) noexcept;
			MeshDataBuffer& operator=(MeshDataBuffer&&) noexcept;
			~MeshDataBuffer();

			GLuint GetHandle() const noexcept;
			GLenum GetType() const noexcept;

			void UpdateData(GLintptr offset, GLsizeiptr dataSize, const GLvoid* data) const noexcept;
			void* MapBuffer(GLintptr offset, GLsizeiptr dataSize, GLbitfield access) const noexcept;
			void UnmapBuffer() const noexcept;

			void Bind() const noexcept;
			void BindBase(GLuint) const noexcept;
			void BindRange(GLuint, GLintptr, GLsizeiptr) const noexcept;
		};
	}
}
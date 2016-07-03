#pragma once
#include "gl_core_4_1.h"

namespace GlProj
{
	namespace Graphics
	{
		class MeshIndexBuffer
		{
			GLuint indexDataHandle = invalidHandle;
		public:
			static const constexpr GLuint invalidHandle = GLuint(-1);

			MeshIndexBuffer() noexcept = default;
			MeshIndexBuffer(GLsizeiptr dataSize, const GLvoid* data, GLenum usage);
			MeshIndexBuffer(MeshIndexBuffer&&) noexcept;
			MeshIndexBuffer& operator=(MeshIndexBuffer&&) noexcept;
			~MeshIndexBuffer();

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
#pragma once
#include "gl_core_4_5.h"

struct aiFace;

namespace GlProj
{
	namespace Graphics
	{
		class MeshIndexBuffer
		{
			GLuint indexDataHandle = invalidHandle;
		public:
			static const constexpr GLuint invalidHandle = GLuint(-1);

			friend bool operator==(const MeshIndexBuffer&, const MeshIndexBuffer&) noexcept;
			friend bool operator!=(const MeshIndexBuffer&, const MeshIndexBuffer&) noexcept;

			MeshIndexBuffer() noexcept = default;
			MeshIndexBuffer(const MeshIndexBuffer&) = delete;
			MeshIndexBuffer(GLsizeiptr, const GLvoid*, GLenum = GL_STATIC_DRAW);
			MeshIndexBuffer(GLsizeiptr, const aiFace*, GLenum = GL_STATIC_DRAW);
			MeshIndexBuffer(MeshIndexBuffer&&) noexcept;
			MeshIndexBuffer& operator=(MeshIndexBuffer&&) noexcept;
			~MeshIndexBuffer();

			GLuint GetHandle() const noexcept;
			GLenum GetType() const noexcept;

			void UpdateData(GLintptr, GLsizeiptr, const GLvoid*) const noexcept;
			void* MapBuffer(GLintptr, GLsizeiptr, GLbitfield) const noexcept;
			void UnmapBuffer() const noexcept;

			void Bind() const noexcept;
			void BindBase(GLuint) const noexcept;
			void BindRange(GLuint, GLintptr, GLsizeiptr) const noexcept;
		};
	}
}
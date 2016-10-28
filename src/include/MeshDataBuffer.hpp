#pragma once
#include "gl_core_4_5.h"

namespace GlProj
{
	namespace Graphics
	{
		enum class BufferType : GLenum
		{
			array = GL_ARRAY_BUFFER,
			copy_read = GL_COPY_READ_BUFFER,
			atomic_counter = GL_ATOMIC_COUNTER_BUFFER,
			copy_write = GL_COPY_WRITE_BUFFER,
			dispatch_indirect = GL_DISPATCH_INDIRECT_BUFFER,
			draw_indirect = GL_DRAW_INDIRECT_BUFFER,
			element_array = GL_ELEMENT_ARRAY_BUFFER,
			pixel_pack = GL_PIXEL_PACK_BUFFER,
			pixel_unpack = GL_PIXEL_UNPACK_BUFFER,
			query = GL_QUERY_BUFFER,
			shader_storage = GL_SHADER_STORAGE_BUFFER,
			texture = GL_TEXTURE_BUFFER,
			transform_feedback = GL_TRANSFORM_FEEDBACK_BUFFER,
			uniform = GL_UNIFORM_BUFFER,
		};

		enum class BufferUsage : GLenum
		{
			stream_draw = GL_STREAM_DRAW,
			stream_read = GL_STREAM_READ,
			stream_copy = GL_STREAM_COPY,
			static_draw = GL_STATIC_DRAW,
			static_read = GL_STATIC_READ,
			static_copy = GL_STATIC_COPY,
			dynamic_draw = GL_DYNAMIC_DRAW,
			dynamic_read = GL_DYNAMIC_READ,
			dynamic_copy = GL_DYNAMIC_COPY,
		};

		class MeshDataBuffer
		{
			GLuint meshDataHandle = invalidHandle;
			GLenum bufferType;
			GLenum dataType;
			GLint elementsPerVertex;
		public:
			static const constexpr GLuint invalidHandle = GLuint(-1);

			friend bool operator==(const MeshDataBuffer&, const MeshDataBuffer&) noexcept;
			friend bool operator!=(const MeshDataBuffer&, const MeshDataBuffer&) noexcept;

			MeshDataBuffer() noexcept = default;
			MeshDataBuffer(const MeshDataBuffer&) = delete;
			MeshDataBuffer(BufferType, GLsizeiptr, const GLvoid*, GLenum, GLint, BufferUsage = BufferUsage::static_read);
			MeshDataBuffer(MeshDataBuffer&&) noexcept;
			MeshDataBuffer& operator=(MeshDataBuffer&&) noexcept;
			~MeshDataBuffer();

			GLuint GetHandle() const noexcept;
			BufferType GetBufferType() const noexcept;
			GLenum GetDataType() const noexcept;
			GLint GetElementsPerVertex() const noexcept;

			void UpdateData(GLintptr offset, GLsizeiptr dataSize, const GLvoid* data) const noexcept;
			void* MapBuffer(GLintptr offset, GLsizeiptr dataSize, GLbitfield access) const noexcept;
			void UnmapBuffer() const noexcept;

			void Bind() const noexcept;
			void BindBase(GLuint) const noexcept;
			void BindRange(GLuint, GLintptr, GLsizeiptr) const noexcept;
		};
	}
}

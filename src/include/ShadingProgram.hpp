#pragma once
#include "gl_core_4_5.h"
#include <string>
#include <vector>
#include <utility>

namespace GlProj
{
	namespace Graphics
	{
		class Shader;
		class Mesh;
		enum class MeshSlots : GLuint;

		struct VertexAttribute
		{
			std::string name;
			GLenum type;
			GLint location;
			GLint size;
		};

		struct UniformInformation
		{
			std::string name;
			GLenum type;
			GLint location;
			GLint size;
		};

		class ShadingProgram
		{
			using VertexAttribStorage = std::vector<VertexAttribute>;
			using UniformInfoStorage = std::vector<UniformInformation>;
			using UniformNameBufStorage = std::vector<std::pair<std::string, UniformInformation*>>;

			VertexAttribStorage attributes;
			UniformInfoStorage uniforms;
			UniformNameBufStorage uniformNameRef;
			GLuint programHandle = invalidHandle;
			bool transformsAreBatchable = false;

		public:
			using VertexAttribConstIterator = VertexAttribStorage::const_iterator;
			using UniformInfoConstIterator = UniformInfoStorage::const_iterator;

			static const constexpr GLuint invalidHandle = GLuint(-1);

			friend bool operator==(const ShadingProgram&, const ShadingProgram&) noexcept;
			friend bool operator!=(const ShadingProgram&, const ShadingProgram&) noexcept;

			ShadingProgram() noexcept = default;
			ShadingProgram(const ShadingProgram&) = delete;
			ShadingProgram(ShadingProgram&&) noexcept;
			ShadingProgram& operator=(ShadingProgram&&) noexcept;
			~ShadingProgram();
			explicit ShadingProgram(GLuint) noexcept;

			GLuint GetHandle() const noexcept;

			VertexAttribConstIterator FindAttribute(const std::string&) const;
			VertexAttribConstIterator FindAttribute(GLint) const;
			VertexAttribConstIterator FindAttribute(MeshSlots) const;
			UniformInfoConstIterator FindUniform(const std::string&) const;
			UniformInfoConstIterator FindUniform(GLint) const;

			VertexAttribConstIterator AttributesEnd() const;
			UniformInfoConstIterator UniformsEnd() const;

			void Bind() const noexcept;
			void FetchProgramInfo();
		};
	}
}

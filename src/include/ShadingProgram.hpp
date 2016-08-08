#pragma once
#include "OpenGLTypes.hpp"
#include <string>
#include <vector>

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

			GLuint programHandle = invalidHandle;
			VertexAttribStorage attributes;
			UniformInfoStorage uniforms;

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

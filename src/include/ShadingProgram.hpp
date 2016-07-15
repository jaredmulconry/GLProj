#pragma once
#include "OpenGLTypes.hpp"
#include <string>
#include <vector>

namespace GlProj
{
	namespace Graphics
	{
		class Shader;

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
			GLuint programHandle = invalidHandle;
			std::vector<VertexAttribute> attributes;
			std::vector<UniformInformation> uniforms;

		public:
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

			void Bind() const noexcept;
			void FetchProgramInfo();
		};
	}
}
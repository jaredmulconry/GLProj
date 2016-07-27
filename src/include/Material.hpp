#pragma once
#include "OpenGLTypes.hpp"
#include "glm\fwd.hpp"
#include <memory>

namespace GlProj
{
	namespace Graphics
	{
		class ShadingProgram;
		class Texture;
		class Sampler;
		struct UniformInformation;

		class Material
		{
			std::shared_ptr<ShadingProgram> program;
			
		public:
			Material() noexcept = default;


			void SetUniform(const UniformInformation&, GLint);
			void SetUniform(const UniformInformation&, GLfloat);
			void SetUniform(const UniformInformation&, const glm::vec2&);
			void SetUniform(const UniformInformation&, const glm::vec3&);
			void SetUniform(const UniformInformation&, const glm::vec4&);
			void SetUniform(const UniformInformation&, int);
		};
	}
}
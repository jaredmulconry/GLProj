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

		enum class TextureSlot : GLint
		{
			Diffuse1,
			Diffuse2,
			Normal,
			Specular,
			Emissive,
			Gloss,
			Displacement,
			Opacity,
			User,
		};

		GLint TextureSlotToGL(TextureSlot s);

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
			void SetUniform(const UniformInformation&, const glm::mat2&);
			void SetUniform(const UniformInformation&, const glm::mat3&);
			void SetUniform(const UniformInformation&, const glm::mat4&);


			void SetUniform(const UniformInformation&, const GLint*, int);
			void SetUniform(const UniformInformation&, const GLfloat*, int);
			void SetUniform(const UniformInformation&, const glm::vec2*, int);
			void SetUniform(const UniformInformation&, const glm::vec3*, int);
			void SetUniform(const UniformInformation&, const glm::vec4*, int);
			void SetUniform(const UniformInformation&, const glm::mat2*, int);
			void SetUniform(const UniformInformation&, const glm::mat3*, int);
			void SetUniform(const UniformInformation&, const glm::mat4*, int);
		};
	}
}
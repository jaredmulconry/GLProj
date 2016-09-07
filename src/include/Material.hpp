#pragma once
#include "OpenGLTypes.hpp"
#include "glm\fwd.hpp"
#include "LocalSharedPtr.hpp"

namespace GlProj
{
	namespace Graphics
	{
		struct Camera;
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

		using GlProj::Utilities::LocalSharedPtr;

		class Material
		{
			LocalSharedPtr<ShadingProgram> program;
			
		public:
			Material() noexcept = default;
			explicit Material(const LocalSharedPtr<ShadingProgram>&);

			Material& operator=(const LocalSharedPtr<ShadingProgram>&);

			void Bind() const;
			const ShadingProgram* GetProgram() const noexcept;
			
			void SetUniform(const UniformInformation&, GLint);
			void SetUniform(const UniformInformation&, GLuint);
			void SetUniform(const UniformInformation&, GLfloat);
			void SetUniform(const UniformInformation&, const glm::vec2&);
			void SetUniform(const UniformInformation&, const glm::vec3&);
			void SetUniform(const UniformInformation&, const glm::vec4&);
			void SetUniform(const UniformInformation&, const glm::mat2&);
			void SetUniform(const UniformInformation&, const glm::mat3&);
			void SetUniform(const UniformInformation&, const glm::mat4&);


			void SetUniform(const UniformInformation&, const GLint*, int);
			void SetUniform(const UniformInformation&, const GLuint*, int);
			void SetUniform(const UniformInformation&, const GLfloat*, int);
			void SetUniform(const UniformInformation&, const glm::vec2*, int);
			void SetUniform(const UniformInformation&, const glm::vec3*, int);
			void SetUniform(const UniformInformation&, const glm::vec4*, int);
			void SetUniform(const UniformInformation&, const glm::mat2*, int);
			void SetUniform(const UniformInformation&, const glm::mat3*, int);
			void SetUniform(const UniformInformation&, const glm::mat4*, int);

			friend bool operator==(const Material&, const Material&) noexcept;
			friend bool operator!=(const Material&, const Material&) noexcept;
			friend bool operator<(const Material&, const Material&) noexcept;
			friend bool operator<=(const Material&, const Material&) noexcept;
			friend bool operator>(const Material&, const Material&) noexcept;
			friend bool operator>=(const Material&, const Material&) noexcept;
		};

		void ApplyTransformUniforms(Material&, const glm::mat4&, const Camera&);
	}
}
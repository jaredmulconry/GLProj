#pragma once
#include "OpenGLTypes.hpp"

namespace GlProj
{
	namespace Graphics
	{
		class Shader
		{
			GLuint shaderHandle = invalidHandle;
			GLenum shaderType;
		public:
			static const constexpr GLuint invalidHandle = GLuint(-1);

			friend bool operator==(const Shader&, const Shader&) noexcept;
			friend bool operator!=(const Shader&, const Shader&) noexcept;

			Shader() noexcept = default;
			Shader(GLenum, GLuint) noexcept;
			Shader(const Shader&) = delete;
			Shader(Shader&&) noexcept;
			Shader& operator=(Shader&&) noexcept;
			~Shader();

			GLuint GetHandle() const noexcept;
			GLenum GetType() const noexcept;
		};
	}
}
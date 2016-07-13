#pragma once
#include "gl_core_4_1.h"
#include <memory>
#include <vector>

namespace GlProj
{
	namespace Graphics
	{
		class Shader;

		class ShadingProgram
		{
			GLuint programHandle = invalidHandle;
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
		};
	}
}
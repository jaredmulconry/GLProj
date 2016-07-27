#pragma once
#include <memory>

namespace GlProj
{
	namespace Graphics
	{
		class ShadingProgram;
		class Texture;
		class Sampler;

		class Material
		{
			std::shared_ptr<ShadingProgram> program;
			
		public:
			Material() noexcept = default;

		};
	}
}
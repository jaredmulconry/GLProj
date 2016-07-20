#pragma once
#include <memory>

namespace GlProj
{
	namespace Graphics
	{
		class ShadingProgram;
		class Texture;
		class Sampler;

		

		struct TextureInput
		{
			std::shared_ptr<Texture> texture;
			std::shared_ptr<Sampler> sampler;
		};
		

		class Material
		{

		};
	}
}
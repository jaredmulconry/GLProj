#pragma once
#include <memory>

namespace GlProj
{
	namespace Graphics
	{
		class ShadingProgram;
		class Texture;
		class Sampler;

		struct MaterialInputBase
		{

		};

		template<typename T>
		struct MaterialInput : public MaterialInputBase
		{
			T input;
		};
		template<>
		struct MaterialInput<Texture> : public MaterialInputBase
		{
			std::shared_ptr<Texture> texture;
			std::shared_ptr<Sampler> sampler;
		};

		class Material
		{
			std::shared_ptr<ShadingProgram> program;

		};
	}
}
#pragma once
#include <memory>
#include <vector>

namespace GlProj
{
	namespace Graphics
	{
		class ShadingProgram;
		class Texture;
		class Sampler;

		template<typename T>
		struct MaterialInput;

		struct MaterialInputBase
		{
			
		};
		template<typename T>
		MaterialInput<T>* GetMaterialInput(MaterialInputBase& x)
		{
			return dynamic_cast<MaterialInput<T>*>(&x);
		}
		template<typename T>
		const MaterialInput<T>* GetMaterialInput(const MaterialInputBase& x)
		{
			return dynamic_cast<const MaterialInput<T>*>(&x);
		}

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
			std::vector<MaterialInput<int>> integerInputs;
			std::vector<MaterialInput<float>> floatInputs;
			std::vector<MaterialInput<Texture>> textureInputs;
			
		public:
			Material() noexcept = default;

		};
	}
}
#pragma once
#include <memory>
#include <vector>

namespace GlProj
{
	namespace Graphics
	{
		class Mesh;
		class Material;

		class Model
		{
			std::vector<std::shared_ptr<Mesh>> meshes;
			std::vector<std::shared_ptr<Material>> materials;
		};
	}
}
#pragma once
#include "Material.hpp"
#include <memory>
#include <vector>

struct aiScene;

namespace GlProj
{
	namespace Graphics
	{
		class Mesh;

		class Model
		{
			std::vector<std::shared_ptr<Mesh>> meshes;
			std::vector<Material> materials;
		public:
			Model() = default;
			Model(const std::vector<std::shared_ptr<Mesh>>&, const std::vector<Material>&);
		};
	}
}
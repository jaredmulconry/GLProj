#include "Model.hpp"

namespace GlProj
{
	namespace Graphics
	{
		Model::Model(const std::vector<std::shared_ptr<Mesh>>& meshes, const std::vector<Material>& materials)
			:meshes(meshes)
			,materials(materials)
		{}
	}
}
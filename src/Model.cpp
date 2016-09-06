#include "Model.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include <utility>

namespace GlProj
{
	namespace Graphics
	{
		void Model::DrawSubmesh(hierarchy_node * n, glm::mat4 transform) const
		{
			transform *= n->data.transform;
			for (auto m : n->data.meshes)
			{
				auto& renderer = submeshes[m];
				renderer.mesh->Bind();
				renderer.material->Bind();
			}
		}
		Model::Model(const std::vector<Renderable>& renderables,
						SceneGraph<ModelData>&& hierarchy)
			:submeshes(renderables)
			,hierarchy(std::move(hierarchy))
		{}
	}
}
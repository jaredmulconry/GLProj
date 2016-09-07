#include "Model.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include <utility>

namespace GlProj
{
	namespace Graphics
	{
		void Model::DrawSubmesh(hierarchy_node * n, const Camera& cam, glm::mat4 transform, Material& mat) const
		{
			transform *= n->data.transform;
			for (auto m : n->data.meshes)
			{
				auto& renderer = submeshes[m];
				renderer.mesh->Bind();
				mat.Bind();
				ApplyTransformUniforms(mat, transform, cam);
			}
		}
		void Model::DrawSubmesh(hierarchy_node * n, const Camera& cam, glm::mat4 transform) const
		{
			transform *= n->data.transform;
			for (auto m : n->data.meshes)
			{
				auto& renderer = submeshes[m];
				renderer.mesh->Bind();
				renderer.material->Bind();
				ApplyTransformUniforms(*renderer.material, transform, cam);

			}
		}
		Model::Model(const std::vector<Renderable>& renderables,
						SceneGraph<ModelData>&& hierarchy)
			:submeshes(renderables)
			,hierarchy(std::move(hierarchy))
		{}
		void Model::Draw(const Camera& cam) const
		{

		}
		void Model::Draw(const Camera& cam, Material& mat) const
		{

		}
	}
}
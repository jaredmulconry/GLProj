#include "gl_core_4_1.h"
#include "GLFW/glfw3.h"
#include "Model.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include <utility>

namespace GlProj
{
	namespace Graphics
	{
		Model::Model(const std::vector<Renderable>& renderables,
						SceneGraph<ModelData>&& hierarchy)
			:submeshes(renderables)
			,hierarchy(std::move(hierarchy))
		{

		}
		void Model::Draw(const Camera& cam) const
		{

			//glDrawElements(GL_TRIANGLES, submeshes[0].mesh->FaceCount(), GL_UNSIGNED_INT, nullptr);
		}
		void Model::Draw(const Camera& cam, Material& mat) const
		{

		}
	}
}
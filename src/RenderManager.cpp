#include "RenderManager.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Transform.hpp"

#include "glm\mat4x4.hpp"

#include <vector>

using GlProj::Utilities::LocalWeakPtr;

namespace GlProj
{
	namespace Graphics
	{
		class RenderManager
		{
			std::vector<LocalWeakPtr<RenderBatch>> batches;
		};
		class RenderBatch
		{
			std::vector<LocalWeakPtr<RenderableHandle>> handles;
			glm::mat4 projectionTransform;
			glm::mat4 viewTransform;
			BatchType type;
			int priority;
			bool groupedByMaterial;
			bool groupedByMesh;
		};
		class RenderableHandle
		{
			glm::mat4 transform;
			Mesh* mesh;
			Material* material;
		};


		RenderManager* GetRenderManager()
		{
			static RenderManager manager;
			return &manager;
		}

		local_shared_ptr<RenderBatch> GenerateRenderBatch(RenderManager* mngr, BatchType bt, int pr, bool gMat, bool gMesh)
		{
			return nullptr;
		}

		Material* SetOverrideMaterial(RenderManager* mngr, RenderBatch* batch, Material* mat)
		{
			return nullptr;
		}

		void UpdateBatchCamera(RenderManager* mngr, RenderBatch* batch, const Camera& cam)
		{
		}

		local_shared_ptr<RenderableHandle> SubmitRenderable(RenderManager* mngr, RenderBatch* batch, Mesh& mesh, Material* mat)
		{
			return nullptr;
		}

		Material* SetMaterial(RenderManager* mngr, RenderBatch* batch, RenderableHandle* rnd, Material* mat)
		{
			return nullptr;
		}

		Transform SetTransform(RenderManager* mngr, RenderBatch* batch, RenderableHandle* rnd, const Transform& trans)
		{
			return Transform();
		}

	}
}
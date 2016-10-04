#include "RenderManager.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Transform.hpp"

#include "glm\mat4x4.hpp"

#include <algorithm>
#include <utility>
#include <vector>

using GlProj::Utilities::LocalWeakPtr;

namespace GlProj
{
	namespace Graphics
	{
		class RenderManager
		{
			std::vector<LocalWeakPtr<RenderBatch>> batches;
		public:
			void RegisterBatch(LocalWeakPtr<RenderBatch> b)
			{
				batches.push_back(std::move(b));
			}
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
		public:
			RenderBatch() = default;
			RenderBatch(BatchType t, int priority, bool groupMat, bool groupMesh)
				: type(t)
				, priority(priority)
				, groupedByMaterial(groupMat)
				, groupedByMesh(groupMesh)
			{}

			LocalSharedPtr<RenderableHandle> AddHandle(LocalWeakPtr<RenderableHandle> h)
			{
				auto insertPos = std::lower_bound(handles.begin(), handles.end(), h, 
					[](const auto& x, const auto& y)
				{
					return x.owner_before(y);
				});
				if (Utilities::CompareWeaks(*insertPos, h))
				{
					return h.lock();
				}
				auto sp = h.lock();
				handles.insert(insertPos, std::move(h));

				return sp;
			}
		};
		class RenderableHandle
		{
			glm::mat4 transform;
			Mesh* mesh;
			Material* material;
		public:

		};


		RenderManager* GetRenderManager()
		{
			static RenderManager manager;
			return &manager;
		}

		local_shared_ptr<RenderBatch> GenerateRenderBatch(RenderManager* mngr, BatchType bt, int pr, bool gMat, bool gMesh)
		{
			auto newBatch = Utilities::make_localshared<RenderBatch>(bt, pr, gMat, gMesh);
			mngr->RegisterBatch(newBatch);

			return newBatch;
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
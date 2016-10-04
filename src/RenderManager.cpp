#include "RenderManager.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Transform.hpp"

#include "glm\mat4x4.hpp"

#include <algorithm>
#include <utility>
#include <vector>

using namespace GlProj::Utilities;

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
			static bool LessWeaks(const LocalWeakPtr<RenderableHandle>& x,
								  const LocalWeakPtr<RenderableHandle>& y) noexcept
			{
				return x.owner_before(y);
			}

			//State needs to be appropriately compact in memory.
			//However, only some of the state has invariants
			//worth protecting.

			std::vector<LocalWeakPtr<RenderableHandle>> handles;
		public:
			glm::mat4 projectionTransform = glm::mat4(1);
			glm::mat4 viewTransform = glm::mat4(1);
		private:
			Material* overrideMaterial = nullptr;
		public:
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
				auto insertPos = std::lower_bound(handles.begin(), handles.end(), h, LessWeaks);
				if (Utilities::CompareWeaks(*insertPos, h))
				{
					return h.lock();
				}
				auto sp = h.lock();
				handles.insert(insertPos, std::move(h));

				return sp;
			}
			bool RemoveHandle(const LocalWeakPtr<RenderableHandle>& h)
			{
				auto handlePos = std::lower_bound(handles.begin(), handles.end(), h, LessWeaks);
				if (CompareWeaks(*handlePos, h))
				{
					handles.erase(handlePos);
					return true;
				}
				return false;
			}
		};
		class RenderableHandle
		{
		public:
			glm::mat4 transform = glm::mat4(1);
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
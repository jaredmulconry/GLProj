#include "RenderManager.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Transform.hpp"

#include "glm\mat4x4.hpp"

#include <algorithm>
#include <type_traits>
#include <utility>
#include <vector>

using namespace GlProj::Utilities;

namespace GlProj
{
	namespace Graphics
	{
		class RenderManager final
		{
		public:
			std::vector<LocalWeakPtr<RenderBatch>> batches;
			bool dirty = true;
			static bool OrderBatchByType(const LocalWeakPtr<RenderBatch>& x, const LocalWeakPtr<RenderBatch>& y) noexcept;
			static bool OrderBatchByPriority(const LocalWeakPtr<RenderBatch>& x, const LocalWeakPtr<RenderBatch>& y) noexcept;

			void RegisterBatch(LocalWeakPtr<RenderBatch> b);
			void OptimiseBatchOrder();
			void CleanStale();
		};
		class RenderBatch final
		{
		public:

			static bool LessWeaks(const LocalWeakPtr<RenderableHandle>& x,
				const LocalWeakPtr<RenderableHandle>& y) noexcept;

			std::vector<LocalWeakPtr<RenderableHandle>> handles;

			glm::mat4 projectionTransform = glm::mat4(1);
			glm::mat4 viewTransform = glm::mat4(1);

			Material* overrideMaterial = nullptr;

			BatchType type;
			int priority;
			bool groupedByMaterial;
			bool groupedByMesh;

			RenderBatch() = default;
			RenderBatch(BatchType t, int priority, bool groupMat, bool groupMesh);

			LocalSharedPtr<RenderableHandle> AddHandle(LocalWeakPtr<RenderableHandle> h);
			bool RemoveHandle(const LocalWeakPtr<RenderableHandle>& h);
		};
		class RenderableHandle final
		{
		public:
			glm::mat4 transform = glm::mat4(1);
			Mesh* mesh = nullptr;
			Material* material = nullptr;

			RenderableHandle() = default;
			RenderableHandle(Mesh* me, Material* ma)
				: mesh(me)
				, material(ma)
			{}
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

		Material* SetOverrideMaterial(RenderBatch* batch, Material* mat)
		{
			auto prev = batch->overrideMaterial;
			batch->overrideMaterial = mat;
			return prev;
		}

		void UpdateBatchCamera(RenderBatch* batch, const Camera& cam)
		{
			batch->viewTransform = cam.View();
			batch->projectionTransform = cam.Projection();
		}

		local_shared_ptr<RenderableHandle> SubmitRenderable(RenderBatch* batch, Mesh& mesh, Material* mat)
		{
			auto newHandle = make_localshared<RenderableHandle>(&mesh, mat);
			batch->AddHandle(newHandle);

			return newHandle;
		}

		bool RemoveRenderable(RenderBatch* batch, local_shared_ptr<RenderableHandle>&& h)
		{
			return batch->RemoveHandle(h);
		}

		Material* SetMaterial(RenderableHandle* rnd, Material* mat)
		{
			auto prev = rnd->material;
			rnd->material = mat;
			return prev;
		}

		glm::mat4 SetTransform(RenderableHandle* rnd, const glm::mat4& trans)
		{
			auto prev = rnd->transform;
			rnd->transform = trans;
			return prev;
		}

		void Draw(RenderManager*)
		{
		}

		void DrawBatch(RenderManager*, RenderBatch*)
		{
		}

		void DrawRenderable(RenderManager*, RenderBatch*, RenderableHandle*)
		{
		}


		inline bool RenderManager::OrderBatchByType(const LocalWeakPtr<RenderBatch>& x, const LocalWeakPtr<RenderBatch>& y) noexcept
		{
			int validState = int(x.expired());
			validState += int(y.expired()) * 2;

			switch (validState)
			{
			case 0:
				using T = std::underlying_type_t<BatchType>;
				return T(x.lock()->type) < T(y.lock()->type);
			case 1:
				return false;
			case 2:
				return true;
			case 3:
				return false;
			default:
				std::terminate();
				break;
			}
		}
		inline bool RenderManager::OrderBatchByPriority(const LocalWeakPtr<RenderBatch>& x, const LocalWeakPtr<RenderBatch>& y) noexcept
		{
			int validState = int(x.expired());
			validState += int(y.expired()) * 2;

			switch (validState)
			{
			case 0:
				return x.lock()->priority > y.lock()->priority;
			case 1:
				return false;
			case 2:
				return true;
			case 3:
				return false;
			default:
				std::terminate();
				break;
			}


		}
		inline void RenderManager::RegisterBatch(LocalWeakPtr<RenderBatch> b)
		{
			CleanStale();
			auto insertRange = std::equal_range(batches.begin(), batches.end(), b, OrderBatchByType);
			auto insertPoint = std::upper_bound(batches.begin(), batches.end(), b, OrderBatchByPriority);
			batches.insert(insertPoint, std::move(b));
		}
		inline void RenderManager::OptimiseBatchOrder()
		{
			CleanStale();
			if (dirty)
			{
				std::stable_sort(batches.begin(), batches.end(), OrderBatchByPriority);
				std::stable_sort(batches.begin(), batches.end(), OrderBatchByType);
				dirty = false;
			}
		}
		void RenderManager::CleanStale()
		{
			batches.erase(std::remove_if(batches.begin(), batches.end(),
				[](const auto& x) { return x.expired(); }), batches.end());
		}
		inline bool RenderBatch::LessWeaks(const LocalWeakPtr<RenderableHandle>& x, const LocalWeakPtr<RenderableHandle>& y) noexcept
		{
			return x.owner_before(y);
		}
		inline RenderBatch::RenderBatch(BatchType t, int priority, bool groupMat, bool groupMesh)
			: type(t)
			, priority(priority)
			, groupedByMaterial(groupMat)
			, groupedByMesh(groupMesh)
		{}
		inline LocalSharedPtr<RenderableHandle> RenderBatch::AddHandle(LocalWeakPtr<RenderableHandle> h)
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
		inline bool RenderBatch::RemoveHandle(const LocalWeakPtr<RenderableHandle>& h)
		{
			auto handlePos = std::lower_bound(handles.begin(), handles.end(), h, LessWeaks);
			if (CompareWeaks(*handlePos, h))
			{
				handles.erase(handlePos);
				return true;
			}
			return false;
		}
	}
}
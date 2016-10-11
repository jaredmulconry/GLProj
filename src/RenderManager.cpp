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
			friend void Draw(RenderManager*);
			friend void DrawBatch(RenderManager*, RenderBatch*);
			friend void DrawRenderable(RenderManager*, RenderBatch*, RenderableHandle*);

		public:
			std::vector<LocalWeakPtr<RenderBatch>> batches;
			bool dirty = true;
			static bool OrderBatchByType(const LocalWeakPtr<RenderBatch>& x,
				const LocalWeakPtr<RenderBatch>& y) noexcept;
			static bool OrderBatchByPriority(const LocalWeakPtr<RenderBatch>& x,
				const LocalWeakPtr<RenderBatch>& y) noexcept;

			void RegisterBatch(LocalWeakPtr<RenderBatch> b);
			void OptimiseBatchOrder();
			void CleanStale();
		};
		class RenderBatch final
		{
			friend void DrawBatch(RenderManager*, RenderBatch*);
			friend void DrawRenderable(RenderManager*, RenderBatch*, RenderableHandle*);

		public:
			static bool LessWeaks(const LocalWeakPtr<RenderableHandle>& x,
				const LocalWeakPtr<RenderableHandle>& y) noexcept;
			static bool OrderHandlesByMaterial(const LocalWeakPtr<RenderableHandle>& x,
				const LocalWeakPtr<RenderableHandle>& y);
			static bool OrderHandlesByMesh(const LocalWeakPtr<RenderableHandle>& x,
				const LocalWeakPtr<RenderableHandle>& y);

			std::vector<LocalWeakPtr<RenderableHandle>> handles;

			glm::mat4 projectionTransform = glm::mat4(1);
			glm::mat4 viewTransform = glm::mat4(1);

			Material* overrideMaterial = nullptr;

			BatchType type;
			int priority;
			bool groupedByMaterial;
			bool groupedByMesh;
			bool dirty = false;

			RenderBatch() = default;
			RenderBatch(BatchType t, int priority, bool groupMat, bool groupMesh);

			template <typename I, typename H>
			inline std::pair<I, I> FindHandleSubrange(I first, I last, const H& h)
			{
				bool byMat = groupedByMaterial, byMesh = groupedByMesh;
				auto elemRange = std::make_pair(first, last);
				if (byMat || byMesh)
				{
					elemRange = byMat ? std::equal_range(elemRange.first, elemRange.second, h,
						OrderHandlesByMaterial)
						: elemRange;
					elemRange = byMesh ? std::equal_range(elemRange.first, elemRange.second,
						h, OrderHandlesByMesh)
						: elemRange;
				}
				return elemRange;
			}

			LocalSharedPtr<RenderableHandle> AddHandle(LocalWeakPtr<RenderableHandle> h);
			bool RemoveHandle(const LocalWeakPtr<RenderableHandle>& h);

			void CleanStale();
			void OptimiseBatch();
			template<typename I, typename P>
			I GetNextSubrange(I first, I last, P pred)
			{
				if (first == last) return last;
				return std::upper_bound(first, last, *first, pred);
			}
		};
		class RenderableHandle final
		{
			friend void DrawRenderable(RenderManager*, RenderBatch*, RenderableHandle*);

		public:
			glm::mat4 transform = glm::mat4(1);
			Mesh* mesh = nullptr;
			Material* material = nullptr;

			RenderableHandle() = default;
			RenderableHandle(Mesh* me, Material* ma) : mesh(me), material(ma) {}
		};

		RenderManager* GetRenderManager()
		{
			static RenderManager manager;
			return &manager;
		}

		local_shared_ptr<RenderBatch> GenerateRenderBatch(RenderManager* mngr,
			BatchType bt,
			int pr,
			bool gMat,
			bool gMesh)
		{
			auto newBatch = Utilities::make_localshared<RenderBatch>(bt, pr, gMat, gMesh);
			mngr->RegisterBatch(newBatch);

			return newBatch;
		}

		Material* SetOverrideMaterial(RenderBatch* batch, Material* mat)
		{
			auto prev = batch->overrideMaterial;
			batch->overrideMaterial = mat;
			batch->dirty = true;
			return prev;
		}

		void UpdateBatchCamera(RenderBatch* batch, const Camera& cam)
		{
			batch->viewTransform = cam.View();
			batch->projectionTransform = cam.Projection();
		}

		local_shared_ptr<RenderableHandle> SubmitRenderable(RenderBatch* batch,
			Mesh& mesh,
			Material* mat)
		{
			auto newHandle = make_localshared<RenderableHandle>(&mesh, mat);
			batch->AddHandle(newHandle);

			return newHandle;
		}

		bool RemoveRenderable(RenderBatch* batch,
			local_shared_ptr<RenderableHandle>&& h)
		{
			return batch->RemoveHandle(h);
		}

		Material* SetMaterial(RenderBatch* batch, RenderableHandle* rnd, Material* mat)
		{
			auto prev = rnd->material;
			rnd->material = mat;
			batch->dirty = true;
			return prev;
		}

		glm::mat4 SetTransform(RenderableHandle* rnd, const glm::mat4& trans)
		{
			auto prev = rnd->transform;
			rnd->transform = trans;
			return prev;
		}

		void Draw(RenderManager* mngr)
		{
			mngr->OptimiseBatchOrder();
			for (auto& b : mngr->batches)
			{
				DrawBatch(mngr, b.lock().get());
			}
		}

		void DrawBatch(RenderManager* mngr, RenderBatch* batch)
		{
			batch->OptimiseBatch();

			auto handlesEnd = batch->handles.end();

			auto materialBegin = batch->handles.begin();
			auto materialEnd = batch->GetNextSubrange(materialBegin,
				handlesEnd,
				RenderBatch::OrderHandlesByMaterial);
			while (materialBegin != handlesEnd)
			{
				//Bind shared material

				//
				auto meshBegin = materialBegin;
				auto meshEnd = batch->GetNextSubrange(meshBegin,
					materialEnd,
					RenderBatch::OrderHandlesByMaterial);

				while (meshBegin != materialEnd)
				{
					//Bind shared meshes
					//Initialize instance buffer
					//Render

					//

					meshBegin = meshEnd;
					meshEnd = batch->GetNextSubrange(meshBegin,
						materialEnd,
						RenderBatch::OrderHandlesByMaterial);
				}
				
				materialBegin = materialEnd;
				materialEnd = batch->GetNextSubrange(materialBegin,
					handlesEnd,
					RenderBatch::OrderHandlesByMaterial);
			}
		}

		void DrawRenderable(RenderManager* mngr,
			RenderBatch* batch,
			RenderableHandle* h)
		{
		}

		inline bool RenderManager::OrderBatchByType(
			const LocalWeakPtr<RenderBatch>& x,
			const LocalWeakPtr<RenderBatch>& y) noexcept
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
		inline bool RenderManager::OrderBatchByPriority(
			const LocalWeakPtr<RenderBatch>& x,
			const LocalWeakPtr<RenderBatch>& y) noexcept
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
			auto insertRange =
				std::equal_range(batches.begin(), batches.end(), b, OrderBatchByType);
			auto insertPoint =
				std::upper_bound(batches.begin(), batches.end(), b, OrderBatchByPriority);
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
				[](const auto& x) { return x.expired(); }),
				batches.end());
		}
		inline bool RenderBatch::LessWeaks(
			const LocalWeakPtr<RenderableHandle>& x,
			const LocalWeakPtr<RenderableHandle>& y) noexcept
		{
			return x.owner_before(y);
		}
		bool RenderBatch::OrderHandlesByMaterial(
			const LocalWeakPtr<RenderableHandle>& x,
			const LocalWeakPtr<RenderableHandle>& y)
		{
			int validState = int(x.expired());
			validState += int(y.expired()) * 2;

			switch (validState)
			{
			case 0:
				return x.lock()->material < y.lock()->material;
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
		bool RenderBatch::OrderHandlesByMesh(const LocalWeakPtr<RenderableHandle>& x,
			const LocalWeakPtr<RenderableHandle>& y)
		{
			int validState = int(x.expired());
			validState += int(y.expired()) * 2;

			switch (validState)
			{
			case 0:
				return x.lock()->mesh < y.lock()->mesh;
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
		inline RenderBatch::RenderBatch(BatchType t,
			int priority,
			bool groupMat,
			bool groupMesh)
			: type(t),
			priority(priority),
			groupedByMaterial(groupMat),
			groupedByMesh(groupMesh)
		{
		}
		inline LocalSharedPtr<RenderableHandle> RenderBatch::AddHandle(
			LocalWeakPtr<RenderableHandle> h)
		{
			CleanStale();
			auto elemRange = FindHandleSubrange(handles.begin(), handles.end(), h);

			auto elemPos =
				std::find_if(elemRange.first, elemRange.second,
					[&h](const auto& x) { return CompareWeaks(h, x); });
			if (elemPos != elemRange.second)
			{
				return h.lock();
			}
			auto sp = h.lock();
			handles.insert(elemPos, std::move(h));

			return sp;
		}
		inline bool RenderBatch::RemoveHandle(const LocalWeakPtr<RenderableHandle>& h)
		{
			auto elemRange = FindHandleSubrange(handles.begin(), handles.end(), h);

			auto elemPos =
				std::find_if(elemRange.first, elemRange.second,
					[&h](const auto& x) { return CompareWeaks(h, x); });
			if (elemPos != elemRange.second)
			{
				handles.erase(elemPos);
				return true;
			}
			return false;
		}
		void RenderBatch::CleanStale()
		{
			handles.erase(std::remove_if(handles.begin(), handles.end(),
				[](const auto& x) { return x.expired(); }),
				handles.end());
		}
		void RenderBatch::OptimiseBatch()
		{
			CleanStale();
			if (dirty)
			{
				if (this->overrideMaterial != nullptr)
				{
					std::stable_sort(handles.begin(), handles.end(), OrderHandlesByMesh);
				}
				else
				{
					std::stable_sort(handles.begin(), handles.end(), OrderHandlesByMesh);
					std::stable_sort(handles.begin(), handles.end(), OrderHandlesByMaterial);
				}
				dirty = false;
			}
		}
	}
}
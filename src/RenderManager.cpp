#include "RenderManager.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "MeshDataBuffer.hpp"
#include "ShadingProgram.hpp"
#include "Transform.hpp"

#include "glm/mat4x4.hpp"

#include <algorithm>
#include <string>
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

			static const constexpr auto MaxBatchedDraws = GLsizeiptr(500);

		public:
			MeshDataBuffer transformBuffer;
			std::vector<LocalWeakPtr<RenderBatch>> batches;
			bool dirty = true;

			explicit RenderManager(GLsizeiptr s = MaxBatchedDraws)
				:transformBuffer(BufferType::array, s * sizeof(glm::mat4), nullptr, GL_FLOAT, 4, BufferUsage::stream_draw)
				, batches()
			{

			}

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

		static const std::string bm_transform_id = "bm_transform";
		static const std::string m_transform_id = "m_transform";
		static const std::string v_transform_id = "v_transform";
		static const std::string p_transform_id = "p_transform";
		static const std::string mv_transform_id = "mv_transform";
		static const std::string vp_transform_id = "vp_transform";
		static const std::string mvp_transform_id = "mvp_transform";

		static const std::string i_m_transform_id = "im_transform";
		static const std::string i_v_transform_id = "iv_transform";
		static const std::string i_p_transform_id = "ip_transform";
		static const std::string i_mv_transform_id = "imv_transform";
		static const std::string i_vp_transform_id = "ivp_transform";
		static const std::string i_mvp_transform_id = "imvp_transform";
		
		void UpdateTransforms(Material* m, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
			bool updateModel = true, bool updateView = true, bool updateProject = true);

		RenderManager* GetRenderManager()
		{
			static RenderManager manager{};
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

		void DrawBatch(RenderManager*, RenderBatch* batch)
		{
			batch->OptimiseBatch();

			const bool usingOverride = batch->overrideMaterial != nullptr;
			if (usingOverride)
			{
				batch->overrideMaterial->Bind();
				UpdateTransforms(batch->overrideMaterial, glm::mat4(1), batch->viewTransform, batch->projectionTransform, false, true, true);
			}

			const auto handlesEnd = batch->handles.end();

			auto materialBegin = batch->handles.begin();
			auto materialEnd = batch->GetNextSubrange(materialBegin,
				handlesEnd,
				RenderBatch::OrderHandlesByMaterial);
			while (materialBegin != handlesEnd)
			{
				auto pinnedMaterial = materialBegin->lock();
				auto& materialInUse = usingOverride ? *batch->overrideMaterial : *pinnedMaterial->material;

				//Bind shared material
				if (usingOverride)
				{

				}
				else
				{
					materialInUse.Bind();
					//Apply non-static bind information to Material
					UpdateTransforms(&materialInUse, glm::mat4(1), batch->viewTransform, batch->projectionTransform, false, true, true);
				}
				//
				auto meshBegin = materialBegin;
				auto meshEnd = batch->GetNextSubrange(meshBegin,
					materialEnd,
					RenderBatch::OrderHandlesByMesh);

				while (meshBegin != materialEnd)
				{
					//Bind shared meshes
					//Initialize instance buffer
					//Render
					auto pinnedMesh = meshBegin->lock();
					auto& meshInUse = *pinnedMesh->mesh;
					meshInUse.Bind();

					while (meshBegin != meshEnd)
					{
						UpdateTransforms(&materialInUse, meshBegin->lock()->transform, batch->viewTransform, batch->projectionTransform, true, false, false);

						glDrawElements(GL_TRIANGLES, meshInUse.PrimitiveCount(), GL_UNSIGNED_INT, nullptr);

						++meshBegin;
					}
					//

					meshEnd = batch->GetNextSubrange(meshBegin,
						materialEnd,
						RenderBatch::OrderHandlesByMesh);
				}

				materialBegin = materialEnd;
				materialEnd = batch->GetNextSubrange(materialBegin,
					handlesEnd,
					RenderBatch::OrderHandlesByMaterial);
			}
		}

		void DrawRenderable(RenderManager*,
			RenderBatch*,
			RenderableHandle*)
		{
		}

		inline void UpdateTransforms(Material* m, glm::mat4 model, glm::mat4 view, glm::mat4 projection,
									bool updateModel, bool updateView, bool updateProject)
		{
			auto prog = m->GetProgram();

			const auto uniformEnd = prog->UniformsEnd();

			if (updateModel)
			{
				auto m_transform_h = prog->FindUniform(m_transform_id);
				auto i_m_transform_h = prog->FindUniform(i_m_transform_id);

				if (m_transform_h != uniformEnd)
				{
					m->SetUniform(*m_transform_h, model);
				}
				if (i_m_transform_h != uniformEnd)
				{
					m->SetUniform(*i_m_transform_h, glm::inverse(model));
				}
			}
			if (updateView)
			{
				auto v_transform_h = prog->FindUniform(v_transform_id);
				auto i_v_transform_h = prog->FindUniform(i_v_transform_id);

				if (v_transform_h != uniformEnd)
				{
					m->SetUniform(*v_transform_h, view);
				}
				if (i_v_transform_h != uniformEnd)
				{
					m->SetUniform(*i_v_transform_h, glm::inverse(view));
				}
			}
			if (updateModel || updateView)
			{
				auto mv_transform_h = prog->FindUniform(mv_transform_id);
				auto i_mv_transform_h = prog->FindUniform(i_mv_transform_id);

				auto mvTran = view * model;

				if (mv_transform_h != uniformEnd)
				{
					m->SetUniform(*mv_transform_h, mvTran);
				}
				if (i_mv_transform_h != uniformEnd)
				{
					m->SetUniform(*i_mv_transform_h, glm::inverse(mvTran));
				}
			}
			if (updateProject)
			{
				auto p_transform_h = prog->FindUniform(p_transform_id);
				auto i_p_transform_h = prog->FindUniform(i_p_transform_id);

				if (p_transform_h != uniformEnd)
				{
					m->SetUniform(*p_transform_h, projection);
				}
				if (i_p_transform_h != uniformEnd)
				{
					m->SetUniform(*i_p_transform_h, glm::inverse(projection));
				}
			}
			if (updateView || updateProject)
			{
				auto vp_transform_h = prog->FindUniform(vp_transform_id);
				auto i_vp_transform_h = prog->FindUniform(i_vp_transform_id);

				auto vpTran = projection * view;

				if (vp_transform_h != uniformEnd)
				{
					m->SetUniform(*vp_transform_h, vpTran);
				}
				if (i_vp_transform_h != uniformEnd)
				{
					m->SetUniform(*i_vp_transform_h, glm::inverse(vpTran));
				}
			}
			if (updateModel || updateView || updateProject)
			{
				auto mvp_transform_h = prog->FindUniform(mvp_transform_id);
				auto i_mvp_transform_h = prog->FindUniform(i_mvp_transform_id);

				auto mvp = projection * view * model;

				if (mvp_transform_h != uniformEnd)
				{
					m->SetUniform(*mvp_transform_h, mvp);
				}
				if (i_mvp_transform_h != uniformEnd)
				{
					m->SetUniform(*i_mvp_transform_h, glm::inverse(mvp));
				}
			}
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

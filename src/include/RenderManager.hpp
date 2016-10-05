#pragma once
#include "glm\fwd.hpp"
#include "LocalSharedPtr.hpp"

namespace GlProj
{
	namespace Utilities
	{
		struct Transform;
	}
	namespace Graphics
	{
		struct Camera;
		class Mesh;
		class Material;
		class RenderableHandle;
		class RenderManager;
		class RenderBatch;
		using GlProj::Utilities::Transform;

		template<typename T>
		using local_shared_ptr = Utilities::LocalSharedPtr<T>;
		template<typename T>
		using local_weak_ptr = Utilities::LocalWeakPtr<T>;

		enum class BatchType : int
		{
			PreProcess,
			Opaque,
			Transparent,
			Overlay,
			PostProcess,
		};

		RenderManager* GetRenderManager();
		local_shared_ptr<RenderBatch> GenerateRenderBatch(RenderManager*, BatchType = BatchType::Opaque,
										 int priority = 0,
										 bool groupMaterials = true,
										 bool groupMeshes = true);
		Material* SetOverrideMaterial(RenderBatch*,  Material*);

		void UpdateBatchCamera(RenderBatch*, const Camera&);
		
		local_shared_ptr<RenderableHandle> SubmitRenderable(RenderBatch*, Mesh&, Material* = nullptr);
		bool RemoveRenderable(RenderBatch*, local_shared_ptr<RenderableHandle>&&);
		Material* SetMaterial(RenderableHandle*, Material*);
		glm::mat4 SetTransform(RenderableHandle*, const glm::mat4&);

		void Draw(RenderManager*);
		void DrawBatch(RenderManager*, RenderBatch*);
		void DrawRenderable(RenderManager*, RenderBatch*, RenderableHandle*);
	}
}
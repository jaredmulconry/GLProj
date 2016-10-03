#pragma once
#include <LocalSharedPtr.hpp>

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

		enum class BatchType : int
		{
			Opaque,
			Translucent,
			Cutout,
			Overlay,
			PreProcess,
			PostProcess,
		};

		RenderManager* GetRenderManager();
		local_shared_ptr<RenderBatch> GenerateRenderBatch(RenderManager*, BatchType,
										 int priority = 0,
										 bool groupMaterials = true,
										 bool groupMeshes = true);
		Material* SetOverrideMaterial(RenderManager*, RenderBatch*,  Material*);

		void UpdateBatchCamera(RenderManager*, RenderBatch*, const Camera&);
		
		local_shared_ptr<RenderableHandle> SubmitRenderable(RenderManager*, RenderBatch*, Mesh&, Material* = nullptr);
		Material* SetMaterial(RenderManager*, RenderBatch*, RenderableHandle*, Material*);
		Transform SetTransform(RenderManager*, RenderBatch*, RenderableHandle*, const Transform&);
		bool RemoveRenderable(RenderManager*, RenderBatch*, RenderableHandle*);

		void DrawBatch(RenderManager*, RenderBatch*);
	}
}
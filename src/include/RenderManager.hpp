#pragma once

namespace GlProj
{
	namespace Graphics
	{
		struct Camera;
		struct RenderableHandle;
		class RenderManager;
		struct RenderBatch;
		struct Transform;

		RenderManager* GetRenderManager();
		RenderBatch* GenerateRenderBatch();

		void UpdateBatchCamera(RenderBatch*, const Camera&);
		

		void SubmitRenderable();
		void DrawBatch(RenderBatch*);
	}
}
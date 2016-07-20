#pragma once
#include <memory>
#include <string>

struct aiMesh;

namespace GlProj
{
	namespace Graphics
	{
		class MeshManager;
		class Mesh;
		
		MeshManager* GetMeshManager();

		std::shared_ptr<Mesh> RegisterMesh(MeshManager*, aiMesh*, const std::string&, bool = false);

		std::shared_ptr<Mesh> FindCachedMeshByName(const MeshManager*, const std::string&);

		void ReleaseUnused(MeshManager*);
	}
}
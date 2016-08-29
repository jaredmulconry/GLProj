#pragma once
#include "LocalSharedPtr.hpp"
#include <string>

struct aiMesh;

namespace GlProj
{
	namespace Graphics
	{
		class MeshManager;
		class Mesh;

		using GlProj::Utilities::LocalSharedPtr;
		
		MeshManager* GetMeshManager();

		LocalSharedPtr<Mesh> RegisterMesh(MeshManager*, aiMesh*, const std::string&, bool = false);

		LocalSharedPtr<Mesh> FindCachedMeshByName(const MeshManager*, const std::string&);

		void ReleaseUnused(MeshManager*);
	}
}
#include "MeshManager.hpp"
#include "gl_core_4_5.h"
#include "GLFW/glfw3.h"
#include "Mesh.hpp"
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace GlProj
{
	namespace Graphics
	{
		using GlProj::Utilities::LocalWeakPtr;
		using GlProj::Utilities::make_localshared;

		class MeshManager
		{
			std::unordered_map<std::string, LocalWeakPtr<Mesh>> registeredMeshes;
		public:
			LocalSharedPtr<Mesh> RegisterMesh(aiMesh* mesh, const std::string& name)
			{
				auto newPtr = make_localshared<Mesh>(mesh);
				auto inserted = registeredMeshes.insert({ name, newPtr });
				if (!inserted.second)
				{
					inserted.first->second = newPtr;
				}
				return std::move(newPtr);
			}
			LocalSharedPtr<Mesh> FindByName(const std::string& name) const
			{
				auto found = registeredMeshes.find(name);
				if(found == registeredMeshes.end())
				{
					return nullptr;
				}

				return found->second.lock();
			}

			void CleanUpDangling()
			{
				auto begin = registeredMeshes.begin();

				while(begin != registeredMeshes.end())
				{
					if(!begin->second.expired())
					{
						++begin;
						continue;
					}
					begin = registeredMeshes.erase(begin);
				}
			}
		};

		MeshManager* GetMeshManager()
		{
			static MeshManager instance;
			return &instance;
		}
		LocalSharedPtr<Mesh> RegisterMesh(MeshManager* manager, aiMesh* mesh, const std::string& name, bool replace)
		{
			if(!replace)
			{
				auto ptr = FindCachedMeshByName(manager, name);
				if(ptr != nullptr)
				{
					return std::move(ptr);
				}
			}

			return manager->RegisterMesh(mesh, name);
		}
		LocalSharedPtr<Mesh> FindCachedMeshByName(const MeshManager* manager, const std::string& name)
		{
			return manager->FindByName(name);
		}
		void ReleaseUnused(MeshManager* manager)
		{
			manager->CleanUpDangling();
		}
	}
}
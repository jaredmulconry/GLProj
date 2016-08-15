#include "MeshManager.hpp"
#include "gl_core_4_1.h"
#include "GLFW\glfw3.h"
#include "Mesh.hpp"
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace GlProj
{
	namespace Graphics
	{
		class MeshManager
		{
			std::unordered_map<std::string, std::weak_ptr<Mesh>> registeredMeshes;
		public:
			std::shared_ptr<Mesh> RegisterMesh(aiMesh* mesh, const std::string& name)
			{
				auto newPtr = std::make_shared<Mesh>(mesh);
				registeredMeshes.insert_or_assign(name, newPtr);
				return std::move(newPtr);
			}
			std::shared_ptr<Mesh> FindByName(const std::string& name) const
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
		std::shared_ptr<Mesh> RegisterMesh(MeshManager* manager, aiMesh* mesh, const std::string& name, bool replace)
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
		std::shared_ptr<Mesh> FindCachedMeshByName(const MeshManager* manager, const std::string& name)
		{
			return manager->FindByName(name);
		}
		void ReleaseUnused(MeshManager* manager)
		{
			manager->CleanUpDangling();
		}
	}
}
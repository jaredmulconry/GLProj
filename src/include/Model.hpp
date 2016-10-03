#pragma once
#include "glm/mat4x4.hpp"
#include "LocalSharedPtr.hpp"
#include "SceneGraph.hpp"
#include <string>
#include <utility>
#include <vector>

namespace GlProj
{
	namespace Graphics
	{
		struct Camera;
		class Mesh;
		class Material;
		using GlProj::Utilities::LocalSharedPtr;
		using GlProj::Utilities::SceneGraph;

		struct ModelData
		{
			glm::mat4 transform;
			std::vector<unsigned int> meshes;
			std::string name;
		};
		inline bool operator==(const ModelData& x, const ModelData& y) noexcept
		{
			return x.transform == y.transform 
				&& x.meshes == y.meshes 
				&& x.name == y.name;
		}
		inline bool operator!=(const ModelData& x, const ModelData& y) noexcept
		{
			return !(x == y);
		}

		struct Renderable
		{
			LocalSharedPtr<Mesh> mesh;
			LocalSharedPtr<Material> material;
		};

		class Model
		{
			std::vector<Renderable> submeshes;
			SceneGraph<ModelData> hierarchy;
			using hierarchy_node = SceneGraph<ModelData>::node_type;

		public:
			Model() = default;
			Model(const std::vector<Renderable>&, SceneGraph<ModelData>&&);

			const SceneGraph<ModelData>& GetHierarchy() const
			{
				return hierarchy;
			}

		};
	}
}

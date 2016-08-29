#pragma once
#include "glm\fwd.hpp"
#include "LocalSharedPtr.hpp"
#include "Material.hpp"
#include <string>
#include <utility>
#include <vector>

struct aiScene;

namespace GlProj
{
	namespace Graphics
	{
		class Mesh;
		using GlProj::Utilities::LocalSharedPtr;

		struct Renderable
		{
			LocalSharedPtr<Mesh> mesh;
			LocalSharedPtr<Material> material;
		};

		class Model
		{
			std::vector<Renderable> submeshes;
		public:
			Model() = default;
			Model(const std::vector<Renderable>&);

			void SetMatrix(const UniformInformation& uniform, const glm::mat4& matrix);

			void Draw();

			void DrawNoBind();
		};
	}
}
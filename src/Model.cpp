#include "Model.hpp"

namespace GlProj
{
	namespace Graphics
	{
		Model::Model(const std::vector<Renderable>& renderables)
			:submeshes(renderables)
		{}
	}
}
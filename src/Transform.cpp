#include "Transform.hpp"
#include "glm\matrix.hpp"
#include "glm\gtc\matrix_transform.hpp"

namespace GlProj
{
	namespace Utilities
	{
		glm::mat4 ToMatrix(const Transform& t) noexcept
		{
			auto result = glm::mat4(1.0f);
			result = glm::scale(result, t.scale);
			result *= glm::mat4_cast(t.rotation);
			result = glm::translate(result, t.position);
			return result;
		}
	}
}

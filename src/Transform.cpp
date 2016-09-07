#include "Transform.hpp"
#include "glm\matrix.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

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
		Transform FromMatrix(const glm::mat4& m) noexcept
		{
			Transform result;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(m, result.scale, result.rotation, result.position, skew, perspective);
			return result;
		}
	}
}

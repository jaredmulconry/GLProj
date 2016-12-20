#include "Transform.hpp"
#include "glm/matrix.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace GlProj
{
	namespace Utilities
	{
		Transform Identity() noexcept
		{
			return{ {0.f, 0.f, 0.f}, {}, {1.f, 1.f, 1.f } };
		}
		glm::mat4 ToMatrix(const Transform& t) noexcept
		{
			auto result = glm::mat4(1.0f);
			result *= glm::scale(glm::mat4(1), t.scale);
			result *= glm::mat4_cast(t.rotation);
			result *= glm::translate(glm::mat4(1), t.position);
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

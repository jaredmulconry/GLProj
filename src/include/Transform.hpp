#pragma once
#include "glm/fwd.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"

namespace GlProj
{
	namespace Utilities
	{
		struct Transform
		{
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
		};

		glm::mat4 ToMatrix(const Transform&) noexcept;
		Transform FromMatrix(const glm::mat4&) noexcept;
	}
}
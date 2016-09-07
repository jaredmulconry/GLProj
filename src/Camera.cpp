#include "Camera.hpp"
#include "glm\matrix.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <exception>

using namespace GlProj::Utilities;

namespace GlProj
{
	namespace Graphics
	{
		glm::mat4 Camera::View() const noexcept
		{
			return ToMatrix(transform);
		}
		glm::mat4 Camera::Projection() const noexcept
		{
			switch (type)
			{
			default:
				std::terminate();
				return glm::mat4(1.0f);
			case CameraType::Perspective:
				return glm::perspective(data.perspective.fov, data.perspective.aspect, nearPlane, farPlane);
			case CameraType::Orthographic:
				return glm::ortho(-data.orthographic.size.x / 2.0f,
					data.orthographic.size.x / 2.0f,
					-data.orthographic.size.y / 2.0f,
					data.orthographic.size.y / 2.0f,
					nearPlane, farPlane);
			}
		}
		glm::mat4 Camera::ViewProjection() const noexcept
		{
			return Projection() * View();
		}
	}
}
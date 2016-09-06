#pragma once
#include "Transform.hpp"
#include "glm/fwd.hpp"
#include "glm/vec2.hpp"

namespace GlProj
{
	namespace Graphics
	{
		enum class CameraType : short
		{
			Perspective,
			Orthographic,
			NotSet,
		};

		struct Camera
		{
			struct Orthographic
			{
				glm::vec2 size;
			};
			struct Perspective
			{
				float fov;
				float aspect;
			};

			struct CommonAspect
			{
				static const constexpr float aspect_21_9 = 21.0f / 9.0f;
				static const constexpr float aspect_256_135 = 256.0f / 135.0f;
				static const constexpr float aspect_16_9 = 16.0f / 9.0f;
				static const constexpr float aspect_16_10 = 16.0f / 10.0f;
				static const constexpr float aspect_4_3 = 4.0f / 3.0f;
			};

			using transform_type = Utilities::Transform;

			~Camera() = default;

			glm::mat4 View() const noexcept;
			glm::mat4 Projection() const noexcept;
			glm::mat4 ViewProjection() const noexcept;

			transform_type transform;
		private:
			union CameraData
			{
				Orthographic orthographic;
				Perspective perspective;
			} data;
			float near;
			float far;
			CameraType type;
		};
	}
}

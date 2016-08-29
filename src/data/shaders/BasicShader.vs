#version 430

layout(location=0) in vec3 position;

layout(location=0) uniform mat4 mvp_transform;

out vec4 f_position;

void main()
{
	gl_Position = mvp_transform * vec4(position, 1);
}
#version 430

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

layout(location=0) uniform mat4 mvp_transform;

out vec4 f_position;
out vec4 f_normal;

void main()
{
	f_position = mvp_transform * vec4(position, 1);
	f_normal = mvp_transform * vec4(normal, 0);
	gl_Position = f_position;
}
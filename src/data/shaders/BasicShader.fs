#version 410

in vec4 f_position;

out vec4 f_colour;

void main()
{
	f_colour = vec4(f_position.xyz, 1.0);
}
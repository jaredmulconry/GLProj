#version 430

in vec4 f_position;
in vec4 f_normal;

out vec4 f_out;

void main()
{
	f_out = vec4(f_normal.xyz, 1.0);
}
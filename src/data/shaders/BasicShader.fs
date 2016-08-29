#version 430

in vec4 f_position;

uniform sampler2D magicalTexture;

out vec4 f_colour;

void main()
{
	f_colour = texture(magicalTexture, f_position.xy);
	//f_colour = vec4(f_position.xyz, 1.0);
}
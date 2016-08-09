#version 410

layout(location=0) in vec3 position;



void main()
{
	gl_Position = position;
}
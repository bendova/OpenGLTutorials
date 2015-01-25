#version 330

in vec4 fragColor;
out vec4 color;

uniform vec4 baseColor;

void main()
{
	color = fragColor;
}
#version 330

smooth in vec4 interpColor;
out vec4 color;

void main()
{
	color = interpColor;
}
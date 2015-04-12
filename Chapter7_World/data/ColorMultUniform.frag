#version 330

smooth in vec4 fragColor;
uniform vec4 baseColor;

out vec4 outputColor;

void main()
{
	outputColor = fragColor * baseColor;
}

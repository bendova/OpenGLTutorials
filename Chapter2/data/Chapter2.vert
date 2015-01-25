#version 330

layout(location = 0) in vec4 position;

uniform vec2 positionOffset;

void main()
{
	gl_Position = position + vec4(positionOffset.x, positionOffset.y, 0.0, 0.0);
}
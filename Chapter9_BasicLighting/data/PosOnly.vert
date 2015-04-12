#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform mat4 mvpTransform;
out vec4 fragColor;

void main()
{
	vec4 clipPosition = mvpTransform * position;
	gl_Position = clipPosition;
	fragColor = color;
}
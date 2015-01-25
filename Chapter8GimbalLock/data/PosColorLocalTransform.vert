#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

out vec4 fragColor;

uniform mat4 modelToCameraTransform; 
uniform mat4 cameraToClipTransform;

void main()
{
	vec4 cameraPosition = modelToCameraTransform * position;
	gl_Position = cameraToClipTransform * cameraPosition;
	fragColor = color;
}
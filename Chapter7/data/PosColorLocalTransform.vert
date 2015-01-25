#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

out vec4 fragColor;

uniform mat4 modelToWorldTransform;
uniform mat4 worldToCameraTransform;
uniform mat4 cameraToClipTransform;

void main()
{
	vec4 worldPosition = modelToWorldTransform * position;
	vec4 cameraPosition = worldToCameraTransform * worldPosition;
	gl_Position = cameraToClipTransform * cameraPosition;
	fragColor = color;
}
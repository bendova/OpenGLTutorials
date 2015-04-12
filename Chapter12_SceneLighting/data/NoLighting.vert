#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 normal;

out vec4 fragColor;

uniform mat4 modelToCameraTransform;
uniform mat4 cameraToClipTransform;

void main()
{
	gl_Position = cameraToClipTransform * modelToCameraTransform * vec4(position, 1.0);
	fragColor = color;
}
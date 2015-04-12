#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 normal;

out vec4 fragPositionCS;
out vec4 fragColor;
out vec4 fragNormalCS;

uniform mat4 modelToCameraTransform;
uniform mat4 cameraToClipTransform;

void main()
{
	vec4 positionInCameraSpace = modelToCameraTransform * vec4(position, 1.0);
	gl_Position = cameraToClipTransform * positionInCameraSpace;

	fragPositionCS = positionInCameraSpace;
	fragNormalCS = modelToCameraTransform * vec4(normal, 0.0);
	fragColor = color;
}
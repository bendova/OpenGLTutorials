#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

smooth out vec4 fragColor;

uniform mat4 modelToWorldMatrix;

layout(std140) uniform GlobalMatrices
{
	mat4 worldToCameraMatrix;
	mat4 cameraToClipMatrix;
};

void main()
{
	vec4 worldPosition = modelToWorldMatrix * position;
	vec4 cameraPosition = worldToCameraMatrix * worldPosition;
	gl_Position = cameraToClipMatrix * cameraPosition;
	fragColor = color;
}
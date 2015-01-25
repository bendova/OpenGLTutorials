#version 330

layout(location = 0) in vec4 position;

uniform mat4 modelToWorldMatrix;

layout(std140) uniform GlobalMatrices
{
	mat4 worldToCameraMatrix;
	mat4 cameraToClipMatrix;
};

void main()
{
	vec4 temp = modelToWorldMatrix * position;
	temp = worldToCameraMatrix * temp;
	gl_Position = cameraToClipMatrix * temp;
}

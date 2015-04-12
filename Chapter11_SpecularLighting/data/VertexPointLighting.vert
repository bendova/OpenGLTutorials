#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 normal;

smooth out vec4 fragColor;

uniform mat4 modelToCameraTransform;
uniform mat4 cameraToClipTransform;

uniform vec4 lightPosition;
uniform vec4 lightIntensity;
uniform vec4 ambientLightIntensity;

void main()
{
	vec4 positionInCameraSpace = modelToCameraTransform * vec4(position, 1.0);
	gl_Position = cameraToClipTransform * positionInCameraSpace;

	vec4 dirToLight = normalize(lightPosition - positionInCameraSpace);
	vec4 normalInCameraSpace = normalize(modelToCameraTransform * vec4(normal, 0.0));
	float cosAngleOfIncidence = dot(normalInCameraSpace, dirToLight);
	cosAngleOfIncidence = clamp(cosAngleOfIncidence, 0, 1);

	fragColor = (cosAngleOfIncidence * color * lightIntensity) + (color * ambientLightIntensity);
}
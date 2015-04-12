#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 normal;

smooth out vec4 interpColor;

uniform vec4 directionToLight;
uniform vec4 ambientLightIntensity;
uniform vec4 lightIntensity;

uniform mat4 modelToCameraMatrix;
uniform mat4 cameraToClipMatrix;

void main()
{
	gl_Position = cameraToClipMatrix * modelToCameraMatrix * position;

	// We use 0.0 for w component of the normal vector
	// because the normal does need the translation transform
	vec4 normalAsVec4 = vec4(normal, 0.0);
	vec4 normalCameraSpace = normalize(modelToCameraMatrix * normalAsVec4);
	float cosAngleOfIncidence = dot(normalCameraSpace, directionToLight);
	cosAngleOfIncidence = clamp(cosAngleOfIncidence, 0, 1);

	interpColor = (lightIntensity * color * cosAngleOfIncidence) + (color * ambientLightIntensity);
}
#version 330

in vec4 fragPositionCS;
in vec4 fragColor;
in vec4 fragNormalCS;

out vec4 outColor;

uniform vec4 cubeLightPosition;
uniform vec4 cubeLightIntensity;

uniform vec4 directionToSun;
uniform vec4 sunLightIntensity;

uniform vec4 ambientLightIntensity;

vec4 computeAmbientLightIntensity();
vec4 computeSunLightIntensity();
vec4 computeCubeLightIntensity();
float GetLightAttenuationFactor(in vec4 vectorToLight);

void main()
{
	vec4 ambientLight = computeAmbientLightIntensity();
	vec4 sunLight = computeSunLightIntensity();
	vec4 cubeLight = computeCubeLightIntensity();
	outColor = ambientLight + sunLight + cubeLight;
}

vec4 computeAmbientLightIntensity()
{
	return (ambientLightIntensity * fragColor);
}

vec4 computeSunLightIntensity()
{
	float cosAngleOfIncidence = dot(directionToSun, normalize(fragNormalCS));
	cosAngleOfIncidence = clamp(cosAngleOfIncidence, 0, 1);
	return (sunLightIntensity * fragColor * cosAngleOfIncidence);
}

vec4 computeCubeLightIntensity()
{
	vec4 vectorToLight = cubeLightPosition - fragPositionCS;
	vec4 dirToLight = normalize(vectorToLight);
	vec4 surfaceNormalCS = normalize(fragNormalCS);
	float cosAngleOfIncidence = dot(surfaceNormalCS, dirToLight);
	cosAngleOfIncidence = clamp(cosAngleOfIncidence, 0, 1);
	float lightAttenuation = GetLightAttenuationFactor(vectorToLight);
	
	return (cosAngleOfIncidence * fragColor * cubeLightIntensity * lightAttenuation);
}

float GetLightAttenuationFactor(in vec4 vectorToLight)
{
	float distanceToLight = sqrt(dot(vectorToLight, vectorToLight));
	float attenuation = (1.0 / (1.0 + 0.5 * distanceToLight));
	return attenuation;
}
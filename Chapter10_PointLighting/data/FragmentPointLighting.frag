#version 330

in vec4 fragColor;
in vec4 fragPositionCS; // CS stands for camera-space
in vec4 fragNormalCS;

out vec4 outColor;

uniform vec4 lightPosition;
uniform vec4 lightIntensity;
uniform vec4 ambientLightIntensity;

float GetLightAttenuationFactor(in vec4 vectorToLight)
{
	float distanceToLight = sqrt(dot(vectorToLight, vectorToLight));
	float attenuation = (1.0 / (1.0 + 0.5 * distanceToLight));
	return attenuation;
}

void main()
{
	vec4 vectorToLight = lightPosition - fragPositionCS;
	vec4 dirToLight = normalize(vectorToLight);
	float cosAngleOfIncidence = dot(normalize(fragNormalCS), dirToLight);
	cosAngleOfIncidence = clamp(cosAngleOfIncidence, 0, 1);

	float lightAttenuation = GetLightAttenuationFactor(vectorToLight);
	outColor = (cosAngleOfIncidence * fragColor * lightIntensity * lightAttenuation) + 
				(fragColor * ambientLightIntensity);
}
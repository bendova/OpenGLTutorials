#version 330

in vec4 fragColor;
in vec4 fragPositionCS; // CS stands for camera-space
in vec4 fragNormalCS;

out vec4 outColor;

uniform vec4 lightPosition;
uniform vec4 lightIntensity;
uniform vec4 ambientLightIntensity;

const float shinessFactor = 10;
const vec4 specularColor = vec4(0.25, 0.25, 0.25, 1.0);

float GetLightAttenuationFactor(in vec4 vectorToLight)
{
	float distanceToLight = sqrt(dot(vectorToLight, vectorToLight));
	float attenuation = (1.0 / (1.0 + 0.5 * distanceToLight));
	return attenuation;
}

float GetPhongTerm(in vec4 dirToLight, in vec4 surfaceNormal, in float cosAngleOfIncidence)
{
	vec4 viewDirection = normalize(-fragPositionCS);
	vec4 reflectDirection = reflect(-dirToLight, surfaceNormal);
	float phongTerm = dot(viewDirection, reflectDirection);
	phongTerm = clamp(phongTerm, 0, 1);
	phongTerm = (cosAngleOfIncidence != 0.0) ? phongTerm : 0.0;
	phongTerm = pow(phongTerm, shinessFactor);
	return phongTerm;
}

float GetBlinnTerm(in vec4 dirToLight, in vec4 surfaceNormal, in float cosAngleOfIncidence)
{
	vec4 viewDirection = normalize(-fragPositionCS);
	vec4 halfVector = normalize(viewDirection + dirToLight);
	float blinnTerm = dot(surfaceNormal, halfVector);
	blinnTerm = clamp(blinnTerm, 0, 1);
	blinnTerm = (cosAngleOfIncidence != 0.0) ? blinnTerm : 0.0;
	blinnTerm = pow(blinnTerm, shinessFactor);
	return blinnTerm;
}

void main()
{
	vec4 vectorToLight = lightPosition - fragPositionCS;
	vec4 dirToLight = normalize(vectorToLight);
	vec4 surfaceNormalCS = normalize(fragNormalCS);
	float cosAngleOfIncidence = dot(surfaceNormalCS, dirToLight);
	cosAngleOfIncidence = clamp(cosAngleOfIncidence, 0, 1);

	float lightAttenuation = GetLightAttenuationFactor(vectorToLight);
	//float specularLightFactor = GetPhongTerm(dirToLight, surfaceNormalCS, cosAngleOfIncidence);
	float specularLightFactor = GetBlinnTerm(dirToLight, surfaceNormalCS, cosAngleOfIncidence);
	outColor = (cosAngleOfIncidence * fragColor * lightIntensity * lightAttenuation) +
			(specularColor * lightAttenuation * specularLightFactor) + 
				(fragColor * ambientLightIntensity);
}
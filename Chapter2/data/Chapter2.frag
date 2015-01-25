#version 330

out vec4 outputColor;

uniform float elapsedTime;
uniform float loopDuration;

const vec4 firstColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
const vec4 secondColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);

void main()
{
	float currentTime = mod(elapsedTime, loopDuration);
	float linearInterpolation = cos((2 * 3.14159f / loopDuration) * currentTime);

	outputColor = mix(firstColor, secondColor, linearInterpolation);
}
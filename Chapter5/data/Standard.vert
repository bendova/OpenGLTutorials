#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

smooth out vec4 fragColor;

uniform vec3 offset;
uniform mat4 perspectiveMatrix;

void main()
{
	vec4 offsetedPosition = position + vec4(offset.x, offset.y, offset.z, 0.0f);
	gl_Position = perspectiveMatrix * position;
	fragColor = color;
}

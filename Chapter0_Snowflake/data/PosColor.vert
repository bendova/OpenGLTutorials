#version 330

layout(location = 0) in vec3 position;

out vec4 fragColor;

uniform mat4 modelToCameraTransform;
uniform mat4 cameraToClipTransform;

void main()
{
	gl_Position = cameraToClipTransform * modelToCameraTransform * vec4(position, 1.0);
	fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
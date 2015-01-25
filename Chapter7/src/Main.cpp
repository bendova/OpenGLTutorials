/* 
* My implementation of chapter 7 from http://www.arcsynthesis.org/gltut/Positioning/Tut06%20Fun%20with%20Matrices.html
*/

#include <vector>
#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

const unsigned VERTEX_COMPONENT_COUNT = 3;
const unsigned COLOR_COMPONENT_COUNT = 4;

const float LEFT_EXTENT = -1.0f;
const float RIGHT_EXTENT = -LEFT_EXTENT;
const float TOP_EXTENT = 1.0f;
const float BOTTOM_EXTENT = -TOP_EXTENT;
const float FRONT_EXTENT = 1.0f;
const float BACK_EXTENT = -FRONT_EXTENT;

#define LEFT_TOP_FRONT_CORNER LEFT_EXTENT, TOP_EXTENT, FRONT_EXTENT
#define LEFT_BOTTOM_FRONT_CORNER LEFT_EXTENT, BOTTOM_EXTENT, FRONT_EXTENT
#define RIGHT_TOP_FRONT_CORNER RIGHT_EXTENT, TOP_EXTENT, FRONT_EXTENT
#define RIGHT_BOTTOM_FRONT_CORNER RIGHT_EXTENT, BOTTOM_EXTENT, FRONT_EXTENT
#define LEFT_TOP_BACK_CORNER LEFT_EXTENT, TOP_EXTENT, BACK_EXTENT
#define LEFT_BOTTOM_BACK_CORNER LEFT_EXTENT, BOTTOM_EXTENT, BACK_EXTENT
#define RIGHT_TOP_BACK_CORNER RIGHT_EXTENT, TOP_EXTENT, BACK_EXTENT
#define RIGHT_BOTTOM_BACK_CORNER RIGHT_EXTENT, BOTTOM_EXTENT, BACK_EXTENT

#define RED 1.0f, 0.0f, 0.0f, 1.0f
#define GREEN 0.0f, 1.0f, 0.0f, 1.0f
#define BLUE 0.0f, 0.0f, 1.0f, 1.0f
#define YELLOW 1.0f, 1.0f, 0.0f, 1.0f
#define MAGENTA 1.0f, 0.0f, 1.0f, 1.0f
#define CYAN 0.0f, 1.0f, 1.0f, 1.0f

const unsigned gVertexCount = 24;
const float gVertexData[] = 
{
	// front face
	LEFT_TOP_FRONT_CORNER,
	RIGHT_TOP_FRONT_CORNER,
	RIGHT_BOTTOM_FRONT_CORNER,
	LEFT_BOTTOM_FRONT_CORNER,

	// back face
	RIGHT_TOP_BACK_CORNER, 
	LEFT_TOP_BACK_CORNER, 
	LEFT_BOTTOM_BACK_CORNER,
	RIGHT_BOTTOM_BACK_CORNER,

	// left face
	LEFT_TOP_BACK_CORNER,
	LEFT_TOP_FRONT_CORNER,
	LEFT_BOTTOM_FRONT_CORNER,
	LEFT_BOTTOM_BACK_CORNER,

	// right face
	RIGHT_TOP_FRONT_CORNER,
	RIGHT_TOP_BACK_CORNER,
	RIGHT_BOTTOM_BACK_CORNER,
	RIGHT_BOTTOM_FRONT_CORNER,

	// top face
	LEFT_TOP_BACK_CORNER,
	RIGHT_TOP_BACK_CORNER,
	RIGHT_TOP_FRONT_CORNER,
	LEFT_TOP_FRONT_CORNER,

	// bottom face
	RIGHT_BOTTOM_BACK_CORNER,
	LEFT_BOTTOM_BACK_CORNER,
	LEFT_BOTTOM_FRONT_CORNER,
	RIGHT_BOTTOM_FRONT_CORNER,

	// front face colors
	RED, RED, RED, RED,
	// back face colors
	GREEN, GREEN, GREEN, GREEN,
	// left face colors
	BLUE, BLUE, BLUE, BLUE,
	// right face colors
	YELLOW, YELLOW, YELLOW, YELLOW,
	// top face colors
	MAGENTA, MAGENTA, MAGENTA, MAGENTA,
	// bottom face colors
	CYAN, CYAN, CYAN, CYAN,
};

const GLshort gIndexData[] =
{
	// front face
	0, 1, 2,
	2, 3, 0,

	// back face
	4, 5, 6,
	6, 7, 4,

	// left face
	8, 9, 10,
	10, 11, 8,

	// right face
	12, 13, 14,
	14, 15, 12,

	// top face
	16, 17, 18,
	18, 19, 16,

	// bottom face
	20, 21, 22,
	22, 23, 20
};

GLuint gProgramID;
GLuint gVertexBufferID;
GLuint gIndexBufferID;
GLuint gVertexArrayObjectID;

GLint gVertexPositionAttribute;
GLint gVertexColorAttribute;
GLint gModelToWorldUniform;
GLint gWorldToCameraUniform;
GLint gCameraToClipUniform;

glm::mat4 gCameraToClipTransform(1.0f);
float gFrustumScale = 1.0f;


const float CAMERA_POSITION_INCREMENT = 0.2f;
const float CAMERA_ROTATION_INCREMENT = 2.0f;
glm::vec3 gCameraPosition(0.0f, 0.0f, 0.0f);
float gCameraRotationX = 0.0f;
float gCameraRotationY = 0.0f;
float gCameraRotationZ = 0.0f;

glm::vec3 gCameraSphereRelativePosition(0.0f, -45.0f, 1.0f);
glm::vec3 gCameraLookAtTarget(0.0f, 0.0f, -1.0f);

void InitializeProgram()
{
	std::vector<GLuint> shaders;

	shaders.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "PosColorLocalTransform.vert"));
	shaders.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "ColorPassthrough.frag"));

	gProgramID = Framework::CreateProgram(shaders);
	
	gVertexPositionAttribute = glGetAttribLocation(gProgramID, "position");
	gVertexColorAttribute = glGetAttribLocation(gProgramID, "color"); 
	gModelToWorldUniform = glGetUniformLocation(gProgramID, "modelToWorldTransform");
	gWorldToCameraUniform = glGetUniformLocation(gProgramID, "worldToCameraTransform");
	gCameraToClipUniform = glGetUniformLocation(gProgramID, "cameraToClipTransform");
}

void InitializeVertexBuffer()
{
	glGenBuffers(1, &gVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexData), gVertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
}

void InitializeIndexBuffer()
{
	glGenBuffers(1, &gIndexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gIndexData), gIndexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
}

void InitializeVertexArrayObject()
{
	glGenVertexArrays(1, &gVertexArrayObjectID);
	glBindVertexArray(gVertexArrayObjectID);
	
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferID);
	glEnableVertexAttribArray(gVertexPositionAttribute);
	glVertexAttribPointer(gVertexPositionAttribute, VERTEX_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(gVertexColorAttribute);

	const unsigned colorOffset = gVertexCount * VERTEX_COMPONENT_COUNT * sizeof(gVertexData[0]);
	glVertexAttribPointer(gVertexColorAttribute, COLOR_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, (void*)colorOffset);

	glBindVertexArray(GL_NONE);
}

void UpdateCameraToClipUniform()
{
	glUseProgram(gProgramID);
	glUniformMatrix4fv(gCameraToClipUniform, 1, GL_FALSE, glm::value_ptr(gCameraToClipTransform));
	glUseProgram(GL_NONE);
}

float GetAngleInRadians(const float angleInDegrees)
{
	const float PI = 3.14159f;
	return angleInDegrees * PI / 180.0f;
}

float GetFrustumScale(const float angleInDegrees)
{
	const float halfAngleInRadians = GetAngleInRadians(angleInDegrees) / 2.0f;
	return 1 / tan(halfAngleInRadians / 2.0f);
}

void InitializeCameraToClipTransform()
{
	gFrustumScale = GetFrustumScale(45.0f);

	const float zNear = 1.0f;
	const float zFar = 1000.0f;
	gCameraToClipTransform[0].x = gFrustumScale;
	gCameraToClipTransform[1].y = gFrustumScale;
	gCameraToClipTransform[2].z = (zNear + zFar) / (zNear - zFar);
	gCameraToClipTransform[2].w = -1.0f;
	gCameraToClipTransform[3].z = (2 * zNear * zFar) / (zNear - zFar);

	UpdateCameraToClipUniform();
}

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();
	InitializeVertexBuffer();
	InitializeIndexBuffer();
	InitializeVertexArrayObject();
	InitializeCameraToClipTransform();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
}

glm::mat4 GetRotationX(const float angleInDegrees)
{
	const float angleInRadians = GetAngleInRadians(angleInDegrees);
	const float sinAngle = sin(angleInRadians);
	const float cosAngle = cos(angleInRadians);
	
	glm::mat4 rotationX(1.0f);
	rotationX[1].y = cosAngle;
	rotationX[1].z = sinAngle;
	rotationX[2].y = -sinAngle;
	rotationX[2].z = cosAngle;

	return rotationX;
}

glm::mat4 GetRotationY(const float angleInDegrees)
{
	const float angleInRadians = GetAngleInRadians(angleInDegrees);
	const float sinAngle = sin(angleInRadians);
	const float cosAngle = cos(angleInRadians);

	glm::mat4 rotationY(1.0f);
	rotationY[0].x = cosAngle;
	rotationY[0].z = -sinAngle;
	rotationY[2].x = sinAngle;
	rotationY[2].z = cosAngle;

	return rotationY;
}

glm::mat4 GetRotationZ(const float angleInDegrees)
{
	const float angleInRadians = GetAngleInRadians(angleInDegrees);
	const float sinAngle = sin(angleInRadians);
	const float cosAngle = cos(angleInRadians);

	glm::mat4 rotationZ(1.0f);
	rotationZ[0].x = cosAngle;
	rotationZ[0].y = sinAngle;
	rotationZ[1].x = -sinAngle;
	rotationZ[1].y = cosAngle;

	return rotationZ;
}

glm::vec3 ResolveCameraPosition()
{
	float phi = GetAngleInRadians(gCameraSphereRelativePosition.x);
	float theta = GetAngleInRadians(gCameraSphereRelativePosition.y + 90.0f);

	float sinPhi = sin(phi);
	float cosPhi = cos(phi);
	float sinTheta = sin(theta);
	float cosTheta = cos(theta);

	glm::vec3 cameraDirection(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
	return (cameraDirection * gCameraSphereRelativePosition.z) + gCameraLookAtTarget;
}

void UpdateModelToWorldTransform()
{
	glm::mat4 translationMatrix(1.0f);
	translationMatrix[3].z = -50.0f;

	glm::mat4 xRotationMatrix = GetRotationX(30.0f);
	glm::mat4 yRotationMatrix = GetRotationY(30.0f);
	glm::mat4 modelToCameraUniform = translationMatrix;// *xRotationMatrix * yRotationMatrix;

	glUniformMatrix4fv(gModelToWorldUniform, 1, GL_FALSE, glm::value_ptr(modelToCameraUniform));
}

void UpdateCameraToWorldUniform(const glm::mat4& worldToCamera)
{
	glUniformMatrix4fv(gWorldToCameraUniform, 1, GL_FALSE, glm::value_ptr(worldToCamera));
}

void UpdateFPSWorldToCameraTransform()
{
	glm::mat4 cameraTranslation(1.0f);
	cameraTranslation[3] = glm::vec4(-gCameraPosition, 1.0f);
	glm::mat4 cameraRotation = GetRotationX(-gCameraRotationX) * GetRotationY(-gCameraRotationY) * GetRotationZ(-gCameraRotationZ);

	UpdateCameraToWorldUniform(cameraRotation * cameraTranslation);
}

void UpdateLookAtWorldToCameraTransform(const glm::vec3& cameraPosition, const glm::vec3& cameraTarget, const glm::vec3& up)
{
	const glm::vec3 forwardVector = glm::normalize(cameraTarget - cameraPosition);
	const glm::vec3 rightVector = glm::normalize(glm::cross(up, forwardVector));
	const glm::vec3 upVector = glm::cross(forwardVector, rightVector);
	
	glm::mat4 rotation(1.0f);
	rotation[0] = glm::vec4(rightVector, 0.0f);
	rotation[1] = glm::vec4(upVector, 0.0f);
	rotation[2] = glm::vec4(-forwardVector, 0.0f);

	rotation = glm::transpose(rotation);

	glm::mat4 translation(1.0f);
	translation[3] = glm::vec4(-cameraPosition, 1.0f);

	UpdateCameraToWorldUniform(rotation * translation);
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gProgramID);
	glBindVertexArray(gVertexArrayObjectID);

	UpdateModelToWorldTransform();
	UpdateFPSWorldToCameraTransform();

	/*const glm::vec3 cameraPosition = ResolveCameraPosition();
	UpdateLookAtWorldToCameraTransform(cameraPosition, gCameraLookAtTarget, glm::vec3(0.0f, 1.0f, 0.0f));*/
	glDrawElements(GL_TRIANGLES, sizeof(gIndexData) / sizeof(gIndexData[0]), GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(GL_NONE);
	glUseProgram(GL_NONE);

	glutSwapBuffers();
}

void OnViewportUpdated(float aspectRatio)
{
	gCameraToClipTransform[0].x = gFrustumScale / aspectRatio;
	UpdateCameraToClipUniform();
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h );
	OnViewportUpdated(static_cast<float>(w) / static_cast<float>(h));
}

void UpdateCameraPosition(const float offsetX, const float offsetY, const float offsetZ)
{
	glm::vec4 positionOffset(offsetX, offsetY, offsetZ, 0.0f);
	glm::mat4 cameraRotationMatrix = GetRotationX(gCameraRotationX) * GetRotationY(gCameraRotationY) * GetRotationZ(gCameraRotationZ);
	positionOffset = cameraRotationMatrix * positionOffset;

	gCameraPosition.x += positionOffset.x;
	gCameraPosition.y += positionOffset.y;
	gCameraPosition.z += positionOffset.z;
}

void keyboard(unsigned char key, int x, int y)
{
	bool isHandled = true;
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	case 'w':
		UpdateCameraPosition(0.0f, 0.0f, -CAMERA_POSITION_INCREMENT);
		break;
	case 's':
		UpdateCameraPosition(0.0f, 0.0f, CAMERA_POSITION_INCREMENT);
		break;
	case 'a':
		UpdateCameraPosition(-CAMERA_POSITION_INCREMENT, 0.0f, 0.0f);
		break;
	case 'd':
		UpdateCameraPosition(CAMERA_POSITION_INCREMENT, 0.0f, 0.0f);
		break;
	case 'i':
		gCameraRotationX += CAMERA_ROTATION_INCREMENT;
		break;
	case 'k':
		gCameraRotationX -= CAMERA_ROTATION_INCREMENT;
		break;
	case 'j':
		gCameraRotationY += CAMERA_ROTATION_INCREMENT;
		break;
	case 'l':
		gCameraRotationY -= CAMERA_ROTATION_INCREMENT;
		break;
	case 'u':
		gCameraRotationZ += CAMERA_ROTATION_INCREMENT;
		break;
	case 'o':
		gCameraRotationZ -= CAMERA_ROTATION_INCREMENT;
		break;
	default:
		isHandled = false;
		break;
	}
	if (isHandled)
	{
		glutPostRedisplay();
	}
}

unsigned int defaults(unsigned int displayMode, int &width, int &height) 
{ 
	return displayMode; 
}

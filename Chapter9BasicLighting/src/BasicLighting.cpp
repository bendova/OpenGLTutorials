#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"
#include "../framework/MathUtil.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>

#define COLOR_RED		1.0f, 0.0f, 0.0f, 1.0f
#define COLOR_GREEN		0.0f, 1.0f, 0.0f, 1.0f
#define COLOR_BLUE		0.0f, 0.0f, 1.0f, 1.0f
#define COLOR_YELLOW	1.0f, 1.0f, 0.0f, 1.0f
#define COLOR_PURPLE	1.0f, 0.0f, 1.0f, 1.0f
#define COLOR_CYAN		0.0f, 1.0f, 1.0f, 1.0f

#define LEFT_EXTENT		-1.0f
#define RIGHT_EXTENT	 1.0f
#define TOP_EXTENT		 1.0f
#define BOTTOM_EXTENT	-1.0f
#define FRONT_EXTENT	 1.0f
#define BACK_EXTENT		-1.0f

#define FRONT_TOP_LEFT_CORNER LEFT_EXTENT, TOP_EXTENT, FRONT_EXTENT
#define FRONT_TOP_RIGHT_CORNER RIGHT_EXTENT, TOP_EXTENT, FRONT_EXTENT
#define FRONT_BOTTOM_LEFT_CORNER LEFT_EXTENT, BOTTOM_EXTENT, FRONT_EXTENT
#define FRONT_BOTTOM_RIGHT_CORNER RIGHT_EXTENT, BOTTOM_EXTENT, FRONT_EXTENT
#define BACK_TOP_LEFT_CORNER LEFT_EXTENT, TOP_EXTENT, BACK_EXTENT
#define BACK_TOP_RIGHT_CORNER RIGHT_EXTENT, TOP_EXTENT, BACK_EXTENT
#define BACK_BOTTOM_LEFT_CORNER LEFT_EXTENT, BOTTOM_EXTENT, BACK_EXTENT
#define BACK_BOTTOM_RIGHT_CORNER RIGHT_EXTENT, BOTTOM_EXTENT, BACK_EXTENT

const unsigned COLOR_COMPONENT_COUNT = 4;
const unsigned VERTEX_COMPONENT_COUNT = 3;
const unsigned VERTEX_COUNT = 24;

const float gVertexBuffer[] =
{
	// front face
	FRONT_TOP_LEFT_CORNER,
	FRONT_TOP_RIGHT_CORNER,
	FRONT_BOTTOM_RIGHT_CORNER,
	FRONT_BOTTOM_LEFT_CORNER,

	// right face
	FRONT_TOP_RIGHT_CORNER,
	BACK_TOP_RIGHT_CORNER,
	BACK_BOTTOM_RIGHT_CORNER,
	FRONT_BOTTOM_RIGHT_CORNER,

	// left face
	BACK_TOP_LEFT_CORNER,
	FRONT_TOP_LEFT_CORNER,
	FRONT_BOTTOM_LEFT_CORNER,
	BACK_BOTTOM_LEFT_CORNER,

	// back face
	BACK_TOP_RIGHT_CORNER,
	BACK_TOP_LEFT_CORNER,
	BACK_BOTTOM_LEFT_CORNER,
	BACK_BOTTOM_RIGHT_CORNER,

	// top face
	BACK_TOP_LEFT_CORNER,
	BACK_TOP_RIGHT_CORNER,
	FRONT_TOP_RIGHT_CORNER,
	FRONT_TOP_LEFT_CORNER,

	// bottom face
	BACK_BOTTOM_RIGHT_CORNER,
	BACK_BOTTOM_LEFT_CORNER,
	FRONT_BOTTOM_LEFT_CORNER,
	FRONT_BOTTOM_RIGHT_CORNER,

	// front face colors
	COLOR_RED,
	COLOR_RED,
	COLOR_RED,
	COLOR_RED,

	// right face colors
	COLOR_GREEN,
	COLOR_GREEN,
	COLOR_GREEN,
	COLOR_GREEN,

	// left face colors
	COLOR_BLUE,
	COLOR_BLUE,
	COLOR_BLUE,
	COLOR_BLUE,

	// back face colors
	COLOR_YELLOW,
	COLOR_YELLOW,
	COLOR_YELLOW,
	COLOR_YELLOW,

	// top face colors
	COLOR_PURPLE,
	COLOR_PURPLE,
	COLOR_PURPLE,
	COLOR_PURPLE,

	// bottom face colors
	COLOR_CYAN,
	COLOR_CYAN,
	COLOR_CYAN,
	COLOR_CYAN,
};

const GLshort gVertexIndexBuffer[] =
{
	// front face
	0, 1, 2,
	2, 3, 0,

	// right face
	4, 5, 6,
	6, 7, 4,

	// left face
	8, 9, 10,
	10, 11, 8,

	// back face
	12, 13, 14,
	14, 15, 12,

	// top face
	16, 17, 18,
	18, 19, 16,

	// top face
	20, 21, 22,
	22, 23, 20,
};

GLuint gProgramID;
GLuint gVertexBufferID; 
GLuint gVertexIndexBufferID;
GLuint gVertexArrayObjectID;

GLint gVertexPositionAttributeID;
GLint gVertexColorAttributeID;
GLint gMVPTransformUniformID;

float gFrustumScale;
glm::mat4 gModelToWorldTransform;
glm::mat4 gWorldToCameraTransform;
glm::mat4 gCameraToClipTransform;
glm::mat4 gMVPTransform;

void initProgram()
{
	std::vector<GLuint> shaders;
	shaders.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "PosOnly.vert"));
	shaders.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "ColorPassthrough.frag"));

	gProgramID = Framework::CreateProgram(shaders);

	gVertexPositionAttributeID = glGetAttribLocation(gProgramID, "position");
	gVertexColorAttributeID = glGetAttribLocation(gProgramID, "color");
	gMVPTransformUniformID = glGetUniformLocation(gProgramID, "mvpTransform");
}

void initVertexBuffer()
{
	glGenBuffers(1, &gVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBuffer), gVertexBuffer, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
}

void initVertexIndexBuffer()
{
	glGenBuffers(1, &gVertexIndexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVertexIndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gVertexIndexBuffer), gVertexIndexBuffer, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
}

void initVertexArrayObject()
{
	glGenVertexArrays(1, &gVertexArrayObjectID);
	glBindVertexArray(gVertexArrayObjectID);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID);
	
	glEnableVertexAttribArray(gVertexPositionAttributeID);
	glVertexAttribPointer(gVertexPositionAttributeID, VERTEX_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(gVertexColorAttributeID);
	const unsigned colorComponentOffset = VERTEX_COUNT * VERTEX_COMPONENT_COUNT * sizeof(gVertexBuffer[0]);
	glVertexAttribPointer(gVertexColorAttributeID, COLOR_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, (void*)colorComponentOffset);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVertexIndexBufferID);

	glBindVertexArray(GL_NONE);
}

float GetAngleInRadians(float angleInDegrees)
{
	return angleInDegrees * 3.14159f / 180.0f;
}

float GetFrustumScale(float angleInDegrees)
{
	return 1 / tan(GetAngleInRadians(angleInDegrees) / 2.0f);
}

void initWorldToCameraTranform()
{

}

void initCameraToClipTranform()
{
	const float zNear = 1.0f;
	const float zFar = 300.0f;

	gFrustumScale = GetFrustumScale(45.0f);

	gCameraToClipTransform[0].x = gFrustumScale;
	gCameraToClipTransform[1].y = gFrustumScale;
	gCameraToClipTransform[2].z = (zNear + zFar) / (zNear - zFar);
	gCameraToClipTransform[2].w = -1.0f;
	gCameraToClipTransform[3].z = (2 * zNear * zFar) / (zNear - zFar);
}

void updateMVPTransform()
{
	gMVPTransform = gCameraToClipTransform * gWorldToCameraTransform * gModelToWorldTransform;
}

void initTranformationMatrices()
{
	initWorldToCameraTranform();
	initCameraToClipTranform();
	updateMVPTransform();
}

void init()
{
	initProgram();
	initVertexBuffer();
	initVertexIndexBuffer();
	initVertexArrayObject();

	initTranformationMatrices();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glEnable(GL_DEPTH_CLAMP);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
}

void translateCube(const glm::vec3& translation)
{
	gModelToWorldTransform[3] = glm::vec4(translation, 1.0f);
	updateMVPTransform();
}

void rotateCube(const glm::mat4& rotationMatrix)
{
	gModelToWorldTransform *= rotationMatrix;
	updateMVPTransform();
}

void rotateCubeX(float angleInDegrees)
{
	rotateCube(MyCode::MathUtil::RotateX(angleInDegrees));
}

void rotateCubeY(float angleInDegrees)
{
	rotateCube(MyCode::MathUtil::RotateY(angleInDegrees));
}

void rotateCubeZ(float angleInDegrees)
{
	rotateCube(MyCode::MathUtil::RotateZ(angleInDegrees));
}

void drawCube()
{
	glUseProgram(gProgramID);
	glBindVertexArray(gVertexArrayObjectID);

	rotateCubeX(-30.0f);
	translateCube(glm::vec3(0.0, 0.0f, -5.0f));
	glUniformMatrix4fv(gMVPTransformUniformID, 1, GL_FALSE, glm::value_ptr(gMVPTransform));

	glDrawElements(GL_TRIANGLES, sizeof(gVertexIndexBuffer) / sizeof(gVertexIndexBuffer[0]), GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(GL_NONE);
	glUseProgram(GL_NONE);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawCube();

	glutSwapBuffers();
}

void onViewportResized(float aspectRatio)
{
	gCameraToClipTransform[0].x = gFrustumScale / aspectRatio;
	updateMVPTransform();
}

void reshape(int w, int h)
{
	glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));

	onViewportResized(static_cast<float>(w) / static_cast<float>(h));
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	}
}

unsigned int defaults(unsigned int displayMode, int &width, int &height)
{
	return displayMode;
}
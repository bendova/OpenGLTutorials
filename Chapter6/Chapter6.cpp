/* 
* My implementation of chapter 6 from http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2006.html
*/

#include <vector>
#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

const unsigned POSITION_ATTRIBUTE_LOCATION = 0;
const unsigned COLOR_ATTRIBUTE_LOCATION = 1;

const unsigned VERTEX_COMPONENT_COUNT = 3;
const unsigned COLOR_COMPONENT_COUNT = 4;

const unsigned VERTEX_COUNT = 4;

const float LEFT_EXTENT = -0.5f;
const float RIGHT_EXTENT = -LEFT_EXTENT;
const float TOP_EXTENT = 0.5f;
const float BOTTOM_EXTENT = -TOP_EXTENT;
const float FRONT_EXTENT = 0.0f;

#define RED 1.0f, 0.0f, 0.0f, 1.0f 
#define GREEN 0.0f, 1.0f, 0.0f, 1.0f 
#define BLUE 0.0f, 0.0f, 1.0f, 1.0f 

const float gVertexData[] =
{
	LEFT_EXTENT, TOP_EXTENT, FRONT_EXTENT,
	RIGHT_EXTENT, TOP_EXTENT, FRONT_EXTENT,
	LEFT_EXTENT, BOTTOM_EXTENT, FRONT_EXTENT,
	RIGHT_EXTENT, BOTTOM_EXTENT, FRONT_EXTENT,

	RED,
	GREEN,
	GREEN,
	BLUE
};

const GLshort gIndexData[] =
{
	0, 1, 3,
	3, 2, 0
};

GLuint gProgramID;
GLuint gVertexBufferID;
GLuint gIndexBufferID;
GLuint gVertexArrayObjectID;

GLint gModelToCameraUniform;
GLint gCameraToClipUniform;

float gFrustumScale;
glm::mat4 gCameraToClipMatrix;

float CalculateFrustumScale(float angleInDegrees)
{
	const float degreesToRadians = 3.14159f * 2.0f / 180.0f;
	float fovInRadians = angleInDegrees * degreesToRadians;
	return 1.0f / tan(fovInRadians / 2.0f);
}

void InitCameraToClipMatrix()
{
	gFrustumScale = CalculateFrustumScale(45.0f);

	const float zNear = 1.0f;
	const float zFar = 45.0f;
	gCameraToClipMatrix[0].x = gFrustumScale;
	gCameraToClipMatrix[1].y = gFrustumScale;
	gCameraToClipMatrix[2].z = (zNear + zFar) / (zNear - zFar);
	gCameraToClipMatrix[2].w = -1.0f;
	gCameraToClipMatrix[3].z = (2 * zNear * zFar) / (zNear - zFar);
}

void InitializeProgram()
{
	std::vector<GLuint> shaders;

	shaders.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "PosColorLocalTransform.vert"));
	shaders.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "ColorPassthrough.frag"));

	gProgramID = Framework::CreateProgram(shaders);

	gModelToCameraUniform = glGetUniformLocation(gProgramID, "modelToCameraTransform");
	gCameraToClipUniform = glGetUniformLocation(gProgramID, "cameraToClipTransform");

	InitCameraToClipMatrix();
	glUseProgram(gProgramID);
	glUniformMatrix4fv(gCameraToClipUniform, 1, GL_FALSE, glm::value_ptr(gCameraToClipMatrix));
	glUseProgram(GL_NONE);
}

void InitializeVertexBuffer()
{
	glGenBuffers(1, &gVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexData), gVertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glGenBuffers(1, &gIndexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gIndexData), gIndexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
}

void InitializeVertexArrayObjects()
{
	glGenVertexArrays(1, &gVertexArrayObjectID);
	
	glBindVertexArray(gVertexArrayObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID);
	glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
	glEnableVertexAttribArray(COLOR_ATTRIBUTE_LOCATION);

	glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, VERTEX_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, 0);

	const unsigned colorOffset = VERTEX_COUNT * VERTEX_COMPONENT_COUNT * sizeof(gVertexData[0]);
	glVertexAttribPointer(COLOR_ATTRIBUTE_LOCATION, COLOR_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, (void*)colorOffset);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferID);
	glBindVertexArray(GL_NONE);
}

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();
	InitializeVertexBuffer();
	InitializeVertexArrayObjects();
}

glm::vec3 NoOffset(float elapsedTime)
{
	return glm::vec3(0.0f, 0.0f, -10.0f);
}

glm::vec3 CircularOffset(float elapsedTime)
{
	const float fLoopDuration = 3.0f;
	const float fScale = 3.14159f * 2.0f / fLoopDuration;

	float fCurrTimeThroughLoop = fmodf(elapsedTime, fLoopDuration);

	return glm::vec3(cosf(fCurrTimeThroughLoop * fScale) * 4.f,
		sinf(fCurrTimeThroughLoop * fScale) * 6.f,
		-10.0f);
}

glm::vec3 BottomOvalOffset(float elapsedTime)
{
	const float fLoopDuration = 12.0f;
	const float fScale = 3.14159f * 2.0f / fLoopDuration;

	float fCurrTimeThroughLoop = fmodf(elapsedTime, fLoopDuration);

	return glm::vec3(cosf(fCurrTimeThroughLoop * fScale) * 5.f,
		-3.5f,
		sinf(fCurrTimeThroughLoop * fScale) * 5.f - 10.0f);
}

class MatrixOffset
{
	typedef glm::vec3(*OffsetFunction)(float);
public:
	MatrixOffset(OffsetFunction func) : mOffsetFunction(func)
	{}

	glm::mat4 CalculateOffsetMatrix(float time)
	{
		glm::mat4 offsetMatrix;
		offsetMatrix[3] = glm::vec4(mOffsetFunction(time), 1.0f);
		return offsetMatrix;
	}
private:
	OffsetFunction mOffsetFunction;
};

MatrixOffset gObjectsToRender[]
{
	MatrixOffset(NoOffset),
	MatrixOffset(CircularOffset),
	MatrixOffset(BottomOvalOffset)
};

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(gProgramID);
	glBindVertexArray(gVertexArrayObjectID);
	
	float elapsedTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	for (unsigned i = 0; i < (sizeof(gObjectsToRender) / sizeof(gObjectsToRender[0])); ++i)
	{
		glm::mat4 offsetMatrix = gObjectsToRender[i].CalculateOffsetMatrix(elapsedTime);
		glUniformMatrix4fv(gModelToCameraUniform, 1, GL_FALSE, glm::value_ptr(offsetMatrix));
		glDrawElements(GL_TRIANGLES, sizeof(gIndexData) / sizeof(gIndexData[0]), GL_UNSIGNED_SHORT, 0);
	}
	
	glBindVertexArray(GL_NONE);
	glUseProgram(GL_NONE);

	glutSwapBuffers();
	glutPostRedisplay();
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h );
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		return;
	}
}

unsigned int defaults(unsigned int displayMode, int &width, int &height) 
{ 
	return displayMode; 
}

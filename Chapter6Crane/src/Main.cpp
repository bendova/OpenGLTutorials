/* 
* My implementation of chapter 7 from http://www.arcsynthesis.org/gltut/Positioning/Tut06%20Fun%20with%20Matrices.html
*/

#include <vector>
#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <memory>
#include <fstream>
#include <stdio.h>

#include "MathUtil.h"
#include "Crane.h"

GLuint gProgramID;
GLuint gCameraToClipUniform;

float gFrustumScale;
glm::mat4 gCameraToClipMatrix(1.0f);

using namespace Hierarchy;

void InitCameraToClipMatrix()
{
	gFrustumScale = MathUtil::GetFrustumScale(45.0f);
	const float zNear = 1.0f;
	const float zFar = 200.0f;
	gCameraToClipMatrix[0].x = gFrustumScale;
	gCameraToClipMatrix[1].y = gFrustumScale;
	gCameraToClipMatrix[2].z = (zNear + zFar) / (zNear - zFar);
	gCameraToClipMatrix[2].w = -1.0f;
	gCameraToClipMatrix[3].z = (2 * zNear * zFar) / (zNear - zFar);
}

void UpdateCameraToClipUniform()
{
	glUseProgram(gProgramID);
	glUniformMatrix4fv(gCameraToClipUniform, 1, GL_FALSE, glm::value_ptr(gCameraToClipMatrix));
	glUseProgram(GL_NONE);
}

void InitializeProgram()
{
	std::vector<GLuint> shaders;
	shaders.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "PosColorLocalTransform.vert"));
	shaders.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "ColorPassthrough.frag"));

	gProgramID = Framework::CreateProgram(shaders);
	gCameraToClipUniform = glGetUniformLocation(gProgramID, "cameraToClipTransform");

	InitCameraToClipMatrix();
	UpdateCameraToClipUniform();
}

std::unique_ptr<Crane> gCrane;
void InitializeCrane()
{
	gCrane = std::make_unique<Crane>(gProgramID);
}

void keyboardUp(unsigned char key, int x, int y);

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();
	InitializeCrane();
	glutKeyboardUpFunc(keyboardUp);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
}

float GetDeltaTime()
{
	static float previousTime = 0.0f;
	float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	return deltaTime;
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	gCrane->Draw(GetDeltaTime());

	glutSwapBuffers();
	glutPostRedisplay();
}

void OnViewportUpdated(float aspectRatio)
{
	gCameraToClipMatrix[0].x = gFrustumScale / aspectRatio;
	UpdateCameraToClipUniform();
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h );

	OnViewportUpdated(static_cast<float>(w) / static_cast<float>(h));
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	default:
		gCrane->HandleKeyDown(key);
		break;
	}
}

void keyboardUp(unsigned char key, int x, int y)
{
	gCrane->HandleKeyUp(key);
}

unsigned int defaults(unsigned int displayMode, int &width, int &height) 
{ 
	return displayMode; 
}

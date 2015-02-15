#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/MyMesh.h"
#include "../framework/MathUtil.h"
#include "../framework/MousePole.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glutil/MatrixStack.h"
#include "glutil/MousePoles.h"
#include "glm/gtc/quaternion.hpp"
#include "render/Cube.h"

#include <vector>
#include <memory>

using MyCode::Mesh;
using MyCode::Cube;
//using Framework::Mesh;

GLuint gProgramID;

GLint gVertexPositionAttributeID;
GLint gVertexColorAttributeID;
GLint gMVPTransformUniformID;

float gFrustumScale;
glm::mat4 gModelToWorldTransform;
glm::mat4 gWorldToCameraTransform;
glm::mat4 gCameraToClipTransform;
glm::mat4 gMVPTransform;

glutil::ObjectData gInitialObjectData =
{
	glm::vec3(0.0f, 0.5f, 0.0f),
	glm::fquat(1.0f, 0.0f, 0.0, 0.0f)
};

glutil::ViewData gInitialViewData =
{
	glm::vec3(0.0f, 0.5f, 0.0f),
	glm::fquat(0.9238f, 0.38268f, 0.0f, 0.0f),
	5.0f,
	0.0f
};

glutil::ViewScale gViewScale =
{
	3.0f, 20.0f,
	1.5f, 0.5f,
	0.0f, 0.0f,
	90.0f / 250.0f
};

glutil::ViewPole gViewPole = glutil::ViewPole(gInitialViewData, gViewScale, glutil::MB_LEFT_BTN);

std::unique_ptr<Mesh> gCubeMesh;
std::unique_ptr<Mesh> gCylinderMesh;
std::unique_ptr<Mesh> gPlaneMesh;
std::unique_ptr<Cube> gMyCube;

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

void initTranformationMatrices()
{
	initWorldToCameraTranform();
	initCameraToClipTranform();
}

void loadMeshes()
{
	gCylinderMesh = std::unique_ptr<Mesh>(new Mesh("UnitCylinder.xml")); 
	gPlaneMesh = std::unique_ptr<Mesh>(new Mesh("UnitPlane.xml"));
	gCubeMesh = std::unique_ptr<Mesh>(new Mesh("UnitCube.xml"));
}

void loadMyCube()
{
	gMyCube = std::unique_ptr<Cube>(new Cube(gVertexPositionAttributeID, gVertexColorAttributeID));
}

void loadRenderObjects()
{
	loadMeshes();
	loadMyCube();
}

namespace
{
	void onMouseClick(int button, int state, int x, int y)
	{
		Framework::ForwardMouseButton(gViewPole, button, state, x, y);
		glutPostRedisplay();
	}

	void onMouseMoved(int x, int y)
	{
		Framework::ForwardMouseMotion(gViewPole, x, y);
		glutPostRedisplay();
	}

	void onMouseWheel(int wheel, int direction, int x, int y)
	{
		Framework::ForwardMouseWheel(gViewPole, wheel, direction, x, y);
		glutPostRedisplay();
	}
}

void init()
{
	initProgram();

	initTranformationMatrices();

	loadRenderObjects();

	glutMouseFunc(onMouseClick);
	glutMotionFunc(onMouseMoved);
	glutMouseWheelFunc(onMouseWheel);

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

void updateMVPTransform()
{
	gMVPTransform = gCameraToClipTransform * gWorldToCameraTransform * gModelToWorldTransform;
}

void applyMVPTransform()
{
	updateMVPTransform();
	glUniformMatrix4fv(gMVPTransformUniformID, 1, GL_FALSE, glm::value_ptr(gMVPTransform));
}

void setModelToWorldTransform(const glm::mat4& modelToWorldTransform)
{
	gModelToWorldTransform = modelToWorldTransform;
	applyMVPTransform();
}

void setWorldToCameraTransform(const glm::mat4& worldToCameraTransform)
{
	gWorldToCameraTransform = worldToCameraTransform;
	applyMVPTransform();
}

void drawCube(glutil::MatrixStack& modelMatrix)
{
	glutil::PushStack push(modelMatrix);
	modelMatrix.Translate(glm::vec3(0.0f, 1.01f, 0.0f));
	setModelToWorldTransform(modelMatrix.Top());
	gMyCube->Render();
}

void drawPlane(glutil::MatrixStack& modelMatrix)
{
	glutil::PushStack push(modelMatrix);
	modelMatrix.Scale(5.0f);
	setModelToWorldTransform(modelMatrix.Top());

	gPlaneMesh->Render();
}

void drawCylinder(glutil::MatrixStack& modelMatrix)
{
	glutil::PushStack push(modelMatrix);
	modelMatrix.Translate(glm::vec3(0.0f, 0.51f, 0.0f));
	setModelToWorldTransform(modelMatrix.Top());

	gCylinderMesh->Render("color");
}

void drawCubeFromMesh()
{
	gCubeMesh->Render();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gProgramID);

	glutil::MatrixStack modelMatrix;
	modelMatrix.SetMatrix(gViewPole.CalcMatrix());

	drawCube(modelMatrix);
	drawPlane(modelMatrix);
	//drawCylinder(modelMatrix);
	//drawCubeFromMesh();

	glUseProgram(GL_NONE);

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
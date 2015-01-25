#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include <glutil/MatrixStack.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/MyMesh.h"

#define COLOR_RED		1.0f, 0.0f, 0.0f, 1.0f
#define COLOR_GREEN		0.0f, 1.0f, 0.0f, 1.0f
#define COLOR_BLUE		0.0f, 0.0f, 1.0f, 1.0f
#define COLOR_MAGENTA	1.0f, 0.0f, 1.0f, 1.0f
#define COLOR_CYAN		0.0f, 1.0f, 1.0f, 1.0f
#define COLOR_YELLOW	1.0f, 1.0f, 0.0f, 1.0f

const unsigned VERTEX_ATTRIBUTE_COUNT = 3;
const unsigned COLOR_ATTRIBUTE_COUNT = 4;

#define LEFT_EXTENT		-1.0f
#define RIGHT_EXTENT	 1.0f
#define TOP_EXTENT		 1.0f
#define BOTTOM_EXTENT	-1.0f
#define FRONT_EXTENT	 1.0f
#define BACK_EXTENT		-1.0f

#define FRONT_TOP_LEFT_CORNER		LEFT_EXTENT,	TOP_EXTENT,		FRONT_EXTENT
#define FRONT_TOP_RIGHT_CORNER		RIGHT_EXTENT,	TOP_EXTENT,		FRONT_EXTENT
#define FRONT_BOTTOM_LEFT_CORNER	LEFT_EXTENT,	BOTTOM_EXTENT,	FRONT_EXTENT
#define FRONT_BOTTOM_RIGHT_CORNER	RIGHT_EXTENT,	BOTTOM_EXTENT,	FRONT_EXTENT
#define BACK_TOP_LEFT_CORNER		LEFT_EXTENT,	TOP_EXTENT,		BACK_EXTENT
#define BACK_TOP_RIGHT_CORNER		RIGHT_EXTENT,	TOP_EXTENT,		BACK_EXTENT
#define BACK_BOTTOM_LEFT_CORNER		LEFT_EXTENT,	BOTTOM_EXTENT,	BACK_EXTENT
#define BACK_BOTTOM_RIGHT_CORNER	RIGHT_EXTENT,	BOTTOM_EXTENT,	BACK_EXTENT

const unsigned VERTICES_COUNT = 24;
const float gVertexData[] =
{
	// front face
	FRONT_TOP_LEFT_CORNER,
	FRONT_TOP_RIGHT_CORNER,
	FRONT_BOTTOM_RIGHT_CORNER,
	FRONT_BOTTOM_LEFT_CORNER,

	// back face
	BACK_TOP_RIGHT_CORNER,
	BACK_TOP_LEFT_CORNER,
	BACK_BOTTOM_LEFT_CORNER,
	BACK_BOTTOM_RIGHT_CORNER,

	// left face
	BACK_TOP_LEFT_CORNER,
	FRONT_TOP_LEFT_CORNER,
	FRONT_BOTTOM_LEFT_CORNER,
	BACK_BOTTOM_LEFT_CORNER,

	// right face
	FRONT_TOP_RIGHT_CORNER,
	BACK_TOP_RIGHT_CORNER,
	BACK_BOTTOM_RIGHT_CORNER,
	FRONT_BOTTOM_RIGHT_CORNER,

	// top face
	BACK_TOP_LEFT_CORNER,
	BACK_TOP_RIGHT_CORNER,
	FRONT_TOP_RIGHT_CORNER,
	FRONT_TOP_LEFT_CORNER,

	// bottom face
	FRONT_BOTTOM_LEFT_CORNER,
	FRONT_BOTTOM_RIGHT_CORNER,
	BACK_BOTTOM_RIGHT_CORNER,
	BACK_BOTTOM_LEFT_CORNER,

	// front face colors
	COLOR_RED, 
	COLOR_RED,
	COLOR_RED,
	COLOR_RED,

	// back face colors
	COLOR_GREEN,
	COLOR_GREEN,
	COLOR_GREEN,
	COLOR_GREEN,

	// left face colors
	COLOR_BLUE,
	COLOR_BLUE,
	COLOR_BLUE,
	COLOR_BLUE,

	// right face colors
	COLOR_MAGENTA,
	COLOR_MAGENTA,
	COLOR_MAGENTA,
	COLOR_MAGENTA,

	// top face colors
	COLOR_CYAN,
	COLOR_CYAN,
	COLOR_CYAN,
	COLOR_CYAN,

	// bottom face colors
	COLOR_YELLOW,
	COLOR_YELLOW,
	COLOR_YELLOW,
	COLOR_YELLOW,
};

const GLshort gVertexIndexes[] =
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
	22, 23, 20,
};

GLuint gProgramID;
GLuint gVertexBufferID;
GLuint gVertexIndexesBufferID;
GLuint gVertexArrayObjectID;
GLuint gPositionAttribute;
GLuint gColorAttribute;
GLuint gModelToCameraTransformUniform;
GLuint gCameraToClipTransformUniform;
GLuint gBaseColorUniform;

float gFrustumScale;
glm::mat4 gCameraToClipTransform;

float gRotationAngleX = 0.0f;
float gRotationAngleY = 0.0f;
float gRotationAngleZ = 0.0f;

std::unique_ptr<MyCode::Mesh> gCubeMesh;
std::unique_ptr<MyCode::Mesh> gShipMesh;
std::unique_ptr<Framework::Mesh> gFWShipMesh;

void InitProgram()
{
	std::vector<GLuint> shaders;
	shaders.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "PosColorLocalTransform.vert"));
	shaders.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "ColorMultUniform.frag"));

	gProgramID = Framework::CreateProgram(shaders);
	gPositionAttribute = glGetAttribLocation(gProgramID, "position");
	gColorAttribute = glGetAttribLocation(gProgramID, "color");
	gModelToCameraTransformUniform = glGetUniformLocation(gProgramID, "modelToCameraTransform");
	gCameraToClipTransformUniform = glGetUniformLocation(gProgramID, "cameraToClipTransform");
	gBaseColorUniform = glGetUniformLocation(gProgramID, "baseColor");
}

void InitVertexBuffers()
{
	glGenBuffers(1, &gVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexData), gVertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glGenBuffers(1, &gVertexIndexesBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVertexIndexesBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gVertexIndexes), gVertexIndexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
}

void InitVertexArrayObject()
{
	glGenVertexArrays(1, &gVertexArrayObjectID);
	glBindVertexArray(gVertexArrayObjectID);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID);
	glEnableVertexAttribArray(gPositionAttribute);
	glVertexAttribPointer(gPositionAttribute, VERTEX_ATTRIBUTE_COUNT, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(gColorAttribute);
	const unsigned COLOR_OFFSET = sizeof(float) * VERTEX_ATTRIBUTE_COUNT * VERTICES_COUNT;
	glVertexAttribPointer(gColorAttribute, COLOR_ATTRIBUTE_COUNT, GL_FLOAT, GL_FALSE, 0, (void*)COLOR_OFFSET);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVertexIndexesBufferID);

	glBindVertexArray(GL_NONE);
}

void LoadMeshes()
{
	try
	{
		gFWShipMesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh("Ship.xml"));

		gCubeMesh = std::unique_ptr<MyCode::Mesh>(new MyCode::Mesh("UnitCubeColor.xml"));
		gShipMesh = std::unique_ptr<MyCode::Mesh>(new MyCode::Mesh("Ship.xml"));
	}
	catch (std::exception& e)
	{
		printf(e.what());
	}
}

float DegreesToRadians(const float angleInDegrees)
{
	return angleInDegrees * 3.14159f / 180.0f;
}

float GetFrustumScale(const float angleInDegrees)
{
	return 1.0f / tan(DegreesToRadians(angleInDegrees) / 2.0f);
}

void SetCameraToClipTransform()
{
	glUseProgram(gProgramID);
	glUniformMatrix4fv(gCameraToClipTransformUniform, 1, GL_FALSE, glm::value_ptr(gCameraToClipTransform));
	glUseProgram(GL_NONE);
}

void InitCameraToClipTransform()
{
	const float zNear = 1.0f;
	const float zFar = 600.0f;

	gFrustumScale = GetFrustumScale(20.0f);

	gCameraToClipTransform[0].x = gFrustumScale;
	gCameraToClipTransform[1].y = gFrustumScale;
	gCameraToClipTransform[2].z = (zNear + zFar) / (zNear - zFar);
	gCameraToClipTransform[2].w = -1.0f;
	gCameraToClipTransform[3].z = (2 * zNear * zFar) / (zNear - zFar);

	SetCameraToClipTransform();
}

void init()
{
	InitProgram();
	InitVertexBuffers();
	InitVertexArrayObject();
	LoadMeshes();
	InitCameraToClipTransform();
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
}

void DrawCubeFromVertexData(glutil::MatrixStack& modelMatrixStack)
{
	glutil::PushStack push(modelMatrixStack);

	modelMatrixStack.Translate(glm::vec3(-5.0f, 0.0f, 0.0f));
	modelMatrixStack.RotateX(gRotationAngleX);
	modelMatrixStack.RotateY(gRotationAngleY);
	modelMatrixStack.RotateZ(gRotationAngleZ);

	glUniformMatrix4fv(gModelToCameraTransformUniform, 1, GL_FALSE, glm::value_ptr(modelMatrixStack.Top()));

	glBindVertexArray(gVertexArrayObjectID);
	glDrawElements(GL_TRIANGLES, sizeof(gVertexData) / sizeof(gVertexData[0]), GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(GL_NONE);
}

void DrawCubeFromMesh(glutil::MatrixStack& modelMatrixStack)
{
	glutil::PushStack push(modelMatrixStack);

	modelMatrixStack.Translate(glm::vec3(5.0f, 0.0f, 0.0f));
	modelMatrixStack.RotateX(gRotationAngleX);
	modelMatrixStack.RotateY(gRotationAngleY);
	modelMatrixStack.RotateZ(gRotationAngleZ);

	modelMatrixStack.Scale(glm::vec3(2.0f, 2.0f, 2.0f));
	glUniformMatrix4fv(gModelToCameraTransformUniform, 1, GL_FALSE, glm::value_ptr(modelMatrixStack.Top()));
	gCubeMesh->Render();
}

void DrawShipWithFrameworkMesh(glutil::MatrixStack& modelMatrixStack)
{
	glutil::PushStack push(modelMatrixStack);

	modelMatrixStack.Translate(glm::vec3(-20.0f, 0.0f, 0.0f));
	modelMatrixStack.RotateX(gRotationAngleX);
	modelMatrixStack.RotateY(gRotationAngleY);
	modelMatrixStack.RotateZ(gRotationAngleZ);

	modelMatrixStack.Scale(glm::vec3(2.0f, 2.0f, 2.0f));
	glUniformMatrix4fv(gModelToCameraTransformUniform, 1, GL_FALSE, glm::value_ptr(modelMatrixStack.Top()));
	gFWShipMesh->Render();
}

void DrawShipWithMyMesh(glutil::MatrixStack& modelMatrixStack)
{
	glutil::PushStack push(modelMatrixStack);

	modelMatrixStack.Translate(glm::vec3(20.0f, 0.0f, 0.0f));
	modelMatrixStack.RotateX(gRotationAngleX);
	modelMatrixStack.RotateY(gRotationAngleY);
	modelMatrixStack.RotateZ(gRotationAngleZ);

	modelMatrixStack.Scale(glm::vec3(2.0f, 2.0f, 2.0f));
	glUniformMatrix4fv(gModelToCameraTransformUniform, 1, GL_FALSE, glm::value_ptr(modelMatrixStack.Top()));
	gShipMesh->Render();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gProgramID);
	
	glUniform4f(gBaseColorUniform, 1.0f, 1.0f, 1.0f, 1.0f);

	glutil::MatrixStack modelMatrixStack;
	modelMatrixStack.Translate(glm::vec3(0.0f, 0.0f, -200.0f));
	
	DrawCubeFromVertexData(modelMatrixStack);
	DrawCubeFromMesh(modelMatrixStack);

	DrawShipWithFrameworkMesh(modelMatrixStack);
	DrawShipWithMyMesh(modelMatrixStack);

	glUseProgram(GL_NONE);

	glutSwapBuffers();
}

const float ANGLE_INC = 10.0f;
const float ANGLE_INC_SHIFT = 1.0f;

void keyboard(unsigned char key, int x, int y)
{
	bool redraw = true;

	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		redraw = false;
		break;

	case 'w': gRotationAngleX += ANGLE_INC; break;
	case 's': gRotationAngleX -= ANGLE_INC; break;
	case 'a': gRotationAngleY += ANGLE_INC; break;
	case 'd': gRotationAngleY -= ANGLE_INC; break;
	case 'q': gRotationAngleZ += ANGLE_INC; break;
	case 'e': gRotationAngleZ -= ANGLE_INC; break;

	case 'W': gRotationAngleX += ANGLE_INC_SHIFT; break;
	case 'S': gRotationAngleX -= ANGLE_INC_SHIFT; break;
	case 'A': gRotationAngleY += ANGLE_INC_SHIFT; break;
	case 'D': gRotationAngleY -= ANGLE_INC_SHIFT; break;
	case 'Q': gRotationAngleZ += ANGLE_INC_SHIFT; break;
	case 'E': gRotationAngleZ -= ANGLE_INC_SHIFT; break;

	default:
		redraw = false;
	break;
	}

	if (redraw)
	{
		glutPostRedisplay();
	}
}

void OnViewportResized(const float aspectRatio)
{
	gCameraToClipTransform[0].x = gFrustumScale / aspectRatio;
	SetCameraToClipTransform();
}

void reshape(int w, int h)
{
	glViewport(0, 0, static_cast<GLint>(w), static_cast<GLint>(h));

	OnViewportResized(static_cast<float>(w) / static_cast<float>(h));
}

unsigned defaults(unsigned displayMode, int& width, int& height)
{
	return displayMode;
}
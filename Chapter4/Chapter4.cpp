/* 
* My implementation of chapter 4 from http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2004.html
*/

#include <vector>
#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"

const unsigned POSITION_ATTRIBUTE_INDEX = 0u;
const unsigned COLOR_ATTRIBUTE_INDEX = 1u;

const unsigned COMPONENT_COUNT_PER_VERTEX = 4u;
const float gVertexData[] = 
{
	// vertices
	// front face
	  // top triangle
	 -0.5f,  0.5f, -1.0f, 1.0f,
	  0.5f,  0.5f, -1.0f, 1.0f,
	  0.5f, -0.5f, -1.0f, 1.0f,
	  // bottom triangle
	  0.5f, -0.5f, -1.0f, 1.0f,
	 -0.5f, -0.5f, -1.0f, 1.0f,
	 -0.5f,  0.5f, -1.0f, 1.0f,

	// left face
	  // top triangle
	 -0.5f,  0.5f, -2.0f, 1.0f,
	 -0.5f,  0.5f, -1.0f, 1.0f,
	 -0.5f, -0.5f, -1.0f, 1.0f,
	  // bottom triangle
	 -0.5f, -0.5f, -1.0f, 1.0f,
	 -0.5f, -0.5f, -2.0f, 1.0f,
	 -0.5f,  0.5f, -2.0f, 1.0f,

	// colors
	 // front face - red
	 1.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 0.0f, 1.0f,
	 1.0f, 0.0f, 0.0f, 1.0f,
	 // left face - green
	 0.0f, 1.0f, 0.0f, 1.0f,
	 0.0f, 1.0f, 0.0f, 1.0f,
	 0.0f, 1.0f, 0.0f, 1.0f,
	 0.0f, 1.0f, 0.0f, 1.0f,
	 0.0f, 1.0f, 0.0f, 1.0f,
	 0.0f, 1.0f, 0.0f, 1.0f,
};

const float FRUSTUM_SCALE = 0.5f;
const unsigned MATRIX_SIZE = 16;
float gPerspectiveMatrix[MATRIX_SIZE]; // in column major order

GLuint gProgramID;
GLuint gVertexBufferID;
GLuint gVertexArrayObject;
GLint gPositionOffsetUniform;
GLint gPerspectiveMatrixUniform;

void InitializeProgram()
{
	std::vector<GLuint> shaders;
	shaders.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "MatrixPerspective.vert"));
	shaders.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "StandardColors.frag"));

	gProgramID = Framework::CreateProgram(shaders);

	glUseProgram(gProgramID);
	const float offsetX = 0.8f;
	const float offsetY = 0.0f;
	gPositionOffsetUniform = glGetUniformLocation(gProgramID, "positionOffset");
	glUniform2f(gPositionOffsetUniform, offsetX, offsetY);

	memset(gPerspectiveMatrix, 0, MATRIX_SIZE * sizeof(float));

	gPerspectiveMatrix[0] = FRUSTUM_SCALE;
	gPerspectiveMatrix[5] = FRUSTUM_SCALE;

	const float Z_NEAR = 0.0f;
	const float Z_FAR = 3.0f;
	gPerspectiveMatrix[10] = (Z_NEAR + Z_FAR) / (Z_NEAR - Z_FAR);
	gPerspectiveMatrix[14] = (2 * Z_NEAR * Z_FAR) / (Z_NEAR - Z_FAR);
	gPerspectiveMatrix[11] = -1.0f;
	
	gPerspectiveMatrixUniform = glGetUniformLocation(gProgramID, "perspectiveMatrix");
	glUniformMatrix4fv(gPerspectiveMatrixUniform, 1, GL_FALSE, gPerspectiveMatrix);

	glUseProgram(GL_NONE);
}

void InitializeVertexBuffer()
{
	glGenBuffers(1, &gVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexData), gVertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
}

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();
	InitializeVertexBuffer();

	glGenVertexArrays(1, &gVertexArrayObject);
	glBindVertexArray(gVertexArrayObject);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
}
void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(gProgramID);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID);
	glEnableVertexAttribArray(POSITION_ATTRIBUTE_INDEX);
	glEnableVertexAttribArray(COLOR_ATTRIBUTE_INDEX);

	const GLint componentsCountPerVertexAttrib = sizeof(float);
	const GLsizei stride = 0;
	const GLvoid* vertexInitialOffset = 0;
	glVertexAttribPointer(POSITION_ATTRIBUTE_INDEX, componentsCountPerVertexAttrib, GL_FLOAT, GL_FALSE, stride, vertexInitialOffset);

	const GLsizei vertexCount = (sizeof(gVertexData) / (sizeof(gVertexData[0]) * COMPONENT_COUNT_PER_VERTEX)) / 2;
	const GLvoid* colorInitialOffset = (GLvoid*)(vertexCount * COMPONENT_COUNT_PER_VERTEX * sizeof(gVertexData[0]));
	glVertexAttribPointer(COLOR_ATTRIBUTE_INDEX, componentsCountPerVertexAttrib, GL_FLOAT, GL_FALSE, stride, colorInitialOffset);

	const GLint startVertexIndex = 0;
	glDrawArrays(GL_TRIANGLES, startVertexIndex, vertexCount);

	glDisableVertexAttribArray(POSITION_ATTRIBUTE_INDEX);
	glDisableVertexAttribArray(COLOR_ATTRIBUTE_INDEX);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glUseProgram(GL_NONE);

	glutSwapBuffers();
	glutPostRedisplay();
}

void AdjustPerspectiveMatrix(float newWidth, float newHeight)
{
	float newScale = FRUSTUM_SCALE / (newWidth / newHeight);
	gPerspectiveMatrix[0] = newScale;
	gPerspectiveMatrix[5] = FRUSTUM_SCALE;

	glUseProgram(gProgramID);
	glUniformMatrix4fv(gPerspectiveMatrixUniform, 1, GL_FALSE, gPerspectiveMatrix);
	glUseProgram(GL_NONE);
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape(int w, int h)
{
	AdjustPerspectiveMatrix((float)w, (float)h);
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

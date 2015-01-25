/* 
* My implementation of chapter 5 from http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2005.html
*/

#include <vector>
#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))

#define RIGHT_EXTENT 0.8f
#define LEFT_EXTENT -RIGHT_EXTENT
#define TOP_EXTENT 0.20f
#define MIDDLE_EXTENT 0.0f
#define BOTTOM_EXTENT -TOP_EXTENT
#define FRONT_EXTENT -1.25f
#define REAR_EXTENT -1.75f

#define GREEN_COLOR 0.75f, 0.75f, 1.0f, 1.0f
#define BLUE_COLOR 	0.0f, 0.5f, 0.0f, 1.0f
#define RED_COLOR 1.0f, 0.0f, 0.0f, 1.0f
#define GREY_COLOR 0.8f, 0.8f, 0.8f, 1.0f
#define BROWN_COLOR 0.5f, 0.5f, 0.0f, 1.0f

const unsigned NUMBER_OF_VERTICES = 36;
const unsigned COMPONENT_COUNT_PER_VERTEX = 3;
const unsigned COMPONENT_COUNT_PER_COLOR = 4;
const float gVertexData[] = 
{
	// Object 1
	// top face
	LEFT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	LEFT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,

	LEFT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,
	LEFT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,

	LEFT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	LEFT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	LEFT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,

	RIGHT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	RIGHT_EXTENT,	MIDDLE_EXTENT,	FRONT_EXTENT,
	RIGHT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,

	LEFT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,
	LEFT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	RIGHT_EXTENT,	TOP_EXTENT,		REAR_EXTENT,
	RIGHT_EXTENT,	BOTTOM_EXTENT,	REAR_EXTENT,

	//Object 2 positions
	TOP_EXTENT,		RIGHT_EXTENT,	REAR_EXTENT,
	MIDDLE_EXTENT,	RIGHT_EXTENT,	FRONT_EXTENT,
	MIDDLE_EXTENT,	LEFT_EXTENT,	FRONT_EXTENT,
	TOP_EXTENT,		LEFT_EXTENT,	REAR_EXTENT,

	BOTTOM_EXTENT,	RIGHT_EXTENT,	REAR_EXTENT,
	MIDDLE_EXTENT,	RIGHT_EXTENT,	FRONT_EXTENT,
	MIDDLE_EXTENT,	LEFT_EXTENT,	FRONT_EXTENT,
	BOTTOM_EXTENT,	LEFT_EXTENT,	REAR_EXTENT,

	TOP_EXTENT,		RIGHT_EXTENT,	REAR_EXTENT,
	MIDDLE_EXTENT,	RIGHT_EXTENT,	FRONT_EXTENT,
	BOTTOM_EXTENT,	RIGHT_EXTENT,	REAR_EXTENT,
					
	TOP_EXTENT,		LEFT_EXTENT,	REAR_EXTENT,
	MIDDLE_EXTENT,	LEFT_EXTENT,	FRONT_EXTENT,
	BOTTOM_EXTENT,	LEFT_EXTENT,	REAR_EXTENT,
					
	BOTTOM_EXTENT,	RIGHT_EXTENT,	REAR_EXTENT,
	TOP_EXTENT,		RIGHT_EXTENT,	REAR_EXTENT,
	TOP_EXTENT,		LEFT_EXTENT,	REAR_EXTENT,
	BOTTOM_EXTENT,	LEFT_EXTENT,	REAR_EXTENT,

	//Object 1 colors
	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,

	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,

	RED_COLOR,
	RED_COLOR,
	RED_COLOR,

	GREY_COLOR,
	GREY_COLOR,
	GREY_COLOR,

	BROWN_COLOR,
	BROWN_COLOR,
	BROWN_COLOR,
	BROWN_COLOR,

	//Object 2 colors
	RED_COLOR,
	RED_COLOR,
	RED_COLOR,
	RED_COLOR,

	BROWN_COLOR,
	BROWN_COLOR,
	BROWN_COLOR,
	BROWN_COLOR,

	BLUE_COLOR,
	BLUE_COLOR,
	BLUE_COLOR,

	GREEN_COLOR,
	GREEN_COLOR,
	GREEN_COLOR,

	GREY_COLOR,
	GREY_COLOR,
	GREY_COLOR,
	GREY_COLOR,
};

const GLshort gIndexData[]
{
	0, 2, 1,
	3, 2, 0,

	4, 5, 6,
	6, 7, 4,

	8, 9, 10,
	11, 13, 12,

	14, 16, 15,
	17, 16, 14,
};

const unsigned POSITION_ATTRIBUTE_LOCATION = 0;
const unsigned COLOR_ATTRIBUTE_LOCATION = 1;

GLuint gVertexBufferID;
GLuint gIndexBufferID;
GLuint gVertexArrayObject;

GLuint gProgramID;

GLint gOffsetUniform;
GLint gPerspectiveMatrixUniform;

const unsigned MATRIX_SIZE = 16;
const float FRUSTUM_SCALE = 1.0f;
float gPerspectiveMatrix[MATRIX_SIZE];

void InitializeProgram()
{
	std::vector<GLuint> shaders;

	shaders.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "Standard.vert"));
	shaders.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "Standard.frag"));

	gProgramID = Framework::CreateProgram(shaders);

	gOffsetUniform = glGetUniformLocation(gProgramID, "offset");
	gPerspectiveMatrixUniform = glGetUniformLocation(gProgramID, "perspectiveMatrix");

	const float zNear = 1.0f;
	const float zFar = 3.0f;

	memset(gPerspectiveMatrix, 0, sizeof(float) * MATRIX_SIZE);

	gPerspectiveMatrix[0] = FRUSTUM_SCALE;
	gPerspectiveMatrix[5] = FRUSTUM_SCALE;
	gPerspectiveMatrix[10] = (zNear + zFar) / (zNear - zFar);
	gPerspectiveMatrix[14] = (2 * zNear * zFar) / (zNear - zFar);
	gPerspectiveMatrix[11] = -1.0f;

	glUseProgram(gProgramID);
	glUniformMatrix4fv(gPerspectiveMatrixUniform, 1, GL_FALSE, gPerspectiveMatrix);
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
	glGenVertexArrays(1, &gVertexArrayObject);
	glBindVertexArray(gVertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID);
	glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
	glEnableVertexAttribArray(COLOR_ATTRIBUTE_LOCATION);
	glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, COMPONENT_COUNT_PER_VERTEX, GL_FLOAT, GL_FALSE, 0, 0);
	unsigned colorIndexOffset = sizeof(float) * COMPONENT_COUNT_PER_VERTEX * NUMBER_OF_VERTICES;
	glVertexAttribPointer(COLOR_ATTRIBUTE_LOCATION, COMPONENT_COUNT_PER_COLOR, GL_FLOAT, GL_FALSE, 0, (void*)colorIndexOffset);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferID);

	glBindVertexArray(GL_NONE);
}

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();
	InitializeVertexBuffer();
	InitializeVertexArrayObjects();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(gProgramID);

	glBindVertexArray(gVertexArrayObject);
	glUniform3f(gOffsetUniform, 0.0f, 0.0f, 0.0f);
	glDrawElements(GL_TRIANGLES, ARRAY_COUNT(gIndexData), GL_UNSIGNED_SHORT, 0);

	glUniform3f(gOffsetUniform, 0.0f, 0.0f, -1.0f);
	glDrawElementsBaseVertex(GL_TRIANGLES, ARRAY_COUNT(gIndexData), GL_UNSIGNED_SHORT, 0, NUMBER_OF_VERTICES / 2);

	glBindVertexArray(GL_NONE);
	glUseProgram(GL_NONE);
	
	glutSwapBuffers();
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape(int w, int h)
{
	gPerspectiveMatrix[0] = FRUSTUM_SCALE / (float(w) / h);
	glUseProgram(gProgramID);
	glUniformMatrix4fv(gPerspectiveMatrixUniform, 1, GL_FALSE, gPerspectiveMatrix);
	glUseProgram(GL_NONE);

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

/* 
* My implementation of chapter 2 from http://www.arcsynthesis.org/gltut/Basics/Tut02%20Vertex%20Attributes.html
*/

#include <vector>
#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"

const unsigned VERTEX_POSITION_LAYOUT = 0;
const unsigned VERTEX_COLOR_LAYOUT = 1;

GLuint gProgramID;
GLuint gVertexBufferID;
GLuint gVertexArrayObject;

GLint gPositionOffsetUniform;
GLint gElapsedTimeUniform;
GLint gLoopDurationUniform;

const float gLoopDuration = 5.0f;

const float gVertexData[] = 
{
	 0.0f,    0.5f,  0.0f, 1.0f,
	 0.5f, -0.366f,  0.0f, 1.0f,
	-0.5f, -0.366f,  0.0f, 1.0f,
	 1.0f,    0.0f,  0.0f, 1.0f,
	 0.0f,    1.0f,  0.0f, 1.0f,
	 0.0f,    0.0f,  1.0f, 1.0f
};

void InitializeProgram()
{
	std::vector<GLuint> shaders;
	shaders.push_back(Framework::LoadShader(GL_VERTEX_SHADER, "Chapter2.vert"));
	shaders.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, "Chapter2.frag"));

	gProgramID = Framework::CreateProgram(shaders); 
	gPositionOffsetUniform = glGetUniformLocation(gProgramID, "positionOffset");
	gElapsedTimeUniform = glGetUniformLocation(gProgramID, "elapsedTime");
	gLoopDurationUniform = glGetUniformLocation(gProgramID, "loopDuration");

	glUseProgram(gProgramID);
	glUniform1f(gLoopDurationUniform, gLoopDuration);
}

void InitializeVertexBuffer()
{
	glGenBuffers(1, &gVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexData), gVertexData, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
}

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();
	InitializeVertexBuffer();

	glGenVertexArrays(1, &gVertexArrayObject);
	glBindVertexArray(gVertexArrayObject);
}

float GetElapsedTime()
{
	return glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

void ComputeVertexOffsets(float &offsetX, float &offsetY, const float timeOffset = 0.0f)
{
	const float elapsedTime = GetElapsedTime() + timeOffset;
	const float currentTimeThroughLoop = fmodf(elapsedTime, gLoopDuration);

	const float scale = 3.1459f * 2 / gLoopDuration;
	offsetX = cosf(currentTimeThroughLoop * scale) * 0.5f;
	offsetY = sinf(currentTimeThroughLoop * scale) * 0.5f;
}

void display()
{
	float offsetX;
	float offsetY;
	ComputeVertexOffsets(offsetX, offsetY);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(gProgramID);

	glUniform1f(gElapsedTimeUniform, GetElapsedTime());
	glUniform2f(gPositionOffsetUniform, offsetX, offsetY);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferID); 
	glEnableVertexAttribArray(VERTEX_POSITION_LAYOUT);
	
	const unsigned stride = 0;
	const unsigned vertexDataOffsetInBytes = 0;
	glVertexAttribPointer(VERTEX_POSITION_LAYOUT, 4, GL_FLOAT, GL_FALSE, stride, (void*)vertexDataOffsetInBytes);

	const unsigned startTriangleIndex = 0;
	const unsigned trianglesCount = 3;
	glDrawArrays(GL_TRIANGLES, startTriangleIndex, trianglesCount);

	const float secondTriangleOffset = gLoopDuration / 2.0f;
	ComputeVertexOffsets(offsetX, offsetY, secondTriangleOffset);
	glUniform2f(gPositionOffsetUniform, offsetX, offsetY);
	glDrawArrays(GL_TRIANGLES, startTriangleIndex, trianglesCount);

	glDisableVertexAttribArray(VERTEX_POSITION_LAYOUT);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glUseProgram(GL_NONE);

	glutSwapBuffers();
	glutPostRedisplay();
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h / 2);
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

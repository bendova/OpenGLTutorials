#include <glload/gl_3_3.h>
#include "GL/freeglut.h"
#include "SceneLighting.h"
#include <memory>

std::unique_ptr<MyCode::SceneLighting> gSceneLighting;

void init()
{
	gSceneLighting = std::unique_ptr<MyCode::SceneLighting>(new MyCode::SceneLighting());
}

void display()
{
	gSceneLighting->Render();
}

void reshape(int w, int h)
{
	glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
	gSceneLighting->Reshape(static_cast<GLint>(w), static_cast<GLint>(h));
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	default:
		gSceneLighting->HandleInput(key, x, y);
		break;
	}
}

unsigned int defaults(unsigned int displayMode, int &width, int &height)
{
	width = 1000;
	height = 600;
	return displayMode;
}
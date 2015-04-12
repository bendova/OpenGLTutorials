#include "SpecularLighting.h"
#include <memory>
#include <GL/freeglut.h>

std::unique_ptr<MyCode::PointLighting> gBasicLighting;

void init()
{
	gBasicLighting = std::unique_ptr<MyCode::PointLighting>(new MyCode::PointLighting());
}

void display()
{
	gBasicLighting->Render();
}

void reshape(int w, int h)
{
	glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
	gBasicLighting->Reshape(static_cast<GLsizei>(w), static_cast<GLsizei>(h));
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	default:
		gBasicLighting->HandleInput(key, x, y);
		break;
	}
}

unsigned int defaults(unsigned int displayMode, int &width, int &height)
{
	return displayMode;
}
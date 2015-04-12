#include "SceneLighting.h"
#include "GL/freeglut.h"
#include "../framework/MousePole.h"
#include "glutil/MatrixStack.h"

namespace MyCode
{
	glutil::ViewData gInitialViewData =
	{
		glm::vec3(-59.5f, 44.0f, 95.0f),
		glm::fquat(0.92387953f, 0.3826834f, 0.0f, 0.0f),
		50.0f,
		0.0f
	};

	glutil::ViewScale gViewScale =
	{
		3.0f, 80.0f,
		4.0f, 1.0f,
		5.0f, 1.0f,
		90.0f / 250.0f
	};

	glutil::ViewPole gViewPole = glutil::ViewPole(gInitialViewData, gViewScale, glutil::MB_LEFT_BTN);

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

	SceneLighting::SceneLighting()
		: mScene(new Scene())
	{
		ConfigureOpenGL();
		ConfigureInput();
	}

	SceneLighting::~SceneLighting()
	{

	}

	void SceneLighting::ConfigureOpenGL()
	{
		glClearColor(0.529411f, 0.807843f, 0.980392f, 1.0f);//sky blue
		glClearDepth(1.0f);

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);
	}

	void SceneLighting::ConfigureInput()
	{
		glutMouseFunc(onMouseClick);
		glutMotionFunc(onMouseMoved);
		glutMouseWheelFunc(onMouseWheel);
	}

	void SceneLighting::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glutil::MatrixStack modelToCameraTransform;
		modelToCameraTransform.ApplyMatrix(gViewPole.CalcMatrix());
		mScene->Render(modelToCameraTransform);

		glutSwapBuffers();
		glutPostRedisplay();
	}

	void SceneLighting::Reshape(GLint width, GLint height)
	{
		glutil::MatrixStack cameraToClipTransform;
		const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		cameraToClipTransform.Perspective(45.0f, aspectRatio, 1.0f, 1000.0f);

		mScene->SetCameraToClipTransform(cameraToClipTransform.Top());
	}

	void SceneLighting::HandleInput(unsigned char key, int x, int y)
	{
		mScene->HandleInput(key);
		gViewPole.CharPress(key);
	}
}
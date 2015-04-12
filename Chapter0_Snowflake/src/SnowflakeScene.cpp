#include "SnowflakeScene.h"
#include "GL/freeglut.h"
#include "../framework/MousePole.h"
#include "glutil/MatrixStack.h"

namespace MyCode
{
	glutil::ViewData gInitialViewData =
	{
		glm::vec3(0.0f, 0.5f, 0.0f),
		glm::fquat(1.0f, 0.0f, 0.0f, 0.0f),
		5.0f,
		0.0f
	};

	glutil::ViewScale gViewScale =
	{
		1.0f, 20.0f,
		1.0f, 0.1f,
		1.0f, 0.1f,
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

	SnowflakeScene::SnowflakeScene()
		: mPosColorProgram("PosColor.vert", "PosColor.frag")
		, mSnowflake(mPosColorProgram)
	{
		ConfigureOpenGL();
		ConfigureInput();
	}

	SnowflakeScene::~SnowflakeScene()
	{
	}

	void SnowflakeScene::ConfigureOpenGL()
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

	void SnowflakeScene::ConfigureInput()
	{
		glutMouseFunc(onMouseClick);
		glutMotionFunc(onMouseMoved);
		glutMouseWheelFunc(onMouseWheel);
	}

	void SnowflakeScene::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glutil::MatrixStack modelToCameraTransform;
		modelToCameraTransform.ApplyMatrix(gViewPole.CalcMatrix());

		glUseProgram(mPosColorProgram.GetProgramID());
		glUniformMatrix4fv(mPosColorProgram.GetModelToCameraTransformUniform(), 
			1, GL_FALSE, glm::value_ptr(modelToCameraTransform.Top()));

		RenderSnowflake();

		glUseProgram(GL_NONE);

		glutSwapBuffers();
		glutPostRedisplay();
	}

	void SnowflakeScene::RenderSnowflake()
	{
		mSnowflake.Render();
	}

	void SnowflakeScene::Reshape(GLint width, GLint height)
	{
		glutil::MatrixStack cameraToClipTransform;
		const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		cameraToClipTransform.Perspective(45.0f, aspectRatio, 0.1f, 1000.0f);

		glUseProgram(mPosColorProgram.GetProgramID());
		glUniformMatrix4fv(mPosColorProgram.GetCameraToClipTransformUniform(),
			1, GL_FALSE, glm::value_ptr(cameraToClipTransform.Top()));
		glUseProgram(GL_NONE);
	}

	void SnowflakeScene::HandleInput(unsigned char key, int x, int y)
	{
		gViewPole.CharPress(key);
		mSnowflake.HandleInput(key);
	}
}
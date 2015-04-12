#include "SpecularLighting.h"

#include "glm/gtc/type_ptr.hpp"
#include "glutil/MatrixStack.h"
#include "glutil/MousePoles.h"
#include "glm/gtc/quaternion.hpp"
#include "../framework/MousePole.h"
#include "../framework/MathUtil.h"

namespace MyCode
{
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

	glutil::ObjectData gInitialObjectData =
	{
		glm::vec3(0.0f, 0.51f, 0.0f),
		glm::fquat(1.0f, 0.0f, 0.0f, 0.0f),
	};

	glutil::ViewPole gViewPole = glutil::ViewPole(gInitialViewData, gViewScale, glutil::MB_LEFT_BTN);
	glutil::ObjectPole gObjtPole = glutil::ObjectPole(gInitialObjectData, 90.0f / 250.0f,
		glutil::MB_RIGHT_BTN, &gViewPole);

	namespace
	{
		void onMouseClick(int button, int state, int x, int y)
		{
			Framework::ForwardMouseButton(gViewPole, button, state, x, y);
			Framework::ForwardMouseButton(gObjtPole, button, state, x, y);
			glutPostRedisplay();
		}

		void onMouseMoved(int x, int y)
		{
			Framework::ForwardMouseMotion(gViewPole, x, y);
			Framework::ForwardMouseMotion(gObjtPole, x, y);
			glutPostRedisplay();
		}

		void onMouseWheel(int wheel, int direction, int x, int y)
		{
			Framework::ForwardMouseWheel(gObjtPole, wheel, direction, x, y);
			Framework::ForwardMouseWheel(gViewPole, wheel, direction, x, y);
			glutPostRedisplay();
		}
	}

	const float PointLighting::LIGHT_LOOP_DURATION = 5.0f;

	PointLighting::PointLighting()
		: mLightLoopTimer(Framework::Timer::TT_LOOP, LIGHT_LOOP_DURATION)
		, mLightLoopRadius(1.0f)
		, mLightLoopHeight(0.5f)
		, mLightingType(LightingType::VERTEX_LIGHTING)
		, mLightIntensity(1.0f, 1.0f, 1.0f, 1.0f)
		, mAmbientLightIntensity(0.0f, 0.0f, 0.0f, 1.0f)
	{
		ConfigureOpenGL();
		LoadPrograms();
		LoadRenderables();
		InitializeInput();
	}

	PointLighting::~PointLighting()
	{

	}

	void PointLighting::ConfigureOpenGL()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_CLAMP);
	}

	void PointLighting::LoadPrograms()
	{
		mVertexLightingProgram = PointLightingProgramDataPtr(
			new SpecularLightingProgramData("VertexPointLighting.vert", "VertexPointLighting.frag"));

		mFragmentLightingProgram = PointLightingProgramDataPtr(
			new SpecularLightingProgramData("FragmentPointLighting.vert", "FragmentPointLighting.frag"));
	}

	void PointLighting::LoadRenderables()
	{
		mPlaneMesh = MeshPtr(new Mesh("LargePlane.xml")); 
		mCylinderMesh = MeshPtr(new Mesh("UnitCylinder.xml"));
		mLightMesh = MeshPtr(new Mesh("LightCube.xml"));
	}

	void PointLighting::InitializeInput()
	{
		glutMouseFunc(onMouseClick);
		glutMotionFunc(onMouseMoved);
		glutMouseWheelFunc(onMouseWheel);
	}

	void PointLighting::Render()
	{
		mLightLoopTimer.Update();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glutil::MatrixStack modelMatrix;
		modelMatrix.ApplyMatrix(gViewPole.CalcMatrix());
		DrawWithPointLighting(modelMatrix, GetProgramForCurrentLightingType());

		glutSwapBuffers();
		glutPostRedisplay();
	}

	const PointLighting::PointLightingProgramDataPtr& PointLighting::GetProgramForCurrentLightingType()
	{
		switch (mLightingType)
		{
		default:
		case LightingType::VERTEX_LIGHTING:
			return mVertexLightingProgram;
			break;
		case LightingType::FRAGMENT_LIGHTING:
			return mFragmentLightingProgram;
			break;
		}
	}

	void PointLighting::DrawWithPointLighting(glutil::MatrixStack& modelMatrix, const PointLightingProgramDataPtr& program)
	{
		glUseProgram(program->GetProgramID());

		glUniformMatrix4fv(program->GetCameraToClipTransformUniform(), 1, GL_FALSE, glm::value_ptr(mCameraToClipTransform));
		
		glm::vec4 lightPositionInWorldSpace = GetCurrentLightPosition();
		glm::vec4 lightPositionInCameraSpace = modelMatrix.Top() * lightPositionInWorldSpace;
		glUniform4fv(program->GetLightPositionUniform(), 1, glm::value_ptr(lightPositionInCameraSpace));
		
		glUniform4fv(program->GetLightIntensityUniform(), 1, glm::value_ptr(mLightIntensity));
		glUniform4fv(program->GetAmbientLightIntensityUniform(), 1, glm::value_ptr(mAmbientLightIntensity));
		
		const GLint modelToCameraUniform = program->GetModelToCameraTransformUniform();
		DrawPlane(modelMatrix, modelToCameraUniform);
		DrawCylinder(modelMatrix, modelToCameraUniform);
		DrawLightSource(modelMatrix, modelToCameraUniform, lightPositionInWorldSpace);

		glUseProgram(GL_NONE);
	}

	void PointLighting::DrawPlane(glutil::MatrixStack& modelMatrix, GLint modelToCameraUniform)
	{
		glutil::PushStack push(modelMatrix);

		glUniformMatrix4fv(modelToCameraUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		mPlaneMesh->Render();
	}

	void PointLighting::DrawCylinder(glutil::MatrixStack& modelMatrix, GLint modelToCameraUniform)
	{
		glutil::PushStack push(modelMatrix);

		modelMatrix.ApplyMatrix(gObjtPole.CalcMatrix());
		glUniformMatrix4fv(modelToCameraUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		mCylinderMesh->Render("lit-color");
	}

	void PointLighting::DrawLightSource(glutil::MatrixStack& modelMatrix, GLint modelToCameraUniform, 
		const glm::vec4& lightPosition)
	{
		glutil::PushStack push(modelMatrix);

		modelMatrix.Translate(glm::vec3(lightPosition));
		modelMatrix.Scale(0.1f);
		glUniformMatrix4fv(modelToCameraUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		
		mLightMesh->Render("lit");
	}

	void PointLighting::Reshape(GLsizei w, GLsizei h)
	{
		float aspectRatio = static_cast<float>(w) / static_cast<float>(h);
		glutil::MatrixStack cameraMatrix;
		cameraMatrix.Perspective(45.0f, aspectRatio, 1.0f, 300.0f);
		mCameraToClipTransform = cameraMatrix.Top();
	}

	glm::vec4 PointLighting::GetCurrentLightPosition()
	{
		const float currentTime = mLightLoopTimer.GetAlpha();

		glm::vec4 lightPosition(0.0f, mLightLoopHeight, 0.0f, 1.0f);
		lightPosition.x = cosf(currentTime * 2.0f * MyCode::MathUtil::PI) * mLightLoopRadius;
		lightPosition.z = sinf(currentTime * 2.0f * MyCode::MathUtil::PI) * mLightLoopRadius;

		return lightPosition;
	}

	void PointLighting::HandleInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
			case 'h':
				ToggleLightingMode();
			break;
			case ' ':
				ToggleLightingRotation();
			break;
			default:
				UpdateLightLoop(key);
			break;
		}
	}

	void PointLighting::ToggleLightingMode()
	{
		switch (mLightingType)
		{
		case LightingType::VERTEX_LIGHTING:
			mLightingType = LightingType::FRAGMENT_LIGHTING;
			break;
		case LightingType::FRAGMENT_LIGHTING:
			mLightingType = LightingType::VERTEX_LIGHTING;
			break;
		}
	}

	void PointLighting::ToggleLightingRotation()
	{
		mLightLoopTimer.TogglePause();
	}

	void PointLighting::UpdateLightLoop(unsigned char key)
	{
		const float INCREMENT = 0.1f;
		switch (key)
		{
		case 'w':
			mLightLoopRadius -= INCREMENT;
			break;
		case 's':
			mLightLoopRadius += INCREMENT;
			break;
		case 'a':
			mLightLoopHeight -= INCREMENT;
			break;
		case 'd':
			mLightLoopHeight += INCREMENT;
			break;
		}
	}
}
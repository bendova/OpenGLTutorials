#include "BasicLighting.h"
#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/MyMesh.h"
#include "../framework/MathUtil.h"
#include "../framework/MousePole.h"
#include "glm/gtc/type_ptr.hpp"
#include "glutil/MatrixStack.h"
#include "glutil/MousePoles.h"
#include "glm/gtc/quaternion.hpp"
#include "render/Cube.h"
#include "ColorProgramData.h"

#include <vector>

namespace MyCode
{
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

	BasicLighting::BasicLighting()
	{
		Initialize();
		ConfigureOpenGL();
		LoadRenderObjects();
	}

	BasicLighting::~BasicLighting()
	{
	}

	void BasicLighting::Initialize()
	{
		InitPrograms();
		InitTranformationMatrices();
		InitializeInput();
	}

	void BasicLighting::InitPrograms()
	{
		mColorProgramData = std::unique_ptr<ColorProgramData>(new ColorProgramData("PosOnly.vert", "ColorPassthrough.frag"));
	}

	void BasicLighting::InitTranformationMatrices()
	{
		InitCameraToClipTranform();
	}

	void BasicLighting::InitCameraToClipTranform()
	{
		const float zNear = 1.0f;
		const float zFar = 300.0f;

		mFrustumScale = GetFrustumScale(45.0f);

		mCameraToClipTransform[0].x = mFrustumScale;
		mCameraToClipTransform[1].y = mFrustumScale;
		mCameraToClipTransform[2].z = (zNear + zFar) / (zNear - zFar);
		mCameraToClipTransform[2].w = -1.0f;
		mCameraToClipTransform[3].z = (2 * zNear * zFar) / (zNear - zFar);
	}

	float BasicLighting::GetFrustumScale(const float angleInDegrees)
	{
		return 1 / tan(GetAngleInRadians(angleInDegrees) / 2.0f);
	}

	float BasicLighting::GetAngleInRadians(const float angleInDegrees)
	{
		return angleInDegrees * 3.14159f / 180.0f;
	}

	void BasicLighting::InitializeInput()
	{
		glutMouseFunc(onMouseClick);
		glutMotionFunc(onMouseMoved);
		glutMouseWheelFunc(onMouseWheel);
	}

	void BasicLighting::ConfigureOpenGL()
	{
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

	void BasicLighting::LoadRenderObjects()
	{
		LoadMeshes();
		LoadMyCube();
	}

	void BasicLighting::LoadMeshes()
	{
		dCylinderMesh = std::unique_ptr<Mesh>(new Mesh("UnitCylinder.xml"));
		dPlaneMesh = std::unique_ptr<Mesh>(new Mesh("UnitPlane.xml"));
		dCubeMesh = std::unique_ptr<Mesh>(new Mesh("UnitCube.xml"));
	}

	void BasicLighting::LoadMyCube()
	{
		dMyCube = std::unique_ptr<Cube>(new Cube(mColorProgramData->GetVertexPositionAttributeID(), 
			mColorProgramData->GetVertexColorAttributeID()));
	}

	void BasicLighting::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(mColorProgramData->GetProgramID());

		glutil::MatrixStack modelMatrix;
		modelMatrix.SetMatrix(gViewPole.CalcMatrix());

		DrawMyCube(modelMatrix);
		DrawPlane(modelMatrix);
		//DrawCylinder(modelMatrix);
		//DrawCube();

		glUseProgram(GL_NONE);

		glutSwapBuffers();
	}

	void BasicLighting::DrawMyCube(glutil::MatrixStack& modelMatrix)
	{
		glutil::PushStack push(modelMatrix);
		modelMatrix.Translate(glm::vec3(0.0f, 1.01f, 0.0f));
		SetModelToWorldTransform(modelMatrix.Top());
		dMyCube->Render();
	}

	void BasicLighting::DrawCube()
	{
		dCubeMesh->Render();
	}

	void BasicLighting::DrawPlane(glutil::MatrixStack& modelMatrix)
	{
		glutil::PushStack push(modelMatrix);
		modelMatrix.Scale(5.0f);
		SetModelToWorldTransform(modelMatrix.Top());

		dPlaneMesh->Render();
	}

	void BasicLighting::DrawCylinder(glutil::MatrixStack& modelMatrix)
	{
		glutil::PushStack push(modelMatrix);
		modelMatrix.Translate(glm::vec3(0.0f, 0.51f, 0.0f));
		SetModelToWorldTransform(modelMatrix.Top());

		dCylinderMesh->Render("color");
	}

	void BasicLighting::SetModelToWorldTransform(const glm::mat4& modelToWorldTransform)
	{
		mModelToWorldTransform = modelToWorldTransform;
		ApplyMVPTransform();
	}

	void BasicLighting::SetWorldToCameraTransform(const glm::mat4& worldToCameraTransform)
	{
		mWorldToCameraTransform = worldToCameraTransform;
		ApplyMVPTransform();
	}

	void BasicLighting::ApplyMVPTransform()
	{
		UpdateMVPTransform();
		glUniformMatrix4fv(mColorProgramData->GetTransformMVPUniformID(), 1, GL_FALSE, glm::value_ptr(mTransformMVP));
	}

	void BasicLighting::UpdateMVPTransform()
	{
		mTransformMVP = mCameraToClipTransform * mWorldToCameraTransform * mModelToWorldTransform;
	}

	void BasicLighting::Reshape(GLsizei w, GLsizei h)
	{
		OnViewportResized(static_cast<float>(w) / static_cast<float>(h));
	}

	void BasicLighting::OnViewportResized(float aspectRatio)
	{
		mCameraToClipTransform[0].x = mFrustumScale / aspectRatio;
		UpdateMVPTransform();
	}
}


#include "SceneRenderHelper.h"
#include "glutil/MatrixStack.h"
#include "Scene.h"
#include "programs/ILightingProgram.h"

namespace MyCode
{
	SceneRenderHelper::SceneRenderHelper(const Scene& scene)
		: mScene(scene)
		, mGroundMesh(new Mesh("Ground.xml"))
		, mCubeMesh(new Mesh("UnitCube.xml"))
		, mCylinderMesh(new Mesh("UnitCylinder.xml"))
		, mTetrahedronMesh(new Mesh("UnitTetrahedron.xml"))
		, mSphereMesh(new Mesh("UnitSphere.xml"))
		, mLightCubeMesh(new Mesh("LightCube.xml"))
	{
	}

	SceneRenderHelper::~SceneRenderHelper()
	{
	}

	void SceneRenderHelper::Render(const SceneRenderContext& context)
	{
		RenderObjects(context);
	}

	void SceneRenderHelper::RenderObjects(const SceneRenderContext& context)
	{
		RenderSun(context);
		RenderGround(context);
		RenderCylinder(context);
		RenderCube(context);
		RenderTetrahedron(context);
		RenderSphere(context);
		RenderMonolith(context);
		RenderLightCube(context);
	}

	void SceneRenderHelper::RenderSun(const SceneRenderContext& context)
	{
		glutil::PushStack push(context.mModelToCameraTransform);

		glm::vec3 sunPosition(mScene.GetSunPositionInWorldSpace());
		context.mModelToCameraTransform.Translate(sunPosition);
		context.mModelToCameraTransform.Scale(glm::vec3(10.0f, 10.0f, 10.0f));

		RenderObject(mSphereMesh, "lit-color", mScene.GetDiffuseAndSpecularLightingProgram(),
			context.mModelToCameraTransform);
	}

	void SceneRenderHelper::RenderGround(const SceneRenderContext& context)
	{
		glutil::PushStack push(context.mModelToCameraTransform);
		context.mModelToCameraTransform.RotateX(-90.0f);

		RenderObject(mGroundMesh, mScene.GetDiffuseLightingProgram(),
			context.mModelToCameraTransform);
	}

	void SceneRenderHelper::RenderCylinder(const SceneRenderContext& context)
	{
		glutil::PushStack push(context.mModelToCameraTransform);

		context.mModelToCameraTransform.Translate(glm::vec3(-5.0f, 48.0f, -15.0f));
		context.mModelToCameraTransform.Scale(glm::vec3(10.0f, 30.0f, 10.0f));

		RenderObject(mCylinderMesh, "lit-color", mScene.GetDiffuseAndSpecularLightingProgram(),
			context.mModelToCameraTransform);
	}

	void SceneRenderHelper::RenderCube(const SceneRenderContext& context)
	{
		glutil::PushStack push(context.mModelToCameraTransform);

		context.mModelToCameraTransform.Translate(glm::vec3(-80.0f, 20.0f, 42.0f));
		context.mModelToCameraTransform.RotateZ(80.0f);
		context.mModelToCameraTransform.Scale(glm::vec3(10.0f, 10.0f, 10.0f));

		RenderObject(mCubeMesh, "lit-color", mScene.GetDiffuseAndSpecularLightingProgram(),
			context.mModelToCameraTransform);
	}

	void SceneRenderHelper::RenderTetrahedron(const SceneRenderContext& context)
	{
		glutil::PushStack push(context.mModelToCameraTransform);

		context.mModelToCameraTransform.Translate(glm::vec3(80.0f, 22.0f, 90.0f));
		context.mModelToCameraTransform.RotateY(mScene.GetTetrahedronTimer().GetAlpha() * 360.0f);
		context.mModelToCameraTransform.Rotate(glm::vec3(-0.707f, 0.0f, -0.707f), 54.735f);
		context.mModelToCameraTransform.Scale(glm::vec3(10.0f, 10.0f, 10.0f));

		RenderObject(mTetrahedronMesh, "lit-color", mScene.GetDiffuseAndSpecularLightingProgram(),
			context.mModelToCameraTransform);
	}

	void SceneRenderHelper::RenderSphere(const SceneRenderContext& context)
	{
		glutil::PushStack push(context.mModelToCameraTransform);

		context.mModelToCameraTransform.Translate(glm::vec3(85.0f, 10.0f, -85.0f));
		context.mModelToCameraTransform.Scale(glm::vec3(10.0f, 10.0f, 10.0f));
		
		RenderObject(mSphereMesh, "lit-color", mScene.GetDiffuseAndSpecularLightingProgram(),
			context.mModelToCameraTransform);
	}

	void SceneRenderHelper::RenderMonolith(const SceneRenderContext& context)
	{
		glutil::PushStack push(context.mModelToCameraTransform);

		context.mModelToCameraTransform.Translate(glm::vec3(-85.0f, 16.0f, -75.0f));
		context.mModelToCameraTransform.Scale(glm::vec3(10.0f, 20.0f, 6.0f));
		
		RenderObject(mCubeMesh, "lit", mScene.GetDiffuseAndSpecularLightingProgram(),
			context.mModelToCameraTransform);
	}

	void SceneRenderHelper::RenderLightCube(const SceneRenderContext& context)
	{
		glutil::PushStack push(context.mModelToCameraTransform);

		context.mModelToCameraTransform.Translate(glm::vec3(mScene.GetLightCubePositionWS()));

		RenderObject(mLightCubeMesh, "lit-color", mScene.GetDiffuseAndSpecularLightingProgram(),
			context.mModelToCameraTransform);
	}

	void SceneRenderHelper::RenderObject(const MeshPtr& mesh, const ILightingProgram& lightingProgram, 
		const glutil::MatrixStack& modelToCameraTransform)
	{
		RenderObject(mesh, "", lightingProgram, modelToCameraTransform);
	}

	void SceneRenderHelper::RenderObject(const MeshPtr& mesh, const std::string& renderCommand,
		const ILightingProgram& lightingProgram, const glutil::MatrixStack& modelToCameraTransform)
	{
		glUseProgram(lightingProgram.GetProgramID());

		glUniformMatrix4fv(lightingProgram.GetModelToCameraTransformUniform(), 1, GL_FALSE,
			glm::value_ptr(modelToCameraTransform.Top()));

		if (renderCommand.empty())
		{
			mesh->Render();
		}
		else
		{
			mesh->Render(renderCommand);
		}
		glUseProgram(GL_NONE);
	}
}
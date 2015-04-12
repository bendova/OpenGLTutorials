#ifndef _MY_CODE_SCENE_RENDER_HELPER_H
#define _MY_CODE_SCENE_RENDER_HELPER_H

#include "glm/glm.hpp"
#include "../framework/MyMesh.h"

namespace glutil
{
	class MatrixStack;
}

namespace MyCode
{
	class Scene;
	struct SceneRenderContext;
	class ILightingProgram;

	class SceneRenderHelper
	{
	public:
		explicit SceneRenderHelper(const Scene& scene);
		~SceneRenderHelper();

		void Render(const SceneRenderContext& context);
	private:
		typedef std::unique_ptr<MyCode::Mesh> MeshPtr;

		void RenderObjects(const SceneRenderContext& context);
		void RenderSun(const SceneRenderContext& context);
		void RenderGround(const SceneRenderContext& context);
		void RenderCylinder(const SceneRenderContext& context);
		void RenderCube(const SceneRenderContext& context);
		void RenderTetrahedron(const SceneRenderContext& context);
		void RenderSphere(const SceneRenderContext& context); 
		void RenderMonolith(const SceneRenderContext& context);
		void RenderLightCube(const SceneRenderContext& context);
		
		void RenderObject(const MeshPtr& mesh, const ILightingProgram& lightingProgram,
			const glutil::MatrixStack& modelToCameraTransform);
		void RenderObject(const MeshPtr& mesh, const std::string& renderCommand, 
			const ILightingProgram& lightingProgram,
			const glutil::MatrixStack& modelToCameraTransform);

		MeshPtr mGroundMesh;
		MeshPtr mCubeMesh;
		MeshPtr mCylinderMesh;
		MeshPtr mTetrahedronMesh;
		MeshPtr mSphereMesh;
		MeshPtr mLightCubeMesh;

		const Scene& mScene;
	};

	struct SceneRenderContext
	{
	public:
		explicit SceneRenderContext(glutil::MatrixStack& modelToCameraTransform)
			: mModelToCameraTransform(modelToCameraTransform)
		{
		}

		glutil::MatrixStack& mModelToCameraTransform;
	};
}
#endif//_MY_CODE_SCENE_RENDER_HELPER_H
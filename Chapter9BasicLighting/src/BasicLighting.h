#ifndef _MY_CODE_BASIC_LIGHTING_H_
#define _MY_CODE_BASIC_LIGHTING_H_

#include <memory>
#include <glload/gl_3_3.h>
#include "glm/glm.hpp"

namespace glutil
{
	class MatrixStack;
}

namespace MyCode
{
	class Cube;
	class Mesh;
	class ColorProgramData;

	class BasicLighting
	{
	public:
		BasicLighting();
		~BasicLighting();

		void Render();
		void Reshape(GLsizei w, GLsizei h);
	private:
		void Initialize();
		void InitPrograms();
		
		void InitTranformationMatrices();
		void InitCameraToClipTranform();
		float GetFrustumScale(const float angleInDegrees);
		float GetAngleInRadians(const float angleInDegrees);

		void LoadRenderObjects();
		void LoadMeshes();
		void LoadMyCube();

		void InitializeInput();
		void ConfigureOpenGL();

		void DrawMyCube(glutil::MatrixStack& modelMatrix);
		void DrawCube();
		void DrawPlane(glutil::MatrixStack& modelMatrix);
		void DrawCylinder(glutil::MatrixStack& modelMatrix);

		void SetModelToWorldTransform(const glm::mat4& modelToWorldTransform);
		void SetWorldToCameraTransform(const glm::mat4& worldToCameraTransform);
		void ApplyMVPTransform();
		void UpdateMVPTransform();

		void OnViewportResized(const float aspectRatio);

		float mFrustumScale;
		glm::mat4 mModelToWorldTransform;
		glm::mat4 mWorldToCameraTransform;
		glm::mat4 mCameraToClipTransform;
		glm::mat4 mTransformMVP;

		std::unique_ptr<ColorProgramData> mColorProgramData;

		std::unique_ptr<Mesh> dCubeMesh;
		std::unique_ptr<Mesh> dCylinderMesh;
		std::unique_ptr<Mesh> dPlaneMesh;
		std::unique_ptr<Cube> dMyCube;
	};
}

#endif //_MY_CODE_BASIC_LIGHTING_H_
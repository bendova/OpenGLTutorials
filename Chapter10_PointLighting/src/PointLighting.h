#ifndef _MY_CODE_POINT_LIGHTING_H
#define _MY_CODE_POINT_LIGHTING_H

#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include <memory>
#include "glm/glm.hpp"
#include "../framework/MyMesh.h"
#include "../framework/Timer.h"
#include "PointLightingProgramData.h"

namespace glutil
{
	class MatrixStack;
}

namespace MyCode
{
	class PointLighting
	{
	public:
		PointLighting();
		~PointLighting();

		void Render();
		void Reshape(GLsizei  w, GLsizei h);
		void HandleInput(unsigned char key, int x, int y);

	private:
		typedef std::unique_ptr<Mesh> MeshPtr;
		typedef std::unique_ptr<PointLightingProgramData> PointLightingProgramDataPtr;

		void ConfigureOpenGL();
		void LoadPrograms(); 
		void LoadRenderables();
		void InitializeInput();
		
		void DrawWithPointLighting(glutil::MatrixStack& modelMatrix, const PointLightingProgramDataPtr& program);
		void DrawWithFragmentPointLighting(glutil::MatrixStack& modelMatrix);
		void DrawPlane(glutil::MatrixStack& modelMatrix, GLint modelToCameraTransform);
		void DrawCylinder(glutil::MatrixStack& modelMatrix, GLint modelToCameraTransform);
		void DrawLightSource(glutil::MatrixStack& modelMatrix, GLint modelToCameraTransform, const glm::vec4& lightPosition);

		glm::vec4 GetCurrentLightPosition();
		const PointLightingProgramDataPtr& GetProgramForCurrentLightingType();

		void ToggleLightingMode();
		void ToggleLightingRotation();
		void UpdateLightLoop(unsigned char key);

		static const float LIGHT_LOOP_DURATION;
		Framework::Timer mLightLoopTimer;
		float mLightLoopRadius;
		float mLightLoopHeight;

		enum class LightingType
		{
			VERTEX_LIGHTING,
			FRAGMENT_LIGHTING,
		};
		LightingType mLightingType;

		MeshPtr mPlaneMesh;
		MeshPtr mCylinderMesh;
		MeshPtr mLightMesh;

		PointLightingProgramDataPtr mVertexLightingProgram;
		PointLightingProgramDataPtr mFragmentLightingProgram;

		glm::mat4 mCameraToClipTransform;

		glm::vec4 mLightIntensity;
		glm::vec4 mAmbientLightIntensity;
	};
}

#endif //_MY_CODE_POINT_LIGHTING_H
#ifndef _MY_CODE_SCENE_H_
#define _MY_CODE_SCENE_H_

#include <memory>
#include "glm/glm.hpp"
#include "../framework/MyMesh.h"
#include "../framework/Timer.h"
#include "SceneRenderHelper.h"

namespace glutil
{
	class MatrixStack;
}

namespace MyCode
{
	class ILightingProgram;
	class NoLightingProgram;
	class DiffuseLightingProgram;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void Render(glutil::MatrixStack& modelToCameraTransform);

		void SetCameraToClipTransform(const glm::mat4& cameraToClipTransform)
		{
			mCameraToClipTransform = cameraToClipTransform;
		}

		void HandleInput(unsigned char key);

		const glm::vec4& 
		GetSunPositionInWorldSpace() const
		{
			return mSunPositionWS;
		}
		const glm::vec4& 
		GetLightCubePositionWS() const
		{
			return mLightCubePositionWS;
		}
		const Framework::Timer& 
		GetTetrahedronTimer() const
		{
			return mTetrahedronTimer;
		}
		const ILightingProgram& GetNoLightingProgram() const;
		const ILightingProgram& GetDiffuseLightingProgram() const;
		const ILightingProgram& GetDiffuseAndSpecularLightingProgram() const;
	private:
		typedef std::unique_ptr<MyCode::Mesh> MeshPtr;
		typedef std::unique_ptr<MyCode::NoLightingProgram> NoLightingProgramPtr;
		typedef std::unique_ptr<MyCode::DiffuseLightingProgram> DiffuseLightingProgramPtr;

		void UpdateTimers();
		void UpdateSunPosition();

		void UpdateDiffuseLighting(glutil::MatrixStack& modelToCameraTransform);
		void SetSunLightUniforms(const glm::mat4& modelToCameraTransform);
		void SetCubeLightUniforms(const glm::mat4& modelToCameraTransform);
		void SetAmbientLightUniforms();

		const float GetSunLightIntensityModifier();
		const float GetAmbientLightIntensityModifier();
		glm::vec4 GetDirectionToSun(const glm::mat4& modelToCameraTransform);

		SceneRenderHelper mSceneRenderHelper;

		glm::mat4 mCameraToClipTransform;
		glm::vec4 mSunLightIntensity;
		glm::vec4 mCubeLightIntensity;
		glm::vec4 mAmbientLightIntensity;
		glm::vec4 mSunPositionWS;
		glm::vec4 mLightCubePositionWS;
		NoLightingProgramPtr mNoLightingProgram;
		DiffuseLightingProgramPtr mDiffuseLightingProgram;

		Framework::Timer mTetrahedronTimer;
		Framework::Timer mSunTimer;

		static const float SUN_ORBIT_RADIUS;
		static const float DAY_TIME_CYCLE_LENGTH;
		static const float TETRAHEDRON_LOOP_TIME;

		static const float SUNRISE_TIME;
		static const float MIDDAY_TIME;
		static const float SUNSET_TIME;
		static const float MIDNIGHT_TIME;

		static const float LIGHT_INTENSITY_MODIFIER_MIN;
		static const float LIGHT_INTENSITY_MODIFIER_MAX;
	};
}

#endif //_MY_CODE_SCENE_H_
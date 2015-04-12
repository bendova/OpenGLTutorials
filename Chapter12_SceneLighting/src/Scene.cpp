#include "Scene.h"
#include "glutil/MatrixStack.h"
#include "programs/NoLightingProgram.h"
#include "programs/DiffuseLightingProgram.h"
#include "../framework/MathUtil.h"

namespace MyCode
{
	const float Scene::SUN_ORBIT_RADIUS = 300.0f;
	const float Scene::DAY_TIME_CYCLE_LENGTH = 30.0f;
	const float Scene::TETRAHEDRON_LOOP_TIME = 5.0f;

	const float Scene::SUNRISE_TIME = 0.0f;
	const float Scene::MIDDAY_TIME = 0.25f;
	const float Scene::SUNSET_TIME = 0.5f;
	const float Scene::MIDNIGHT_TIME = 0.75f;

	const float Scene::LIGHT_INTENSITY_MODIFIER_MIN = 0.0f;
	const float Scene::LIGHT_INTENSITY_MODIFIER_MAX = 1.0f;

	Scene::Scene()
		: mSunLightIntensity(0.8f, 0.8f, 0.8f, 1.0f)
		, mCubeLightIntensity(0.8f, 0.8f, 0.8f, 1.0f)
		, mAmbientLightIntensity(0.2f, 0.2f, 0.2f, 1.0f)
		, mSunPositionWS()
		, mLightCubePositionWS(0.0f, 40.0f, 0.0f, 1.0f)
		, mNoLightingProgram(new NoLightingProgram("NoLighting.vert", "NoLighting.frag"))
		, mDiffuseLightingProgram(new DiffuseLightingProgram("DiffuseLighting.vert", "DiffuseLighting.frag"))
		, mTetrahedronTimer(Framework::Timer::TT_LOOP, 5.0f)
		, mSunTimer(Framework::Timer::TT_LOOP, DAY_TIME_CYCLE_LENGTH)
		, mSceneRenderHelper(*this)
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::Render(glutil::MatrixStack& modelToCameraTransform)
	{
		UpdateTimers();
		UpdateSunPosition();

		UpdateDiffuseLighting(modelToCameraTransform);
		mSceneRenderHelper.Render(SceneRenderContext(modelToCameraTransform));
	}

	void Scene::UpdateTimers()
	{
		mTetrahedronTimer.Update();
		mSunTimer.Update();
	}

	void Scene::UpdateSunPosition()
	{
		const float dayTime = mSunTimer.GetAlpha();

		mSunPositionWS.x = cosf(dayTime * 2 * MyCode::MathUtil::PI) * SUN_ORBIT_RADIUS;
		mSunPositionWS.y = sinf(dayTime * 2 * MyCode::MathUtil::PI) * SUN_ORBIT_RADIUS;
	}

	void Scene::UpdateDiffuseLighting(glutil::MatrixStack& modelToCameraTransform)
	{
		glUseProgram(mDiffuseLightingProgram->GetProgramID());

		const glm::mat4& mtc = modelToCameraTransform.Top();
		SetSunLightUniforms(mtc);
		SetCubeLightUniforms(mtc);
		SetAmbientLightUniforms();

		glUniformMatrix4fv(mDiffuseLightingProgram->GetCameraToClipTransformUniform(), 1, GL_FALSE, 
			glm::value_ptr(mCameraToClipTransform));
		
		glUseProgram(GL_NONE);
	}

	void Scene::SetSunLightUniforms(const glm::mat4& modelToCameraTransform)
	{
		glm::vec4 directionToSun = GetDirectionToSun(modelToCameraTransform);
		glUniform4fv(mDiffuseLightingProgram->GetDirectionToSunUniform(), 1, 
			glm::value_ptr(directionToSun));

		glUniform4fv(mDiffuseLightingProgram->GetSunLightIntensityUniform(), 1,
			glm::value_ptr(mSunLightIntensity * GetSunLightIntensityModifier()));
	}

	glm::vec4 Scene::GetDirectionToSun(const glm::mat4& modelToCameraTransform)
	{
		glm::vec4 vectorToSun(mSunPositionWS);
		vectorToSun.w = 0.0f;
		glm::vec4 directionToSun = (modelToCameraTransform * vectorToSun);
		directionToSun = glm::normalize(directionToSun);

		return directionToSun;
	}

	void Scene::SetCubeLightUniforms(const glm::mat4& modelToCameraTransform)
	{
		const glm::vec4 lightCubePositionCS = modelToCameraTransform * mLightCubePositionWS;

		glUniform4fv(mDiffuseLightingProgram->GetCubeLightPositionUniform(), 1,
			glm::value_ptr(lightCubePositionCS));
		glUniform4fv(mDiffuseLightingProgram->GetCubeLightIntensityUniform(), 1,
			glm::value_ptr(mCubeLightIntensity));
	}

	void Scene::SetAmbientLightUniforms()
	{
		glUniform4fv(mDiffuseLightingProgram->GetAmbientLightIntensityUniform(), 1,
			glm::value_ptr(mAmbientLightIntensity * GetAmbientLightIntensityModifier()));
	}

	const float Scene::GetSunLightIntensityModifier()
	{
		const float dayTime = mSunTimer.GetAlpha();
		const float lightIntensityModifier = (dayTime <= 0.5) ? LIGHT_INTENSITY_MODIFIER_MAX : LIGHT_INTENSITY_MODIFIER_MIN;
		return lightIntensityModifier;
	}

	const float Scene::GetAmbientLightIntensityModifier()
	{
		const float dayTime = mSunTimer.GetAlpha();
		float lightIntensityModifier;
		if (dayTime <= SUNSET_TIME)
		{
			lightIntensityModifier = LIGHT_INTENSITY_MODIFIER_MAX - (abs(dayTime - MIDDAY_TIME) / MIDDAY_TIME);
		}
		else
		{
			lightIntensityModifier = LIGHT_INTENSITY_MODIFIER_MIN;
		}
		return lightIntensityModifier;
	}

	void Scene::HandleInput(unsigned char key)
	{
		const float SUN_TIMER_INC = 0.05f;
		const float LIGHT_CUBE_POS_INC = 0.5f;

		switch (key)
		{
		case ' ':
			mSunTimer.TogglePause();
			break;
		case '-':
			mSunTimer.Rewind(SUN_TIMER_INC);
			break;
		case '+':
		case '=':
			mSunTimer.Fastforward(SUN_TIMER_INC);
			break;
		case 'i':
			mLightCubePositionWS.x += LIGHT_CUBE_POS_INC;
			break;
		case 'k':
			mLightCubePositionWS.x -= LIGHT_CUBE_POS_INC;
			break;
		case 'j':
			mLightCubePositionWS.z -= LIGHT_CUBE_POS_INC;
			break;
		case 'l':
			mLightCubePositionWS.z += LIGHT_CUBE_POS_INC;
			break;
		case 'u':
			mLightCubePositionWS.y -= LIGHT_CUBE_POS_INC;
			break;
		case 'o':
			mLightCubePositionWS.y += LIGHT_CUBE_POS_INC;
			break;
		}
	}

	const ILightingProgram& Scene::GetNoLightingProgram() const
	{
		return *mNoLightingProgram;
	}

	const ILightingProgram& Scene::GetDiffuseLightingProgram() const
	{
		return *mDiffuseLightingProgram;
	}

	const ILightingProgram& Scene::GetDiffuseAndSpecularLightingProgram() const
	{
		return *mDiffuseLightingProgram;
	}
}
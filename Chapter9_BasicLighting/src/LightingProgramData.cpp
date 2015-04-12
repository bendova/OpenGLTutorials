#include "LightingProgramData.h"

namespace MyCode
{
	LightingProgramData::LightingProgramData(const std::string& vertexShader, const std::string& fragmentShader)
		: BaseProgramData(vertexShader, fragmentShader)
	{
		InitVariablesLocations();
	}

	LightingProgramData::~LightingProgramData()
	{}

	void LightingProgramData::InitVariablesLocations()
	{
		mModelToCameraMatrixUniform = glGetUniformLocation(mProgramID, "modelToCameraMatrix");
		mCameraToClipMatrixUniform = glGetUniformLocation(mProgramID, "cameraToClipMatrix");
		mDirectionToLightUniform = glGetUniformLocation(mProgramID, "directionToLight"); 
		mAmbientLightIntensityUniform = glGetUniformLocation(mProgramID, "ambientLightIntensity");
		mLightIntensityUniform = glGetUniformLocation(mProgramID, "lightIntensity");
	}
}
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
		mNormalModelToCameraMatrixUniform = glGetUniformLocation(mProgramID, "normalModelToCameraMatrix");
		mDirectionToLightUniform = glGetUniformLocation(mProgramID, "directionToLight");
		mLightIntensityUniform = glGetUniformLocation(mProgramID, "lightIntensity");
	}
}
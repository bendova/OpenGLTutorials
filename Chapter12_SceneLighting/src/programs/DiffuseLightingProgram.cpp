#include "DiffuseLightingProgram.h"

namespace MyCode
{
	DiffuseLightingProgram::DiffuseLightingProgram(const std::string& vertexShader, const std::string& fragmentShader)
		: BaseProgramData(vertexShader, fragmentShader)
	{
		InitUniform(mModelToCameraTranformUniform, "modelToCameraTransform"); 
		InitUniform(mCameraToClipTranformUniform, "cameraToClipTransform");
		InitUniform(mDirectionToSunUniform, "directionToSun");
		InitUniform(mSunLightIntensityUniform, "sunLightIntensity");
		InitUniform(mCubeLightPositionUniform, "cubeLightPosition");
		InitUniform(mCubeLightIntensityUniform, "cubeLightIntensity");
		InitUniform(mAmbientLightIntensityUniform, "ambientLightIntensity");
	}

	DiffuseLightingProgram::~DiffuseLightingProgram()
	{

	}
}
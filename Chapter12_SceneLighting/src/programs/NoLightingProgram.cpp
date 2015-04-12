#include "NoLightingProgram.h"

namespace MyCode
{
	NoLightingProgram::NoLightingProgram(const std::string& vertexShader, const std::string& fragmentShader)
		: BaseProgramData(vertexShader, fragmentShader)
	{
		InitUniform(mModelToCameraTranformUniform, "modelToCameraTransform");
		InitUniform(mCameraToClipTranformUniform, "cameraToClipTransform");
	}

	NoLightingProgram::~NoLightingProgram()
	{

	}
}
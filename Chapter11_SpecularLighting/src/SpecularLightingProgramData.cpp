#include "SpecularLightingProgramData.h"

namespace MyCode
{
	SpecularLightingProgramData::SpecularLightingProgramData(const std::string& vertexShader, const std::string& fragmentShader)
		: BaseProgramData(vertexShader, fragmentShader)
	{
		InitUniform(mModelToCameraTranformUniform,	"modelToCameraTransform");
		InitUniform(mCameraToClipTranformUniform,	"cameraToClipTransform");
		InitUniform(mLightPositionUniform,			"lightPosition");
		InitUniform(mLightIntensityUniform,			"lightIntensity");
		InitUniform(mAmbientLightIntensityUniform,	"ambientLightIntensity");
	}

	SpecularLightingProgramData::~SpecularLightingProgramData()
	{

	}
}
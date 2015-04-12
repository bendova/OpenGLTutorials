#include "PointLightingProgramData.h"

namespace MyCode
{
	PointLightingProgramData::PointLightingProgramData(const std::string& vertexShader, const std::string& fragmentShader)
		: BaseProgramData(vertexShader, fragmentShader)
	{
		InitUniform(mModelToCameraTranformUniform,	"modelToCameraTransform");
		InitUniform(mCameraToClipTranformUniform,	"cameraToClipTransform");
		InitUniform(mLightPositionUniform,			"lightPosition");
		InitUniform(mLightIntensityUniform,			"lightIntensity");
		InitUniform(mAmbientLightIntensityUniform,	"ambientLightIntensity");
	}

	PointLightingProgramData::~PointLightingProgramData()
	{

	}

	void PointLightingProgramData::InitUniform(GLint& uniform, const char* uniformName)
	{
		uniform = glGetUniformLocation(mProgramID, uniformName);
	}
}
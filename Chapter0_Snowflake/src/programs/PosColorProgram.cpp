#include "PosColorProgram.h"

namespace MyCode
{
	PosColorProgram::PosColorProgram(const std::string& vertexShader, const std::string& fragmentShader)
		: BaseProgramData(vertexShader, fragmentShader)
	{
		InitAttribute(mPositionAttributeID, "position");
		InitAttribute(mColorAttributeID, "color");
		InitUniform(mModelToCameraTranformUniform, "modelToCameraTransform");
		InitUniform(mCameraToClipTranformUniform, "cameraToClipTransform");
	}

	PosColorProgram::~PosColorProgram()
	{

	}
}
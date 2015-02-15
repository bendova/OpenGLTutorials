#include "ColorProgramData.h"

namespace MyCode
{
	ColorProgramData::ColorProgramData(const std::string& vertexShader, const std::string& fragmentShader)
		: BaseProgramData(vertexShader, fragmentShader)
	{
		InitVariablesLocations();
	}

	ColorProgramData::~ColorProgramData()
	{}

	void ColorProgramData::InitVariablesLocations()
	{
		mVertexPositionAttributeID = glGetAttribLocation(mProgramID, "position");
		mVertexColorAttributeID = glGetAttribLocation(mProgramID, "color");
		mTransformMVPUniformID = glGetUniformLocation(mProgramID, "mvpTransform");
	}
}
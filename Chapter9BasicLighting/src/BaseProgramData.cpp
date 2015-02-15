#include "BaseProgramData.h"

#include <vector>
#include "../framework/framework.h"

namespace MyCode
{
	BaseProgramData::BaseProgramData(const std::string& vertexShader, const std::string& fragmentShader)
	{
		CreateProgram(vertexShader, fragmentShader);
	}

	BaseProgramData::~BaseProgramData()
	{

	}

	void BaseProgramData::CreateProgram(const std::string& vertexShader, const std::string& fragmentShader)
	{
		std::vector<GLuint> shaderList;

		shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
		shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

		mProgramID = Framework::CreateProgram(shaderList);
	}
}
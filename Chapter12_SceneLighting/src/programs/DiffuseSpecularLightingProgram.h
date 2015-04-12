#ifndef _MY_CODE_DIFFUSE_SPECULAR_LIGHTING_PROGRAM_H_
#define _MY_CODE_DIFFUSE_SPECULAR_LIGHTING_PROGRAM_H_

#include "../framework/BaseProgramData.h"
#include "ILightingProgram.h"

namespace MyCode
{
	class DiffuseSpecularLightingProgram // TODO
		: public BaseProgramData
		, public ILightingProgram
	{
	public:
		DiffuseSpecularLightingProgram(const std::string& vertexShader, const std::string& fragmentShader);
		virtual ~DiffuseSpecularLightingProgram();
	};
}

#endif //_MY_CODE_DIFFUSE_SPECULAR_LIGHTING_PROGRAM_H_
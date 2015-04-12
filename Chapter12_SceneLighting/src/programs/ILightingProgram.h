#ifndef _MY_CODE_I_LIGHTING_PROGRAM_H_
#define _MY_CODE_I_LIGHTING_PROGRAM_H_

#include "../framework/IBaseProgramData.h"

namespace MyCode
{
	class ILightingProgram : public IBaseProgramData
	{
	public:
		ILightingProgram() {}
		virtual ~ILightingProgram() {}

		virtual GLint GetModelToCameraTransformUniform() const = 0;
		virtual GLint GetCameraToClipTransformUniform() const = 0;
	};
}

#endif //_MY_CODE_I_LIGHTING_PROGRAM_H_
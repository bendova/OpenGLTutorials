#ifndef _MY_CODE_NO_LIGHTING_PROGRAM_H_
#define _MY_CODE_NO_LIGHTING_PROGRAM_H_

#include "../framework/BaseProgramData.h"
#include "ILightingProgram.h"

namespace MyCode
{
	class NoLightingProgram 
		: public BaseProgramData
		, public ILightingProgram
	{
	public:
		NoLightingProgram(const std::string& vertexShader, const std::string& fragmentShader);
		virtual ~NoLightingProgram();

		virtual GLuint GetProgramID() const
		{
			return mProgramID;
		}
		virtual GLint GetModelToCameraTransformUniform() const
		{
			return mModelToCameraTranformUniform; 
		}
		virtual GLint GetCameraToClipTransformUniform()	const
		{ 
			return mCameraToClipTranformUniform;	
		}

	private:
		GLint mModelToCameraTranformUniform;
		GLint mCameraToClipTranformUniform;
	};
}

#endif //_MY_CODE_NO_LIGHTING_PROGRAM_H_
#ifndef _MY_CODE_NO_LIGHTING_PROGRAM_H_
#define _MY_CODE_NO_LIGHTING_PROGRAM_H_

#include "../framework/BaseProgramData.h"

namespace MyCode
{
	class PosColorProgram 
		: public BaseProgramData
	{
	public:
		PosColorProgram(const std::string& vertexShader, const std::string& fragmentShader);
		virtual ~PosColorProgram();
		
		GLint GetPositionAttributeID() const
		{
			return mPositionAttributeID;
		}
		GLint GetColorAttributeID() const
		{
			return mColorAttributeID;
		}
		GLint GetModelToCameraTransformUniform() const
		{
			return mModelToCameraTranformUniform; 
		}
		GLint GetCameraToClipTransformUniform()	const
		{ 
			return mCameraToClipTranformUniform;	
		}

	private:
		GLint mPositionAttributeID;
		GLint mColorAttributeID;
		GLint mModelToCameraTranformUniform;
		GLint mCameraToClipTranformUniform;
	};
}

#endif //_MY_CODE_NO_LIGHTING_PROGRAM_H_
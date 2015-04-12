#ifndef _MY_CODE_DIFFUSE_LIGHTING_PROGRAM_H_
#define _MY_CODE_DIFFUSE_LIGHTING_PROGRAM_H_

#include "../framework/BaseProgramData.h"
#include "ILightingProgram.h"

namespace MyCode
{
	class DiffuseLightingProgram 
		: public BaseProgramData
		, public ILightingProgram
	{
	public:
		DiffuseLightingProgram(const std::string& vertexShader, const std::string& fragmentShader);
		virtual ~DiffuseLightingProgram();

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
		GLint GetDirectionToSunUniform() const
		{ 
			return mDirectionToSunUniform;			
		}
		GLint GetSunLightIntensityUniform() const
		{ 
			return mSunLightIntensityUniform;			
		}
		GLint GetCubeLightPositionUniform() const
		{
			return mCubeLightPositionUniform;
		}
		GLint GetCubeLightIntensityUniform() const
		{
			return mCubeLightIntensityUniform;
		}
		GLint GetAmbientLightIntensityUniform()	const
		{ 
			return mAmbientLightIntensityUniform;		
		}

	private:
		GLint mModelToCameraTranformUniform;
		GLint mCameraToClipTranformUniform;
		GLint mDirectionToSunUniform;
		GLint mSunLightIntensityUniform;
		GLint mCubeLightPositionUniform;
		GLint mCubeLightIntensityUniform;
		GLint mAmbientLightIntensityUniform;
	};
}

#endif //_MY_CODE_DIFFUSE_LIGHTING_PROGRAM_H_
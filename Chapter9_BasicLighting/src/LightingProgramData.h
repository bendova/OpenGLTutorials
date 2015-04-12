#ifndef _MY_CODE_LIGHTING_PROGRAM_DATA_H
#define _MY_CODE_LIGHTING_PROGRAM_DATA_H

#include "../framework/BaseProgramData.h"

namespace MyCode
{
	class LightingProgramData: public BaseProgramData
	{
	public:
		LightingProgramData(const std::string& vertexShader, const std::string& fragmentShader);
		virtual ~LightingProgramData();

		GLuint GetModelToCameraMatrixUniform()		{ return mModelToCameraMatrixUniform;	}
		GLuint GetCameraToClipMatrixUniform()		{ return mCameraToClipMatrixUniform;	}
		GLuint GetDirectionToLightUniform()			{ return mDirectionToLightUniform;		}
		GLuint GetAmbientLightIntensityUniform()	{ return mAmbientLightIntensityUniform; }
		GLuint GetLightIntensityUniform()			{ return mLightIntensityUniform;		}

	private:
		void InitVariablesLocations();

		GLuint mModelToCameraMatrixUniform;
		GLuint mCameraToClipMatrixUniform;
		GLuint mDirectionToLightUniform;
		GLuint mAmbientLightIntensityUniform;
		GLuint mLightIntensityUniform;
	};
}

#endif // _MY_CODE_LIGHTING_PROGRAM_DATA_H
#ifndef _MY_CODE_LIGHTING_PROGRAM_DATA_H
#define _MY_CODE_LIGHTING_PROGRAM_DATA_H

#include "BaseProgramData.h"

namespace MyCode
{
	class LightingProgramData: public BaseProgramData
	{
	public:
		LightingProgramData(const std::string& vertexShader, const std::string& fragmentShader);
		virtual ~LightingProgramData();

		GLuint GetModelToCameraMatrixUniform() { return mModelToCameraMatrixUniform; }
		GLuint GetNormalModelToCameraMatrixUniform() { return mNormalModelToCameraMatrixUniform; }
		GLuint GetDirectionToLightUniform() { return mDirectionToLightUniform; }
		GLuint GetLightIntensityUniform() { return mLightIntensityUniform; }

	private:
		void InitVariablesLocations();

		GLuint mModelToCameraMatrixUniform;
		GLuint mNormalModelToCameraMatrixUniform;
		GLuint mDirectionToLightUniform;
		GLuint mLightIntensityUniform;
	};
}

#endif // _MY_CODE_LIGHTING_PROGRAM_DATA_H
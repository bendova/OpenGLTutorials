#ifndef _MY_CODE_POINT_LIGHTING_PROGRAM_DATA_H
#define _MY_CODE_POINT_LIGHTING_PROGRAM_DATA_H

#include "../framework/BaseProgramData.h"
#include <string>

namespace MyCode
{
	class SpecularLightingProgramData : public BaseProgramData
	{
	public:
		SpecularLightingProgramData(const std::string& vertexShader, const std::string& fragmentShader);
		virtual ~SpecularLightingProgramData();

		GLint GetModelToCameraTransformUniform()	{ return mModelToCameraTranformUniform;		}
		GLint GetCameraToClipTransformUniform()		{ return mCameraToClipTranformUniform;		}
		GLint GetLightPositionUniform()				{ return mLightPositionUniform;				}
		GLint GetLightIntensityUniform()			{ return mLightIntensityUniform;			}
		GLint GetAmbientLightIntensityUniform()		{ return mAmbientLightIntensityUniform;		}

	private:

		GLint mModelToCameraTranformUniform;
		GLint mCameraToClipTranformUniform;
		GLint mLightPositionUniform;
		GLint mLightIntensityUniform;
		GLint mAmbientLightIntensityUniform;
	};
}

#endif //_MY_CODE_POINT_LIGHTING_PROGRAM_DATA_H
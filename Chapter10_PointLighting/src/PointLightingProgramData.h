#ifndef _MY_CODE_POINT_LIGHTING_PROGRAM_DATA_H
#define _MY_CODE_POINT_LIGHTING_PROGRAM_DATA_H

#include "../framework/BaseProgramData.h"
#include <string>

namespace MyCode
{
	class PointLightingProgramData : public BaseProgramData
	{
	public:
		PointLightingProgramData(const std::string& vertexShader, const std::string& fragmentShader);
		virtual ~PointLightingProgramData();

		GLint GetModelToCameraTransformUniform()	{ return mModelToCameraTranformUniform;		}
		GLint GetCameraToClipTransformUniform()		{ return mCameraToClipTranformUniform;		}
		GLint GetLightPositionUniform()				{ return mLightPositionUniform;				}
		GLint GetLightIntensityUniform()			{ return mLightIntensityUniform;			}
		GLint GetAmbientLightIntensityUniform()		{ return mAmbientLightIntensityUniform;		}

	private:
		void InitUniform(GLint& uniform, const char* uniformName);

		GLint mModelToCameraTranformUniform;
		GLint mCameraToClipTranformUniform;
		GLint mLightPositionUniform;
		GLint mLightIntensityUniform;
		GLint mAmbientLightIntensityUniform;
	};
}

#endif //_MY_CODE_POINT_LIGHTING_PROGRAM_DATA_H
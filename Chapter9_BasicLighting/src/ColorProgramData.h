#ifndef _MY_CODE_COLOR_PROGRAM_DATA_H
#define _MY_CODE_COLOR_PROGRAM_DATA_H

#include "../framework/BaseProgramData.h"

namespace MyCode
{
	class ColorProgramData: public BaseProgramData
	{
	public:
		ColorProgramData(const std::string& vertexShader, const std::string& fragmentShader);
		virtual ~ColorProgramData();

		GLuint GetVertexPositionAttributeID() { return mVertexPositionAttributeID; }
		GLuint GetVertexColorAttributeID() { return mVertexColorAttributeID; }
		GLuint GetTransformMVPUniformID() { return mTransformMVPUniformID; }

	private:
		void InitVariablesLocations();

		GLint mVertexPositionAttributeID;
		GLint mVertexColorAttributeID;
		GLint mTransformMVPUniformID;
	};
}

#endif // _MY_CODE_COLOR_PROGRAM_DATA_H
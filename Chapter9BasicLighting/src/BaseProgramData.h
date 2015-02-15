#ifndef _MY_CODE_BASE_PROGRAM_DATA_H_
#define _MY_CODE_BASE_PROGRAM_DATA_H_

#include <string>
#include <glload/gl_3_3.h>

namespace MyCode
{ 
	class BaseProgramData
	{
	public:
		BaseProgramData(const std::string& vertexShader, const std::string& fragmentShader);
		virtual ~BaseProgramData();

		GLuint GetProgramID() { return mProgramID; }

	protected:
		GLuint mProgramID;

	private:
		void CreateProgram(const std::string& vertexShader, const std::string& fragmentShader);
	};
}

#endif // _MY_CODE_BASE_PROGRAM_DATA_H_
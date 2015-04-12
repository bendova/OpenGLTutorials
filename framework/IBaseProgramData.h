#ifndef _MY_CODE_IBASE_PROGRAM_DATA_H_
#define _MY_CODE_IBASE_PROGRAM_DATA_H_

#include <glload/gl_3_3.h>

namespace MyCode
{ 
	class IBaseProgramData
	{
	public:
		IBaseProgramData() {}
		virtual ~IBaseProgramData() {}

		virtual GLuint GetProgramID() const = 0;
	};
}

#endif // _MY_CODE_IBASE_PROGRAM_DATA_H_
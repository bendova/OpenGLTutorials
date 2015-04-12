#ifndef _MY_CODE_CUBE_H
#define _MY_CODE_CUBE_H

#include <glload/gl_3_3.h>

namespace MyCode
{
	class Cube
	{
	public:
		Cube(const GLint vertexPositionAttributeID, const GLint vertexColorAttributeID);
		~Cube();

		void Render() const;

	private:
		void InitVertexBuffer();
		void InitVertexIndexBuffer();
		void InitVertexArrayObject();

		GLint mVertexPositionAttributeID;
		GLint mVertexColorAttributeID;

		GLuint mVertexBufferID;
		GLuint mVertexIndexBufferID;
		GLuint mVertexArrayObjectID;
	};
}

#endif //_MY_CODE_CUBE_H
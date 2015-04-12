#ifndef _RENDERING_DATA_H_
#define _RENDERING_DATA_H_

#include "glm/glm.hpp"
#include <glload/gl_3_3.h>

namespace Hierarchy
{
	class RenderContext
	{
	public:
		RenderContext(GLuint programID);

		void InitializeVertexBuffer(const float vertexData[], unsigned vertexDataSize);
		void InitializeIndexBuffer(const GLshort indexData[], unsigned indexDataSize);
		void InitializeVertexArrayObject(const unsigned vertexComponentCount,
			const unsigned vertexOffset,
			const unsigned colorComponentCount,
			const unsigned colorOffset);
		
		void StartDraw();
		void SetModelToCameraMatrix(const glm::mat4& modelToCameraMatrix);
		void Draw(const unsigned indexesCount);
		void EndDraw();

	private:
		GLuint mProgramID;
		GLuint mVertexBufferID;
		GLuint mIndexBufferID;
		GLuint mVertexArrayObjectID;

		GLint mPositionAttributeLocation;
		GLint mColorAttributeLocation;

		GLint mModelToCameraUniform;
		GLint mCameraToClipUniform;
	};
}
#endif
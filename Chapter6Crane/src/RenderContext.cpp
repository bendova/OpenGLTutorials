#include "RenderContext.h"
#include "glm/gtc/type_ptr.hpp"

namespace Hierarchy
{
	RenderContext::RenderContext(GLuint programID)
		: mVertexBufferID(GL_NONE)
		, mIndexBufferID(GL_NONE)
		, mVertexArrayObjectID(GL_NONE)
	{
		mProgramID = programID;

		mPositionAttributeLocation = glGetAttribLocation(mProgramID, "position");
		mColorAttributeLocation = glGetAttribLocation(mProgramID, "color");
		mModelToCameraUniform = glGetUniformLocation(mProgramID, "modelToCameraTransform");
	}

	void RenderContext::InitializeVertexBuffer(const float vertexData[], unsigned vertexDataSize)
	{
		glGenBuffers(1, &mVertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	}

	void RenderContext::InitializeIndexBuffer(const GLshort indexData[], unsigned indexDataSize)
	{
		glGenBuffers(1, &mIndexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
	}

	void RenderContext::InitializeVertexArrayObject(const unsigned vertexComponentCount,
		const unsigned vertexOffset,
		const unsigned colorComponentCount,
		const unsigned colorOffset)
	{
		glGenVertexArrays(1, &mVertexArrayObjectID);
		glBindVertexArray(mVertexArrayObjectID);

		glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
		glEnableVertexAttribArray(mPositionAttributeLocation);
		glVertexAttribPointer(mPositionAttributeLocation, vertexComponentCount, GL_FLOAT, GL_FALSE, 0, (void*)vertexOffset);

		glEnableVertexAttribArray(mColorAttributeLocation);
		glVertexAttribPointer(mColorAttributeLocation, colorComponentCount, GL_FLOAT, GL_FALSE, 0, (void*)colorOffset);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID);

		glBindVertexArray(GL_NONE);
	}

	void RenderContext::StartDraw()
	{
		glUseProgram(mProgramID);
		glBindVertexArray(mVertexArrayObjectID);
	}

	void RenderContext::SetModelToCameraMatrix(const glm::mat4& modelToCameraMatrix)
	{
		glUniformMatrix4fv(mModelToCameraUniform, 1, GL_FALSE, glm::value_ptr(modelToCameraMatrix));
	}

	void RenderContext::Draw(const unsigned indexesCount)
	{
		glDrawElements(GL_TRIANGLES, indexesCount, GL_UNSIGNED_SHORT, 0);
	}

	void RenderContext::EndDraw()
	{
		glBindVertexArray(GL_NONE);
		glUseProgram(GL_NONE);
	}
}
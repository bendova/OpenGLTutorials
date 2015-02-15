#include "Cube.h"

namespace MyCode
{
#define COLOR_RED		1.0f, 0.0f, 0.0f, 1.0f
#define COLOR_GREEN		0.0f, 1.0f, 0.0f, 1.0f
#define COLOR_BLUE		0.0f, 0.0f, 1.0f, 1.0f
#define COLOR_YELLOW	1.0f, 1.0f, 0.0f, 1.0f
#define COLOR_PURPLE	1.0f, 0.0f, 1.0f, 1.0f
#define COLOR_CYAN		0.0f, 1.0f, 1.0f, 1.0f

#define LEFT_EXTENT		-1.0f
#define RIGHT_EXTENT	 1.0f
#define TOP_EXTENT		 1.0f
#define BOTTOM_EXTENT	-1.0f
#define FRONT_EXTENT	 1.0f
#define BACK_EXTENT		-1.0f

#define FRONT_TOP_LEFT_CORNER LEFT_EXTENT, TOP_EXTENT, FRONT_EXTENT
#define FRONT_TOP_RIGHT_CORNER RIGHT_EXTENT, TOP_EXTENT, FRONT_EXTENT
#define FRONT_BOTTOM_LEFT_CORNER LEFT_EXTENT, BOTTOM_EXTENT, FRONT_EXTENT
#define FRONT_BOTTOM_RIGHT_CORNER RIGHT_EXTENT, BOTTOM_EXTENT, FRONT_EXTENT
#define BACK_TOP_LEFT_CORNER LEFT_EXTENT, TOP_EXTENT, BACK_EXTENT
#define BACK_TOP_RIGHT_CORNER RIGHT_EXTENT, TOP_EXTENT, BACK_EXTENT
#define BACK_BOTTOM_LEFT_CORNER LEFT_EXTENT, BOTTOM_EXTENT, BACK_EXTENT
#define BACK_BOTTOM_RIGHT_CORNER RIGHT_EXTENT, BOTTOM_EXTENT, BACK_EXTENT

	const unsigned COLOR_COMPONENT_COUNT = 4;
	const unsigned VERTEX_COMPONENT_COUNT = 3;
	const unsigned VERTEX_COUNT = 24;

	const float gVertexBuffer[] =
	{
		// front face
		FRONT_TOP_LEFT_CORNER,
		FRONT_TOP_RIGHT_CORNER,
		FRONT_BOTTOM_RIGHT_CORNER,
		FRONT_BOTTOM_LEFT_CORNER,

		// right face
		FRONT_TOP_RIGHT_CORNER,
		BACK_TOP_RIGHT_CORNER,
		BACK_BOTTOM_RIGHT_CORNER,
		FRONT_BOTTOM_RIGHT_CORNER,

		// left face
		BACK_TOP_LEFT_CORNER,
		FRONT_TOP_LEFT_CORNER,
		FRONT_BOTTOM_LEFT_CORNER,
		BACK_BOTTOM_LEFT_CORNER,

		// back face
		BACK_TOP_RIGHT_CORNER,
		BACK_TOP_LEFT_CORNER,
		BACK_BOTTOM_LEFT_CORNER,
		BACK_BOTTOM_RIGHT_CORNER,

		// top face
		BACK_TOP_LEFT_CORNER,
		BACK_TOP_RIGHT_CORNER,
		FRONT_TOP_RIGHT_CORNER,
		FRONT_TOP_LEFT_CORNER,

		// bottom face
		BACK_BOTTOM_RIGHT_CORNER,
		BACK_BOTTOM_LEFT_CORNER,
		FRONT_BOTTOM_LEFT_CORNER,
		FRONT_BOTTOM_RIGHT_CORNER,

		// front face colors
		COLOR_RED,
		COLOR_RED,
		COLOR_RED,
		COLOR_RED,

		// right face colors
		COLOR_GREEN,
		COLOR_GREEN,
		COLOR_GREEN,
		COLOR_GREEN,

		// left face colors
		COLOR_BLUE,
		COLOR_BLUE,
		COLOR_BLUE,
		COLOR_BLUE,

		// back face colors
		COLOR_YELLOW,
		COLOR_YELLOW,
		COLOR_YELLOW,
		COLOR_YELLOW,

		// top face colors
		COLOR_PURPLE,
		COLOR_PURPLE,
		COLOR_PURPLE,
		COLOR_PURPLE,

		// bottom face colors
		COLOR_CYAN,
		COLOR_CYAN,
		COLOR_CYAN,
		COLOR_CYAN,
	};

	const GLushort gVertexIndexBuffer[] =
	{
		// front face
		0, 1, 2,
		2, 3, 0,

		// right face
		4, 5, 6,
		6, 7, 4,

		// left face
		8, 9, 10,
		10, 11, 8,

		// back face
		12, 13, 14,
		14, 15, 12,

		// top face
		16, 17, 18,
		18, 19, 16,

		// top face
		20, 21, 22,
		22, 23, 20,
	};

	Cube::Cube(const GLint vertexPositionAttributeID, const GLint vertexColorAttributeID)
		: mVertexPositionAttributeID(vertexPositionAttributeID)
		, mVertexColorAttributeID(vertexColorAttributeID)
	{
		InitVertexBuffer();
		InitVertexIndexBuffer();
		InitVertexArrayObject();
	}

	Cube::~Cube()
	{
	}

	void Cube::Render()
	{
		glBindVertexArray(mVertexArrayObjectID);
		glDrawElements(GL_TRIANGLES, sizeof(gVertexIndexBuffer) / sizeof(gVertexIndexBuffer[0]), GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(GL_NONE);
	}

	void Cube::InitVertexBuffer()
	{
		glGenBuffers(1, &mVertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBuffer), gVertexBuffer, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	}

	void Cube::InitVertexIndexBuffer()
	{
		glGenBuffers(1, &mVertexIndexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexIndexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gVertexIndexBuffer), gVertexIndexBuffer, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
	}

	void Cube::InitVertexArrayObject()
	{
		glGenVertexArrays(1, &mVertexArrayObjectID);
		glBindVertexArray(mVertexArrayObjectID);

		glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);

		glEnableVertexAttribArray(mVertexPositionAttributeID);
		glVertexAttribPointer(mVertexPositionAttributeID, VERTEX_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(mVertexColorAttributeID);
		const unsigned colorComponentOffset = VERTEX_COUNT * VERTEX_COMPONENT_COUNT * sizeof(gVertexBuffer[0]);
		glVertexAttribPointer(mVertexColorAttributeID, COLOR_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, (void*)colorComponentOffset);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexIndexBufferID);

		glBindVertexArray(GL_NONE);
	}
}
#include "Snowflake.h"
#include "../programs/PosColorProgram.h"

namespace MyCode
{
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

	const unsigned VERTEX_COMPONENT_COUNT = 3;

	const int Snowflake::VERTEX_BUFFER_MAX_SIZE = 600000;
	const int Snowflake::MIN_SNOWFLAKE_ITERATIONS = 0;
	const int Snowflake::MAX_SNOWFLAKE_ITERATIONS = 7;

	Snowflake::Snowflake(const PosColorProgram& posColorProgram)
		: mPosColorProgram(posColorProgram)
		, mVertexBufferID(GL_INVALID_INDEX)
		, mVertexArrayObjectID(GL_INVALID_INDEX)
		, mPoints()
		, mSnowflakeIterations(MIN_SNOWFLAKE_ITERATIONS)
		, mSnowflakeOutside(true)
	{
		InitVertexBuffer();
		InitVertexArrayObject();
		GenerateSnowflake();
	}

	Snowflake::~Snowflake()
	{

	}

	void Snowflake::InitVertexBuffer()
	{
		glGenBuffers(1, &mVertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_MAX_SIZE, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	}

	void Snowflake::InitVertexArrayObject()
	{
		glGenVertexArrays(1, &mVertexArrayObjectID);
		glBindVertexArray(mVertexArrayObjectID);

		glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);

		const auto positionAttributeID = mPosColorProgram.GetPositionAttributeID();
		glEnableVertexAttribArray(positionAttributeID);
		glVertexAttribPointer(positionAttributeID, VERTEX_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(GL_NONE);
	}

	void Snowflake::GenerateSnowflake()
	{
		GenerateSnowflakeVertices();
		UploadSnowflakeVerticesToOpenGL();
	}

	void Snowflake::GenerateSnowflakeVertices()
	{
		Point a{ FRONT_BOTTOM_LEFT_CORNER };
		Point c{ FRONT_BOTTOM_RIGHT_CORNER };
		Point b{ GetTrianglePoint(a, c) };

		std::vector<Point> initialPoints{ a, b, c };
		mPoints = GenerateSnowflakeIteration(initialPoints, mSnowflakeIterations);
	}

	std::vector<Point>
	Snowflake::GenerateSnowflakeIteration(const std::vector<Point>& initialPoints, const int iterations)
	{
		std::vector<Point> points = initialPoints;
		for (int i = 0; i < iterations; ++i)
		{
			points = GenerateSnowflake(points);
		}
		return points;
	}

	void Snowflake::UploadSnowflakeVerticesToOpenGL()
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
		const unsigned bufferSize = mPoints.size() * sizeof(Point);
		glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, &mPoints[0]);
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	}

	std::vector<Point> 
	Snowflake::GenerateSnowflake(const std::vector<Point>& initialPoints)
	{
		std::vector<Point> generatedPoints;
		//       c
		//      / \
		//     /   \
		//a---b     d---e
		const int pointsCount = initialPoints.size();
		const int lastPointIndex = pointsCount - 1;
		for (int i = 0; i < pointsCount; ++i)
		{
			const Point& a = initialPoints[i];
			const int nextPointIndex = (i < lastPointIndex) ? (i + 1) : 0;
			const Point& e = initialPoints[nextPointIndex];
			Vector bd = (e - a) / 3.0f;
			const Point b{ a.x + bd.x, a.y + bd.y, a.z + bd.z };
			const Point d{ a.x + 2.0f * bd.x, a.y + 2.0f * bd.y, a.z + 2.0f * bd.z };
			const Point& c = GetTrianglePoint(b, d, mSnowflakeOutside);
			
			generatedPoints.push_back(b);
			generatedPoints.push_back(c);
			generatedPoints.push_back(d);
			generatedPoints.push_back(e);
		}
		return generatedPoints;
	}

	Point Snowflake::GetTrianglePoint(const Point& a, const Point& b, const bool outside)
	{
		Vector ab = b - a;
		
		Vector forward{ 0.0f, 0.0f, 1.0f };
		Vector abPerpendicular = (outside) ? cross(forward, ab) : cross(ab, forward);
		abPerpendicular.normalize();
		abPerpendicular.scale(length(ab));
		
		Vector abHalf = ab / 2;
		Point abMiddle = a + abHalf;
		Point c = abMiddle + abPerpendicular;

		return c;
	}

	void Snowflake::Render() const
	{
		glBindVertexArray(mVertexArrayObjectID);
		glLineWidth(3.0f);
		glDrawArrays(GL_LINE_LOOP, 0, mPoints.size());
		glBindVertexArray(GL_NONE);
	}

	void Snowflake::HandleInput(unsigned char key)
	{
		bool handled = true;
		switch (key)
		{
		case ' ':
			ToggleSnowflakeOrientation();
			break;
		case 'i':
			IncreaseSnowflakeIterations();
			break;
		case 'k':
			DecreaseSnowflakeIterations();
			break;
		default:
			handled = false;
			break;
		}

		if (handled)
		{
			GenerateSnowflake();
		}
	}

	void Snowflake::ToggleSnowflakeOrientation()
	{
		mSnowflakeOutside = !mSnowflakeOutside;
	}

	void Snowflake::IncreaseSnowflakeIterations()
	{
		++mSnowflakeIterations;
		mSnowflakeIterations = (mSnowflakeIterations > MAX_SNOWFLAKE_ITERATIONS) ?
								MAX_SNOWFLAKE_ITERATIONS :
								mSnowflakeIterations;
	}

	void Snowflake::DecreaseSnowflakeIterations()
	{
		--mSnowflakeIterations;
		mSnowflakeIterations = (mSnowflakeIterations < MIN_SNOWFLAKE_ITERATIONS) ? 
								MIN_SNOWFLAKE_ITERATIONS : 
								mSnowflakeIterations;
	}
}
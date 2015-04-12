#ifndef _MY_CODE_SNOWFLAKE_H_
#define _MY_CODE_SNOWFLAKE_H_

#include <glload/gl_3_3.h>
#include <vector>

namespace MyCode
{
	class PosColorProgram;
	struct Point;
	struct Vector;
	class Snowflake
	{
	public:
		Snowflake(const PosColorProgram& posColorProgram);
		~Snowflake();

		void Render() const;
		void HandleInput(unsigned char key);
	private:
		std::vector<Point>
		GenerateSnowflakeIteration(const std::vector<Point>& initialPoints, const int iterations = 1);
		std::vector<Point>
		GenerateSnowflake(const std::vector<Point>& initialPoints);
		Point GetTrianglePoint(const Point& a, const Point& b, const bool outside = true);

		void InitVertexBuffer();
		void InitVertexArrayObject();

		void GenerateSnowflake();
		void GenerateSnowflakeVertices();
		void UploadSnowflakeVerticesToOpenGL();

		void ToggleSnowflakeOrientation();
		void IncreaseSnowflakeIterations();
		void DecreaseSnowflakeIterations();

		static const int VERTEX_BUFFER_MAX_SIZE;
		static const int MIN_SNOWFLAKE_ITERATIONS;
		static const int MAX_SNOWFLAKE_ITERATIONS;
		const PosColorProgram& mPosColorProgram;
		GLuint mVertexBufferID;
		GLuint mVertexArrayObjectID;
		std::vector<Point> mPoints;
		int mSnowflakeIterations;
		bool mSnowflakeOutside;
	};

	template<typename T>
	float length(const T& a);

	struct Vector
	{
		Vector(const float v) : x(v), y(v), z(v)
		{}
		Vector(const float fX, const float fY, const float fZ) : x(fX), y(fY), z(fZ)
		{}
		Vector() : x(0.0f), y(0.0f), z(0.0f)
		{}

		void normalize()
		{
			const float vectorLength = length(*this);
			x /= vectorLength;
			y /= vectorLength;
			z /= vectorLength;
		}

		void scale(const float value)
		{
			x *= value;
			y *= value;
			z *= value;
		}

		float x, y, z;
	};

	struct Point
	{
		Point(const float fX, const float fY, const float fZ) : x(fX), y(fY), z(fZ)
		{}
		Point() : x(0.0f), y(0.0f), z(0.0f)
		{}
		float x, y, z;
	};

	inline
	Vector operator-(const Point& lhs, const Point& rhs)
	{
		return Vector(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}

	inline
	Vector operator/(const Vector& lhs, const float& rhs)
	{
		return Vector(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
	}

	inline
	Point operator+(const Point& p, const Vector& v)
	{
		return Point(p.x + v.x, p.y + v.y, p.z + v.z);
	}

	template<typename T>
	float length(const T& a)
	{
		return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	}

	inline
	float dot(const Vector& a, const Vector& b)
	{
		return a.x * b.x +
			a.y * b.y +
			a.z * b.z;
	}

	inline
	Vector cross(const Vector& lhs, const Vector& rhs)
	{
		return Vector(lhs.y * rhs.z - lhs.z * rhs.y,
					  lhs.z * rhs.x - lhs.x * rhs.z,
					  lhs.x * rhs.y - lhs.y * rhs.x);
	}
}

#endif//_MY_CODE_SNOWFLAKE_H_
#include <math.h>
#include "MathUtil.h"

namespace Hierarchy
{
	namespace MathUtil
	{
		float DegreesToRadians(float angleInDegrees)
		{
			return angleInDegrees * PI / 180.0f;
		}

		// http://en.wikipedia.org/wiki/Angle_of_view#Derivation_of_the_angle-of-view_formula
		float GetFrustumScale(float angleInDegrees)
		{
			float halfAngleInRadians = DegreesToRadians(angleInDegrees) / 2.0f;
			return 1.0f / tan(halfAngleInRadians / 2.0f);
		}

		glm::mat4 RotateX(float angleInDegrees)
		{
			float angleInRadians = DegreesToRadians(angleInDegrees);
			float cosOfAngle = cos(angleInRadians);
			float sinOfAngle = sin(angleInRadians);

			glm::mat4 xRotationMatrix(1.0f);
			xRotationMatrix[1].y = cosOfAngle;
			xRotationMatrix[1].z = sinOfAngle;
			xRotationMatrix[2].y = -sinOfAngle;
			xRotationMatrix[2].z = cosOfAngle;

			return xRotationMatrix;
		}

		glm::mat4 RotateY(float angleInDegrees)
		{
			float angleInRadians = DegreesToRadians(angleInDegrees);
			float cosOfAngle = cos(angleInRadians);
			float sinOfAngle = sin(angleInRadians);

			glm::mat4 yRotationMatrix(1.0f);
			yRotationMatrix[0].x = cosOfAngle;
			yRotationMatrix[0].z = -sinOfAngle;
			yRotationMatrix[2].x = sinOfAngle;
			yRotationMatrix[2].z = cosOfAngle;

			return yRotationMatrix;
		}

		glm::mat4 RotateZ(float angleInDegrees)
		{
			float angleInRadians = DegreesToRadians(angleInDegrees);
			float cosOfAngle = cos(angleInRadians);
			float sinOfAngle = sin(angleInRadians);

			glm::mat4 zRotationMatrix(1.0f);
			zRotationMatrix[0].x = cosOfAngle;
			zRotationMatrix[0].y = sinOfAngle;
			zRotationMatrix[1].x = -sinOfAngle;
			zRotationMatrix[1].y = cosOfAngle;

			return zRotationMatrix;
		}

		float Clamp(const float value, const float min, const float max)
		{
			if (value < min)
			{
				return min;
			}
			else if (value > max)
			{
				return max;
			}
			else
			{
				return value;
			}
		}
	}
}

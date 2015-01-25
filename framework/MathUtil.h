#ifndef _MATH_UTIL_
#define _MATH_UTIL_

#include <math.h>
#include <glm/glm.hpp>

namespace MyCode
{
	namespace MathUtil
	{
		const float PI = 3.14159f;

		float DegreesToRadians(float angleInDegrees);
		float GetFrustumScale(float angleInDegrees);
		glm::mat4 RotateX(float angleInDegrees);
		glm::mat4 RotateY(float angleInDegrees);
		glm::mat4 RotateZ(float angleInDegrees);

		float Clamp(const float value, const float min, const float max);
	}
}
#endif
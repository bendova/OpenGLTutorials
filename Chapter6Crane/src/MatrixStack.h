#ifndef _MATRIX_STACK
#define _MATRIX_STACK

#include <stack>
#include <glm/glm.hpp>

namespace Hierarchy
{
	class MatrixStack
	{
	public:
		MatrixStack();

		void Clear();

		void Push();
		void Pop();
		const glm::mat4& Top() const;

		void Scale(const glm::vec3& scaleVector);
		void Translate(const glm::vec3& offsetVector);

		void RotateX(float angleInDegrees);
		void RotateY(float angleInDegrees);
		void RotateZ(float angleInDegrees);
	private:
		glm::mat4 mCurrentMatrix;
		std::stack<glm::mat4> mMatrices;
	};
}
#endif
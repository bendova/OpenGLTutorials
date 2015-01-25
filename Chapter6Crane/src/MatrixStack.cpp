#include "MatrixStack.h"
#include "MathUtil.h"

namespace Hierarchy
{
	MatrixStack::MatrixStack()
		: mCurrentMatrix(1.0f)
	{}

	void MatrixStack::Clear()
	{
		mCurrentMatrix = glm::mat4(1.0f);
		mMatrices.empty();
	}

	void MatrixStack::Push()
	{
		mMatrices.push(mCurrentMatrix);
	}

	void MatrixStack::Pop()
	{
		mCurrentMatrix = mMatrices.top();
		mMatrices.pop();
	}

	const glm::mat4& MatrixStack::Top() const
	{
		return mCurrentMatrix;
	}

	void MatrixStack::Translate(const glm::vec3& offsetVector)
	{
		glm::mat4 translationMatrix(1.0f);
		translationMatrix[3] = glm::vec4(offsetVector, 1.0f);
		mCurrentMatrix = mCurrentMatrix * translationMatrix;
	}

	void MatrixStack::Scale(const glm::vec3& scaleVector)
	{
		glm::mat4 scaleMatrix(1.0f);
		scaleMatrix[0].x = scaleVector.x;
		scaleMatrix[1].y = scaleVector.y;
		scaleMatrix[2].z = scaleVector.z;
		mCurrentMatrix = mCurrentMatrix * scaleMatrix;
	}

	void MatrixStack::RotateX(float angleInDegrees)
	{
		mCurrentMatrix = mCurrentMatrix * MathUtil::RotateX(angleInDegrees);
	}

	void MatrixStack::RotateY(float angleInDegrees)
	{
		mCurrentMatrix = mCurrentMatrix * MathUtil::RotateY(angleInDegrees);
	}

	void MatrixStack::RotateZ(float angleInDegrees)
	{
		mCurrentMatrix = mCurrentMatrix * MathUtil::RotateZ(angleInDegrees);
	}
}
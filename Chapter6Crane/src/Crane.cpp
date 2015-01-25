#include "Crane.h"
#include "MathUtil.h"

namespace Hierarchy
{
	Crane::Crane(GLuint programID)
		: mRenderingContext(programID)
		, mCraneBasePosition(3.0f, -5.0f, -60.f)
		, mBaseScaleZ(3.0f)
		, mCraneBaseAngleY(0.0f)
		, mLeftBasePosition(2.0f, 0.0f, 0.0f)
		, mRightBasePosition(-2.0f, 0.0f, 0.0f)
		, mArmBaseScaleZ(4.0f)
		, mArmBaseAngleX(-60.0f)
		, mForeArmAngleX(-30.0f)
		, mForeArmScaleX(0.8f)
		, mForeArmScaleY(0.8f)
		, mForeArmScaleZ(3.0f)
		, mForeArmLength(3.0f)
		, mWristScale(1.2f)
		, mWristAngleZ(0.0f)
		, mFingerScaleX(0.4f)
		, mFingerScaleY(0.4f)
		, mFingerScaleZ(1.2f)
		, mBottomFingerAngleY(30.0f)
		, mTopFingerAngleY(-30.0f)
		, mMovementPerSecond(10.0f)
		, mCraneTurnAnglePerSecond(45.0f)
		, mArmTurnAnglePerSecond(90.0f)
		, mTurningCraneLeft(false)
		, mTurningCraneRight(false)
		, mMovingCraneForward(false)
		, mMovingCraneBackward(false)
		, mMovingArmDown(false)
		, mMovingArmUp(false)
		, mTurningWristLeft(false)
		, mTurningWristRight(false)
		, mOpeningFingers(false)
		, mClosingFingers(false)
	{
		mRenderingContext.InitializeVertexBuffer(CraneVertexData::gVertexData, sizeof(CraneVertexData::gVertexData));
		mRenderingContext.InitializeIndexBuffer(CraneVertexData::gIndexData, sizeof(CraneVertexData::gIndexData));
		mRenderingContext.InitializeVertexArrayObject(CraneVertexData::VERTEX_COMPONENT_COUNT, CraneVertexData::VERTEX_OFFSET,
			CraneVertexData::COLOR_COMPONENT_COUNT, CraneVertexData::COLOR_OFFSET);
	}

	void Crane::Draw(float deltaTime)
	{
		UpdateCraneComponents(deltaTime);

		mRenderingContext.StartDraw();

		mModelToCameraStack.Translate(mCraneBasePosition);
		mModelToCameraStack.RotateY(mCraneBaseAngleY);
		
		DrawLeftBase();
		DrawRightBase();
		DrawArm();

		mRenderingContext.EndDraw();

		mModelToCameraStack.Clear();
	}

	void Crane::DrawLeftBase()
	{
		mModelToCameraStack.Push();
		mModelToCameraStack.Translate(mLeftBasePosition);
		mModelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, mBaseScaleZ));
		Draw(mModelToCameraStack.Top());
		mModelToCameraStack.Pop();
	}

	void Crane::DrawRightBase()
	{
		mModelToCameraStack.Push();
		mModelToCameraStack.Translate(mRightBasePosition);
		mModelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, mBaseScaleZ));
		Draw(mModelToCameraStack.Top());
		mModelToCameraStack.Pop();
	}

	void Crane::DrawArm()
	{
		mModelToCameraStack.Push();
		mModelToCameraStack.RotateX(mArmBaseAngleX);
		DrawArmBase();
		mModelToCameraStack.Pop();
	}

	void Crane::DrawArmBase()
	{
		mModelToCameraStack.Push();
		mModelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, mArmBaseScaleZ));
		{
			mModelToCameraStack.Push();
			mModelToCameraStack.Scale(glm::vec3(1.0f, 1.0f, mArmBaseScaleZ));
			Draw(mModelToCameraStack.Top());
			mModelToCameraStack.Pop();

			mModelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, mArmBaseScaleZ));
			mModelToCameraStack.RotateX(mForeArmAngleX);
			DrawForeArm();
		}
		mModelToCameraStack.Pop();
	}

	void Crane::DrawForeArm()
	{
		mModelToCameraStack.Push();
		mModelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, mForeArmScaleZ));
		{
			mModelToCameraStack.Push();
			mModelToCameraStack.Scale(glm::vec3(mForeArmScaleX, mForeArmScaleY, mForeArmScaleZ));
			Draw(mModelToCameraStack.Top());
			mModelToCameraStack.Pop();

			mModelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, mForeArmScaleZ));
			mModelToCameraStack.RotateZ(mWristAngleZ);
			DrawWrist();

			mModelToCameraStack.Push();
			mModelToCameraStack.Translate(glm::vec3(mWristScale, 0.0f, mWristScale));
			DrawLeftFingers();
			mModelToCameraStack.Pop();

			mModelToCameraStack.Push();
			mModelToCameraStack.Translate(glm::vec3(-mWristScale, 0.0f, mWristScale));
			DrawRightFingers();
			mModelToCameraStack.Pop();
		}
		mModelToCameraStack.Pop();
	}

	void Crane::DrawWrist()
	{
		mModelToCameraStack.Push();
		mModelToCameraStack.Scale(glm::vec3(mWristScale, mWristScale, mWristScale));
		Draw(mModelToCameraStack.Top());
		mModelToCameraStack.Pop();
	}

	void Crane::DrawLeftFingers()
	{
		mModelToCameraStack.Push();
		mModelToCameraStack.Translate(glm::vec3(-mFingerScaleX / 2.0f, 0.0f, mFingerScaleZ));
		mModelToCameraStack.RotateY(mBottomFingerAngleY);
		{
			mModelToCameraStack.Push();
			mModelToCameraStack.Scale(glm::vec3(mFingerScaleX, mFingerScaleY, mFingerScaleZ));
			Draw(mModelToCameraStack.Top());
			mModelToCameraStack.Pop();

			mModelToCameraStack.Push();
			mModelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, 2 * mFingerScaleZ));
			mModelToCameraStack.RotateY(mTopFingerAngleY);
			mModelToCameraStack.Scale(glm::vec3(mFingerScaleX, mFingerScaleY, mFingerScaleZ));
			Draw(mModelToCameraStack.Top());
			mModelToCameraStack.Pop();
		}
		mModelToCameraStack.Pop();
	}

	void Crane::DrawRightFingers()
	{
		mModelToCameraStack.Push();
		mModelToCameraStack.Translate(glm::vec3(mFingerScaleX / 2.0f, 0.0f, mFingerScaleZ));
		mModelToCameraStack.RotateY(-mBottomFingerAngleY);
		{
			mModelToCameraStack.Push();
			mModelToCameraStack.Scale(glm::vec3(mFingerScaleX, mFingerScaleY, mFingerScaleZ));
			Draw(mModelToCameraStack.Top());
			mModelToCameraStack.Pop();

			mModelToCameraStack.Push();
			mModelToCameraStack.Translate(glm::vec3(0.0f, 0.0f, 2 * mFingerScaleZ));
			mModelToCameraStack.RotateY(-mTopFingerAngleY);
			mModelToCameraStack.Scale(glm::vec3(mFingerScaleX, mFingerScaleY, mFingerScaleZ));
			Draw(mModelToCameraStack.Top());
			mModelToCameraStack.Pop();
		}
		mModelToCameraStack.Pop();
	}

	void Crane::Draw(const glm::mat4& modelToCameraMatrix)
	{
		mRenderingContext.SetModelToCameraMatrix(modelToCameraMatrix);
		mRenderingContext.Draw(sizeof(CraneVertexData::gIndexData) / sizeof(CraneVertexData::gIndexData[0]));
	}

	void Crane::HandleKeyDown(unsigned char key)
	{
		switch (key)
		{
		case CRANE_LEFT_KEY:
			ToggleTurningCraneLeft(true);
			break;
		case CRANE_RIGHT_KEY:
			ToggleTurningCraneRight(true);
			break;
		case CRANE_FORWARD_KEY:
			ToggleMovingCraneForward(true);
			break;
		case CRANE_BACKWARD_KEY:
			ToggleMovingCraneBackward(true);
			break;
		case WRIST_LEFT_KEY:
			ToggleTurningWristLeft(true);
			break;
		case WRIST_RIGHT_KEY:
			ToggleTurningWristRight(true);
			break;
		case ARM_UP_KEY:
			ToggleMovingArmUp(true);
			break;
		case ARM_DOWN_KEY:
			ToggleMovingArmDown(true);
			break;
		case FINGER_OPEN_KEY:
			ToggleFingersOpening(true);
			break;
		case FINGER_CLOSE_KEY:
			ToggleFingersClosing(true);
			break;
		}
	}
	void Crane::HandleKeyUp(unsigned char key)
	{
		switch (key)
		{
		case CRANE_LEFT_KEY:
			ToggleTurningCraneLeft(false);
			break;
		case CRANE_RIGHT_KEY:
			ToggleTurningCraneRight(false);
			break;
		case CRANE_FORWARD_KEY:
			ToggleMovingCraneForward(false);
			break;
		case CRANE_BACKWARD_KEY:
			ToggleMovingCraneBackward(false);
			break;
		case WRIST_LEFT_KEY:
			ToggleTurningWristLeft(false);
			break;
		case WRIST_RIGHT_KEY:
			ToggleTurningWristRight(false);
			break;
		case ARM_UP_KEY:
			ToggleMovingArmUp(false);
			break;
		case ARM_DOWN_KEY:
			ToggleMovingArmDown(false);
			break;
		case FINGER_OPEN_KEY:
			ToggleFingersOpening(false);
			break;
		case FINGER_CLOSE_KEY:
			ToggleFingersClosing(false);
			break;
		}
	}

	void Crane::UpdateCraneComponents(float deltaTime)
	{
		if (mTurningCraneLeft)
		{
			TurnCraneLeft(deltaTime);
		}
		if (mTurningCraneRight)
		{
			TurnCraneRight(deltaTime);
		}

		if (mMovingCraneForward)
		{
			MoveCraneForward(deltaTime);
		}
		if (mMovingCraneBackward)
		{
			MoveCraneBackward(deltaTime);
		}

		if (mTurningWristLeft)
		{
			TurnWristLeft(deltaTime);
		}
		if (mTurningWristRight)
		{
			TurnWristRight(deltaTime);
		}

		if (mMovingArmUp)
		{
			MoveArmUp(deltaTime);
		}
		if (mMovingArmDown)
		{
			MoveArmDown(deltaTime);
		}

		if (mOpeningFingers)
		{
			OpenFingers(deltaTime);
		}
		if (mClosingFingers)
		{
			CloseFingers(deltaTime);
		}
	}

	void Crane::MoveCraneForward(float deltaTime)
	{
		glm::vec4 rotatedDirection = MathUtil::RotateY(mCraneBaseAngleY) * glm::vec4(0.0f, 0.0f, mMovementPerSecond * deltaTime, 0.0f);
		mCraneBasePosition.x += rotatedDirection.x;
		mCraneBasePosition.y += rotatedDirection.y;
		mCraneBasePosition.z += rotatedDirection.z;
	}
	void Crane::MoveCraneBackward(float deltaTime)
	{
		glm::vec4 rotatedDirection = MathUtil::RotateY(mCraneBaseAngleY) * glm::vec4(0.0f, 0.0f, mMovementPerSecond * deltaTime, 0.0f);
		mCraneBasePosition.x -= rotatedDirection.x;
		mCraneBasePosition.y -= rotatedDirection.y;
		mCraneBasePosition.z -= rotatedDirection.z;
	}

	void Crane::TurnCraneLeft(float deltaTime)
	{
		mCraneBaseAngleY += mCraneTurnAnglePerSecond * deltaTime;
	}
	void Crane::TurnCraneRight(float deltaTime)
	{
		mCraneBaseAngleY -= mCraneTurnAnglePerSecond * deltaTime;
	}

	void Crane::TurnWristLeft(float deltaTime)
	{
		mWristAngleZ -= mArmTurnAnglePerSecond * deltaTime;
	}
	void Crane::TurnWristRight(float deltaTime)
	{
		mWristAngleZ += mArmTurnAnglePerSecond * deltaTime;
	}

	void Crane::MoveArmUp(float deltaTime)
	{
		IncArmAngleX(-mArmTurnAnglePerSecond * deltaTime);
	}
	void Crane::MoveArmDown(float deltaTime)
	{
		IncArmAngleX(mArmTurnAnglePerSecond * deltaTime);
	}
	void Crane::IncArmAngleX(const float deltaAngle)
	{
		mForeArmAngleX = MathUtil::Clamp(mForeArmAngleX + deltaAngle, -30.0f, 100.0f);
	}

	void Crane::OpenFingers(float deltaTime)
	{
		IncFingersAngleY(mArmTurnAnglePerSecond * deltaTime);
	}
	void Crane::CloseFingers(float deltaTime)
	{
		IncFingersAngleY(-mArmTurnAnglePerSecond * deltaTime);
	}
	void Crane::IncFingersAngleY(const float deltaAngle)
	{
		mBottomFingerAngleY = MathUtil::Clamp(mBottomFingerAngleY + deltaAngle, 0.0f, 50.0f);
	}
}
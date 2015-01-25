#ifndef _CRANE_CLASS_H_
#define _CRANE_CLASS_H_

#include <glload/gl_3_3.h>
#include "MatrixStack.h"
#include "RenderContext.h"
#include <array>

namespace Hierarchy
{
	class Crane
	{
	public:
		Crane(GLuint programID);

		void Draw(float deltaTime);

		void HandleKeyDown(unsigned char key);
		void HandleKeyUp(unsigned char key);

	private:
		void DrawLeftBase();
		void DrawRightBase();
		void DrawArm();
		void DrawArmBase();
		void DrawForeArm();
		void DrawWrist();
		void DrawLeftFingers();
		void DrawRightFingers();

		void Draw(const glm::mat4& modelToCameraMatrix);

		void ToggleTurningCraneLeft(bool turnLeft) { mTurningCraneLeft = turnLeft; }
		void ToggleTurningCraneRight(bool turnRight) { mTurningCraneRight = turnRight; }
		void ToggleMovingCraneForward(bool moveForward) { mMovingCraneForward = moveForward; }
		void ToggleMovingCraneBackward(bool moveBackward) { mMovingCraneBackward = moveBackward; }

		void ToggleTurningWristLeft(bool turnLeft) { mTurningWristLeft = turnLeft; }
		void ToggleTurningWristRight(bool turnRight) { mTurningWristRight = turnRight; }

		void ToggleMovingArmUp(bool moveUp) { mMovingArmUp = moveUp; }
		void ToggleMovingArmDown(bool moveDown) { mMovingArmDown = moveDown; }

		void ToggleFingersOpening(bool open) { mOpeningFingers = open; }
		void ToggleFingersClosing(bool close) { mClosingFingers = close; }

		void UpdateCraneComponents(float deltaTime);

		void MoveCraneForward(float deltaTime);
		void MoveCraneBackward(float deltaTime);
		void TurnCraneLeft(float deltaTime);
		void TurnCraneRight(float deltaTime);

		void TurnWristLeft(float deltaTime);
		void TurnWristRight(float deltaTime);

		void MoveArmUp(float deltaTime);
		void MoveArmDown(float deltaTime);
		void IncArmAngleX(const float angle);

		void OpenFingers(float deltaTime);
		void CloseFingers(float deltaTime);
		void IncFingersAngleY(const float deltaAngle);

		static const char CRANE_LEFT_KEY = 'a';
		static const char CRANE_RIGHT_KEY = 'd';
		static const char CRANE_FORWARD_KEY = 'w';
		static const char CRANE_BACKWARD_KEY = 's';

		static const char WRIST_LEFT_KEY = 'j';
		static const char WRIST_RIGHT_KEY = 'l';
		static const char ARM_UP_KEY = 'i';
		static const char ARM_DOWN_KEY = 'k';
		static const char FINGER_OPEN_KEY = 'q';
		static const char FINGER_CLOSE_KEY = 'e';

		RenderContext mRenderingContext;
		MatrixStack mModelToCameraStack;

		glm::vec3 mCraneBasePosition;
		const float mBaseScaleZ;
		float mCraneBaseAngleY;

		const glm::vec3 mLeftBasePosition;
		const glm::vec3 mRightBasePosition;

		const float mArmBaseAngleX;
		const float mArmBaseScaleZ;

		const float mForeArmScaleX;
		const float mForeArmScaleY;
		const float mForeArmScaleZ;
		const float mForeArmLength;
		float mForeArmAngleX;

		const float mWristScale;
		float mWristAngleZ;

		const float mFingerScaleX;
		const float mFingerScaleY;
		const float mFingerScaleZ;

		float mBottomFingerAngleY;
		float mTopFingerAngleY;

		const float mMovementPerSecond;
		const float mCraneTurnAnglePerSecond;
		const float mArmTurnAnglePerSecond;

		bool mTurningCraneLeft;
		bool mTurningCraneRight;
		bool mMovingCraneForward;
		bool mMovingCraneBackward;

		bool mTurningWristLeft;
		bool mTurningWristRight;

		bool mMovingArmUp;
		bool mMovingArmDown;

		bool mOpeningFingers;
		bool mClosingFingers;
	};

	namespace CraneVertexData
	{
		const unsigned VERTEX_COMPONENT_COUNT = 3;
		const unsigned COLOR_COMPONENT_COUNT = 4;

		const float LEFT_EXTENT = -1.0f;
		const float RIGHT_EXTENT = -LEFT_EXTENT;
		const float TOP_EXTENT = 1.0f;
		const float BOTTOM_EXTENT = -TOP_EXTENT;
		const float FRONT_EXTENT = 1.0f;
		const float BACK_EXTENT = -FRONT_EXTENT;

#define LEFT_TOP_FRONT_CORNER LEFT_EXTENT, TOP_EXTENT, FRONT_EXTENT
#define RIGHT_TOP_FRONT_CORNER RIGHT_EXTENT, TOP_EXTENT, FRONT_EXTENT
#define LEFT_BOTTOM_FRONT_CORNER LEFT_EXTENT, BOTTOM_EXTENT, FRONT_EXTENT
#define RIGHT_BOTTOM_FRONT_CORNER RIGHT_EXTENT, BOTTOM_EXTENT, FRONT_EXTENT
#define LEFT_TOP_BACK_CORNER LEFT_EXTENT, TOP_EXTENT, BACK_EXTENT
#define RIGHT_TOP_BACK_CORNER RIGHT_EXTENT, TOP_EXTENT, BACK_EXTENT
#define LEFT_BOTTOM_BACK_CORNER LEFT_EXTENT, BOTTOM_EXTENT, BACK_EXTENT
#define RIGHT_BOTTOM_BACK_CORNER RIGHT_EXTENT, BOTTOM_EXTENT, BACK_EXTENT

#define COLOR_RED 1.0f, 0.0f, 0.0f, 1.0f
#define COLOR_GREEN 0.0f, 1.0f, 0.0f, 1.0f
#define COLOR_BLUE 0.0f, 0.0f, 1.0f, 1.0f
#define COLOR_YELLOW 1.0f, 1.0f, 0.0f, 1.0f
#define COLOR_CYAN 0.0f, 1.0f, 1.0f, 1.0f
#define COLOR_MAGENTA 1.0f, 0.0f, 1.0f, 1.0f

		const unsigned VERTEX_COUNT = 24;
		const float gVertexData[]
		{
			// front face
			LEFT_TOP_FRONT_CORNER,
			RIGHT_TOP_FRONT_CORNER,
			RIGHT_BOTTOM_FRONT_CORNER,
			LEFT_BOTTOM_FRONT_CORNER,

			// back face
			LEFT_TOP_BACK_CORNER,
			LEFT_BOTTOM_BACK_CORNER,
			RIGHT_BOTTOM_BACK_CORNER,
			RIGHT_TOP_BACK_CORNER,

			// left face
			LEFT_TOP_BACK_CORNER,
			LEFT_TOP_FRONT_CORNER,
			LEFT_BOTTOM_FRONT_CORNER,
			LEFT_BOTTOM_BACK_CORNER,

			// right face
			RIGHT_TOP_BACK_CORNER,
			RIGHT_BOTTOM_BACK_CORNER,
			RIGHT_BOTTOM_FRONT_CORNER,
			RIGHT_TOP_FRONT_CORNER,

			// top face
			LEFT_TOP_BACK_CORNER,
			RIGHT_TOP_BACK_CORNER,
			RIGHT_TOP_FRONT_CORNER,
			LEFT_TOP_FRONT_CORNER,

			// bottom face
			LEFT_BOTTOM_BACK_CORNER,
			LEFT_BOTTOM_FRONT_CORNER,
			RIGHT_BOTTOM_FRONT_CORNER,
			RIGHT_BOTTOM_BACK_CORNER,

			// front face colors
			COLOR_RED,
			COLOR_RED,
			COLOR_RED,
			COLOR_RED,

			// back face colors
			COLOR_CYAN,
			COLOR_CYAN,
			COLOR_CYAN,
			COLOR_CYAN,

			// left face colors
			COLOR_GREEN,
			COLOR_GREEN,
			COLOR_GREEN,
			COLOR_GREEN,

			// right face colors
			COLOR_MAGENTA,
			COLOR_MAGENTA,
			COLOR_MAGENTA,
			COLOR_MAGENTA,

			// top face colors
			COLOR_BLUE,
			COLOR_BLUE,
			COLOR_BLUE,
			COLOR_BLUE,

			// bottom face colors
			COLOR_YELLOW,
			COLOR_YELLOW,
			COLOR_YELLOW,
			COLOR_YELLOW,
		};

#undef LEFT_TOP_FRONT_CORNER
#undef RIGHT_TOP_FRONT_CORNER
#undef LEFT_BOTTOM_FRONT_CORNER
#undef RIGHT_BOTTOM_FRONT_CORNER
#undef LEFT_TOP_BACK_CORNER
#undef RIGHT_TOP_BACK_CORNER
#undef LEFT_BOTTOM_BACK_CORNER
#undef RIGHT_BOTTOM_BACK_CORNER

#undef COLOR_RED
#undef COLOR_GREEN
#undef COLOR_BLUE
#undef COLOR_YELLOW
#undef COLOR_CYAN
#undef COLOR_MAGENTA

		const GLshort gIndexData[]
		{
			0, 1, 2,
			2, 3, 0,

			4, 5, 6,
			6, 7, 4,

			8, 9, 10,
			10, 11, 8,

			12, 13, 14,
			14, 15, 12,

			16, 17, 18,
			18, 19, 16,

			20, 21, 22,
			22, 23, 20,
		};

		const unsigned VERTEX_OFFSET = 0;
		const unsigned COLOR_OFFSET = VERTEX_COUNT * VERTEX_COMPONENT_COUNT * sizeof(gVertexData[0]);
	};
}

#endif
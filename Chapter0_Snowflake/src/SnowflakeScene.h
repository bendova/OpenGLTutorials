#ifndef _SCENE_LIGHTING_PROGRAM_H_
#define _SCENE_LIGHTING_PROGRAM_H_

#include <glload/gl_3_3.h>
#include <memory>
#include "Scene.h"
#include "renderables/Snowflake.h"
#include "programs/PosColorProgram.h"

namespace MyCode
{
	class SnowflakeScene
	{
	public:
		SnowflakeScene();
		~SnowflakeScene();

		void Render();
		void Reshape(GLint width, GLint height);
		void HandleInput(unsigned char key, int x, int y);
	private:
		void ConfigureOpenGL();
		void ConfigureInput();

		void RenderSnowflake();

		PosColorProgram mPosColorProgram;
		Snowflake mSnowflake;
	};
}

#endif
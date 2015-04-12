#ifndef _SCENE_LIGHTING_PROGRAM_H_
#define _SCENE_LIGHTING_PROGRAM_H_

#include <glload/gl_3_3.h>
#include <memory>
#include "Scene.h"

namespace MyCode
{
	class SceneLighting
	{
	public:
		SceneLighting();
		~SceneLighting();

		void Render();
		void Reshape(GLint width, GLint height);
		void HandleInput(unsigned char key, int x, int y);
	private:
		typedef std::unique_ptr<MyCode::Scene> ScenePtr;
		
		void ConfigureOpenGL();
		void ConfigureInput();

		ScenePtr mScene;
	};
}

#endif
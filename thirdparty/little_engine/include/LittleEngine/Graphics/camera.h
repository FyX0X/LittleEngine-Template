#pragma once

#include <glm/glm.hpp>


namespace LittleEngine
{


	//used to change the view.
	//whenever you render something, it will be
	//rendered relative to the current camera position.
	//so you can render 2 different things in the same frame at different camera positions.
	//(you will do that for ui for example that you will want to draw with the camera at 0 0).
	struct Camera
	{
		glm::vec2  position = {};

		// Camera rotation (roll) in degrees
		float rotation = 0.f;

		// Camera zoom (scaling), should be 1.0f by default
		float zoom = 1.f;

		void Reset() { *this = Camera{}; }

		//Used to follow objects (player for example).
		//The followed object (pos) will get placed in the center of the screen.
		//maxDistance is the maximum possible distance.
		//w and h are the dimensions of the window
		void Follow(glm::vec2 pos, float speed, float maxDistance, float w, float h);


		glm::mat4 GetViewMatrix();
	};


}
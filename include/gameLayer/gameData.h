#pragma once

#include <glm/glm.hpp>

namespace game
{


	struct GameData
	{
		glm::vec2 rectPos = { 1,1 };
		float zoom = 1.f;

		LittleEngine::Color color = LittleEngine::Colors::Green;

	};

}
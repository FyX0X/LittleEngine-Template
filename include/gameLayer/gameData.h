#pragma once

#include <glm/glm.hpp>

namespace game
{


	struct GameData
	{
		glm::vec2 rectPos = { 1,1 };
		glm::vec2 pos2 = { 0,0 };
		float zoom = 50.f;

		LittleEngine::Graphics::Color color = LittleEngine::Graphics::Colors::Green;

	};

}
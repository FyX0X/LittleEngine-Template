#include "LittleEngine/Graphics/camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace LittleEngine
{

	void Camera::Follow(glm::vec2 pos, float speed, float maxDistance, float w, float h)
	{
		// TODO MAKE REAL IMPLEMENTATION
		position = pos;
	}

	glm::mat4 Camera::GetViewMatrix()
	{
		glm::mat4 view = glm::mat4(1.f);
		view = glm::scale(view, glm::vec3(zoom, zoom, 1.f));          // scale first
		view = glm::rotate(view, glm::radians(rotation), glm::vec3(0, 0, -1));
		view = glm::translate(view, glm::vec3(-position, 0.f));       // then translate
		return view;
	}
}

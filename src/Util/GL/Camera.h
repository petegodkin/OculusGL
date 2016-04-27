#pragma once

#include "glm/glm.hpp"

class Camera
{
public:
	Camera(glm::mat4 view, glm::mat4 proj, glm::vec3 pos);
	glm::mat4 view();
	glm::mat4 proj();
	glm::vec3 position();
private:
	glm::mat4 _view;
	glm::mat4 _proj;
	glm::vec3 _position;
};
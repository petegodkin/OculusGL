#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include "Shape.h"

class Light {
public:
	Light(glm::vec3 pos);
	Light(glm::vec3 pos, glm::vec3 color, float intensity, Shape* shape);
	glm::vec3 pos;
	glm::vec3 color;
	float intensity;
	Shape* shape;
	glm::mat4 transform();
	glm::mat4 _transform;
	glm::mat4 calc_scale_mat();
};
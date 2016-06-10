#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include "MeshSet.h"
#include "Entity.h"

#define CONSTANT_AT 0.0
#define LINEAR_AT 0.7
#define EXP_AT 0.5

class Light : public Entity {
public:
	Light(const MeshSet* mesh, glm::vec3 pos, glm::vec3 color, float intensity, const MeshSet* shape);
	glm::vec3 default_pos;
	glm::vec3 color;
	float intensity;
	const MeshSet* sphere;
	glm::mat4 transform();
	glm::mat4 _transform;
	glm::mat4 _scale;
	glm::mat4 calc_scale_mat();
	void update();
	float total = 0;
};
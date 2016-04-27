#pragma once
#include "Shape.h"
#include <glm/glm.hpp>
#include "Texture.h"

class Entity
{
public:
	Entity();
	Entity(Shape* shape);
	Entity(Shape* shape, std::string filename);
	virtual ~Entity();
	Shape* shape();
	glm::mat4 modelMat();
	void setScale(float entScale);
	std::shared_ptr<Texture> texture();
private:
	Shape* _shape;
	glm::mat4 _modelMat;
	glm::vec3 _scale;
	std::shared_ptr<Texture> _texture = nullptr;
};
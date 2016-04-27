#include "Entity.h"
#include "glm/gtc/matrix_transform.hpp"

Entity::Entity() {

}


Entity::Entity(Shape* shape, std::string filename) : Entity(shape) {
	_texture = std::make_shared<Texture>();
	_texture->setFilename(filename);
	_texture->init();
}


Entity::Entity(Shape* shape) {
	_shape = shape;
	_modelMat = glm::mat4();
	_scale = glm::vec3(1, 1, 1);
}

Entity::~Entity() {

}

Shape* Entity::shape() {
	return _shape;
}

std::shared_ptr<Texture> Entity::texture() {
	return _texture;
}

glm::mat4 Entity::modelMat() {
	return glm::scale(_modelMat, _scale);
}

void Entity::setScale(float entScale)
{
	_scale = glm::vec3(entScale, entScale, entScale);
}
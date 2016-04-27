#include "Camera.h"

Camera::Camera(glm::mat4 view, glm::mat4 proj, glm::vec3 pos) {
	_view = view;
	_proj = proj;
	_position = pos;
}

glm::mat4 Camera::view() {
	return _view;
}

glm::mat4 Camera::proj() {
	return _proj;
}

glm::vec3 Camera::position() {
	return _position;
}
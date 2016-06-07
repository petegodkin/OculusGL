#include "Light.h"
#include <glm/gtx/transform.hpp>

#define CONSTANT_AT 0.0
#define LINEAR_AT 0.5
#define EXP_AT 0.3

#include <glm/glm.hpp>

Light::Light(glm::vec3 pos) : pos(pos) {

}

Light::Light(glm::vec3 pos, glm::vec3 color, float intensity, const MeshSet* shape) : pos(pos), color(color), intensity(intensity),
shape(shape) {
	_transform = glm::translate(glm::mat4(1.0f), pos) * calc_scale_mat();
}

glm::mat4 Light::transform()
{
	return _transform;
}

//calculate how big the size needs to be based on intensity
glm::mat4 Light::calc_scale_mat()
{
	float MaxChannel = fmax(fmax(color.x, color.y), color.z);

	float constant = CONSTANT_AT;
	float linear = LINEAR_AT;
	float exp = EXP_AT;

	float r = (-linear + sqrtf(linear * linear -
		4 * exp * (constant - 256 * MaxChannel * intensity)))
		/ 2 * exp;
	return glm::scale(glm::mat4(1.0f), glm::vec3(r, r, r));
}
#include "Plane.h"

//#include <src/util/util_base.h>
//#include <src/util/functions/functions.h>

//#include "../../util/util_base.h"
//#include "../../util/functions/functions.h"

using namespace utility;

Plane::Plane() 
{
	coefficients = glm::vec4(0, 1, 0, 0);
}

Plane::Plane(float a, float b, float c, float d) {
    coefficients = glm::vec4(a, b, c, d);
}

Plane::Plane(glm::vec3 abc, float d) {
	coefficients = glm::vec4(abc, d);
}

Plane::Plane(const Plane & plane) 
{
	coefficients = plane.coefficients;
}

float & Plane::a() {return coefficients.x;}
float & Plane::b() {return coefficients.y;}
float & Plane::c() {return coefficients.z;}
float & Plane::d() {return coefficients.w;}

glm::vec3 Plane::abc() { return glm::vec3(coefficients); }

void Plane::normalize() {
    glm::vec3 nor = glm::vec3(coefficients);
	//std::cout << "Non normalized: " << GUtils::vecToString(nor) << std::endl;
	nor = glm::normalize(nor);

	//std::cout << "NORMALIZED: " << GUtils::vecToString(nor) << std::endl;

	coefficients = glm::vec4(nor, coefficients.w);

    //what should happen if this is 0?
    /*if ( != 0)
        coefficients /= nor.norm();*/
}

//Plane Plane::normalized() {
//    return Plane(util::safelyNormalize(
//     glm::vec3(coefficients.segment<3>(0))), d());
//}

float Plane::distanceTo(glm::vec3 point) {
    normalize();
    return point.x * a() + point.y * b() + point.z * c() + d();
}

std::string Plane::description(std::string label) {
	std::stringstream str;
	str << label << "\t(" <<
		a() << ", " <<
		b() << ", " <<
		c() << ", " <<
		d() << ")" << std::endl;

	return str.str();
}


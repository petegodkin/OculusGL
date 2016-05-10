///
///
///
///
///
///

#ifndef ____game_Plane__
#define ____game_Plane__

#include "GraphicsUtil.h"

namespace utility {

    struct Plane {
    private:
        glm::vec4 coefficients;
    public:
        
        Plane();
        Plane(float a, float b, float c, float d);
        Plane(glm::vec3 abc, float d);
        Plane(const Plane & plane);
        
        float & a();
        float & b();
        float & c();
        float & d();
        
        glm::vec3 abc();
        
        void normalize();
        Plane normalized();
        
        float distanceTo(glm::vec3 point);
		// description();
    };
}

#endif


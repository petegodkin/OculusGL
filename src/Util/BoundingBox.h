///
///
///
///
///
///

#ifndef ____game_BoundingBox__
#define ____game_BoundingBox__

#include "Collideable.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

namespace utility {

    struct BoundingBox {
    public:
        static const int kNumComps = 3;
        static const int kNumCorners = 1 << kNumComps;
    
        /// Creates a bounding box using `s` and `e`, two far corners of a
        /// box. Note that irregardless of the order of `s` and `e`, `start`
        /// will become the "least" point of the box (closest to -infinity on
        /// all axes) and `end` will become the "greatest" point of the box
        /// (closest to infinitiy on all axes).
        BoundingBox(glm::vec3 s = glm::vec3(0.0), glm::vec3 e = glm::vec3(0.0), boolean bMinMax = false);
        
        glm::vec3 getStart();
        glm::vec3 getEnd();
        glm::vec3 getDimensions();
        glm::vec3 getCenter();
		float getRadius(); //this is based off the distance from the middle to the corner.
        
        BoundingBox withOffset(glm::vec3 offset);
        
        bool encloses(glm::vec3 p);
        bool encloses(Collideable * obj);
        
        BoundingBox boundingBoxForCollideable(Collideable * obj);
        
        bool intersects(Collideable * obj);
        bool intersects(BoundingBox other);
        /// Test ray casting intersection
        bool intersects(glm::vec3 start, glm::vec3 dir);
        /// Test sphere intersection
        bool intersects(glm::vec3 center, float radius);
        
        /// Test sphere inscription
        bool encloses(glm::vec3 center, float radius);
        
        bool overlapsWith(Collideable * obj);
        bool overlapsWith(BoundingBox other);
        
        int numCornersContained(Collideable * obj);
        int numCornersContained(BoundingBox other);
        glm::vec3 getCorner(int whichCorner);
        std::string description();
        
    private:
        glm::vec3 start, end;
    };

}

#endif

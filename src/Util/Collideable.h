///
///
///
///
///
///

#include <glm/glm.hpp>

#ifndef ____game_Collideable__
#define ____game_Collideable__

    class Collideable {
    public:
        virtual bool positionChanged() = 0;
        virtual void setPositionChanged(bool changed) = 0;
        virtual float getBoundingSphereRadius() = 0;
        
        virtual unsigned int getCollisionMask() = 0;
        
        virtual void setPosition(glm::vec3 pos) = 0;
        virtual glm::vec3 getPosition() const = 0;
    };

#endif

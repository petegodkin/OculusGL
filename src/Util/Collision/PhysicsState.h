///
///
///
///
///
///

#ifndef ____game_TransformSet__
#define ____game_TransformSet__


#include "BoundingBox.h"
#include <glm/glm.hpp>

#define TURN_COEFF 0.1f

/// Represents the collection of physics components an object may have, such
/// as position, velocity, facing direction, mass, etc. It is used primarily
/// for physics simulations.
class PhysicsState : public Collideable {
public:
    /// The position of the current object.
    glm::vec3 position;
    bool positionHasChanged;
    /// The velocity direction of the object.
    glm::vec3 direction;
    /// The orientation of the object.
    glm::quat orientation;
        
    /// Desired orientation of turning object.
    glm::quat goalOrient;
    /// Time spent turning so far.
    float turnTime;
        
    /// The speed of the object.
    float velocity;
    /// The mass of the object.
    float mass;
        
    /// The radius of the sphere that minimally encapsulates this object.
    float boundingSphereRadius;
    float getBoundingSphereRadius();
    void setBoundingSphereRadius(float radius);
        
    glm::vec3 getDirection();
    /// Sets the `direction` of this object. In partcicular, `dir` is
    /// safely normalized before it is set.
    void setDirection(glm::vec3 dir);
        
    glm::quat getOrientation();
    void setOrientation(glm::quat orient);
        
    /// Creates a physics state for an object at (0, 0, 0) facing down the
    /// z-axis with a mass of 1 and no velocity.
    PhysicsState();
        
    /// Creates a physics state with the given properties, setting the pass
    /// to 1.0.
    PhysicsState(glm::vec3 pos, glm::vec3 dir, float vel);
        
    /// In-place initializer of this physics state.
    void init(glm::vec3 pos, glm::vec3 dir, float vel, float mass);

    /// Returns the current velocity.
    float getVelocity();
    void setVelocity(float v) {velocity = v;}
        
    /// Returns the current position.
    glm::vec3 getPosition();
    /// Sets the current position.
    void setPosition(glm::vec3 pos);
        
    bool positionChanged();
    void setPositionChanged(bool changed);
        
    uint32_t getCollisionMask();
        
    /// Returns the position if `dt` were applied.
    glm::vec3 getNewPosition(float dt);
    /// Return the velocity if `dt` were applied to `acc` (the acceleration
    /// acting on the body).
    glm::vec3 getNewVelocity(float dt, glm::vec3 acc);
    float getNewVelocity(float dt, float friction);
        
    /// Returns the position projected onto the XZ-plane
    glm::vec3 getPlanarPosition();
        
    /// Orients this body in its velocity `direction` smoothly.
    void orientTowardsDirection();
        
    /// Orients this body in its velocity `direction` now.
    void orientTowardsDirectionForce();
        
    ///
    void update(float dt);
        
    ///
    void updateOrientation(float dt);
    void setDirectionToOrientation();
        
    /// Bounds `positions` to be within the volume defined between
    /// `cornerStart` and `cornerEnd`.
    void boundPosition(glm::vec3 cornerStart, glm::vec3 cornerEnd);
        
    /// Returns whether this physics body intersect another.
    bool intersectsOther(PhysicsState & other);
    /// Separates this Physics Body from another.
    void separateFrom(PhysicsState & other);
        
    /// Returns whether this physics body intersect another.
    bool intersectsOther(Collideable * other);
        
    /// Separates this Physics Body from another.
    void separateFrom(Collideable * other);
        
    /// Calculate a "realistic" collision using momentum equation for two bodies
    void momentumCollide(PhysicsState & other);
        
    /// Causes the body to orient towards `pos`.
    void lookAt(glm::vec3 pos, glm::vec3 up);
};

#endif


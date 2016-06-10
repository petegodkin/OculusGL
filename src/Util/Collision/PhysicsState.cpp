///
///
///
///
///
///

#include "PhysicsState.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GraphicsUtil.h"
//#include "../util/functions/functions.h"

PhysicsState::PhysicsState() {
    init(glm::vec3(0, 0, 0), -FACE_DIR, 0.0, 1.0);
}

PhysicsState::PhysicsState(glm::vec3 pos, glm::vec3 dir, float vel) {
    init(pos, dir, vel, 1.0);
}

void PhysicsState::init(glm::vec3 pos, glm::vec3 dir, float vel,
 float mass) {
    setPosition(pos);
    setDirection(dir);
    orientTowardsDirectionForce();
    orientTowardsDirection();
    velocity = vel;
    boundingSphereRadius = 0.0;
    this->mass = mass;
}

float PhysicsState::getBoundingSphereRadius() {return boundingSphereRadius;}
void PhysicsState::setBoundingSphereRadius(float radius) {
    boundingSphereRadius = radius;
}

glm::vec3 PhysicsState::getDirection() {return direction;}

void PhysicsState::setDirection(glm::vec3 dir) {
    direction = GUtils::safeNorm(dir, -FACE_DIR);
}

glm::quat PhysicsState::getOrientation() {return orientation;}
void PhysicsState::setOrientation(glm::quat orient) {
    goalOrient = orientation;
    orientation = orient;
}


float PhysicsState::getVelocity() {return velocity;}
glm::vec3 PhysicsState::getPosition() const {return position;}
void PhysicsState::setPosition(glm::vec3 pos) {
    position = pos;
    setPositionChanged(true);
}
bool PhysicsState::positionChanged() {return positionHasChanged;}
void PhysicsState::setPositionChanged(bool changed) {positionHasChanged = true;} //<-- this is stupid
uint32_t PhysicsState::getCollisionMask() {return 0;}

glm::vec3 PhysicsState::getNewPosition(float dt) {return position
         + velocity * dt * getDirection();}

glm::vec3 PhysicsState::getNewVelocity(float dt, glm::vec3 acc) {
    return getDirection() * velocity + dt * acc;
}

float PhysicsState::getNewVelocity(float dt, float friction) {
    return velocity - velocity * dt * friction;
}

glm::vec3 PhysicsState::getPlanarPosition() {
    return glm::vec3(position.x, 0, position.z);
}

/// Orients this body in its velocity `direction` smoothly.
void PhysicsState::orientTowardsDirection() {
    goalOrient = glm::quat(-FACE_DIR, getDirection()); //TODO normalize dis shit (WHOLE THING)
    turnTime = 0;
}

void PhysicsState::setDirectionToOrientation() {
	setDirection((glm::normalize(getOrientation()) *-FACE_DIR)); //TODO normalize dis shit
}

/// Orients this body in its velocity `direction` now.
void PhysicsState::orientTowardsDirectionForce() {
    goalOrient = orientation = glm::normalize(glm::quat(-FACE_DIR, getDirection()));
}

void PhysicsState::update(float dt) {
    updateOrientation(dt);
    setPosition(getNewPosition(dt));
    velocity = getNewVelocity(dt, 0.05f);
}

void PhysicsState::updateOrientation(float dt) {
    turnTime += dt;
    
    if (dt > TURN_COEFF) {
        orientation = goalOrient;
    }
    else {
        /*setOrientation(glm::normalize(glm::slerp(orientation(dt / TURN_COEFF,
         glm::normalize(goalOrient)));*/
		setOrientation(glm::normalize(goalOrient)); //HOW DO I SLERP WITH TWO QUATS
    }
}

/// Bounds `positions` to be within the volume defined between
/// `cornerStart` and `cornerEnd`.
void PhysicsState::boundPosition(glm::vec3 cornerStart, glm::vec3 cornerEnd) {
    position.x = GUtils::clampToRange(position.x,
     boundingSphereRadius, cornerStart.x, cornerEnd.x);
    position.y = GUtils::clampToRange(position.y,
     boundingSphereRadius, cornerStart.y, cornerEnd.y);
    position.z = GUtils::clampToRange(position.z,
     boundingSphereRadius, cornerStart.z, cornerEnd.z);
}

/// Returns whether this physics body intersect another.
bool PhysicsState::intersectsOther(PhysicsState & other) {
    return intersectsOther(&other);
}
/// Separates this Physics Body from another.
void PhysicsState::separateFrom(PhysicsState & other) {separateFrom(&other);}

/// Returns whether this physics body intersect another.
bool PhysicsState::intersectsOther(Collideable * other) {
    return GUtils::norm(getPosition() - other->getPosition())
     < (getBoundingSphereRadius() + other->getBoundingSphereRadius());
}

/// Separates this Physics Body from another.
void PhysicsState::separateFrom(Collideable * other) {
    if (intersectsOther(other)) {
        glm::vec3 toSelf = getPosition() - other->getPosition();
        toSelf = (getBoundingSphereRadius()
         + other->getBoundingSphereRadius())
         * GUtils::safeNorm(toSelf, glm::vec3(1, 0, 0));
        setPosition(other->getPosition() + toSelf);
    }
}

/// Calculate a "realistic" collision using momentum equation for two bodies
void PhysicsState::momentumCollide(PhysicsState & other) {
    glm::vec3 v1 = velocity * getDirection();
    glm::vec3 v2 = other.velocity * other.getDirection();
    float m1 = mass;
    float m2 = other.mass;
    glm::vec3 x1 = position;
    glm::vec3 x2 = other.position;

    glm::vec3 result1 = v1 - (2 * m2) / (m1 + m2) * 
     glm::dot((v1 - v2), (x1 - x2)) / (glm::normalize(x1 - x2) * glm::normalize(x1 - x2)) * (x1 - x2);
    velocity = GUtils::norm(result1);
    setDirection(result1);

    glm::vec3 result2 = v2 - 
     (2 * m1) / (m1 + m2) * 
     glm::dot((v2 - v1), (x2 - x1)) / (glm::normalize(x2 - x1) * glm::normalize(x2 - x1)) *
     (x2 - x1);
    other.velocity = GUtils::norm(result2);
    other.setDirection(result2);
}

/// Causes the body to orient towards `pos`.
void PhysicsState::lookAt(glm::vec3 pos, glm::vec3 up) {
	glm::mat4 lookAtMat = glm::lookAt(position, pos, up);
    orientation = glm::quat(lookAtMat);
}
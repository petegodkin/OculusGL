///
///
///
///
///
///

#include "Entity.h"
#include "glm/gtc/matrix_transform.hpp"

//#include "Collision/PhysicsState.h"

#include "GraphicsUtil.h"

using namespace glm;

DialogContainer::DialogContainer() {
    curMessage = 0;
}

DialogContainer::DialogContainer(std::string text) {
    setText(text);
}

DialogContainer::DialogContainer(std::vector<std::string> text) {
    setText(text);
}

void DialogContainer::setText(std::string text) {
    clearText();
    messageList.push_back(text);
}

void DialogContainer::setText(std::vector<std::string> text) {
    clearText();
    messageList = text;
}

bool DialogContainer::hasAnyText() {
    return messageList.size() > 0;
}

bool DialogContainer::hasMoreText() {
    return curMessage < (int) messageList.size();
}

std::string DialogContainer::getCurrentText() {
    return messageList[curMessage];
}

void DialogContainer::moveToNextText() {
    curMessage++;
}

void DialogContainer::resetText() {
    curMessage = 0;
}

void DialogContainer::clearText() {
    messageList.clear();
    curMessage = 0;
}

void Entity::setAsFriendly(bool friendly) {
    if (friendly) {
        addToCollisionMask(kFriendlyBody);
        removeFromCollisionMask(kEnemyBody);
    }
    else {
        addToCollisionMask(kEnemyBody);
        removeFromCollisionMask(kFriendlyBody);
    }
}

void Entity::setAsEnemy(bool fiendish) {setAsFriendly(!fiendish);}

///
///
///
///
PhysicsState & Entity::getBody() {return body;}
//PhongMaterial & Entity::getMaterial() {return material;}

//Entity::Entity() {init(PhysicsState(), PhongMaterial(), NULL);}

Entity::Entity(const Entity & o) {
    Entity & other = *((Entity *) &o);
    init(other.getBody());
    
    allowDrawing = other.allowDrawing;
    collisionMask = other.collisionMask;
    drawScale = other.drawScale;
    drawOffset = other.drawOffset;
}

Entity & Entity::operator=(const Entity & o) {
    Entity & other = *((Entity *) &o);
    init(other.getBody());
    
    allowDrawing = other.allowDrawing;
    collisionMask = other.collisionMask;
    drawScale = other.drawScale;
    drawOffset = other.drawOffset;
    
    return (*this);
}

Entity::~Entity() {}

void Entity::init(PhysicsState pstat) {
    body = pstat;
    updateOrientation();
    
    allowDrawing = true;
    collisionMask = Entity::kStaticBody;
    drawScale = glm::vec3(1, 1, 1);
    drawOffset = glm::vec3(0, 0, 0);
}

void Entity::update(float dt) {
    body.update(dt);
	//AAnimator.update(dt);
    for (size_t i = 0; i < operationsPerUpdate.size(); i++) {
        operationsPerUpdate[i](this, dt);
    }
}

void Entity::setDirection(glm::vec3 dir) {
    body.setDirection(dir);
    updateOrientation();
}

float Entity::getBoundingRadius() {
    //return body.boundingSphereRadius;
	return getBoundingSphereRadius();
}

//void Entity::setBoundingRadius(float rad)
//{
//	body.setBoundingSphereRadius(rad);
//	//std::cout << "Entity::SetBoundingRadius: Param: " << rad << ", Body's: " << body.getBoundingSphereRadius() << std::endl;
//}

void Entity::setBoundingBox(glm::vec3 start, glm::vec3 end)
{

}

utility::BoundingBox Entity::getBoundingBox()
{
	return m_boundingBox;
}

void Entity::updateOrientation() {
    body.orientTowardsDirection();
}

glm::quat Entity::getOrientation() {
    return getBody().getOrientation();
}

void Entity::setOrientation(glm::quat orient) {
    getBody().setOrientation(glm::normalize(orient));
}


std::string Entity::description() {
    return "(Entity) {pos: " + GUtils::vecToString(getPosition()) + "}";
}


glm::vec3 Entity::getPosition() {return getBody().getPosition();}

void Entity::setPosition(glm::vec3 pos) {getBody().setPosition(pos);}
bool Entity::positionChanged() {return getBody().positionChanged();}
void Entity::setPositionChanged(bool changed) {
    getBody().setPositionChanged(changed);
}
float Entity::getBoundingSphereRadius() {
    return getBody().getBoundingSphereRadius();
}

uint32_t Entity::getCollisionMask() {
    return collisionMask;
}
void Entity::setCollisionMask(uint32_t mask) {
    collisionMask = mask;
}
void Entity::addToCollisionMask(uint32_t bitMask) {
    collisionMask |= bitMask;
}
void Entity::removeFromCollisionMask(uint32_t bitMask) {
    collisionMask &= ~bitMask;
}
bool Entity::hasCollisionBits(uint32_t bitMask) {
    return (collisionMask & bitMask) != 0;
}


void Entity::lookAt(glm::vec3 pos, glm::vec3 up) {
    body.lookAt(pos, up);
    body.setDirectionToOrientation();
    updateOrientation();
}

Entity::Entity() {

}

/*Entity::Entity(const Shape* shape, std::string filename) : Entity(shape) {
	_texture = std::make_shared<Texture>();
	_texture->setFilename(filename);
	_texture->init();
}*/


Entity::Entity(const MeshSet* shape, glm::vec3 pos) {
	_shape = shape;
	_modelMat = glm::mat4();
	_scale = glm::vec3(shape->getScale());
	m_position = pos;

	body = PhysicsState(pos, glm::vec3(1, 0, 0), 0.0);
	updateOrientation();

	configBoundingBox();

	//	std::cout << "Center in Entity(): " << GUtils::vecToString(_shape->getCenter()) << std::endl;
	std::cout << "BoundingBox in Entity() for " << _shape->m_fileName << ":\n\tmin: "
		<< GUtils::vecToString(m_boundingBox.getStart()) //BBPrint
		<< "\n\tmax: " << GUtils::vecToString(m_boundingBox.getEnd()) 
		<< "\n\tradius: "<< getBoundingRadius()
		<< "\n\tposition: " << GUtils::vecToString(body.getPosition()) << std::endl;

	allowDrawing = true;
	collisionMask = Entity::kStaticBody;
	drawScale = glm::vec3(1, 1, 1);
	drawOffset = glm::vec3(0, 0, 0);
}

void Entity::configBoundingBox()
{
	createBoundingBoxFromMeshSet();
	m_boundingBox.scaleBox(_scale);
	m_boundingBox.moveToPosition(m_position);

	body.setBoundingSphereRadius(m_boundingBox.getRadius());
}

//Entity::~Entity() {
//
//}

void Entity::createBoundingBoxFromMeshSet()
{
	/*for (Mesh *mesh : _shape->getMeshes())
	{
		mesh->calcBoundingBox();
	}*/

	m_boundingBox = _shape->calcBoundingBox();
}

const MeshSet* Entity::shape() {
	return _shape;
}

void Entity::setShape(const MeshSet *shape)
{
	_shape = shape;
	_scale = glm::vec3(shape->getScale());
}

//Texture* Entity::texture() {
//	return _texture;
//}
//
//void Entity::setTexture(Texture* texture) {
//	_texture = texture;
//}

glm::mat4 Entity::modelMat() {
	return  glm::scale(glm::translate(_modelMat, body.getPosition()) * getRotMat(), _scale);
}

void Entity::setScale(float entScale)
{
	_scale = glm::vec3(entScale);
}

glm::mat4 Entity::getRotMat()
{
	glm::vec3 def = _shape->rotations;
	mat4 model_rot_x = rotate(mat4(1.0f), def.x + rotations.x, vec3(1.f, 0.f, 0.f));
	mat4 model_rot_y = rotate(mat4(1.0f), def.y + rotations.y, vec3(0.f, 1.f, 0.f));
	mat4 model_rot_z = rotate(mat4(1.0f), def.z + rotations.z, vec3(0.f, 0.f, 1.f));

	return model_rot_y * model_rot_z * model_rot_x;
}

void Entity::setRotations(glm::vec3 rots)
{
	rotations = rots;
}
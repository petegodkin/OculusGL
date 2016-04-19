///
///
///
///
///
///

#ifndef ____Entity__
#define ____Entity__

#include "Collision\PhysicsState.h"

#include <complex>
#include <functional>

struct DialogContainer {
public:
    int curMessage;
    std::vector<std::string> messageList;
    
    DialogContainer();
    DialogContainer(std::string text);
    DialogContainer(std::vector<std::string> text);

    void setText(std::string text);
    void setText(std::vector<std::string> text);
    
    bool hasAnyText();
    bool hasMoreText();
    std::string getCurrentText();
    
    void moveToNextText();
    void resetText();
    void clearText();
};

class Entity : public Collideable {
public:

    static const uint32_t kStaticBody   = (1 << 0);
    static const uint32_t kDynamicBody  = (1 << 1);
    static const uint32_t kEnemyBody    = (1 << 2);
    static const uint32_t kFriendlyBody = (1 << 3);
    static const uint32_t kPlayerBody   = (1 << 4);
    static const uint32_t kInteractiveBody = (1 << 5);
    
    DialogContainer dialogContainer;
    
    uint32_t collisionMask;
    virtual uint32_t getCollisionMask();
    void setCollisionMask(uint32_t mask);
    void addToCollisionMask(uint32_t bitMask);
    void removeFromCollisionMask(uint32_t bitMask);
    bool hasCollisionBits(uint32_t bitMask);
    
    bool isStaticBody()     {return hasCollisionBits(kStaticBody);}
    bool isDynamicBody()    {return hasCollisionBits(kDynamicBody);}
    bool isEnemyBody()      {return hasCollisionBits(kEnemyBody);}
    bool isFriendlyBody()   {return hasCollisionBits(kFriendlyBody);}
    bool isPlayerBody()     {return hasCollisionBits(kPlayerBody);}
    bool isInteractiveBody()     {return hasCollisionBits(kInteractiveBody);}
    
    void setAsFriendly(bool friendly);
    void setAsEnemy(bool fiendish);
    
    //RolePlayingStats rpgStats;
    
    PhysicsState body;
    //utility::PhongMaterial material;
    //utility::GameModel * model;
    
    bool immobile() {return isStaticBody();}
    bool allowDrawing;
    glm::vec3 drawScale, drawOffset;
    
    PhysicsState & getBody();
    //utility::PhongMaterial & getMaterial();

    std::map<std::string, GUtils::Property> userData;
    std::vector< std::function<void(Entity *, float)> > operationsPerUpdate;
    
    Entity();
    //Entity(utility::PhongMaterial mat, utility::GameModel * mod);
    
    Entity(const Entity & other);
    Entity & operator=(const Entity & other);
    
    virtual ~Entity();
    
	virtual void init(PhysicsState pstat);
    
    //virtual void setModel(utility::GameModel * mod);
    //virtual utility::GameModel * getModel() {return model;}
    //virtual void draw(nsgl::Program & prog);
    virtual void update(float dt);
    virtual void interact(Entity *other) { }
    virtual std::string dialogue() { return std::string(); }
    
    virtual void setDirection(glm::vec3 dir);
    virtual float getBoundingRadius();
    virtual void updateOrientation();
    virtual glm::quat getOrientation();
    virtual void setOrientation(glm::quat orient);
    
    virtual std::string description();
    
      virtual glm::vec3 getPosition();
      virtual void setPosition(glm::vec3 pos);
    virtual bool positionChanged();
    virtual void setPositionChanged(bool changed);
    virtual float getBoundingSphereRadius();
//    
//    /// Causes the body to orient towards `pos`.
    void lookAt(glm::vec3 pos, glm::vec3 up);
};


#endif

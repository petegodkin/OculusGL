///
///
///
///
///
///

#ifndef ____CollisionDetector__
#define ____CollisionDetector__

#include "OctTree.h"

class CollisionDetector {
public:

    struct Intersection {
        Entity * bodyA, * bodyB;
        Intersection() {init(NULL, NULL);}
        Intersection(Entity * a, Entity * b) {init(a, b);}
        void init(Entity * a, Entity * b) {bodyA = a; bodyB = b;}
    };

    int numEncounteredObjs;
    std::vector<Entity *> encounteredObjs;

    OctTree * tree;
    CollisionDetector(OctTree * tree = NULL);

    std::vector<Intersection> getPossibleTreeIntersections();
    std::vector<Intersection> getPossibleTreeIntersections(OctTree * node,
     std::vector<Entity *> parentObjects);

};


#endif



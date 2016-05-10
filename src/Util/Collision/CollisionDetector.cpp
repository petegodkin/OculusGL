///
///
///
///
///
///

#include "CollisionDetector.h"
#include "GraphicsUtil.h"

CollisionDetector::CollisionDetector(OctTree * tree) {
    this->tree = tree;
}

std::vector<CollisionDetector::Intersection>
 CollisionDetector::getPossibleTreeIntersections() {
    std::vector<CollisionDetector::Intersection> intersections;
    ///
    numEncounteredObjs = 0;
    encounteredObjs.clear();
    ///
    if (tree != NULL) {
        tree->checkAndUpdateAllObjects();
        intersections = getPossibleTreeIntersections(tree,
         std::vector<Entity *>());
    }
    ///
    return intersections;
}

std::vector<CollisionDetector::Intersection>
 CollisionDetector::getPossibleTreeIntersections(OctTree * node,
 std::vector<Entity *> parentObjects) {
    UTIL_ASSERT(node != NULL);
    std::vector<Intersection> intersections;
    std::vector<Entity *> enclosedObjects =  node->getEnclosedObjects();
    
    ///
    numEncounteredObjs += enclosedObjects.size();
    for (int i = 0; i < (int) enclosedObjects.size(); i++)
        encounteredObjs.push_back(enclosedObjects[i]);
    ///
    
    /// Check all parent objects against objects in our current node.
    for (int i = 0; i < (int) enclosedObjects.size(); i++) {
        Entity * bodyA = enclosedObjects[i];
        /// Check against parent objects.
        for (int j = 0; j < (int) parentObjects.size(); j++) {
            Entity * bodyB = parentObjects[j];
            if (bodyB->immobile() && bodyA->immobile()) continue;
            float distanceBetween
             = GUtils::norm(bodyA->getPosition() - bodyB->getPosition());
            float desiredDist = bodyA->getBoundingSphereRadius()
             + bodyB->getBoundingSphereRadius();
            
            if (distanceBetween < desiredDist) {
                intersections.push_back(Intersection(bodyA, bodyB));
            }
        }
        
        /// Check against local objects.
        for (int j = i + 1; j < (int) enclosedObjects.size(); j++) {
            Entity * bodyB = enclosedObjects[j];
            float distanceBetween
             = GUtils::norm(bodyA->getPosition() - bodyB->getPosition());
            float desiredDist = bodyA->getBoundingSphereRadius()
             + bodyB->getBoundingSphereRadius();
            
            if (distanceBetween < desiredDist) {
                intersections.push_back(Intersection(bodyA, bodyB));
            }
        }
    }
    
    if (node->activeChildren != 0) {
        /// Merge local object with parent object list.
        for (int i = 0; i < (int) enclosedObjects.size(); i++)
            parentObjects.push_back(enclosedObjects[i]);
        
        /// Now send this off to active children and combine their results.
        for (int i = 0; i < OctTree::kNumNodes; i++) {
            if (node->getChild(i) != NULL) {
                std::vector<Intersection> childIntersections
                 = getPossibleTreeIntersections(node->getChild(i), parentObjects);
                for (int j = 0; j < (int) childIntersections.size(); j++) {
                    intersections.push_back(childIntersections[j]);
                }
            }
        }
    }
    
    return intersections;
}



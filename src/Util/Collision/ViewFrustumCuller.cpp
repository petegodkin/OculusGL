///
///
///
///
///
///
///

#include "ViewFrustumCuller.h"
#include "GraphicsUtil.h"

ViewFrustumCuller::ViewFrustumCuller(OctTree * tree) {
    this->tree = tree;
}

std::vector<Entity *> ViewFrustumCuller::getVisibleObjects(
 utility::ViewFrustum frustum) {
    if (tree != NULL)
        return getVisibleObjects(frustum, tree);
    else
        return std::vector<Entity *>();
}

std::vector<Entity *> ViewFrustumCuller::getVisibleObjects(
 utility::ViewFrustum frustum, OctTree * node) {
    std::vector<Entity *> intersections, enclosedObjects;
    
    if (frustum.enclosesBox(node->getBoundingBox())) {
        enclosedObjects = node->getEnclosedObjects();
        
        for (int i = 0; i < (int) enclosedObjects.size(); i++) {
            if (frustum.enclosesSphere(enclosedObjects[i]->getPosition(), 
				enclosedObjects[i]->getBoundingSphereRadius()))
                intersections.push_back(enclosedObjects[i]);
        }
        
        for (int i = 0; i < OctTree::kNumNodes; i++) {
            if (node->getChild(i) != NULL) {
                std::vector<Entity *> chldInts
                 = getVisibleObjects(frustum, node->getChild(i));
                for (int j = 0; j < (int) chldInts.size(); j++) {
                    intersections.push_back(chldInts[j]);
                }
            }
        }
    }
    
    return intersections;
}



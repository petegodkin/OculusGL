///
///
///
///
///
///

#ifndef ____ViewFrustumCuller__
#define ____ViewFrustumCuller__

#include "OctTree.h"
#include "ViewFrustum.h"

class ViewFrustumCuller {
public:

    OctTree * tree;
    ViewFrustumCuller(OctTree * tree = NULL);

    std::vector<Entity *> getVisibleObjects(utility::ViewFrustum frustum);
    std::vector<Entity *> getVisibleObjects(utility::ViewFrustum frustum,
     OctTree * node);

	void setTree(OctTree *tree);

};


#endif



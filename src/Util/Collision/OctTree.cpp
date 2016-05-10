///
///
///
///
///
///

#define NOMINMAX
#include "OctTree.h"
#include <algorithm>
#include "GraphicsUtil.h"

float OctTree::kMinLeafDim = 0.1f;

OctTree::OctTree() {
    init(BoundingBox(), 1);
}

OctTree::OctTree(OctTree::BoundingBox bound, int maxObjects) {
    init(bound, maxObjects);
}

OctTree::OctTree(const OctTree & other) {
    UTIL_ASSERT(false /* This is not allowed! */);
}

OctTree::~OctTree() {
    pruneEverything();
}

void OctTree::init(BoundingBox bound, int maxObjects) {
    maxObjectsAtLeaf = std::max(1, maxObjects);
    boundingBox = bound;
    parent = NULL;
    for (int i = 0; i < kNumNodes; i++)
        children[i] = NULL;
    activeChildren = 0;
    address = -1;
}


bool OctTree::isActive() {
    return (activeChildren != 0) || (numDirectlyHeldObjects() > 0);
}

bool OctTree::childIsActive(int whichChild) {
    return (activeChildren & (1 << whichChild)) != 0;
}

void OctTree::setChildActive(int whichChild,
 bool shouldBeActive) {
    if (shouldBeActive) activeChildren |=  (1 << whichChild);
    else                activeChildren &= ~(1 << whichChild);
}

int OctTree::numDirectlyHeldObjects() {
    return enclosedObjects.size();
}

int OctTree::getMaxLeafObjects() {
    return maxObjectsAtLeaf;
}

OctTree::BoundingBox OctTree::getBoundingBox() {
    return boundingBox;
}

int OctTree::numActiveChildren() {
    int numActive = 0;
    for (int i = 0; i < kNumNodes; i++) {
        if (children[i] != NULL && children[i]->isActive())
                numActive++;
    }
    return numActive;
}

OctTree * OctTree::getChild(int whichChild) {
    return children[whichChild];
}

OctTree * OctTree::getChild(std::vector<int> childAddress) {
    OctTree * node = this;
    int which = -1;
    
    if (childAddress.size() != 0) {
        while (++which < (int) childAddress.size()
         && node->getChild(childAddress[which]) != NULL) {
            node = node->getChild(childAddress[which]);
        }
        if (which < (int) childAddress.size())
            node = NULL;
    }
    return node;
}

OctTree * OctTree::getParent() {
    return parent;
}

int OctTree::getAddress() {
    return address;
}

std::vector<int> OctTree::getCompleteAddress() {
    std::vector<int> totalAddress;
    OctTree * down = this;
    while (down->parent != NULL) {
        totalAddress.push_back(down->getAddress());
        down = down->parent;
    }
    std::reverse(totalAddress.begin(), totalAddress.end());
    return totalAddress;
}

void OctTree::insert(OctTree::ObjType * newObj) {
    UTIL_ASSERT(newObj != NULL);
    handleObjectPlacement(newObj);
}

void OctTree::add(
 OctTree::ObjType * newObj) {
    if (!directlyHoldsObject(newObj))
        enclosedObjects.push_back(newObj);
}

std::vector<Entity *> OctTree::getEnclosedObjects() {
    return enclosedObjects;
}

bool OctTree::directlyHoldsObject(OctTree::ObjType * obj) {
    bool found = false;
    if (obj != NULL) {
        /// See if it's alread in our enclosed list.
        std::vector<ObjType *>::iterator loc
         = std::find(enclosedObjects.begin(), enclosedObjects.end(), obj);
        found = (loc != enclosedObjects.end());
    }
    /// Finally, return if found
    return found;
}

bool OctTree::encloses(OctTree::Vec p) {
    return boundingBox.encloses(p);
}

bool OctTree::encloses(OctTree::ObjType * obj) {
    Vec pos = obj->getPosition();
    return obj != NULL && encloses(pos);
}

OctTree * OctTree::whereIs(ObjType * obj, bool exhaustive) {
    OctTree * node = NULL;
    
    /// Non-exhaustive approach looks where `obj` should be according to
    /// its position. If it's not there, we simply return NULL. Think of
    /// this is our best guess.
    if (directlyHoldsObject(obj))
        node = this;
    else if (boundingBox.numCornersContained(obj) == kNumNodes) {
        int whichChild = whichChildContainsObject(obj);
        if (children[whichChild] != NULL)
            node = children[whichChild]->whereIs(obj, false);
    }
    
    /// For exhaustive search, we try our best guess first and then search
    /// the entire tree.
    if (exhaustive && node == NULL) {
        int whichChild = -1;
        while (node == NULL && ++whichChild < kNumNodes) {
            if (children[whichChild] != NULL)
                node = children[whichChild]->whereIs(obj);
        }
    }
    
    return node;
}

void OctTree::remove(OctTree::ObjType * obj) {
    OctTree * node = whereIs(obj, true);
    
    if (node != NULL) {
        std::vector<ObjType *>::iterator loc
         = std::find(node->enclosedObjects.begin(), node->enclosedObjects.end(),
         obj);
        
        node->enclosedObjects.erase(loc);
        updateActiveStates(node);
    }
}

void OctTree::updateActiveStates(OctTree * node) {
    while (node->parent != NULL) {
        node->parent->setChildActive(node->address, node->isActive());
        node = node->parent;
    }
}

void OctTree::removeAllObjects() {
    pruneEverything();
    enclosedObjects.clear();
    updateActiveStates(this);
}

OctTree * OctTree::getTreeRoot() {
    OctTree * up = this;
    while (up->parent != NULL)
        up = up->parent;
    return up;
}

std::vector<int> OctTree::getExpectedObjAddress(OctTree::ObjType * obj) {
    std::vector<int> address;
    OctTree * root = getTreeRoot(), * node, * oldNode = NULL ;
    int whichChild;
    
    node = root;
    while (node->boundingBox.numCornersContained(obj) == kNumNodes
     && node != oldNode) {
        if (node != root)
            address.push_back(node->address);
        whichChild = -1;
        oldNode = node;
        while (++whichChild < kNumNodes && oldNode == node) {
            OctTree * child = node->getChild(whichChild);
            if (child != NULL
             && child->boundingBox.numCornersContained(obj) == kNumNodes)
                node = child;
        }
    }
    
    return address;
}

void OctTree::checkAndUpdateObjectLocation(OctTree::ObjType * obj) {
    if (obj != NULL && obj->positionChanged()) {
        handleObjectPlacement(obj);
        
        /// Sanity Check
        OctTree * root = getTreeRoot(), * expNode;
        OctTree * node = root->whereIs(obj, false);
        
        if (node == NULL) {
            node = root->whereIs(obj, true);
            UTIL_ASSERT(node != NULL);
            //UTIL_LOG("Root: " + *root);
            //UTIL_LOG(*obj + " is at node " + node->getCompleteAddress() + " "
             //+ *node);
            UTIL_ASSERT(root->getChild(node->getCompleteAddress()) == node);
            
            for (int i = 0; i < kNumNodes; i++) {
                if (root->getChild(i) != NULL) {
                    //UTIL_LOG("Child [" + i + "]: " + *root->getChild(i));
                    UTIL_LOG("Should have bounding box: "
                     + root->boundingBoxForChild(i).description());
                    UTIL_LOG(std::string("Expects parent bounding box: ")
                     + root->getChild(i)->boundingBoxForParent(root->getChild(i)->getAddress()).description());
                }
                else
                    UTIL_LOG("Child [" + i + "]: NULL");
            }
            
            
            
            std::vector<int> expAddress = root->getExpectedObjAddress(obj);
            UTIL_LOG("Instead, expected address " + expAddress);
            expNode = root->getChild(expAddress);
            if (expNode != NULL)
                //UTIL_LOG("Node: " + *expNode);
            
            UTIL_ASSERT(false);
        }
    }
}

void OctTree::checkAndUpdateAllObjects() {
    flushObjectsToLeaves();
    /// Handle this node's object first
    std::vector<Entity *> saved = enclosedObjects;
    for (int i = 0; i < (int) saved.size(); i++)
        checkAndUpdateObjectLocation(saved[i]);
    /// This subtree is not a leaf, so dig down further
    if (activeChildren != 0) {
        for (int i = 0; i < kNumNodes; i++) {
            if (children[i] != NULL)
                children[i]->checkAndUpdateAllObjects();
        }
    }
    pruneEmptyBranches();
}

void OctTree::flushObjectsToLeaves() {
    if (activeChildren == 0
     && numDirectlyHeldObjects() > getMaxLeafObjects()
     && GUtils::norm(getBoundingBox().getDimensions()) / 2.0 > kMinLeafDim) {
        /// Flush enclosed objects to children
        std::vector<ObjType *> saved = enclosedObjects;
        for (int i = 0; i < (int) saved.size(); i++) {
            handleObjectPlacement(saved[i]);
        }
    }
}

int OctTree::totalNumberOfObjectsInHierarchy() {
    int num = numDirectlyHeldObjects();
    for (int i = 0; i < kNumNodes; i++) {
        if (getChild(i) != NULL)
            num += getChild(i)->totalNumberOfObjectsInHierarchy();
    }
    return num;
}

void OctTree::pruneEmptyBranches() {
    for (int i = 0; i < kNumNodes; i++) {
        if (children[i] != NULL && children[i]->activeChildren == 0
         && children[i]->numDirectlyHeldObjects() == 0) {
            UTIL_ASSERT(children[i]->totalNumberOfObjectsInHierarchy() == 0);
            children[i]->pruneEverything();
            delete children[i];
            children[i] = NULL;
            setChildActive(i, false);
        }
    }
}

void OctTree::pruneEverything() {
    for (int i = 0; i < kNumNodes; i++) {
        if (children[i] != NULL) {
            children[i]->pruneEverything();
            delete children[i];
            children[i] = NULL;
        }
    }
    activeChildren = 0;
}

bool OctTree::objectLiesOnChildBoundaries(OctTree::ObjType * obj) {
    int whichChild = -1;
    BoundingBox objBBox = boundingBox.boundingBoxForCollideable(obj);
    BoundingBox chdBox;
    int numThatEnclose = 0;
    while (++whichChild < kNumNodes) {
        chdBox = boundingBoxForChild(whichChild);
        int numCorners = chdBox.numCornersContained(objBBox);
        if (numCorners > 0 && numCorners < kNumNodes)
            numThatEnclose++;
    }
    
    return numThatEnclose > 0;
}

bool OctTree::touchesBorder(OctTree::ObjType * obj) {
    int numCorners = boundingBox.numCornersContained(boundingBox
     .boundingBoxForCollideable(obj));
    return 0 < numCorners && numCorners < kNumNodes;
}

void OctTree::handleObjectPlacement(OctTree::ObjType * obj) {
    int whereIsObj;
    BoundingBox objbox = boundingBox.boundingBoxForCollideable(obj);
    /// Remove object and try the parent.
    if (parent != NULL
     && boundingBox.numCornersContained(objbox) != kNumNodes) {
        remove(obj);
        parent->handleObjectPlacement(obj);
    }
    /// We need to rebase and then add the object.
    else {
     
        /// First, if the do not enclode the object, then
        while (boundingBox.numCornersContained(objbox) != kNumNodes) {
            whereIsObj = whichChildForPoint(obj->getPosition());
            rebase((~whereIsObj) & (kNumNodes - 1));
        }
        
        UTIL_ASSERT(encloses(obj->getPosition()));
        UTIL_ASSERT(boundingBox.numCornersContained(objbox) == kNumNodes);
        BoundingBox childbox = boundingBoxForChild(whichChildForPoint(
         obj->getPosition()));
        
        /// We pass it onto the child
        if (!objectLiesOnChildBoundaries(obj) && (activeChildren != 0
         || numDirectlyHeldObjects() > getMaxLeafObjects())
         && (GUtils::norm(getBoundingBox().getDimensions()) / 2.0) > kMinLeafDim
         && childbox.numCornersContained(objbox) == kNumNodes) {
            remove(obj);
            
            whereIsObj = whichChildForPoint(obj->getPosition());
            UTIL_ASSERT(0 <= whereIsObj && whereIsObj < kNumNodes);
            if (children[whereIsObj] == NULL) {
                children[whereIsObj] = createChild(whereIsObj);
            }
            
            //setChildActive(whereIsObj, true);
            children[whereIsObj]->insert(obj);
        }
        /// We keep it to ourselves
        else {
            UTIL_ASSERT(encloses(obj->getPosition()));
            UTIL_ASSERT(boundingBox.numCornersContained(objbox) == kNumNodes);
            add(obj);
            updateActiveStates(this);
            obj->setPositionChanged(false);
        }
    }
     
}

void OctTree::rebase(int whichChildAmI) {
    UTIL_ASSERT(parent == NULL);
    UTIL_ASSERT(whichChildAmI >= 0 && whichChildAmI < kNumNodes);
    UTIL_ASSERT(GUtils::norm(boundingBox.getDimensions()) < 100000);
    
    OctTree * copy = createChild(boundingBox);
    copy->enclosedObjects = enclosedObjects;
    enclosedObjects.clear();
    
    for (int i = 0; i < kNumNodes; i++) {
        copy->children[i] = children[i];
        if (children[i] != NULL)
            copy->children[i]->parent = copy;
        children[i] = NULL;
    }
    
    copy->address = whichChildAmI;
    copy->activeChildren = activeChildren;
    activeChildren = 0;

    setChildActive(whichChildAmI, copy->isActive());
    children[whichChildAmI] = copy;
    boundingBox = boundingBoxForParent(whichChildAmI);
    updateActiveStates(copy);
}

OctTree::BoundingBox OctTree::boundingBoxForParent(int whichChildAmI) {
    Vec pCenter = boundingBox.getCorner((~whichChildAmI) & (kNumNodes - 1));
    Vec diff = boundingBox.getDimensions();
    return BoundingBox(pCenter - diff, pCenter + diff);
}

OctTree::BoundingBox OctTree::boundingBoxForChild(int whichChild) {
    Vec center = boundingBox.getCenter();
    Vec corner = boundingBox.getCorner(whichChild);
    return BoundingBox(center, corner);
}

int OctTree::whichChildForPoint(OctTree::Vec p) {
    int whichChild = 0;
    Vec adjustedVec = p - boundingBox.getCenter();
    for (int i = 0; i < kNumDims; i++) {
        if (adjustedVec[i] < 0)
            whichChild = whichChild | (1 << i);
    }
    return whichChild;
}

int OctTree::whichChildContainsObject(
 OctTree::ObjType * obj) {
    UTIL_ASSERT(obj != NULL);
    return whichChildForPoint(obj->getPosition());
}

OctTree * OctTree::createChild(OctTree::BoundingBox bound) {
    OctTree * child = new OctTree(bound, getMaxLeafObjects());
    child->parent = this;
    return child;
}

OctTree * OctTree::createChild(int whichChild) {
    UTIL_ASSERT(whichChild >= 0 && whichChild < kNumNodes);
    OctTree * child = createChild(boundingBoxForChild(whichChild));
    child->address = whichChild;
    return child;
}

std::string OctTree::description() {
    return std::string("(OctTree)") + " { address: " + address + ", bound: "
     + "\n[" + boundingBox.description() + "]\n" + ", numObjs: " + numDirectlyHeldObjects()
     + ", activeChildren: " + activeChildren + " }";
}


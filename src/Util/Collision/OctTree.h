///
///
///
///
///
///

#ifndef ____OctTree__
#define ____OctTree__

#include "Entity.h"
#include "MorphableEntity.h"

class OctTree {
public:

    static const int kNumDims = 3;
    static const int kNumNodes = 8;
    typedef utility::BoundingBox BoundingBox;
	typedef glm::vec3 Vec;
    typedef Entity ObjType;
    
    BoundingBox boundingBox;
    int address;
private:
    OctTree * parent;
    OctTree * children[kNumNodes];
    std::vector< ObjType * > enclosedObjects;
public:
    int maxObjectsAtLeaf;
    static float kMinLeafDim;
    int activeChildren;
    
    /// Creates a default tree with an 1x1x1 bounding box and 1 object per
    /// level.
    OctTree();
    /// Creates a spacial data tree using the given `bound` and the max
    /// number of objects that can occupy a given leaf.
    OctTree(BoundingBox bound, int maxObjects);
    
private:
    /// No one should call this constructor.
    OctTree(const OctTree & other);
public:
    /// Upon deconstruction, the SpatialDataTree will prune everything.
    ~OctTree();
    
    /// In-place initializer for this OctTree.
    void init(BoundingBox bound, int maxObjects);
    
    /// Returns whether this node, or child nodes, contain objects.
    bool isActive();
    /// Returns back whether a child is active.
    bool childIsActive(int whichChild);
    void setChildActive(int whichChild, bool shouldBeActive);
    
    void updateActiveStates(OctTree * node);
    
    /// Returns the maximum number of objects that will be held at a leaf
    /// before this node will automatically subdivide.
    int getMaxLeafObjects();
    
    bool touchesBorder(ObjType * obj);
    
    BoundingBox getBoundingBox();
    int numNodesPerSubdivision() {return kNumNodes;}
    /// Returns a pointer to a child. May be NULL.
    
    int getAddress();
    std::vector<int> getCompleteAddress();
    
    OctTree * getChild(int whichChild);
    /// Returns the child for the given sequence of child address numbers.
    /// If `childAddress` is empty, will return the current tree. Otherwise,
    /// it tries getting the child. If it fails, or the child is NULL, returns
    /// NULL.
    OctTree * getChild(std::vector<int> childAddress);
    
    int numActiveChildren();
    /// Returns a pointer to the parent. May be NULL.
    OctTree * getParent();
    
    /// Places `newObj` on the pending insertion queue to be taken care of
    /// on the next call to `update`.
    void insert(ObjType * newObj);
private:
    /// Adds `newObj` to `enclosedObjects`, being careful not to add a
    /// duplicate to this list.
    void add(ObjType * newObj);
    
public:

    int numDirectlyHeldObjects();
    bool directlyHoldsObject(ObjType * obj);
    std::vector<ObjType *> getEnclosedObjects();
    
    OctTree * getTreeRoot();
    int totalNumberOfObjectsInHierarchy();
    
    ///
    OctTree * whereIs(ObjType * obj, bool exhaustive = false);
    std::vector<int> getExpectedObjAddress(ObjType * obj);
    
    /// Tests if `obj` is within `boundingBox`.
    bool encloses(ObjType * obj);
    /// Tests if `p` is within `boundingBox`.
    bool encloses(Vec p);
    
    /// Safely tries to remove `obj` from the subtree formed by this node.
    /// If `obj` is in the pending queue, the queue will be flushed and
    /// the object will then be removed.
    void remove(ObjType * obj);
    
    /// Removes all object from the tree, pruning every part of the tree
    /// structure as well.
    void removeAllObjects();

    /// Checks whether `obj` has registered a change in its position
    /// via `positionChanged`. If the position changed,
    /// `handleObjectPlacement` is called.
    void checkAndUpdateObjectLocation(ObjType * obj);
    
    /// Flushes all pending insertions, and then recursively updates all
    /// objects that have moved. After which any empty branches will be
    /// automatically pruned.
    void checkAndUpdateAllObjects();

    /// Pulls off all of the objects from the pending queue and adds them
    /// to this tree as appropriate. If flushing the queue would make this
    /// leaf to subdivide, then this is handled automatically.
    void flushObjectsToLeaves();
    
    /// Causes empty branches and leaves to be deleted.
    void pruneEmptyBranches();
    
    /// Deletes all children in the tree.
    void pruneEverything();
    
    bool objectLiesOnChildBoundaries(ObjType * obj);
private:
    /// Tries to insert `obj` into the hiearchy. Moves `obj` up the
    /// hierarchy as necessary and places it on the pending queue for the
    /// highest node in this tree. If this tree cannot "contain" `obj`, the
    /// tree will `rebase` as needed to accomodate the new object and place
    /// it on its pending queue. `obj` will then have its positionChanged
    /// field set to FALSE.
    void handleObjectPlacement(ObjType * obj);
public:
    /// Gets the bounding box for an imaginary parent based on if this
    /// current node where `whichChildAmI`. Essentially performs the reverse
    /// `boundingBoxForChild`.
    BoundingBox boundingBoxForParent(int whichChildAmI);
private:
    /// Used to rebase this tree as if this current node were
    /// `whichChildAmI`. Assumes that this node does not have a parent
    /// and that `whichChildAmI` is between 0 and `kNumNodes`.
    void rebase(int whichChildAmI);
public:
    /// Returns the bounding box for a child. The convention for how
    /// `whichChild` is changed into a bounding box for arbitrary space
    /// works as such:
    ///     1) Get a vector that points along the positive axis of each
    ///         dimension. Here I use the dimension of the parent tree
    ///         as the vector (hint: this will cause the eventual box
    ///         formation to be the desired size).
    ///     2) Associate each bit of `whichChild` from least to most
    ///         significant bit with a dimension. In this case I make it
    ///         is so that bit 0 (whichChild & 1) corresponds to the x
    ///         axis, and so on.
    ///     3) For each bit that is non-zero, negate the corresponding axis
    ///         in our vector from (1).
    ///     4) Our bounding box is from the center of the current bounding
    ///         box to the current center + the vector from (1) that has
    ///         been changed in (3).
    ///
    /// NOTE: This assumes SpatialBoundingBox will automatically accomodate
    /// arbitrary `start` and `end` vectors.
    BoundingBox boundingBoxForChild(int whichChild);
    
    /// Returns the child index for the point `p`. It is assumed that
    /// `p` is already within `boundingBox`. If you are unsure of this,
    /// use `containsPoint` to determine if it is.
    int whichChildForPoint(Vec p);
    
    /// Returns the bitmask that tells which child would "contain" `obj`.
    int whichChildContainsObject(ObjType * obj);
    
    std::string description();
private:
    /// Creates a new child using `bound`, setting `this` as the parent.
    OctTree * createChild(BoundingBox bound);
    
    /// Creates a new node at spacial region `which`. (See
    /// `boundingBoxForChild` for details about bounding box calculations).
    OctTree * createChild(int whichChild);
};




#endif

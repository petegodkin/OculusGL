#include "BoundingBox.h"
//#include <src/util/util_base.h>
//#include "../../util/util_base.h"

using namespace utility;

BoundingBox::BoundingBox(glm::vec3 s, glm::vec3 e) {
    start = end = glm::vec3(0.0);
    for (int i = 0; i < kNumComps; i++) {
        start[i] = std::fmin(s[i], e[i]);
        end[i] = std::fmax(s[i], e[i]);
    }
}

glm::vec3 BoundingBox::getStart() {return start;}
glm::vec3 BoundingBox::getEnd() {return end;}
glm::vec3 BoundingBox::getDimensions() {return end - start;}
glm::vec3 BoundingBox::getCenter() {return (start + end) / float(2.0);}

BoundingBox BoundingBox::withOffset(glm::vec3 offset) {
    return BoundingBox(start + offset, end + offset);
}

bool BoundingBox::encloses(glm::vec3 p) {
    bool inRange = true;
    int whichComp = -1;
    while (++whichComp < kNumComps && inRange) {
        /// Here, if components are equal, we treat them as if they
        /// mean "unbounded".
        if ((end[whichComp] - start[whichComp]) > float(0.0001)) {
            inRange = inRange
             && start[whichComp] <= p[whichComp]
             &&     p[whichComp] <  end[whichComp];
        }
    }
    return inRange;
}

bool BoundingBox::encloses(Collideable * obj) {
    return encloses(obj->getPosition());
}

BoundingBox BoundingBox::boundingBoxForCollideable(Collideable * obj) {
    glm::vec3 center = obj->getPosition(), diff = glm::vec3(0.0);
    for (int i = 0; i < kNumComps; i++)
        diff[i] = obj->getBoundingSphereRadius();
    return BoundingBox(center - diff, center + diff);
}

bool BoundingBox::intersects(Collideable * obj) {
    return intersects(boundingBoxForCollideable(obj));
}

bool BoundingBox::intersects(BoundingBox other) {
    int whichCorner = -1;
    bool inters = false;
    while (!inters && ++whichCorner < kNumCorners)
        inters = encloses(other.getCorner(whichCorner));
    return inters;
}

/// Test ray casting intersection
bool BoundingBox::intersects(glm::vec3 start, glm::vec3 dir) {
    bool intersectsLine = false;
    int whichCorner = -1;
    while (!intersectsLine && (++whichCorner < kNumCorners)) {
        glm::vec3 corner = getCorner(whichCorner);
        glm::vec3 amp = start - corner;
        glm::vec3 fromLineToCorner = glm::dot(amp, dir) * dir - amp;
        /// Equation:
        ///     (1) C - l(t) = l2c
        ///     (2) C - (S + t * d) = l2c
        ///     (3) C - S - l2c = t * d
        /// (d is normalized)
        ///     (4) norm(C - S - l2c) = abs(t)
        float t = GUtils::norm(corner - start - fromLineToCorner);
        if (GUtils::norm((corner - (start + t * dir) - fromLineToCorner))
         > 0.0001) {
            t = -t;
        }
        intersectsLine = encloses(start + t * dir);
    }
    return intersectsLine;
}

/// Test sphere intersection
bool BoundingBox::intersects(glm::vec3 center, float radius) {
    float dist_squared = radius * radius;
    
    if (center.x < start.x) {
        dist_squared -= (center.x - start.x) * (center.x - start.x);
    }
    else if (center.x > end.x) {
        dist_squared -= (center.x - end.x) * (center.x - end.x);
    }
    
    if (center.y < start.y) {
        dist_squared -= (center.y - start.y) * (center.y - start.y);
    }
    else if (center.y > end.y) {
        dist_squared -= (center.y - end.y) * (center.y - end.y);
    }
    
    if (center.z < start.z) {
        dist_squared -= (center.z - start.z) * (center.z - start.z);
    }
    else if (center.z > end.z) {
        dist_squared -= (center.z - end.z) * (center.z - end.z);
    }
    
    return dist_squared > 0;
}

/// Test sphere inscription
bool BoundingBox::encloses(glm::vec3 center, float radius) {
    if (center.x - radius < start.x ||
     center.y - radius < start.y ||
     center.z - radius < start.z) {
        return false;
    }
    
    if (center.x + radius > end.x ||
     center.y + radius > end.y ||
     center.z + radius > end.z) {
        return false;
    }
    
    return true;
}

bool BoundingBox::overlapsWith(Collideable * obj) {
    return overlapsWith(boundingBoxForCollideable(obj));
}

bool BoundingBox::overlapsWith(BoundingBox other) {
    return this->intersects(other) || other.intersects(*this);
}

int BoundingBox::numCornersContained(Collideable * obj) {
    return numCornersContained(boundingBoxForCollideable(obj));
}

int BoundingBox::numCornersContained(BoundingBox other) {
    int whichCorner = -1;
    int numContained = 0;
    while (++whichCorner < kNumCorners) {
        if (encloses(other.getCorner(whichCorner)))
            numContained++;
    }
    return numContained;
}

glm::vec3 BoundingBox::getCorner(int whichCorner) {
    glm::vec3 diff = getDimensions();
    for (int i = 0; i < kNumComps; i++) {
        if ((whichCorner & (1 << i)) != 0)
            diff[i] = float(0);
    }
    return getStart() + diff;
}

std::string BoundingBox::description() {
    return std::string("(BoundingBox) { start: ") +
		GUtils::vecToString(getStart()) + ", end: " + 
		GUtils::vecToString(getEnd()) + ", dim: " +
		GUtils::vecToString(getDimensions()) + "}";
}



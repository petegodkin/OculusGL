///
///
///
///
///
///


#include "ViewFrustum.h"
#include <vector>

//#include <src/util/functions/functions.h>
//#include "../../util/functions/functions.h"

using namespace utility;

ViewFrustum::ViewFrustum() {
    PV = glm::mat4();
    extractPlanes(true);
}
ViewFrustum::ViewFrustum(glm::mat4 ProjView) {
    PV = ProjView;
    extractPlanes(true);
}

glm::vec4 ViewFrustum::getRow(int ndx, glm::mat4 mat)
{
	/*return glm::vec4(
		mat[ndx][0],
		mat[ndx][1],
		mat[ndx][2],
		mat[ndx][3]);*/

	return glm::vec4(
		mat[0][ndx],
		mat[1][ndx],
		mat[2][ndx],
		mat[3][ndx]);
}

glm::vec3 ViewFrustum::getRow(int ndx, glm::mat3 mat)
{
	return glm::vec3(
		mat[ndx][0],
		mat[ndx][1],
		mat[ndx][2]);
}

void ViewFrustum::extractPlanes(bool normalize) {
	std::cout << "The ViewMatrix:\n" << GUtils::matToString(PV);

    glm::vec4 row;
    
    //Left plane
    //row = (PV.block<1, 4>(3, 0) + PV.block<1, 4>(0, 0)).transpose();
	
	row = getRow(3, PV) + getRow(0, PV);
    planes[PLANE_LEFT] = Plane(row.x, row.y, row.z, row.w);

    //Right plane
	row = getRow(3, PV) - getRow(0, PV);
    planes[PLANE_RIGHT] = Plane(row.x, row.y, row.z, row.w);

    //Bottom plane
	row = getRow(3, PV) + getRow(1, PV);
    planes[PLANE_BOTTOM] = Plane(row.x, row.y, row.z, row.w);

    //Top plane
	row = getRow(3, PV) - getRow(1, PV);
    planes[PLANE_TOP] = Plane(row.x, row.y, row.z, row.w);

    //Near plane
	row = getRow(3, PV) + getRow(2, PV);
    planes[PLANE_NEAR] = Plane(row.x, row.y, row.z, row.w);

    //Far plane
	row = getRow(3, PV) - getRow(2, PV);
    planes[PLANE_FAR] = Plane(row.x, row.y, row.z, row.w);
    
	std::vector<std::string> labels = {"Left: ", "Right: ", "Bott: ", "Top: ", "Near: ", "Far: "};

	/*std::cout << "Current Planes:" << std::endl;
	for (int i = 0; i < kNumPlanes; i++)
	{
<<<<<<< HEAD
		Plane pl = planes[i];
		std::cout << "\t" << labels[i] << "\t(" <<  
							 pl.a() << ", " << 
							 pl.b() << ", " <<
						   	 pl.c() << ", " <<
							 pl.d() << ")" << std::endl;
=======
		std::cout << "\t" << planes[i].description(labels[i]);
>>>>>>> 8879dc75c56498bec3c7e238831dcb9fbb39c58a
	}*/

    if (normalize) {
        for (int i = 0; i < kNumPlanes; i++)
            planes[i].normalize();
    }

	std::cout << "Current Normalized Planes:" << std::endl;
	for (int i = 0; i < kNumPlanes; i++)
	{
		std::cout << "\t" << planes[i].description(labels[i]);
	}
    
    for (int i = 0; i < kNumCorners; i++)
        corners[i] = calculateCorner(i);
}

glm::vec3 ViewFrustum::getCorner(int whichCorner) {
    return corners[whichCorner];
}

glm::vec3 ViewFrustum::calculateCorner(int whichCorner) {
    std::vector<PlaneID> desiredPlanes, lower, upper;
	glm::mat3x4 sysEq;
    glm::mat3 abcValues;
    glm::vec3 dValues;
    
    /// Find out which planes we are talking about
	lower = { PLANE_LEFT, PLANE_NEAR, PLANE_BOTTOM };
	upper = { PLANE_RIGHT, PLANE_FAR, PLANE_TOP };
    for (int i = 0; i < (int) lower.size(); i++) {
        if ((whichCorner & (1 << i)) != 0)
            desiredPlanes.push_back(lower[i]);
        else
            desiredPlanes.push_back(upper[i]);
    }
    
    /// Find the intersection of those three planes.
    //abcValues.block<1, 3>(0, 0) = planes[desiredPlanes[0]].abc().transpose();
    //abcValues.block<1, 3>(1, 0) = planes[desiredPlanes[1]].abc().transpose();
    //abcValues.block<1, 3>(2, 0) = planes[desiredPlanes[2]].abc().transpose();

	sysEq = glm::mat3x4(
		glm::vec4(planes[desiredPlanes[0]].abc(), -planes[desiredPlanes[0]].d()),
		glm::vec4(planes[desiredPlanes[1]].abc(), -planes[desiredPlanes[1]].d()),
		glm::vec4(planes[desiredPlanes[2]].abc(), -planes[desiredPlanes[2]].d()));

	/*std::vector<float> r0 = getRow(0, planes[desiredPlanes[0]].abc);
	std::vector<float> r1 = getRow(1, planes[desiredPlanes[1]].abc);
	std::vector<float> r2 = getRow(2, planes[desiredPlanes[2]].abc);
	std::vector<std::vector<float>>
    
    dValues = glm::vec3(-planes[desiredPlanes[0]].d(), -planes[desiredPlanes[1]].d(),
     -planes[desiredPlanes[2]].d());*/
    
	return GUtils::gauss(sysEq);
}

///
bool ViewFrustum::encloses(glm::vec3 point) {
    bool doesEnclose = true;
    int whichPlane = -1;
    while (doesEnclose && ++whichPlane < kNumPlanes)
        doesEnclose = planes[whichPlane].distanceTo(point) >= 0;
    return doesEnclose;
}

bool ViewFrustum::enclosesBox(BoundingBox box) {
	bool in = true;
    int numCornersContained;
	
	for (int i = 0; i < kNumPlanes && in; i++) {
        numCornersContained = 0;
		for (int j = 0; j < BoundingBox::kNumCorners; j++) {
            if (planes[i].distanceTo(box.getCorner(j)) >= 0)
                numCornersContained++;
		}
        in = numCornersContained;
	}
	return in;
}

bool ViewFrustum::enclosesSphere(glm::vec3 center, float rad) {
	float dist;

	for(int i=0; i < kNumPlanes; i++) {
		dist = planes[i].distanceTo(center);
		if (dist < -rad)
			return false;
	}
	return true;
}


/*
bool ViewFrustum::partiallyOverlaps(BoundingBox box) {
    bool boxOverlaps = false;
    int whichCorner = -1;
    
    while (!boxOverlaps && ++whichCorner < kNumCorners)
        boxOverlaps = box.encloses(getCorner(whichCorner));

    return boxOverlaps|| numEnclosedCorners(box) != 0;
}*/

int ViewFrustum::numEnclosedCorners(BoundingBox box) {
    int numCorners = 0;
    for (int i = 0; i < BoundingBox::kNumCorners; i++) {
        if (encloses(box.getCorner(i)))
            numCorners++;
    }
    return numCorners;
}

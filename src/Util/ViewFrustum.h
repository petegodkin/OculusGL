///
///
///
///
///
///


#ifndef ____game_ViewFrustum__
#define ____game_ViewFrustum__

#include "Plane.h"
#include "BoundingBox.h"

//#include "../spatial/BoundingBox.h"

namespace utility {
	///
    struct ViewFrustum {
    public:
		typedef enum {
			PLANE_LEFT = 0,
			PLANE_RIGHT,
			PLANE_TOP,
			PLANE_BOTTOM,
			PLANE_NEAR,
			PLANE_FAR,
            NUM_PLANES // Do not remove this
		} PlaneID;
        
        static const int kNumPlanes = NUM_PLANES;
        static const int kNumCorners = 8;
        
        glm::mat4 PV;
        
        ViewFrustum();
		ViewFrustum(glm::mat4 ProjView);

		static glm::vec3 getRow(int ndx, glm::mat3 mat);
		static glm::vec4 getRow(int ndx, glm::mat4 mat);
		void extractPlanes(bool normalize = true);
        
        glm::vec3 getCorner(int whichCorner);
        glm::vec3 calculateCorner(int whichCorner);
        
        ///
		bool encloses(glm::vec3 point);
		bool enclosesBox(BoundingBox box);
		bool enclosesSphere(glm::vec3 center, float rad);
        //bool partiallyOverlaps(BoundingBox box);
        int numEnclosedCorners(BoundingBox box);
        
    private:
        Plane planes[kNumPlanes];
        glm::vec3 corners[kNumCorners];
    };

}

#endif
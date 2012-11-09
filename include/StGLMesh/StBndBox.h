/**
 * Copyright © 2011 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StBndBox_h_
#define __StBndBox_h_

#include "StBndContainer.h"

/**
 * The bounding box of a finite geometric object is the box with minimal volume, that contains the object.
 * Edges of bounding box are parallel to the coordinate axes, and is thus defined by its maximum and minimum extents for all axes.
 * This is the computationally simplest of all linear bounding containers.
 */
class ST_LOCAL StBndBox : public StBndContainer {

        protected:

    StGLVec3 myMin; //!< x/y/z minimal values
    StGLVec3 myMax; //!< x/y/z maximal values

        public: //!< inheritance methods

    virtual ~StBndBox();
    virtual void reset();
    virtual bool isIn(const StGLVec3& thePnt) const;
    virtual void enlarge(const GLfloat theTolerance);
    virtual void enlarge(const StGLVec3& theNewPnt);
    virtual void enlarge(const StArray<StGLVec3>& thePoints);

        public:

    /**
     * Create the empty (void) bounding box.
     */
    StBndBox();

    /**
     * Define the bounding box with min / max vectors.
     */
    StBndBox(const StGLVec3& theMin,
             const StGLVec3& theMax)
    : myMin(theMin),
      myMax(theMax) {
        setDefined();
    }

    /**
     * Return the x/y/z minimal values.
     */
    const StGLVec3& getMin() const {
        return myMin;
    }

    /**
     * Return the x/y/z maximal values.
     */
    const StGLVec3& getMax() const {
        return myMax;
    }

    /**
     * Get width for the boundary box.
     */
    GLfloat getDX() const {
        return myMax.x() - myMin.x();
    }

    /**
     * Get height for the boundary box.
     */
    GLfloat getDY() const {
        return myMax.y() - myMin.y();
    }

    /**
     * Get DZ for the boundary box.
     */
    GLfloat getDZ() const {
        return myMax.z() - myMin.z();
    }

    /**
     * Compute boundary box center.
     */
    StGLVec3 getCenter() const {
        return StGLVec3::getLERP(myMin, myMax, 0.5f);
    }

    /**
     * Check that bounding boxes are disjoint.
     * @param theBndBox (const StBndBox& ) - another bounding box;
     * @return true if bounding boxes are disjoint.
     */
    bool areDisjoint(const StBndBox& theBndBox) const;

    /**
     * Check that bounding boxes has intersection.
     * @param theBndBox (const StBndBox& ) - another bounding box;
     * @return true if bounding boxes has intersection.
     */
    bool areIntersect(const StBndBox& theBndBox) const {
        return !areDisjoint(theBndBox);
    }

};

#endif //__StBndBox_h_
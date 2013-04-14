/**
 * Copyright © 2010-2013 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StGLUVSphere_h_
#define __StGLUVSphere_h_

#include "StGLMesh.h"

class StBndSphere;

/**
 * Class represents configurable UV sphere with computed vertices, noramles, texture coordinates
 * and indices to faster (and correct) draw using one glMultiDrawElements() call.
 */
class StGLUVSphere : public StGLMesh {

        public:

    /**
     * Defines the UV sphere mesh with specified parameters.
     */
    ST_CPPEXPORT StGLUVSphere(const StGLVec3& theCenter,
                              const GLfloat   theRadius,
                              const size_t    theRings);

    /**
     * Defines the UV sphere from boundary sphere.
     */
    ST_CPPEXPORT StGLUVSphere(const StBndSphere& theBndSphere,
                              const size_t       theRings);

    ST_CPPEXPORT virtual ~StGLUVSphere();

    /**
     * Compute the mesh using current configuration.
     */
    ST_CPPEXPORT virtual bool computeMesh();

        protected:

    /**
     * Draw the primitives itself.
     */
    ST_CPPEXPORT virtual void drawKernel(StGLContext& theCtx) const;

        private:

    StArrayList<GLsizei> myPrimCounts;
    StArrayList<void*>   myIndPointers;
    StGLVec3             myCenter;
    GLfloat              myRadius;
    size_t               myRings;

};

#endif //__StGLUVSphere_h_

/**
 * StGLWidgets, small C++ toolkit for writing GUI using OpenGL.
 * Copyright © 2009-2016 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StGLSeekBar_h_
#define __StGLSeekBar_h_

#include <StGLWidgets/StGLWidget.h>
#include <StGL/StGLVertexBuffer.h>

/**
 * Simple seeking bar widget.
 */
class StGLSeekBar : public StGLWidget {

        public: //! @name public methods

    /**
     * Default constructor.
     */
    ST_CPPEXPORT StGLSeekBar(StGLWidget* theParent,
                             const int   theTop,
                             const int   theMargin);

    /**
     * Destructor.
     */
    ST_CPPEXPORT virtual ~StGLSeekBar();

    /**
     * @param theProgress - current progress from 0.0f to 1.0f;
     */
    ST_LOCAL void setProgress(const GLfloat theProgress) {
        myProgress = theProgress;
    }

    ST_LOCAL void setMoveTolerance(const int theTolerPx) {
        myMoveTolerPx = theTolerPx;
    }

    ST_CPPEXPORT virtual void stglResize() ST_ATTR_OVERRIDE;
    ST_CPPEXPORT virtual bool stglInit() ST_ATTR_OVERRIDE;
    ST_CPPEXPORT virtual void stglUpdate(const StPointD_t& theCursor) ST_ATTR_OVERRIDE;
    ST_CPPEXPORT virtual void stglDraw(unsigned int theView) ST_ATTR_OVERRIDE;
    ST_CPPEXPORT virtual bool doScroll(const StScrollEvent& theEvent) ST_ATTR_OVERRIDE;

        public:  //!< Signals

    struct {
        /**
         * Emit callback Slot on mouse click.
         * @param theMouseBtnId mouse button id
         * @param theProgress   current progress value
         */
        StSignal<void (const int , const double )> onSeekClick;

        /**
         * Emit callback Slot on scrolling.
         * @param theDelta scrolling direction
         */
        StSignal<void (const double )> onSeekScroll;
    } signals;

        private: //! @name callback Slots (private overriders)

    ST_LOCAL void doMouseClick  (const int );
    ST_LOCAL void doMouseUnclick(const int );

        private: //! @name private methods

    ST_LOCAL void stglUpdateVertices();
    ST_LOCAL double getPointInEx(const StPointD_t& thePointZo) const;

        private:

    class StProgramSB;
    StHandle<StProgramSB> myProgram;    //!< GLSL program

    StGLVertexBuffer      myVertices;   //!< vertices VBO
    StGLVertexBuffer      myColors;     //!< colors   VBO
    GLfloat               myProgress;   //!< current progress 0..1
    int                   myProgressPx; //!< current progress - width in pixels
    int                   myClickPos;
    int                   myMoveTolerPx;

};

#endif // __StGLSeekBar_h_
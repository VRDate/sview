/**
 * Copyright © 2010-2013 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StImage_h_
#define __StImage_h_

#include "StImagePlane.h"

class StImage {

        public:

    typedef enum tagImgColorModel {
        ImgColor_RGB,     //!< Red, Green, Blue - generic color model on PC (native for GPU and Displays)
        ImgColor_RGBA,    //!< same as RGB but has Alpha channel to perform color blending with background
        ImgColor_GRAY,    //!< just gray scale
        ImgColor_YUV,     //!< Y 16..235; U and V 16..240
        ImgColor_YUVjpeg, //!< full-scale YUV 0..255
        ImgColor_CMYK,    //!< Cyan, Magenta, Yellow and Black - generally used in printing process
        ImgColor_HSV,     //!< Hue, Saturation, Value (also known as HSB - Hue, Saturation, Brightness)
        ImgColor_HSL,     //!< Hue, Saturation, Lightness/Luminance (also known as HLS or HSI - Hue, Saturation, Intensity))
    } ImgColorModel;

    ST_CPPEXPORT static StString formatImgColorModel(ImgColorModel theColorModel);
    ST_LOCAL inline StString formatImgColorModel() const { return formatImgColorModel(myColorModel); }

        private:

    StImagePlane   myPlanes[4];
    GLfloat              myPAR; // pixel aspect ratio
    ImgColorModel myColorModel;

        protected:

    ST_LOCAL inline StString getDescription() const {
       return StString() + getSizeX() + " x " + getSizeY()
            + ", " + formatImgColorModel()
            + ", " + getPlane().formatImgFormat();
    }

        private:

    /**
     * Decode yuv420p pixel into RGB pixel.
     */
    ST_CPPEXPORT StPixelRGB getRGBFromYUV(const size_t theRow, const size_t theCol) const;

    inline float getScaleFactorX(const size_t thePlane) const {
        return float(getPlane(thePlane).getSizeX()) / float(getPlane(0).getSizeX());
    }

    inline float getScaleFactorY(const size_t thePlane) const {
        return float(getPlane(thePlane).getSizeY()) / float(getPlane(0).getSizeY());
    }

    inline size_t getScaledCol(const size_t thePlane,
                               const size_t theCol) const {
        return size_t(getScaleFactorX(thePlane) * float(theCol));
    }

    inline size_t getScaledRow(const size_t thePlane,
                               const size_t theRow) const {
        return size_t(getScaleFactorY(thePlane) * float(theRow));
    }

    template<typename Type>
    static inline uint8_t clamp(const Type x) {
        return x < 0x00 ? 0x00 : (x > 0xff ? 0xff : uint8_t(x));
    }

    /**
     * Avoid copy.
     */
    StImage(const StImage& theCopy);

        public: //!< Initializers

    /**
     * Empty constructor.
     */
    ST_CPPEXPORT StImage();
    ST_CPPEXPORT virtual ~StImage();

    /**
     * Initialize as copy (data will be copied!).
     */
    ST_CPPEXPORT bool initCopy(const StImage& theCopy);

    /**
     * Initialize as wrapper (data will not be copied).
     */
    ST_CPPEXPORT bool initWrapper(const StImage& theCopy);

    /**
     * Initialize as wrapper of input data in RGB format
     * or tries to convert data to RGB.
     */
    ST_CPPEXPORT bool initRGB(const StImage& theCopy);

    /**
     * Method initialize the cross-eyed stereoscopic image.
     */
    ST_CPPEXPORT bool initSideBySide(const StImage& theImageL,
                                     const StImage& theImageR,
                                     const int theSeparationDx,
                                     const int theSeparationDy);

    ST_CPPEXPORT bool fill(const StImage& theCopy);

        public: //!< Image information

    /**
     * @return true if all color components are stored in one plane (thus - interleaved).
     */
    inline bool isPacked() const {
        return myPlanes[1].isNull();
    }

    /**
     * @return true if color components are stored in separate image planes.
     */
    inline bool isPlanar() const {
        return !myPlanes[1].isNull();
    }

    /**
     * Return color model.
     */
    inline ImgColorModel getColorModel() const {
        return myColorModel;
    }

    /**
     * Setup color model.
     */
    inline void setColorModel(ImgColorModel theColorModel) {
        myColorModel = theColorModel;
    }

    /**
     * Determine the color model from image plane.
     * Valid only for packed image.
     */
    inline void setColorModelPacked(StImagePlane::ImgFormat theImgFormat) {
        switch(theImgFormat) {
            case StImagePlane::ImgGray:
            case StImagePlane::ImgGrayF:
                myColorModel = ImgColor_GRAY; return;
            case StImagePlane::ImgRGBA:
            case StImagePlane::ImgBGRA:
            case StImagePlane::ImgRGBAF:
            case StImagePlane::ImgBGRAF:
                myColorModel = ImgColor_RGBA; return;
            case StImagePlane::ImgRGB:
            case StImagePlane::ImgBGR:
            case StImagePlane::ImgRGBF:
            case StImagePlane::ImgBGRF:
            default:
                myColorModel = ImgColor_RGB; return;
        }
    }

    /**
     * @return width / height.
     */
    inline GLfloat getRatio() const {
        size_t aSizeY = getSizeY();
        return (aSizeY > 0) ? (GLfloat(getSizeX()) / GLfloat(aSizeY)) : 1.0f;
    }

    /**
     * @return image width in pixels.
     */
    inline size_t getSizeX() const {
        return myPlanes[0].getSizeX();
    }

    /**
     * @return image height in pixels.
     */
    inline size_t getSizeY() const {
        return myPlanes[0].getSizeY();
    }

    /**
     * Access to the image plane by ID (from 0 to 3).
     */
    inline const StImagePlane& getPlane(const size_t theId = 0) const {
        ST_DEBUG_ASSERT(theId < 4);
        return myPlanes[theId];
    }

    /**
     * Access to the image plane by ID (from 0 to 3).
     */
    inline StImagePlane& changePlane(const size_t theId = 0) {
        ST_DEBUG_ASSERT(theId < 4);
        return myPlanes[theId];
    }

    /**
     * Returns Pixel Aspect Ratio.
     * If pixels are quads than PAR = 1.0 (normal case).
     * PAR should be used to compute correct DAR (Display Aspect Ratio).
     */
    inline GLfloat getPixelRatio() const {
        return myPAR;
    }

    inline void setPixelRatio(GLfloat thePAR) {
        myPAR = thePAR;
    }

    /**
     * @return true if data is NULL.
     */
    ST_CPPEXPORT bool isNull() const;

    ST_CPPEXPORT void nullify();

};

#endif //__StImage_h_

/**
 * Copyright © 2011-2012 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StLibAV_h_
#define __StLibAV_h_

#include <StStrings/StString.h>

// libav* libraries written on pure C,
// and we must around includes manually
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
};

#if(LIBAVUTIL_VERSION_INT < AV_VERSION_INT(50, 13, 0))
    // enumeration was renamed from CodecType
    enum AVMediaType {
        AVMEDIA_TYPE_UNKNOWN = -1,
        AVMEDIA_TYPE_VIDEO,
        AVMEDIA_TYPE_AUDIO,
        AVMEDIA_TYPE_DATA,
        AVMEDIA_TYPE_SUBTITLE,
        AVMEDIA_TYPE_ATTACHMENT,
        AVMEDIA_TYPE_NB
    };
#endif


#if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 30, 0))
    // own lock function
    extern "C" int stFFmpegLock(void** theMutexPtrPtr, enum AVLockOp theOperation);
#endif

/**
 * This namespace to perform libav* initialization only once
 */
namespace stLibAV {

//!< entry functions

    /**
     * Should be called at application start.
     */
    bool init();

    /**
     * Returns string description for AVError code.
     */
    StString getAVErrorDescription(int avErrCode);

    /**
     * Version-control. This version may be used to provide information to user
     * and to perform run-time compatibility checks.
     */
    struct Version {

        unsigned myMajor; //!< Major version number, indicates API changes
        unsigned myMinor;
        unsigned myMicro;

            public:

        /**
         * Initialize the version structure from integer, returned by libav* library.
         */
        Version(const unsigned theVersionInt);

        /**
         * Returns the string representation for version.
         */
        StString toString() const;

            public: //!< return the version for each library

        static Version libavutil();
        static Version libavcodec();
        static Version libavformat();
        static Version libavdevice();
        static Version libswscale();

    };

    // this is just redeclaration AV_NOPTS_VALUE
    extern const int64_t NOPTS_VALUE;

    /**
     * Convert time units into seconds. Returns zero for invalid value.
     * @param theTimeBase  (const AVRational& ) - the timebase;
     * @param theTimeUnits (const int64_t     ) - value to convert;
     * @return converted time units in seconds.
     */
    double unitsToSeconds(const AVRational& theTimeBase,
                          const int64_t     theTimeUnits);

    /**
     * Convert seconds into time units.
     * @param theTimeBase    (const AVRational& ) - the timebase;
     * @param theTimeSeconds (const double      ) - value to convert;
     * @return time units.
     */
    int64_t secondsToUnits(const AVRational& theTimeBase,
                           const double      theTimeSeconds);

    /**
     * Convert time units into seconds using stream base.
     * @param theStream    (const AVStream* ) - the stream;
     * @param theTimeUnits (const int64_t   ) - value to convert;
     * @return converted time units in seconds.
     */
    inline double unitsToSeconds(const AVStream* theStream,
                                 const int64_t   theTimeUnits) {
        return stLibAV::unitsToSeconds(theStream->time_base, theTimeUnits);
    }

    /**
     * Convert seconds into time units for stream.
     * @param theStream      (const AVStream* ) - the stream;
     * @param theTimeSeconds (const double )    - value to convert;
     * @return time units.
     */
    inline int64_t secondsToUnits(const AVStream* theStream,
                                  const double    theTimeSeconds) {
        return stLibAV::secondsToUnits(theStream->time_base, theTimeSeconds);
    }

    /**
     * Convert time units into seconds for context.
     * @param theTimeUnits (const int64_t ) - value to convert;
     * @return converted time units in seconds.
     */
    double unitsToSeconds(const int64_t theTimeUnits);

    /**
     * Convert seconds into time units for context.
     * @param theTimeSeconds (const double ) - value to convert;
     * @return time units.
     */
    int64_t secondsToUnits(const double theTimeSeconds);

    /**
     * Image frame pixel formats. Originally defined as enumerations
     * but because them not maintained in future-compatible way (thus - changed between not major versions)
     * redefined as extern values.
     */
    namespace PIX_FMT {
        extern const PixelFormat NONE;
        extern const PixelFormat GRAY8;    ///< Y, 8bpp
        // planar YUV formats
        extern const PixelFormat YUV420P;  ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
        extern const PixelFormat YUV422P;  ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
        extern const PixelFormat YUV444P;  ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
        extern const PixelFormat YUV410P;  ///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
        extern const PixelFormat YUV411P;  ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
        extern const PixelFormat YUV440P;  ///< planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
        //extern const PixelFormat YUVA420P; ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
        // fullscale YUV formats (deprecated?)
        extern const PixelFormat YUVJ420P; ///< planar YUV 4:2:0, 12bpp, full scale (JPEG)
        extern const PixelFormat YUVJ422P; ///< planar YUV 4:2:2, 16bpp, full scale (JPEG)
        extern const PixelFormat YUVJ444P; ///< planar YUV 4:4:4, 24bpp, full scale (JPEG)
        extern const PixelFormat YUVJ440P; ///< planar YUV 4:4:0 full scale (JPEG)
        // RGB formats
        extern const PixelFormat RGB24;    ///< packed RGB 8:8:8, 24bpp, RGBRGB...
        extern const PixelFormat BGR24;    ///< packed RGB 8:8:8, 24bpp, BGRBGR...
        extern const PixelFormat RGBA32;
        extern const PixelFormat BGRA32;

        StString getString(const PixelFormat theFrmt);
    };

    /**
     * Auxiliary function to check that frame is in one of the YUV planar pixel format.
     * @return true if PixelFormat is planar YUV.
     */
    bool isFormatYUVPlanar(const AVCodecContext* theCtx);

    /**
     * Same as above but provide width/height information per component's plane.
     * @return true if PixelFormat is planar YUV.
     */
    bool isFormatYUVPlanar(const AVCodecContext* theCtx,
                           size_t& theWidthY, size_t& theHeightY,
                           size_t& theWidthU, size_t& theHeightU,
                           size_t& theWidthV, size_t& theHeightV,
                           bool& isFullScale);
    /**
     * Audio functions
     */
    namespace audio {

        StString getSampleFormatString (const AVCodecContext* theCtx);
        StString getSampleRateString   (const AVCodecContext* theCtx);
        StString getChannelLayoutString(const AVCodecContext* theCtx);

        namespace SAMPLE_FMT {
        #if(LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 0, 0))
            extern const AVSampleFormat U8;
            extern const AVSampleFormat S16;
            extern const AVSampleFormat S32;
            extern const AVSampleFormat FLT;
            extern const AVSampleFormat DBL;
        #else
            extern const SampleFormat   U8;
            extern const SampleFormat   S16;
            extern const SampleFormat   S32;
            extern const SampleFormat   FLT;
            extern const SampleFormat   DBL;
        #endif
        };

    };

};

#endif //__StLibAV_h_
/**
 * Copyright © 2016 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StAVIOContext_h_
#define __StAVIOContext_h_

#include <StAV/stAV.h>

/**
 * Wrapper over AVIOContext for passing the custom I/O.
 */
class StAVIOContext {

        public:

    /**
     * Main constructor.
     */
    ST_CPPEXPORT StAVIOContext();

    /**
     * Destructor.
     */
    ST_CPPEXPORT virtual ~StAVIOContext();

    /**
     * Access AVIO context.
     */
    ST_LOCAL AVIOContext* getAvioContext() const { return myAvioCtx; }

    /**
     * Virtual method for reading the data.
     */
    virtual int read(uint8_t* theBuf,
                     int      theBufSize) = 0;

    /**
     * Virtual method for writing the data.
     */
    virtual int write(uint8_t* theBuf,
                      int      theBufSize) = 0;

    /**
     * Virtual method for seeking to new position.
     */
    virtual int64_t seek(int64_t theOffset,
                         int     theWhence) = 0;

        protected:

    AVIOContext* myAvioCtx;

};

#endif // __StAVIOContext_h_

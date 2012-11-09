/**
 * Copyright © 2011-2012 Kirill Gavrilov <kirill@sview.ru>
 *
 * StCore library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StCore library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#if (defined(__APPLE__))

#include "StWindowImpl.h"
#include "stvkeysxarray.h" // X keys to VKEYs lookup array

#include <StStrings/StLogger.h>

#include "StCocoaView.h"
#include "StCocoaWin.h"

#include <StCocoa/StCocoaLocalPool.h>
#include <StCocoa/StCocoaString.h>

static const NSOpenGLPixelFormatAttribute THE_DOUBLE_BUFF[] = {
    //NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute) 32,
    //NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute) 0,
    //NSOpenGLPFAStencilSize, 0,
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccelerated,
    0
};

static const NSOpenGLPixelFormatAttribute THE_QUAD_BUFF[] = {
    NSOpenGLPFAStereo, 1,
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccelerated,
    0
};

void StWindowImpl::setTitle(const StString& theTitle) {
    myWindowTitle = theTitle;
    if(myMaster.hWindow != NULL) {
        StCocoaLocalPool aLocalPool;
        StCocoaString aTitle(myWindowTitle);
        [myMaster.hWindow setTitle: aTitle.toStringNs()];
    }
}

void StWindowImpl::setPlacement(const StRectI_t& theRect) {
    myRectNorm  = theRect;
    myIsUpdated = true;

    if(myMaster.hWindow != NULL
    && !myWinAttribs.isFullScreen) {
        StCocoaLocalPool aLocalPool;
        NSRect aFrameRect = [myMaster.hWindow frameRectForContentRect: myCocoaCoords.normalToCocoa(myRectNorm)];
        [myMaster.hWindow setFrame: aFrameRect display: YES];
    }
}

void StWindowImpl::show(const int& winNum) {
    if(winNum == ST_WIN_MASTER && myWinAttribs.isHide) {
        if(myMaster.hWindow != NULL) {
            [myMaster.hWindow orderFront: NULL];
        }
        myWinAttribs.isHide = false;
        updateWindowPos();
    } else if(winNum == ST_WIN_SLAVE && myWinAttribs.isSlaveHide) {
        if(mySlave.hWindow != NULL) {
            [mySlave.hWindow orderFront: NULL];
        }
        myWinAttribs.isSlaveHide = false;
        updateWindowPos();
    }
}

void StWindowImpl::hide(const int& winNum) {
    StCocoaLocalPool aLocalPool;
    if(winNum == ST_WIN_MASTER && !myWinAttribs.isHide) {
        if(myMaster.hWindow != NULL) {
            [myMaster.hWindow orderOut: NULL];
        }
        myWinAttribs.isHide = true;
    } else if(winNum == ST_WIN_SLAVE && !myWinAttribs.isSlaveHide) {
        if(mySlave.hWindow != NULL) {
            [mySlave.hWindow orderOut: NULL];
        }
        myWinAttribs.isSlaveHide = true;
    }
}

@interface StWinInitInfo : NSObject
    {
        StWindowImpl*    myWinSt;
        NSOpenGLContext* myGLContextMaster;
        NSOpenGLContext* myGLContextSlave;
    }

    - (id ) init: (StWindowImpl* )    theWinSt
          master: (NSOpenGLContext* ) theGLContextMaster
           slave: (NSOpenGLContext* ) theGLContextSlave;

    - (void ) doCreateWindows: (id ) theSender;
@end

@implementation StWinInitInfo

    - (id ) init: (StWindowImpl* )    theWinSt
          master: (NSOpenGLContext* ) theGLContextMaster
           slave: (NSOpenGLContext* ) theGLContextSlave {
        self = [super init];
        if(self == NULL) {
            return NULL;
        }
        myWinSt = theWinSt;
        myGLContextMaster = theGLContextMaster;
        myGLContextSlave  = theGLContextSlave;
        return self;
    }

    - (void ) doCreateWindows: (id ) theSender {
        myWinSt->doCreateWindows(myGLContextMaster, myGLContextSlave);
    }

@end

void StWindowImpl::doCreateWindows(NSOpenGLContext* theGLContextMaster,
                                   NSOpenGLContext* theGLContextSlave) {
    StCocoaLocalPool aLocalPool;

    // create the Master window
    NSUInteger aWinStyle = myWinAttribs.isNoDecor
                         ? NSBorderlessWindowMask
                         : NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask;
    myMaster.hWindow = [[StCocoaWin alloc] initWithStWin: this
                                                    rect: myRectNorm
                                               styleMask: aWinStyle];
    if(myMaster.hWindow == NULL) {
        stError("Cocoa, Master Window Creation Error");
        myInitState = STWIN_ERROR_COCOA_CREATEWIN;
        return;
    }
    StCocoaString aTitle(myWindowTitle);
    [myMaster.hWindow setTitle: aTitle.toStringNs()];

    // create the view in Master window
    myMaster.hViewGl = [[StCocoaView alloc] initWithStWin: this
                                                    nsWin: myMaster.hWindow];
    if(myMaster.hViewGl == NULL) {
        myMaster.close();
        stError("Cocoa, Master GL View Creation Error");
        myInitState = STWIN_ERROR_COCOA_CREATEWIN;
        return;
    }
    // setup GL context (NSOpenGLView create context on first call
    // so nothing is released here)
    [myMaster.hViewGl setOpenGLContext: theGLContextMaster];
    [theGLContextMaster setView: myMaster.hViewGl];

    if(myWinAttribs.isSlave) {
        // create the Slave window
        StRectI_t aRectSlave(getSlaveTop(),  getSlaveTop() + getSlaveHeight(),
                             getSlaveLeft(), getSlaveLeft() + getSlaveWidth());
        mySlave.hWindow = [[StCocoaWin alloc] initWithStWin: this
                                                       rect: aRectSlave
                                                  styleMask: NSBorderlessWindowMask];
        if(mySlave.hWindow == NULL) {
            myMaster.close();
            mySlave.close();
            stError("Cocoa, Slave Window Creation Error");
            myInitState = STWIN_ERROR_COCOA_CREATEWIN;
            return;
        }
        [mySlave.hWindow setTitle: @"Slave window"];

        // create the view in Slave window
        mySlave.hViewGl = [[StCocoaView alloc] initWithStWin: this
                                                       nsWin: mySlave.hWindow];
        if(mySlave.hViewGl == NULL) {
            stError("Cocoa, Slave GL View Creation Error");
            myMaster.close();
            mySlave.close();
            myInitState = STWIN_ERROR_COCOA_CREATEWIN;
            return;
        }
        // setup GL context (NSOpenGLView create context on first call
        // so nothing is released here)
        [mySlave.hViewGl setOpenGLContext: theGLContextSlave];
        [theGLContextSlave setView: mySlave.hViewGl];

        // make slave window topmost
        [mySlave.hWindow setLevel: kCGMaximumWindowLevel];
        if(!myWinAttribs.isSlaveHide && (!isSlaveIndependent() || myMonitors.size() > 1)) {
            [mySlave.hWindow orderFront: NULL];
        }
    }

    [myMaster.hWindow makeKeyAndOrderFront: NSApp];
}

bool StWindowImpl::stglCreate(const StWinAttributes_t* theAttributes,
                              const StNativeWin_t*     theParentWindow) {
    myInitState = STWIN_INITNOTSTART;
    if(theParentWindow != NULL) {
        ///stMemCpy(&myParentWin, theParentWindow, sizeof(StNativeWin_t));
        ///myParentWin.stWinPtr = this;
    }
    size_t bytesToCopy = (theAttributes->nSize > sizeof(StWinAttributes_t)) ? sizeof(StWinAttributes_t) : theAttributes->nSize;
    stMemCpy(&myWinAttribs, theAttributes, bytesToCopy); // copy as much as possible
    myWinAttribs.nSize = sizeof(StWinAttributes_t);     // restore own size
    updateSlaveConfig();
    updateChildRect();

    if(NSApp == NULL) {
        stError("Cocoa, main application doesn't exists");
        myInitState = STWIN_ERROR_COCOA_NO_APP;
        return false;
    }

    StCocoaLocalPool aLocalPool;

    // create the Master GL context
    ///NSOpenGLPixelFormat* aGlFormatMaster = [[[NSOpenGLView class] defaultPixelFormat] retain];
    NSOpenGLPixelFormat* aGLFormat    = NULL;
    NSOpenGLContext* aGLContextMaster = NULL;
    NSOpenGLContext* aGLContextSlave  = NULL;
    if(myWinAttribs.isGlStereo) {
        aGLFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes: THE_QUAD_BUFF] autorelease];
        aGLContextMaster = [[[NSOpenGLContext alloc] initWithFormat: aGLFormat
                                                       shareContext: NULL] autorelease];
        if(aGLContextMaster == NULL) {
            aGLFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes: THE_DOUBLE_BUFF] autorelease];
            aGLContextMaster = [[[NSOpenGLContext alloc] initWithFormat: aGLFormat
                                                           shareContext: NULL] autorelease];
            if(aGLContextMaster == NULL) {
                stError("Cocoa, fail to create Double Buffered OpenGL context");
                myInitState = STWIN_ERROR_COCOA_NO_GL;
                return false;
            } else {
                stError("Cocoa, fail to create Quad Buffered OpenGL context");
            }
        }
    } else {
        aGLFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes: THE_DOUBLE_BUFF] autorelease];
        aGLContextMaster = [[[NSOpenGLContext alloc] initWithFormat: aGLFormat
                                                       shareContext: NULL] autorelease];
        if(aGLContextMaster == NULL) {
            stError("Cocoa, fail to create Double Buffered OpenGL context");
            myInitState = STWIN_ERROR_COCOA_NO_GL;
            return false;
        }
    }

    // create the Slave GL context
    if(myWinAttribs.isSlave) {
        aGLContextSlave = [[[NSOpenGLContext alloc] initWithFormat: aGLFormat
                                                      shareContext: aGLContextMaster] autorelease];
        if(aGLContextSlave == NULL) {
            stError("Cocoa, fail to create Slave OpenGL context");
            myInitState = STWIN_ERROR_COCOA_NO_GL;
            return false;
        }
    }

    // create special object to invoke in main thread
    StWinInitInfo* aWinInit = [[StWinInitInfo alloc] init: this
                                                   master: aGLContextMaster
                                                    slave: aGLContextSlave];
    if(aWinInit == NULL) {
        myInitState = STWIN_ERROR_COCOA_CREATEWIN;
        return false;
    }
    if([NSThread isMainThread]) {
        [aWinInit doCreateWindows: NULL];
    } else {
        [aWinInit performSelectorOnMainThread: @selector(doCreateWindows:)
                                   withObject: NULL
                                waitUntilDone: YES];
    }
    [aWinInit release];

    if(myInitState != STWIN_INITNOTSTART) {
        return false;
    }

    myMaster.glMakeCurrent();

    myIsUpdated = true;
    myInitState = STWIN_INIT_SUCCESS;
    return true;
}

/**
 * Update StWindow position according to native parent position.
 */
void StWindowImpl::updateChildRect() {
    ///
}

void StWindowImpl::setFullScreen(bool theFullscreen) {
    if(myWinAttribs.isFullScreen != theFullscreen) {
        myWinAttribs.isFullScreen = theFullscreen;
        if(myWinAttribs.isFullScreen) {
            myFullScreenWinNb.increment();
        } else {
            myFullScreenWinNb.decrement();
        }
    }

    if(myWinAttribs.isFullScreen) {
        const StMonitor& aMon = (myMonMasterFull == -1) ? myMonitors[myRectNorm.center()] : myMonitors[myMonMasterFull];
        myRectFull = aMon.getVRect();

        if(myWinAttribs.isSlave && mySlave.hViewGl != NULL) {
            [mySlave.hViewGl goToFullscreen];
        }
        if(myMaster.hViewGl != NULL) {
            [myMaster.hViewGl goToFullscreen];
        }
    } else {
        if(myWinAttribs.isSlave && mySlave.hViewGl != NULL) {
            [mySlave.hViewGl goToWindowed];
        }
        if(myMaster.hViewGl != NULL) {
            [myMaster.hViewGl goToWindowed];
        }
    }
}

void StWindowImpl::updateWindowPos() {
    if(myWinAttribs.isSlave && !myWinAttribs.isSlaveHide
    && (!isSlaveIndependent() || myMonitors.size() > 1)
    && !myWinAttribs.isFullScreen) {
        StRectI_t aRectSlave(getSlaveTop(),  getSlaveTop() + getSlaveHeight(),
                             getSlaveLeft(), getSlaveLeft() + getSlaveWidth());
        NSRect aFrameRect = [mySlave.hWindow frameRectForContentRect: myCocoaCoords.normalToCocoa(aRectSlave)];
        [mySlave.hWindow setFrame: aFrameRect display: YES];
    }

    // detect when window moved to another monitor
    if(!myWinAttribs.isFullScreen && myMonitors.size() > 1) {
        int aNewMonId = myMonitors[myRectNorm.center()].getId();
        if(myWinOnMonitorId != aNewMonId) {
            myMessageList.append(StMessageList::MSG_WIN_ON_NEW_MONITOR);
            myWinOnMonitorId = aNewMonId;
        }
    }
}

// Function set to argument-buffer given events
void StWindowImpl::callback(StMessage_t* theMessages) {
    // detect master window movements
    if(myWinAttribs.isFullScreen) {
        if(myRectNormPrev != myRectFull) {
            myRectNormPrev = myRectFull;
            myIsUpdated    = true;
            myMessageList.append(StMessageList::MSG_RESIZE);
        }
    } else {
        StRectI_t aWinRectNew = myCocoaCoords.cocoaToNormal([myMaster.hWindow contentRectForFrameRect: [myMaster.hWindow frame]]);
        if(myRectNormPrev != aWinRectNew) {
            myRectNorm     = aWinRectNew;
            myRectNormPrev = aWinRectNew;
            myIsUpdated    = true;
            myMessageList.append(StMessageList::MSG_RESIZE);
        }
    }

    // detect mouse movements
    StPointD_t aNewMousePt = getMousePos();
    if(aNewMousePt.x() >= 0.0 && aNewMousePt.x() <= 1.0 && aNewMousePt.y() >= 0.0 && aNewMousePt.y() <= 1.0) {
        StPointD_t aDspl = aNewMousePt - myMousePt;
        if(std::abs(aDspl.x()) >= 0.0008 || std::abs(aDspl.y()) >= 0.0008) {
            myMessageList.append(StMessageList::MSG_MOUSE_MOVE);
        }
    }
    myMousePt = aNewMousePt;

    if(myIsUpdated) {
        // update position only when all messages are parsed
        updateWindowPos();
        myIsUpdated = false;
    }
    updateActiveState();

    myMessageList.popList(theMessages);
}

#endif // __APPLE__
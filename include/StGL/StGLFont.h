/**
 * Copyright © 2012 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StGLFont_h_
#define __StGLFont_h_

#include <StFT/StFTFont.h>
#include <StGL/StGLTexture.h>
#include <StGL/StGLVec.h>
#include <StTemplates/StRect.h>

#include <map>

typedef StRect<GLfloat> StGLRect;

/**
 * Simple structure stores tile rectangle.
 */
struct StGLTile {

    StGLRect uv;      //!< UV coordinates in texture
    StGLRect px;      //!< pixel displacement coordinates
    GLuint   texture; //!< GL texture ID

        public: //! compare operators

    bool operator==(const StGLTile& theCompare) const {
        return uv == theCompare.uv && px == theCompare.px;
    }

};

template<> inline void StArray< StHandle<StGLTexture> >::sort() {}
template<> inline void StArray<StGLTile>::sort() {}
template<> inline void StArray<StGLRect>::sort() {}

/**
 * Texture font.
 */
class ST_LOCAL StGLFont : public StGLResource {

        public:

    /**
     * Main constructor.
     */
    StGLFont(StHandle<StFTFont>& theFont);

    /**
     * Destructor - should be called after release()!
     */
    virtual ~StGLFont();

    /**
     * Release GL resources.
     */
    virtual void release(StGLContext& theCtx);

    /**
     * @return FreeType font instance specified on construction.
     */
    StHandle<StFTFont>& getFont() {
        return myFont;
    }

    /**
     * @return true if font was loaded successfully.
     */
    bool isValid() const {
        return !myTextures.isEmpty() && myTextures[0]->isValid();
    }

    /**
     * Notice that this method doesn't return initialization success state.
     * Use isValid() instead.
     * @return true if initialization was already called.
     */
    bool wasInitialized() const {
        return !myTextures.isEmpty();
    }

    /**
     * Initialize GL resources.
     * FreeType font instance should be already initialized!
     */
    bool stglInit(StGLContext& theCtx);

    /**
     * Compute advance to the next character with kerning applied when applicable.
     * Assuming text rendered horizontally.
     */
    float getAdvanceX(const stUtf32_t theUChar,
                      const stUtf32_t theUCharNext) {
        return myFont->getAdvanceX(theUChar, theUCharNext);
    }

    /**
     * @return vertical distance from the horizontal baseline to the highest character coordinate.
     */
    GLfloat getAscender() const {
        return myAscender;
    }

    /**
     * @return default line spacing (the baseline-to-baseline distance).
     */
    GLfloat getLineSpacing() const {
        return myLineSpacing;
    }

    /**
     * Compute glyph rectangle at specified pen position (on baseline)
     * and render it to texture if not already.
     * @param theCtx       - active context
     * @param theUChar     - unicode symbol to render
     * @param theUCharNext - next symbol to compute advance with kerning when available
     * @param theGlyph     - computed glyph position rectangle, texture ID and UV coordinates
     * @param thePen       - pen position on baseline to place new glyph
     */
    void renderGlyph(StGLContext&    theCtx,
                     const stUtf32_t theUChar,
                     const stUtf32_t theUCharNext,
                     StGLTile&       theGlyph,
                     StGLVec2&       thePen);

        protected:

    /**
     * Render new glyph to the texture.
     */
    bool renderGlyph(StGLContext&    theCtx,
                     const stUtf32_t theChar);

    /**
     * Allocate new texture.
     */
    bool createTexture(StGLContext& theCtx);

        protected:

    StHandle<StFTFont> myFont;                //!< FreeType font instance
    GLfloat            myAscender;            //!< ascender     provided my FT font
    GLfloat            myLineSpacing;         //!< line spacing provided my FT font
    GLsizei            myTileSizeX;           //!< tile width
    GLsizei            myTileSizeY;           //!< tile height
    size_t             myLastTileId;          //!< id of last tile
    StRect<int>        myLastTilePx;
    GLint              myTextureFormat;       //!< texture format

    StArrayList< StHandle<StGLTexture> > myTextures; //!< texture list
    StArrayList<StGLTile> myTiles;                   //!< tiles list

    std::map<stUtf32_t, size_t> myGlyphMap;

};

#endif // __StGLFont_h_
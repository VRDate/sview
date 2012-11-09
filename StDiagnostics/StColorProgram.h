/**
 * Copyright © 2010-2012 Kirill Gavrilov <kirill@sview.ru>
 *
 * StDiagnostics program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StDiagnostics program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __StColorProgram_h_
#define __StColorProgram_h_

#include <StGL/StGLVec.h>
#include <StGLMesh/StGLMesh.h>

/**
 * Very simple color program.
 */
class ST_LOCAL StColorProgram : public StGLMeshProgram {

        public:

    StColorProgram();

    virtual bool init(StGLContext& theCtx);

    void setScaleTranslate(StGLContext&    theCtx,
                           const StGLVec4& theScaleVec4,
                           const StGLVec4& theTranslateVec4);

        private:

    StGLVarLocation uniScaleLoc;
    StGLVarLocation uniTranslateLoc;

};

#endif //__StColorProgram_h_
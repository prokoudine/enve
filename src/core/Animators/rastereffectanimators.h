// enve - 2D animations software
// Copyright (C) 2016-2019 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef RasterEffectANIMATORS_H
#define RasterEffectANIMATORS_H
#include "Animators/dynamiccomplexanimator.h"
#include "smartPointers/ememory.h"
#include "RasterEffects/rastereffect.h"
#include "ShaderEffects/shadereffect.h"

class BoundingBox;
struct BoxRenderData;
qsptr<RasterEffect> readIdCreateRasterEffect(eReadStream& src);
typedef DynamicComplexAnimator<
    RasterEffect, &RasterEffect::writeIdentifier,
    &readIdCreateRasterEffect> RasterEffectAnimatorsBase;
class RasterEffectAnimators : public RasterEffectAnimatorsBase {
    e_OBJECT
protected:
    RasterEffectAnimators(BoundingBox * const parentBox);
public:
    bool SWT_isRasterEffectAnimators() const { return true; }

    BoundingBox *getParentBox() { return mParentBox_k; }
    bool hasEffects();

    void addEffects(const qreal relFrame,
                    BoxRenderData * const data);

    void updateIfUsesProgram(const ShaderEffectProgram * const program);
    //void readRasterEffect(QIODevice *target);
    const QMargins& getMaxForcedMargin() const {
        return mMaxForcedMargin;
    }
private:
    void updateMaxForcedMargin();

    QMargins mMaxForcedMargin;

    bool mUnbound = false;
    BoundingBox * const mParentBox_k;
};

#endif // RasterEffectANIMATORS_H

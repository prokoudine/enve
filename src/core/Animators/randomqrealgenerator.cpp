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

#include "randomqrealgenerator.h"
#include "pointhelpers.h"
#include "qrealanimator.h"
#include "qrealkey.h"
#include "pointhelpers.h"
#include "Properties/comboboxproperty.h"
#include "intanimator.h"

RandomQrealGenerator::RandomQrealGenerator() :
    QrealValueEffect("noise") {
    mTime = enve::make_shared<QrealAnimator>(0, 0, 9999, 1, "time");
    ca_addChild(mTime);
    mSmoothness = QrealAnimator::sCreate0to1Animator("smoothness");
    ca_addChild(mSmoothness);
    mMaxDev = enve::make_shared<QrealAnimator>(0, 0, 999, 1, "amplitude");
    ca_addChild(mMaxDev);
    mType = enve::make_shared<ComboBoxProperty>("type",
                QStringList() << "add" << "subtract" << "overlay");
    mType->setCurrentValue(2);
    ca_addChild(mType);

    mSeedAssist = IntAnimator::sCreateSeed();
    ca_addChild(mSeedAssist);
}

void RandomQrealGenerator::anim_setAbsFrame(const int frame) {
    ComplexAnimator::anim_setAbsFrame(frame);
    if(mMaxDev->getEffectiveValue() < 0.001) return;
    prp_afterChangedCurrent(UpdateReason::frameChange);
}

FrameRange RandomQrealGenerator::prp_getIdenticalRelRange(const int relFrame) const {
    if(mMaxDev->getEffectiveValue(relFrame) < 0.001) {
        return ComplexAnimator::prp_getIdenticalRelRange(relFrame);
    }
    return {relFrame, relFrame};
}

qreal RandomQrealGenerator::getDevAtRelFrame(const qreal relFrame) {
    const int seed = mSeedAssist->getBaseIntValue(relFrame);
    const qreal time = mTime->getEffectiveValue(relFrame);
    const qreal smooth = mSmoothness->getEffectiveValue(relFrame);
    const qreal maxDev = mMaxDev->getEffectiveValue(relFrame);
    const int type = mType->getCurrentValue();
    const int iPrevTime = qFloor(time);
    const int iNextTime = qCeil(time);
    const qreal min = (type == 0 ? 0 : (type == 1 ? -1 : -0.5));
    const qreal max = (type == 0 ? 1 : (type == 1 ? 0 : 0.5));
    QRandomGenerator gen;
    gen.seed(static_cast<uint>(seed + iPrevTime));
    const qreal prevDev = (gen.generateDouble()*(max - min) + min)*maxDev;
    gen.seed(static_cast<uint>(seed + iNextTime));
    const qreal nextDev = (gen.generateDouble()*(max - min) + min)*maxDev;

    const qreal frac = time - iPrevTime;

    if(isZero4Dec(smooth)) return frac*nextDev + (1 - frac)*prevDev;

    qCubicSegment2D seg({0, 0}, {smooth, 0}, {1 - smooth, 1}, {1, 1});
    qreal bestT;
    seg.xSeg().minDistanceTo(frac, &bestT);
    const qreal smoothFrac = seg.ySeg().valAtT(bestT);
    return smoothFrac*nextDev + (1 - smoothFrac)*prevDev;
}

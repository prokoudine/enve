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

#ifndef SMARTSmartVectorPath_H
#define SMARTSmartVectorPath_H
#include <QPainterPath>
#include <QLinearGradient>
#include "pathbox.h"
#include "Animators/SmartPath/smartpathcollection.h"

class NodePoint;
class ContainerBox;
class PathAnimator;

enum class CanvasMode : short;

class SmartVectorPathEdge;

class SmartVectorPath : public PathBox {
    e_OBJECT
protected:
    SmartVectorPath();
public:
    bool SWT_isSmartVectorPath() const { return true; }

    void setupCanvasMenu(PropertyMenu * const menu);

    SkPath getPathAtRelFrameF(const qreal relFrame);

    bool differenceInEditPathBetweenFrames(const int frame1,
                                           const int frame2) const;
    void applyCurrentTransformation();

    void loadSkPath(const SkPath& path);

    SmartPathCollection *getPathAnimator();

    QList<qsptr<SmartVectorPath>> breakPathsApart_k();
protected:
    void getMotionBlurProperties(QList<Property*> &list) const;
    qsptr<SmartPathCollection> mPathAnimator;
};

#endif // SMARTSmartVectorPath_H

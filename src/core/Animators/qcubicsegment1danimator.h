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

#ifndef QCUBICSEGMENT1DANIMATOR_H
#define QCUBICSEGMENT1DANIMATOR_H
#include "Animators/interpolationanimatort.h"
#include "Segments/qcubicsegment1d.h"
#include "../ReadWrite/basicreadwrite.h"
class qCubicSegment1DAnimator;

class qCubicSegment1DAnimator : public InterpolationAnimatorT<qCubicSegment1D> {
    e_OBJECT
    Q_OBJECT
public:
    qCubicSegment1DAnimator(const QString &name);

    bool SWT_isQCubicSegment1DAnimator() const { return true; }
signals:
    void currentValueChanged(qCubicSegment1D);
protected:
    void afterValueChanged() {
        emit currentValueChanged(mCurrentValue);
    }
};

#endif // QCUBICSEGMENT1DANIMATOR_H

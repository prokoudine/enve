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

#include "boolanimator.h"

BoolAnimator::BoolAnimator(const QString &name) : IntAnimator(name) {
    setIntValueRange(0, 1);
}

bool BoolAnimator::getCurrentBoolValue() {
    return getCurrentIntValue() == 1;
}

void BoolAnimator::setCurrentBoolValue(const bool bT) {
    setCurrentIntValue((bT ? 1 : 0));
}

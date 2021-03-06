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

#include "smartpathanimator.h"
#include "Animators/qrealpoint.h"
#include "smartpathcollection.h"
#include "MovablePoints/pathpointshandler.h"

SmartPathAnimator::SmartPathAnimator() :
    GraphAnimator("path"),
    pathChanged([this]() { pathChangedExec(); }) {
    const auto ptsHandler = enve::make_shared<PathPointsHandler>(this);
    QObject::connect(this, &Property::prp_currentFrameChanged,
                     this, [ptsHandler] {
        ptsHandler->updateAllPoints();
    });
    setPointsHandler(ptsHandler);
}

SmartPathAnimator::SmartPathAnimator(const SkPath &path) :
    SmartPathAnimator() {
    mBaseValue.setPath(path);
    mCurrentPath = path;
    updateAllPoints();
}

SmartPathAnimator::SmartPathAnimator(const SmartPath &baseValue) :
    SmartPathAnimator() {
    mBaseValue = baseValue;
    mPathUpToDate = false;
    updateAllPoints();
}

QMimeData *SmartPathAnimator::SWT_createMimeData() {
    return new eMimeData(QList<SmartPathAnimator*>() << this);
}

#include "typemenu.h"
#include "Private/document.h"
void SmartPathAnimator::prp_setupTreeViewMenu(PropertyMenu * const menu) {
    const auto spClipboard = Document::sInstance->getSmartPathClipboard();
    menu->addPlainAction("Paste Path", [this, spClipboard]() {
        pastePath(spClipboard->path());
    })->setEnabled(spClipboard);
    menu->addPlainAction("Copy Path", [this] {
        const auto spClipboard = enve::make_shared<SmartPathClipboard>(mBaseValue);
        Document::sInstance->replaceClipboard(spClipboard);
    });
    menu->addSeparator();
    const PropertyMenu::PlainSelectedOp<SmartPathAnimator> dOp =
    [](SmartPathAnimator* const eff) {
        const auto parent = eff->getParent<DynamicComplexAnimatorBase<SmartPathAnimator>>();
        parent->removeChild(eff->ref<SmartPathAnimator>());
    };
    menu->addPlainAction("Delete Path(s)", dOp);
    menu->addSeparator();
    Animator::prp_setupTreeViewMenu(menu);
}

void SmartPathAnimator::prp_readProperty(eReadStream& src) {
    anim_readKeys(src);
    src >> mBaseValue;
    src.read(&mMode, sizeof(Mode));
    prp_afterWholeInfluenceRangeChanged();
    updateAllPoints();
}

void SmartPathAnimator::prp_writeProperty(eWriteStream &dst) const {
    anim_writeKeys(dst);
    dst << mBaseValue;
    dst.write(&mMode, sizeof(Mode));
}

void SmartPathAnimator::graph_getValueConstraints(
        GraphKey *key, const QrealPointType type,
        qreal &minValue, qreal &maxValue) const {
    if(type == QrealPointType::KEY_POINT) {
        minValue = key->getRelFrame();
        maxValue = minValue;
        //getFrameConstraints(key, type, minValue, maxValue);
    } else {
        minValue = -DBL_MAX;
        maxValue = DBL_MAX;
    }
}

void SmartPathAnimator::actionDisconnectNodes(const int node1Id,
                                              const int node2Id) {
    const auto& keys = anim_getKeys();
    for(const auto &key : keys) {
        const auto spKey = static_cast<SmartPathKey*>(key);
        auto& keyPath = spKey->getValue();
        keyPath.actionDisconnectNodes(node1Id, node2Id);
    }
    mBaseValue.actionDisconnectNodes(node1Id, node2Id);
    prp_afterWholeInfluenceRangeChanged();
}

void SmartPathAnimator::updateAllPoints() {
    const auto handler = getPointsHandler();
    const auto pathHandler = static_cast<PathPointsHandler*>(handler);
    pathHandler->updateAllPoints();
}

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

#ifndef SMARTPATHANIMATOR_H
#define SMARTPATHANIMATOR_H
#include "../interpolationanimatort.h"
#include "differsinterpolate.h"
#include "../../ReadWrite/basicreadwrite.h"
#include "smartpathkey.h"
#include "../../MovablePoints/segment.h"
#include "../../skia/skiahelpers.h"
#include "simpletask.h"

class SmartPathAnimator : public GraphAnimator {
    e_OBJECT
    Q_OBJECT
protected:
    SmartPathAnimator();
    SmartPathAnimator(const SkPath& path);
    SmartPathAnimator(const SmartPath& baseValue);
public:
    enum Mode {
        NORMAL,
        ADD, REMOVE, REMOVE_REVERSE,
        INTERSECT, EXCLUDE,
        DIVIDE
    };

    bool SWT_isSmartPathAnimator() const { return true; }

    QMimeData *SWT_createMimeData();

    void prp_setupTreeViewMenu(PropertyMenu * const menu);

    void prp_drawCanvasControls(
            SkCanvas * const canvas, const CanvasMode mode,
            const float invScale, const bool ctrlPressed) {
        SkiaHelpers::drawOutlineOverlay(canvas, mCurrentPath, invScale,
                                        toSkMatrix(getTransform()));
        Property::prp_drawCanvasControls(canvas, mode, invScale, ctrlPressed);
    }

    void prp_afterChangedAbsRange(const FrameRange &range,
                                  const bool clip = true) {
        if(range.inRange(anim_getCurrentAbsFrame()))
            updateBaseValue();
        GraphAnimator::prp_afterChangedAbsRange(range, clip);
    }

    void anim_setAbsFrame(const int frame) {
        if(frame == anim_getCurrentAbsFrame()) return;
        const int lastRelFrame = anim_getCurrentRelFrame();
        Animator::anim_setAbsFrame(frame);
        const bool diff = prp_differencesBetweenRelFrames(
                    anim_getCurrentRelFrame(), lastRelFrame);
        if(diff) {
            updateBaseValue();
            prp_afterChangedCurrent(UpdateReason::frameChange);
        }
    }

    void anim_addKeyAtRelFrame(const int relFrame) {
        if(anim_getKeyAtRelFrame(relFrame)) return;
        const auto newKey = enve::make_shared<SmartPathKey>(this);
        newKey->setRelFrame(relFrame);
        deepCopySmartPathFromRelFrame(relFrame, newKey->getValue());
        anim_appendKey(newKey);
    }

    stdsptr<Key> anim_createKey() {
        return enve::make_shared<SmartPathKey>(this);
    }

    void prp_readProperty(eReadStream& src);
    void prp_writeProperty(eWriteStream& dst) const;

    void graph_getValueConstraints(
            GraphKey *key, const QrealPointType type,
            qreal &minValue, qreal &maxValue) const;

    void anim_afterKeyOnCurrentFrameChanged(Key* const key) {
        const auto spk = static_cast<SmartPathKey*>(key);
        if(spk) mPathBeingChanged_d = &spk->getValue();
        else mPathBeingChanged_d = &mBaseValue;
    }

    void deepCopySmartPathFromRelFrame(const int relFrame,
                                       SmartPath &result) const {
        const auto prevKey = anim_getPrevKey<SmartPathKey>(relFrame);
        const auto nextKey = anim_getNextKey<SmartPathKey>(relFrame);
        const auto keyAtFrame = anim_getKeyAtRelFrame<SmartPathKey>(relFrame);
        deepCopySmartPathFromRelFrame(relFrame, prevKey, nextKey,
                                      keyAtFrame, result);
    }

    SkPath getPathAtAbsFrame(const qreal frame) {
        return getPathAtRelFrame(prp_absFrameToRelFrameF(frame));
    }

    SkPath getPathAtRelFrame(const qreal frame) {
        const auto diff = prp_differencesBetweenRelFrames(
                    qRound(frame), anim_getCurrentRelFrame());
        if(!diff) return getCurrentPath();
        const auto pn = anim_getPrevAndNextKeyIdF(frame);
        const int prevId = pn.first;
        const int nextId = pn.second;

        const auto prevKey = anim_getKeyAtIndex<SmartPathKey>(prevId);
        const auto nextKey = anim_getKeyAtIndex<SmartPathKey>(nextId);
        const bool adjKeys = pn.second - pn.first == 1;
        const auto keyAtRelFrame = adjKeys ?
                    nullptr :
                    anim_getKeyAtIndex<SmartPathKey>(pn.first + 1);
        if(keyAtRelFrame) return keyAtRelFrame->getValue().getPathAt();
        if(prevKey && nextKey) {
            SkPath result;
            const qreal nWeight = prevKeyWeight(prevKey, nextKey, frame);
            SmartPath sPath;
            gInterpolate(prevKey->getValue(), nextKey->getValue(),
                         nWeight, sPath);
            return sPath.getPathAt();
        } else if(!prevKey && nextKey) {
            return nextKey->getPath();
        } else if(prevKey && !nextKey) {
            return prevKey->getPath();
        }
        return mBaseValue.getPathAt();
    }

    SmartPath * getCurrentlyEditedPath() const {
        return mPathBeingChanged_d;
    }

    bool isClosed() const {
        return mBaseValue.isClosed();
    }

    void beforeBinaryPathChange() {
        if(anim_isRecording() && !anim_getKeyOnCurrentFrame()) {
            anim_saveCurrentValueAsKey();
        }
    }

    void startPathChange() {
        if(mPathChanged) return;
        mPathChanged = true;
        if(anim_isRecording() && !anim_getKeyOnCurrentFrame()) {
            anim_saveCurrentValueAsKey();
        }
        mPathBeingChanged_d->save();
    }

    SimpleTaskScheduler pathChanged;
    void pathChangedExec() {
        const auto spk = anim_getKeyOnCurrentFrame<SmartPathKey>();
        if(spk) anim_updateAfterChangedKey(spk);
        else prp_afterWholeInfluenceRangeChanged();
    }

    void cancelPathChange() {
        if(!mPathChanged) return;
        mPathChanged = false;
        mPathBeingChanged_d->restore();
        const auto spk = anim_getKeyOnCurrentFrame<SmartPathKey>();
        if(spk) anim_updateAfterChangedKey(spk);
        else prp_afterWholeInfluenceRangeChanged();
    }

    void finishPathChange() {
        if(!mPathChanged) return;
        mPathChanged = false;
    }

    void actionRemoveNode(const int nodeId, const bool approx) {
        const auto& keys = anim_getKeys();
        for(const auto &key : keys) {
            const auto spKey = static_cast<SmartPathKey*>(key);
            spKey->getValue().actionRemoveNode(nodeId, approx);
        }
        mBaseValue.actionRemoveNode(nodeId, approx);
        prp_afterWholeInfluenceRangeChanged();
    }


    int actionAddNewAtStart(const QPointF &relPos) {
        return actionAddNewAtStart({false, false, CTRLS_CORNER,
                                    relPos, relPos, relPos});
    }

    int actionAddNewAtStart(const NormalNodeData &data) {
        if(mBaseValue.getNodeCount() == 0)
            return actionAddFirstNode(data);
        beforeBinaryPathChange();
        const auto& keys = anim_getKeys();
        for(const auto &key : keys) {
            const auto spKey = static_cast<SmartPathKey*>(key);
            spKey->getValue().actionPrependNode();
        }
        const int id = mBaseValue.actionPrependNode();
        getCurrentlyEditedPath()->actionSetNormalNodeValues(id, data);
        prp_afterWholeInfluenceRangeChanged();
        return id;
    }

    int actionAddNewAtEnd(const QPointF &relPos) {
        return actionAddNewAtEnd({false, false, CTRLS_CORNER,
                                  relPos, relPos, relPos});
    }

    int actionAddNewAtEnd(const NormalNodeData &data) {
        if(mBaseValue.getNodeCount() == 0)
            return actionAddFirstNode(data);
        beforeBinaryPathChange();
        const auto& keys = anim_getKeys();
        for(const auto &key : keys) {
            const auto spKey = static_cast<SmartPathKey*>(key);
            spKey->getValue().actionAppendNodeAtEndNode();
        }
        const int id = mBaseValue.actionAppendNodeAtEndNode();
        getCurrentlyEditedPath()->actionSetNormalNodeValues(id, data);
        prp_afterWholeInfluenceRangeChanged();
        return id;
    }

    int actionInsertNodeBetween(const int node1Id,
                                const int node2Id,
                                const qreal t) {
        beforeBinaryPathChange();
        const auto curr = getCurrentlyEditedPath();
        if(curr->getNodePtr(node1Id)->getCtrlsMode() == CTRLS_SYMMETRIC) {
            curr->actionSetNormalNodeCtrlsMode(node1Id, CTRLS_SMOOTH);
        }
        if(curr->getNodePtr(node2Id)->getCtrlsMode() == CTRLS_SYMMETRIC) {
            curr->actionSetNormalNodeCtrlsMode(node2Id, CTRLS_SMOOTH);
        }
        const auto& keys = anim_getKeys();
        for(const auto &key : keys) {
            const auto spKey = static_cast<SmartPathKey*>(key);
            spKey->getValue().actionInsertNodeBetween(node1Id, node2Id, t);
        }
        const int id = mBaseValue.actionInsertNodeBetween(node1Id, node2Id, t);
        curr->actionPromoteDissolvedNodeToNormal(id);
        prp_afterWholeInfluenceRangeChanged();
        return id;
    }

    void actionConnectNodes(const int node1Id, const int node2Id) {
        const auto& keys = anim_getKeys();
        for(const auto &key : keys) {
            const auto spKey = static_cast<SmartPathKey*>(key);
            spKey->getValue().actionConnectNodes(node1Id, node2Id);
        }
        mBaseValue.actionConnectNodes(node1Id, node2Id);
        prp_afterWholeInfluenceRangeChanged();
    }

    void actionMergeNodes(const int node1Id, const int node2Id) {
        const auto& keys = anim_getKeys();
        for(const auto &key : keys) {
            const auto spKey = static_cast<SmartPathKey*>(key);
            spKey->getValue().actionMergeNodes(node1Id, node2Id);
        }
        mBaseValue.actionMergeNodes(node1Id, node2Id);
        prp_afterWholeInfluenceRangeChanged();
    }

    void actionMoveNodeBetween(const int nodeId,
                               const int prevNodeId,
                               const int nextNodeId) {
        beforeBinaryPathChange();
        getCurrentlyEditedPath()->actionMoveNodeBetween(
                    nodeId, prevNodeId, nextNodeId);
        pathChanged();
    }

    void actionClose() {
        actionConnectNodes(0, mBaseValue.getNodeCount() - 1);
    }

    void actionDisconnectNodes(const int node1Id, const int node2Id);

    void actionReverseCurrent() {
        beforeBinaryPathChange();
        getCurrentlyEditedPath()->actionReversePath();
        pathChanged();
    }

    void actionReverseAll() {
        const auto& keys = anim_getKeys();
        for(const auto &key : keys) {
            const auto spKey = static_cast<SmartPathKey*>(key);
            spKey->getValue().actionReversePath();
        }
        mBaseValue.actionReversePath();
        prp_afterWholeInfluenceRangeChanged();
    }

    void actionAppendMoveAllFrom(SmartPathAnimator * const other) {
        anim_coordinateKeysWith(other);
        const auto& keys = anim_getKeys();
        for(int i = 0; i < keys.count(); i++) {
            const auto thisKey = anim_getKeyAtIndex<SmartPathKey>(i);
            const auto otherKey = other->anim_getKeyAtIndex<SmartPathKey>(i);
            thisKey->getValue().actionAppendMoveAllFrom(
                        std::move(otherKey->getValue()));
        }
        mBaseValue.actionAppendMoveAllFrom(std::move(other->getBaseValue()));
        prp_afterWholeInfluenceRangeChanged();
        other->prp_afterWholeInfluenceRangeChanged();
        updateAllPoints();
    }

    void actionPrependMoveAllFrom(SmartPathAnimator * const other) {
        anim_coordinateKeysWith(other);
        const auto& keys = anim_getKeys();
        for(int i = 0; i < keys.count(); i++) {
            const auto thisKey = anim_getKeyAtIndex<SmartPathKey>(i);
            const auto otherKey = other->anim_getKeyAtIndex<SmartPathKey>(i);
            thisKey->getValue().actionPrependMoveAllFrom(
                        std::move(otherKey->getValue()));
        }
        mBaseValue.actionPrependMoveAllFrom(std::move(other->getBaseValue()));
        prp_afterWholeInfluenceRangeChanged();
        other->prp_afterWholeInfluenceRangeChanged();
        updateAllPoints();
    }

    bool hasDetached() const {
        return mBaseValue.hasDetached();
    }

    qsptr<SmartPathAnimator> createFromDetached() {
        if(!hasDetached()) return nullptr;
        const auto baseDetached = mBaseValue.getAndClearLastDetached();
        SmartPath baseSmartPath(baseDetached);
        const auto newAnim = enve::make_shared<SmartPathAnimator>(baseSmartPath);
        const auto& keys = anim_getKeys();
        for(const auto &key : keys) {
            const auto spKey = static_cast<SmartPathKey*>(key);
            auto& sp = spKey->getValue();
            const auto keyDetached = sp.getAndClearLastDetached();
            SmartPath keySmartPath(keyDetached);
            const auto newKey = enve::make_shared<SmartPathKey>(
                        keySmartPath, key->getRelFrame(), newAnim.get());
            newAnim->anim_appendKey(newKey);
        }
        return newAnim;
    }


    void applyTransform(const QMatrix &transform) {
        const auto& keys = anim_getKeys();
        for(const auto &key : keys) {
            const auto spKey = static_cast<SmartPathKey*>(key);
            spKey->getValue().applyTransform(transform);
        }
        mBaseValue.applyTransform(transform);
        updateAllPoints();
    }

    const SkPath& getCurrentPath() {
        if(!mPathUpToDate) {
            mCurrentPath = getCurrentlyEditedPath()->getPathAt();
            mPathUpToDate = true;
        }
        return mCurrentPath;
    }

    void setMode(const Mode mode) {
        if(mMode == mode) return;
        mMode = mode;
        prp_afterWholeInfluenceRangeChanged();
        emit pathBlendModeChagned(mode);
    }

    Mode getMode() const { return mMode; }

    void pastePath(const SmartPath& path) {
        return pastePath(anim_getCurrentRelFrame(), path);
    }

    void pastePath(const int frame, SmartPath path) {
        if(!anim_isRecording()) {
            mBaseValue = path;
            return prp_afterWholeInfluenceRangeChanged();
        }
        const bool pasteClosed = path.isClosed();
        const bool baseClosed = mBaseValue.isClosed();

        if(pasteClosed != baseClosed) {
            if(pasteClosed) path.actionOpen();
            else {
                const auto& keys = anim_getKeys();
                for(const auto &key : keys) {
                    const auto spKey = static_cast<SmartPathKey*>(key);
                    auto& sp = spKey->getValue();
                    sp.actionOpen();
                }
            }
        }

        const int pasteNodes = path.getNodeCount();
        const int baseNodes = mBaseValue.getNodeCount();
        const int addNodes = pasteNodes - baseNodes;
        if(addNodes > 0) {
            const auto& keys = anim_getKeys();
            for(const auto &key : keys) {
                const auto spKey = static_cast<SmartPathKey*>(key);
                auto& sp = spKey->getValue();
                sp.addDissolvedNodes(addNodes);
            }
        } else if(addNodes < 0) {
            path.addDissolvedNodes(-addNodes);
        }

        auto key = anim_getKeyAtRelFrame<SmartPathKey>(frame);
        if(key) key->setValue(path);
        else {
            const auto newKey = enve::make_shared<SmartPathKey>(path, frame, this);
            anim_appendKey(newKey);
        }
    }
signals:
    void pathBlendModeChagned(Mode);
protected:
    SmartPath& getBaseValue() {
        return mBaseValue;
    }
private:
    int actionAddFirstNode(const QPointF &relPos) {
        return actionAddFirstNode({false, false, CTRLS_SYMMETRIC,
                                   relPos, relPos, relPos});
    }

    int actionAddFirstNode(const NormalNodeData &data) {
        const auto& keys = anim_getKeys();
        for(const auto &key : keys) {
            const auto spKey = static_cast<SmartPathKey*>(key);
            spKey->getValue().actionAddFirstNode(data);
        }
        const int id = mBaseValue.actionAddFirstNode(data);
        prp_afterWholeInfluenceRangeChanged();
        return id;
    }

    void updateBaseValue() {
        const auto prevK = anim_getPrevKey<SmartPathKey>(anim_getCurrentRelFrame());
        const auto nextK = anim_getNextKey<SmartPathKey>(anim_getCurrentRelFrame());
        const auto keyAtFrame = anim_getKeyOnCurrentFrame<SmartPathKey>();
        mPathUpToDate = false;
        deepCopySmartPathFromRelFrame(anim_getCurrentRelFrame(),
                                      prevK, nextK, keyAtFrame,
                                      mBaseValue);
    }

    void updateAllPoints();

    void deepCopySmartPathFromRelFrame(const int relFrame,
                                       SmartPathKey * const prevKey,
                                       SmartPathKey * const nextKey,
                                       SmartPathKey * const keyAtFrame,
                                       SmartPath &result) const {
        if(keyAtFrame) {
            result = keyAtFrame->getValue();
        } else if(prevKey && nextKey) {
            const qreal nWeight = prevKeyWeight(prevKey, nextKey, relFrame);
            gInterpolate(prevKey->getValue(), nextKey->getValue(),
                         nWeight, result);
        } else if(prevKey) {
            result = prevKey->getValue();
        } else if(nextKey) {
            result = nextKey->getValue();
        } else {
            if(&result == &mBaseValue) return;
            result = mBaseValue;
        }
    }    

    bool mPathChanged = false;
    bool mPathUpToDate = true;
    SkPath mCurrentPath;
    SmartPath mBaseValue;
    SmartPath * mPathBeingChanged_d = &mBaseValue;
    Mode mMode = NORMAL;
};

#endif // SMARTPATHANIMATOR_H

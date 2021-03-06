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

#include "Boxes/textbox.h"
#include <QInputDialog>
#include <QMenu>
#include "canvas.h"
#include "Animators/gradientpoints.h"
#include "Animators/qstringanimator.h"
#include "Animators/rastereffectanimators.h"
#include "typemenu.h"
#include "Animators/transformanimator.h"
#include "Animators/outlinesettingsanimator.h"

TextBox::TextBox() : PathBox(TYPE_TEXT) {
    prp_setName("text");

    mFillSettings->setPaintType(PaintType::FLATPAINT);
    mFillSettings->setCurrentColor(QColor(0, 0, 0));
    mStrokeSettings->setPaintType(PaintType::NOPAINT);

    const auto pathsUpdater = [this](const UpdateReason reason) {
        setPathsOutdated(reason);
    };

    mText = enve::make_shared<QStringAnimator>("text");
    ca_prependChildAnimator(mRasterEffectsAnimators.data(), mText);
    connect(mText.get(), &Property::prp_currentFrameChanged,
            this, pathsUpdater);

    mLinesDist = enve::make_shared<QrealAnimator>(100, 0, 100, 1, "line dist");
    ca_prependChildAnimator(mRasterEffectsAnimators.data(), mLinesDist);
    connect(mLinesDist.get(), &Property::prp_currentFrameChanged,
            this, pathsUpdater);
}

#include <QApplication>
#include <QDesktopWidget>

void TextBox::openTextEditor(QWidget* dialogParent) {
    bool ok;
    const QString text =
            QInputDialog::getMultiLineText(
                dialogParent, prp_getName() + " text",
                "Text:", mText->getCurrentValue(), &ok);
    if(ok) mText->setCurrentValue(text);
}

void TextBox::setFont(const QFont &font) {
    mFont = font;
    prp_afterWholeInfluenceRangeChanged();
    setPathsOutdated(UpdateReason::userChange);
}

void TextBox::setSelectedFontSize(const qreal size) {
    QFont newFont = mFont;
    newFont.setPointSizeF(size);
    setFont(newFont);
}

void TextBox::setSelectedFontFamilyAndStyle(const QString &fontFamily,
                                            const QString &fontStyle) {
    QFont newFont = mFont;
    newFont.setFamily(fontFamily);
    newFont.setStyleName(fontStyle);
    setFont(newFont);
}

qreal TextBox::getFontSize() {
    return mFont.pointSize();
}

QString TextBox::getFontFamily() {
    return mFont.family();
}

QString TextBox::getFontStyle() {
    return mFont.styleName();
}

QString TextBox::getCurrentValue() {
    return mText->getCurrentValue();
}

qreal textLineX(const Qt::Alignment &alignment,
                const qreal lineWidth,
                const qreal maxWidth) {
    if(alignment == Qt::AlignCenter) {
        return (maxWidth - lineWidth)*0.5;
    } else if(alignment == Qt::AlignLeft) {
        return 0;
    } else {// if(alignment == Qt::AlignRight) {
        return maxWidth - lineWidth;
    }
}

void TextBox::setupCanvasMenu(PropertyMenu * const menu) {
    if(menu->hasActionsForType<TextBox>()) return;
    menu->addedActionsForType<TextBox>();
    PathBox::setupCanvasMenu(menu);
    const auto widget = menu->getParentWidget();
    PropertyMenu::PlainSelectedOp<TextBox> op = [widget](TextBox * box) {
        box->openTextEditor(widget);
    };
    menu->addPlainAction("Set Text...", op);
}

SkPath TextBox::getPathAtRelFrameF(const qreal relFrame) {
    const QString textAtFrame = mText->getValueAtRelFrame(relFrame);
    const qreal linesDistAtFrame =
            mLinesDist->getEffectiveValue(relFrame)*0.01;
    const QStringList lines = textAtFrame.split(QRegExp("\n|\r\n|\r"));
    const QFontMetricsF fm(mFont);
    qreal maxWidth = 0;
    for(const auto& line : lines) {
        const qreal lineWidth = fm.width(line);
        if(lineWidth > maxWidth) maxWidth = lineWidth;
    }

    const SkFont font = toSkFont(mFont);
    SkPath result;
    //QPainterPath result;
    for(int i = 0; i < lines.count(); i++) {
        const auto& line = lines.at(i);
        if(line.isEmpty()) continue;
        const qreal lineWidth = fm.width(line);
        SkPath linePath;
        const qreal lineX = textLineX(mAlignment, lineWidth, maxWidth);
        const qreal lineY = i*fm.height()*linesDistAtFrame;
        const auto lineStd = line.toStdString();
        const auto lineCStr = lineStd.c_str();
        SkTextUtils::GetPath(lineCStr,
                             static_cast<size_t>(line.length()),
                             SkTextEncoding::kUTF8,
                             toSkScalar(lineX), toSkScalar(lineY),
                             font, &linePath);
        result.addPath(linePath);
        //result.addText(lineX, lineY, mFont, line);
    }
    //return toSkPath(result);
    return result;
}

void TextBox::setCurrentValue(const QString &text) {
    mText->setCurrentValue(text);
}

bool TextBox::differenceInEditPathBetweenFrames(
        const int frame1, const int frame2) const {
    if(mText->prp_differencesBetweenRelFrames(frame1, frame2)) return true;
    return mLinesDist->prp_differencesBetweenRelFrames(frame1, frame2);
}


void TextBox::writeBoundingBox(eWriteStream& dst) {
    PathBox::writeBoundingBox(dst);
    dst.write(&mAlignment, sizeof(Qt::Alignment));
    dst << mFont.pointSizeF();
    dst << mFont.family();
    dst << mFont.styleName();
}

void TextBox::readBoundingBox(eReadStream& src) {
    PathBox::readBoundingBox(src);
    src.read(&mAlignment, sizeof(Qt::Alignment));
    qreal fontSize;
    QString fontFamily;
    QString fontStyle;
    src >> fontSize;
    src >> fontFamily;
    src >> fontStyle;
    mFont.setPointSizeF(fontSize);
    mFont.setFamily(fontFamily);
    mFont.setStyleName(fontStyle);
}

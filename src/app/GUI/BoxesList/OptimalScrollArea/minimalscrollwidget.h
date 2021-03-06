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

#ifndef MINIMALSCROLLWIDGET_H
#define MINIMALSCROLLWIDGET_H


#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QtMath>
class SingleWidget;
class MinimalScrollWidgetVisiblePart;
class ScrollArea;

class MinimalScrollWidget : public QWidget {
    Q_OBJECT
public:
    explicit MinimalScrollWidget(ScrollArea * const parent);

    virtual void updateHeight() = 0;

    void scrollParentAreaBy(const int by);

    void changeVisibleTop(const int top);
    void changeVisibleHeight(const int height);
    void setWidth(const int width);
protected:
    virtual void createVisiblePartWidget() = 0;
    MinimalScrollWidgetVisiblePart *mMinimalVisiblePartWidget;
    ScrollArea *mParentScrollArea;
};


#endif // MINIMALSCROLLWIDGET_H

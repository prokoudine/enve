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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDockWidget>
#include <QMainWindow>
#include <QGraphicsView>
#include <QComboBox>
#include <QPushButton>
#include "undoredo.h"
#include "Private/Tasks/taskscheduler.h"
#include "effectsloader.h"
#include "Private/document.h"
#include "audiohandler.h"
#include "actions.h"
#include "layouthandler.h"
#include "Private/esettings.h"
#include "renderhandler.h"
#include "fileshandler.h"
#include "ekeyfilter.h"

class VideoEncoder;

class SwitchButton;
class ActionButton;
class BoxesList;
class FontsWidget;
class UpdateScheduler;
class AnimationDockWidget;
class ColorSettingsWidget;
class FillStrokeSettingsWidget;
class TimelineDockWidget;
class BrushSelectionWidget;
class CanvasWindow;
class MemoryHandler;

class ObjectSettingsWidget;
class BoxScrollWidget;
class ScrollWidget;
class ScrollArea;
class UsageWidget;
class CentralWidget;
//class SoundComposition;

const QString MENU_STYLESHEET =
        "QMenu {\
            background-color: rgb(255, 255, 255);\
            border: 1px solid black;\
        }\
        \
        QMenu::item {\
            spacing: 3px;\
            padding: 2px 25px 2px 25px;\
            background: transparent;\
            color: black;\
        }\
        \
        QMenu::item:selected {\
            background-color: rgb(200, 200, 200);\
            color: black;\
        };";

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(Document &document, Actions &actions,
               AudioHandler &audioHandler,
               RenderHandler &renderHandler,
               QWidget * const parent = nullptr);
    ~MainWindow();

//    void (MainWindow::*mBoxesUpdateFinishedFunction)(void) = nullptr;

    static MainWindow *sGetInstance();

    AnimationDockWidget *getAnimationDockWidget();
    BoxScrollWidget *getObjectSettingsList();

    FillStrokeSettingsWidget *getFillStrokeSettings();
    void saveToFile(const QString &path);
    void loadEVFile(const QString &path);
    void clearAll();
    void updateTitle();
    void setFileChangedSinceSaving(const bool changed);
    void disableEventFilter();
    void enableEventFilter();

    void scheduleBoxesListRepaint();
    void disable();
    void enable();

    void updateCanvasModeButtonsChecked();

    //void addBoxAwaitingUpdate(BoundingBox *box);
    void setCurrentBox(BoundingBox *box);

//    void nextSaveOutputFrame();
//    void nextPlayPreviewFrame();

    void setResolutionFraction(qreal percent);

    void updateSettingsForCurrentCanvas(Canvas * const scene);

    void addCanvas(Canvas * const newCanvas);

    FontsWidget *getFontsWidget() const {
        return mFontWidget;
    }
    SimpleBrushWrapper *getCurrentBrush() const;

    UsageWidget* getUsageWidget() const {
        return mUsageWidget;
    }

    //void playPreview();
   // void stopPreview();
    void setResolutionFractionValue(const qreal value);

    void addCanvasToRenderQue();

    const QStringList& getRecentFiles() const {
        return mRecentFiles;
    }
    stdsptr<void> lock();

    bool processKeyEvent(QKeyEvent *event);

    void installNumericFilter(QObject* const object) {
        object->installEventFilter(mNumericFilter);
    }

    void installLineFilter(QObject* const object) {
        object->installEventFilter(mLineFilter);
    }

    void togglePaintBrushDockVisible()
    { mBrushSettingsDockAction->toggle(); }

    void toggleFillStrokeSettingsDockVisible()
    { mFillAndStrokeSettingsDock->toggle(); }
public:
    //void saveOutput(QString renderDest);
    //void renderOutput();
    //void sendNextBoxForUpdate();

    void newFile();
    bool askForSaving();
    void openFile();
    void openFile(const QString& openPath);
    void saveFile();
    void saveFileAs();
    void saveBackup();
    bool closeProject();
    void linkFile();
    void importImageSequence();
    void importFile();
    //void importVideo();
    void revert();
protected:
    void lockFinished();
private:
    QWidget * mWelcomeDialog = nullptr;
    CentralWidget * mCentralWidget = nullptr;

    void openWelcomeDialog();
    void closeWelcomeDialog();

    eKeyFilter* mNumericFilter = eKeyFilter::sCreateNumberFilter(this);
    eKeyFilter* mLineFilter = eKeyFilter::sCreateLineFilter(this);

    friend class Lock;
    class Lock : public StdSelfRef {
        e_OBJECT
    protected:
        Lock(MainWindow * const window) : mWindow(window) {}
    public:
        ~Lock() { mWindow->lockFinished(); }
    private:
        MainWindow * const mWindow;
    };
    stdptr<Lock> mLock;
    static MainWindow *sInstance;

    void updateRecentMenu();

    void addRecentFile(const QString& recent) {
        if(mRecentFiles.contains(recent))
            mRecentFiles.removeOne(recent);
        while(mRecentFiles.count() >= 8)
            mRecentFiles.removeLast();
        mRecentFiles.prepend(recent);
        updateRecentMenu();
        writeRecentFiles();
    }

    void readRecentFiles() {
        QFile file(eSettings::sSettingsDir() + "/recent");
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            while(!stream.atEnd()) {
                const QString path = stream.readLine();
                if(path.isEmpty()) continue;
                //if(QFile(path).exists())
                mRecentFiles.append(path);
            }
        }
    }

    void writeRecentFiles() {
        QFile file(eSettings::sSettingsDir() + "/recent");
        if(file.open(QIODevice::WriteOnly | QIODevice::Text |
                     QIODevice::Truncate)) {
            QTextStream stream(&file);
            for(const auto& recent : mRecentFiles) {
                stream << recent << endl;
            }
        }
    }

    QStringList mRecentFiles;

//    bool mRendering = false;

//    bool mCancelLastBoxUpdate = false;
//    BoundingBox *mLastUpdatedBox = nullptr;
//    QList<BoundingBox*> mBoxesAwaitingUpdate;
//    bool mNoBoxesAwaitUpdate = true;

    QDockWidget *mFillStrokeSettingsDock;
    QDockWidget *mBottomDock;
    QDockWidget *mLeftDock;
    QDockWidget *mLeftDock2;
    QDockWidget *mBrushSettingsDock;
    TimelineDockWidget *mBoxesListAnimationDockWidget = nullptr;
    BrushSelectionWidget *mBrushSelectionWidget = nullptr;

    QStatusBar* mStatusBar;
    UsageWidget* mUsageWidget = nullptr;
    QToolBar *mToolBar;

    SwitchButton *mBoxTransformMode;
    SwitchButton *mPointTransformMode;
    SwitchButton *mAddPointMode;
    SwitchButton *mPickPaintSettingsMode;

    SwitchButton *mCircleMode;
//
    SwitchButton *mRectangleMode;
    SwitchButton *mTextMode;
    SwitchButton *mPaintMode;
//
    ActionButton *mActionConnectPoints;
    ActionButton *mActionDisconnectPoints;
    ActionButton *mActionMergePoints;
//
    ActionButton *mActionSymmetricPointCtrls;
    ActionButton *mActionSmoothPointCtrls;
    ActionButton *mActionCornerPointCtrls;
//
    ActionButton *mActionLine;
    ActionButton *mActionCurve;
//
    QAction *mNoneQuality;
    QAction *mLowQuality;
    QAction *mMediumQuality;
    QAction *mHighQuality;
    QAction *mDynamicQuality;

    QAction *mClipViewToCanvas;
    QAction *mRasterEffectsVisible;
    QAction *mPathEffectsVisible;
    QAction *mCurrentObjectDock;
    QAction *mFilesDock;
    QAction *mObjectsAndAnimationsDock;
    QAction *mFillAndStrokeSettingsDock;
    QAction *mBrushSettingsDockAction;
    QAction *mBrushColorBookmarksAction;

    FontsWidget *mFontWidget = nullptr;

    QMenuBar *mMenuBar;
    QMenu *mFileMenu;
    QMenu *mRecentMenu;
    QMenu *mSelectSameMenu;
    QMenu *mEditMenu;
    QMenu *mObjectMenu;
    QMenu *mPathMenu;
    QMenu *mEffectsMenu;
    QMenu *mSceneMenu;
    QMenu *mViewMenu;
    QMenu *mPanelsMenu;
    QMenu *mRenderMenu;

    FilesHandler mFilesHandler;
    Document& mDocument;
    Actions& mActions;
    AudioHandler& mAudioHandler;
    RenderHandler& mRenderHandler;

    LayoutHandler *mLayoutHandler = nullptr;
    stdptr<UndoRedoStack> mCurrentUndoRedoStack;

    FillStrokeSettingsWidget *mFillStrokeSettings;

    bool mChangedSinceSaving = false;
    bool mEventFilterDisabled = true;
    bool isEnabled();
    QWidget *mGrayOutWidget = nullptr;
    bool mDisplayedFillStrokeSettingsUpdateNeeded = false;

    BoxScrollWidget *mObjectSettingsWidget = nullptr;
    ScrollArea *mObjectSettingsScrollArea;

    void setupStatusBar();
    void setupToolBar();
    void connectToolBarActions();
    void setupMenuBar();

    QList<Gradient*> mLoadedGradientsList;
protected:
    void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject *obj, QEvent *e);
    void closeEvent(QCloseEvent *e);
};

#endif // MAINWINDOW_H

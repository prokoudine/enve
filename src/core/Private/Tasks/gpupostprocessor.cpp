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

#include "gpupostprocessor.h"
#include <QImage>
#include <QOpenGLTexture>
#include "skia/skqtconversions.h"
#include "Private/Tasks/taskscheduler.h"

GpuPostProcessor::GpuPostProcessor() {
    connect(this, &QThread::finished,
            this, &GpuPostProcessor::afterProcessed);
}

void GpuPostProcessor::initialize() {
    OffscreenQGL33c::initialize();
    moveContextToThread(this);
}

void GpuPostProcessor::afterProcessed() {
    if(unhandledException())
        gPrintExceptionCritical(handleException());
    mAllDone = true;
    for(const auto& task : _mHandledProcesses) {
        const bool nextStep = !task->waitingToCancel() && task->nextStep();
        if(nextStep) TaskScheduler::sGetInstance()->queCpuTask(task);
        else task->finishedProcessing();
    }
    _mHandledProcesses.clear();
    handleScheduledProcesses();
    TaskScheduler::sGetInstance()->afterCpuGpuTaskFinished();
}

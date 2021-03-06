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

#include "outputsettingsdisplaywidget.h"
#include "GUI/global.h"

OutputSettingsDisplayWidget::OutputSettingsDisplayWidget(QWidget *parent) :
    QWidget(parent) {
//    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mMainLayout = new QVBoxLayout(this);
    setLayout(mMainLayout);
    mAudioVideoLayout = new QHBoxLayout();
    mOutputFormatLabel = new QLabel("<b>Format:</b>", this);
    mVideoCodecLabel = new QLabel("<b>Video codec:</b>", this);
    mVideoPixelFormatLabel = new QLabel("<b>Pixel format:</b>", this);
    mVideoBitrateLabel = new QLabel("<b>Video bitrate:</b>", this);
    mAudioCodecLabel = new QLabel("<b>Audio codec:</b>", this);
    mAudioSampleRateLabel = new QLabel("<b>Audio sample rate:</b>", this);
    mAudioSampleFormatLabel = new QLabel("<b>Audio sample format:</b>", this);
    mAudioBitrateLabel = new QLabel("<b>Audio bitrate:</b>", this);
    mAudioChannelLayoutLabel = new QLabel("<b>Audio channel layout:</b>", this);

    mOutputFormatLabel->setFixedHeight(MIN_WIDGET_DIM);
    mVideoCodecLabel->setFixedHeight(MIN_WIDGET_DIM);
    mVideoPixelFormatLabel->setFixedHeight(MIN_WIDGET_DIM);
    mVideoBitrateLabel->setFixedHeight(MIN_WIDGET_DIM);
    mAudioCodecLabel->setFixedHeight(MIN_WIDGET_DIM);
    mAudioSampleRateLabel->setFixedHeight(MIN_WIDGET_DIM);
    mAudioSampleFormatLabel->setFixedHeight(MIN_WIDGET_DIM);
    mAudioBitrateLabel->setFixedHeight(MIN_WIDGET_DIM);
    mAudioChannelLayoutLabel->setFixedHeight(MIN_WIDGET_DIM);

    mVideoLayout = new QVBoxLayout();
    mVideoLayout->addWidget(mVideoCodecLabel);
    mVideoLayout->addWidget(mVideoPixelFormatLabel);
    mVideoLayout->addWidget(mVideoBitrateLabel);

    mAudioLayout = new QVBoxLayout();
    mAudioLayout->addWidget(mAudioCodecLabel);
    mAudioLayout->addWidget(mAudioSampleRateLabel);
    mAudioLayout->addWidget(mAudioSampleFormatLabel);
    mAudioLayout->addWidget(mAudioBitrateLabel);
    mAudioLayout->addWidget(mAudioChannelLayoutLabel);

    mAudioVideoLayout->addLayout(mVideoLayout);
    mAudioVideoLayout->addLayout(mAudioLayout);
    mAudioVideoLayout->setAlignment(mVideoLayout, Qt::AlignLeft);
    mAudioVideoLayout->setAlignment(mAudioLayout, Qt::AlignLeft);
    mAudioVideoLayout->setAlignment(Qt::AlignLeft);
    mAudioVideoLayout->setSpacing(MIN_WIDGET_DIM);
    mAudioLayout->setSpacing(0);
    mVideoLayout->setSpacing(0);
    mVideoLayout->setAlignment(Qt::AlignTop);

    mMainLayout->addWidget(mOutputFormatLabel);
    mMainLayout->addLayout(mAudioVideoLayout);
}

void OutputSettingsDisplayWidget::setOutputSettings(const OutputSettings &settings) {
    if(!settings.outputFormat) {
        setOutputFormatText("-");
    } else {
        setOutputFormatText(QString(settings.outputFormat->long_name));
    }
    if(!mAlwaysShowAll) {
        setVideoLabelsVisible(settings.videoEnabled);
    }
    if(!settings.videoCodec) {
        setVideoCodecText("-");
    } else {
        setVideoCodecText(QString(settings.videoCodec->long_name));
    }
    const char *pixelFormat = av_get_pix_fmt_name(settings.videoPixelFormat);
    if(!pixelFormat) {
        setPixelFormatText("-");
    } else {
        setPixelFormatText(QString(pixelFormat));
    }
    setVideoBitrateText(QString::number(settings.videoBitrate/1000000) + " Mbps");
    if(!mAlwaysShowAll) {
        setAudioLabelsVisible(settings.audioEnabled);
    }
    if(!settings.audioCodec) {
        setAudioCodecText("-");
    } else {
        setAudioCodecText(QString(settings.audioCodec->long_name));
    }
    setAudioSampleRateText(QString::number(settings.audioSampleRate) + " Hz");
    int formatId = settings.audioSampleFormat;
    if(OutputSettings::SAMPLE_FORMATS_NAMES.find(formatId) ==
            OutputSettings::SAMPLE_FORMATS_NAMES.end()) {
        setAudioSampleFormatText("-");
    } else {
        setAudioSampleFormatText(OutputSettings::SAMPLE_FORMATS_NAMES.at(formatId));
    }
    setAudioBitrateText(QString::number(settings.audioBitrate/1000) + " kbps");

    const auto channLay = OutputSettings::sGetChannelsLayoutName(
                settings.audioChannelsLayout);
    setAudioChannelLayoutText(channLay);
}

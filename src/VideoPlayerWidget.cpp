/*
 *  Copyright (c) 2015 Álan Crístoffer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#include "VideoPlayerWidget.h"
#include "ui_VideoPlayerWidget.h"

#include <math.h>

#include <QCloseEvent>
#include <QMediaPlayer>
#include <QStyle>

struct VideoPlayerWidgetPrivate
{
    std::unique_ptr<Ui::VideoPlayerWidget> ui;
    bool                                   isPlaying = false;
    QString                                secondsToString(qint64 seconds) const;
};

VideoPlayerWidget::VideoPlayerWidget(QWidget *parent) : QWidget(parent)
{
    _p.reset(new VideoPlayerWidgetPrivate);
    _p->ui.reset(new Ui::VideoPlayerWidget);
    _p->ui->setupUi(this);

    pause();

    connect(_p->ui->play,   &QToolButton::clicked,    this, &VideoPlayerWidget::playPauseSwitch);
    connect(_p->ui->volume, &QSlider::sliderReleased, [&]() {
        emit volumeSet( static_cast<quint8> ( _p->ui->volume->value() ) );
    });

    connect(_p->ui->duration, &QSlider::sliderReleased, [&]() {
        emit positionSet(_p->ui->duration->value() * 1000);
    });
}

QVideoWidget *VideoPlayerWidget::videoWidget() const
{
    return _p->ui->video;
}

void VideoPlayerWidget::play()
{
    _p->isPlaying = true;
    _p->ui->play->setIcon( style()->standardIcon(QStyle::SP_MediaPause) );
}

void VideoPlayerWidget::pause()
{
    _p->isPlaying = false;
    _p->ui->play->setIcon( style()->standardIcon(QStyle::SP_MediaPlay) );
}

void VideoPlayerWidget::playPauseSwitch()
{
    if ( _p->isPlaying ) {
        emit pausePressed();
        pause();
    } else {
        emit playPressed();
        play();
    }
}

void VideoPlayerWidget::setVolume(const quint8 percent)
{
    _p->ui->duration->setValue(percent);
}

void VideoPlayerWidget::setDuration(const qint64 miliseconds)
{
    _p->ui->duration->setMaximum( static_cast<int> (miliseconds / 1000) );
}

void VideoPlayerWidget::setPosition(const qint64 miliseconds)
{
    qint64 seconds = miliseconds / 1000;

    setDuration( reinterpret_cast<QMediaPlayer *> ( sender() )->duration() );

    _p->ui->duration->setValue( static_cast<int> (seconds) );
    _p->ui->durationLabel->setText( _p->secondsToString(seconds) + " / " + _p->secondsToString( _p->ui->duration->maximum() ) );
}

VideoPlayerWidget::~VideoPlayerWidget() = default;

void VideoPlayerWidget::closeEvent(QCloseEvent *event)
{
    emit closing();

    event->accept();
}

QString VideoPlayerWidgetPrivate::secondsToString(qint64 seconds) const
{
    seconds = (seconds < 0 ? -seconds : seconds);
    int hours = static_cast<int> (seconds / 3600);
    seconds = seconds % 3600;
    int minutes = static_cast<int> (seconds / 60);
    seconds = seconds % 60;

    QString s;

    if ( hours > 0 ) {
        s += QString("%1:").arg( hours, 2, 10, QChar('0') );
    }

    s += QString("%1:").arg( minutes, 2, 10, QChar('0') );

    return s + QString("%1").arg( seconds, 2, 10, QChar('0') );
}

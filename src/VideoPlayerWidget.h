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

#ifndef VIDEOPLAYERWIDGET_H
#define VIDEOPLAYERWIDGET_H

#include <memory>

#include <QVideoWidget>
#include <QWidget>

struct VideoPlayerWidgetPrivate;

class VideoPlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayerWidget(QWidget *parent = 0);
    ~VideoPlayerWidget();

    QVideoWidget *videoWidget() const;

public slots:
    void play();
    void pause();
    void playPauseSwitch();
    void setVolume(const quint8 percent);
    void setDuration(const qint64 miliseconds);
    void setPosition(const qint64 miliseconds);

signals:
    void closing();
    void playPressed();
    void pausePressed();
    void volumeSet(const quint8 percent);
    void positionSet(const qint64 miliseconds);

private:
    std::shared_ptr<VideoPlayerWidgetPrivate> _p;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // VIDEOPLAYERWIDGET_H

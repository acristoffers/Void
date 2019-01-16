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

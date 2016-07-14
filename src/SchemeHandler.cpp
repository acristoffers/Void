#include "SchemeHandler.h"

#include <QBuffer>
#include <QImage>
#include <QWebEngineUrlRequestJob>

#include "Runner.h"
#include "Store.h"

struct SchemeHandlerPrivate
{
    std::shared_ptr<Store> store;
};

SchemeHandler::SchemeHandler(std::shared_ptr<Store> store)
{
    _p.reset(new SchemeHandlerPrivate);

    _p->store = store;
}

SchemeHandler::~SchemeHandler() = default;

void SchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    Runner *runner = new Runner([job, this]() {
        QString path = job->requestUrl().path();
        QString scheme = job->requestUrl().scheme();
        QBuffer *buffer = new QBuffer;
        QByteArray data( _p->store->decryptFile(path) );

        buffer->open(QIODevice::ReadWrite);
        connect(job, &QObject::destroyed, buffer, &QObject::deleteLater);

        if ( _p->store->error != Store::Success ) {
            job->fail(QWebEngineUrlRequestJob::RequestFailed);
            return;
        }

        if ( scheme == "thumb" ) {
            QImage image = QImage::fromData(data);
            image = image.scaledToWidth(200, Qt::SmoothTransformation);
            image.save(buffer, "PNG");
        } else {
            buffer->write(data);
        }

        buffer->seek(0);

        job->reply("image/png", buffer);
    });

    QThreadPool::globalInstance()->start(runner);
}

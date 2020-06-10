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
    QString path = job->requestUrl().path();

    if ( path.isEmpty() ) {
        job->fail(QWebEngineUrlRequestJob::RequestFailed);
        return;
    }

    QString scheme  = job->requestUrl().scheme();
    QString mime    = _p->store->fileMetadata(path, "mimetype");
    QBuffer *buffer = new QBuffer;
    QByteArray data(_p->store->decryptFile(path) );

    buffer->open(QIODevice::ReadWrite);
    connect(job, &QObject::destroyed, buffer, &QObject::deleteLater);

    if ( _p->store->error != Store::Success ) {
        job->fail(QWebEngineUrlRequestJob::RequestFailed);
        return;
    } else if ( scheme == "thumb" ) {
        QImage image = QImage::fromData(data);
        image        = image.scaledToWidth(200, Qt::SmoothTransformation);
        image.save(buffer, "PNG");
    } else {
        buffer->write(data);
    }

    buffer->seek(0);

    job->reply(scheme == "thumb" ? "image/png" : mime.toUtf8(), buffer);
}

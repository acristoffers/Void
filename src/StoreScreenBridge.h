/*
 *  Copyright (c) 2015 Álan Crístoffer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */

#ifndef STORESCREENBRIDGE_H
#define STORESCREENBRIDGE_H

#include <memory>

#include <QObject>

#include "Store.h"
#include "StoreScreen.h"

struct StoreScreenBridgePrivate;

class StoreScreenBridge : public QObject
{
    Q_OBJECT
public:
    StoreScreenBridge(const QString &path, const QString &password, const bool create, StoreScreen *parent);
    ~StoreScreenBridge();
    std::shared_ptr<Store> store();

    Q_INVOKABLE void setLang(QString lang);
    Q_INVOKABLE QString lang() const;

    Q_INVOKABLE void asyncAddFile(const QString fsPath, const QString storePath);
    Q_INVOKABLE void decrypt(const QStringList paths, const QString currentP);
    Q_INVOKABLE QStringList getFile() const;
    Q_INVOKABLE QString getFolder() const;
    Q_INVOKABLE QStringList listFilesInFolder(const QString folder) const;
    Q_INVOKABLE void playVideo(const QString path) const;
    Q_INVOKABLE void saveSetting(const QString key, const QString value) const;
    Q_INVOKABLE QString setting(const QString key) const;

    Store::StoreError error;
private:
    std::unique_ptr<StoreScreenBridgePrivate> _p;
public slots:
    // Kind of ugly hack to make threaded signal work. Seems like connections
    // from QWebChannel are not queued, so we need a router to get it out of
    // the thread and into GUI-thread before dispatching it to the browser.
    void routeSignalSlot(const QString signal, const QVariantList args);

signals:
    void routeSignalSignal(const QString signal, const QVariantList args);
    void startAddFile(const QString fsPath, const QString storePath);
    void endAddFile(const QString fsPath, const QString storePath);
    void startDecryptFile(const QString path);
    void endDecryptFile(const QString path);
};

#endif // STORESCREENBRIDGE_H

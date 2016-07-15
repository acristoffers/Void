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

#include "StoreScreenBridge.h"

#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QtWebChannel>

#include "Runner.h"

struct StoreScreenBridgePrivate
{
    std::shared_ptr<Store> store;
    StoreScreen            *parent;
    QMutex                 mutex;
};

StoreScreenBridge::StoreScreenBridge(const QString &path, const QString &password, const bool create, StoreScreen *parent) : QObject()
{
    _p.reset(new StoreScreenBridgePrivate);
    _p->store.reset( new Store(path, password, create) );
    error      = _p->store->error;
    _p->parent = parent;

    if ( error == Store::Success ) {
        _p->parent->channel()->registerObject( QStringLiteral("store"), _p->store.get() );
    }

    connect(this, &StoreScreenBridge::routeSignalSignal, this, &StoreScreenBridge::routeSignalSlot, Qt::QueuedConnection);
}

std::shared_ptr<Store> StoreScreenBridge::store()
{
    return _p->store;
}

StoreScreenBridge::~StoreScreenBridge() = default;

void StoreScreenBridge::setLang(QString lang)
{
    QSettings settings;

    settings.setValue(QStringLiteral("lang"), lang);
}

QString StoreScreenBridge::lang() const
{
    QSettings settings;

    if ( !settings.contains( QStringLiteral("lang") ) ) {
        settings.setValue( QStringLiteral("lang"), QStringLiteral("en") );
    }

    return settings.value( QStringLiteral("lang") ).toString();
}

void StoreScreenBridge::asyncAddFile(const QString fsPath, const QString storePath)
{
    Runner *runner = new Runner([fsPath, storePath, this]() {
        QMutexLocker locker(&_p->mutex);

        QVariantList args;
        args << fsPath;
        args << storePath;

        emit routeSignalSignal("startAddFile", args);
        _p->store->addFile(fsPath, storePath);
        emit routeSignalSignal("endAddFile", args);
    });

    QThreadPool::globalInstance()->start(runner);
}

void StoreScreenBridge::decrypt(const QStringList paths, const QString currentPath)
{
    QString target_path = QFileDialog::getExistingDirectory( nullptr, QStringLiteral("Save to"), QDir::homePath() );

    if ( target_path.isEmpty() ) {
        return;
    }

    QMap<QString, QString> fileList;
    for ( QString path : paths ) {
        for ( QString file : _p->store->searchStartsWith(path, 1) ) {
            QString target_file_path = file;
            target_file_path = target_path + "/" + target_file_path.remove( 0, currentPath.size() );
            fileList[file]   = target_file_path.replace(QRegularExpression("/+"), "/");
        }
    }

    for ( QString path : fileList.keys() ) {
        QString dest    = fileList[path];
        Runner  *runner = new Runner([path, dest, this]() {
            QVariantList args;
            args << path;

            emit routeSignalSignal("startDecryptFile", args);
            _p->store->decryptFile(path, dest);
            emit routeSignalSignal("endDecryptFile", args);
        });

        QThreadPool::globalInstance()->start(runner);
    }
}

QStringList StoreScreenBridge::getFile() const
{
    return QFileDialog::getOpenFileNames( nullptr, QStringLiteral("Load Store"), QDir::homePath() );
}

QString StoreScreenBridge::getFolder() const
{
    return QFileDialog::getExistingDirectory( nullptr, QStringLiteral("Load Store"), QDir::homePath() );
}

QStringList StoreScreenBridge::listFilesInFolder(const QString folder) const
{
    QStringList fs;
    QStringList ds;

    ds << folder;

    while ( !ds.empty() ) {
        QDir d( ds.first() );
        ds.removeFirst();

        for ( auto  e : d.entryList(QDir::Files | QDir::Readable) ) {
            fs << d.filePath(e);
        }

        for ( auto  e : d.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable) ) {
            ds << d.filePath(e);
        }
    }

    return fs;
}

void StoreScreenBridge::saveSetting(const QString key, const QString value) const
{
    QSettings settings;

    settings.beginGroup("js");
    settings.setValue(key, value);
}

QString StoreScreenBridge::setting(const QString key) const
{
    QSettings settings;

    settings.beginGroup("js");
    return settings.value(key, "").toString();
}

void StoreScreenBridge::routeSignalSlot(const QString signal, const QVariantList args)
{
    if ( signal == "startAddFile" ) {
        emit startAddFile( args[0].toString(), args[1].toString() );
    } else if ( signal == "endAddFile" ) {
        emit endAddFile( args[0].toString(), args[1].toString() );
    } else if ( signal == "startDecryptFile" ) {
        emit startDecryptFile( args[0].toString() );
    } else if ( signal == "endDecryptFile" ) {
        emit endDecryptFile( args[0].toString() );
    }
}

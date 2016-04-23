#include "WelcomeScreenBridge.h"

#include <QFileDialog>
#include <QSettings>

#include "StoreScreen.h"

struct WelcomeScreenBridgePrivate
{
    QString       path;
    WelcomeScreen *parent;
};

WelcomeScreenBridge::WelcomeScreenBridge(WelcomeScreen *parent)
{
    _p.reset(new WelcomeScreenBridgePrivate);
    _p->parent = parent;
}

WelcomeScreenBridge::~WelcomeScreenBridge() = default;

bool WelcomeScreenBridge::getExisting()
{
    _p->path = QFileDialog::getExistingDirectory( nullptr, "Load Store", QDir::homePath() );

    return !_p->path.isEmpty();
}

bool WelcomeScreenBridge::getNew()
{
    _p->path = QFileDialog::getSaveFileName( nullptr, "Load Store", QDir::homePath() );

    return !_p->path.isEmpty();
}

QString WelcomeScreenBridge::open(const QString password)
{
    // deletes itself
    StoreScreen *store = new StoreScreen(_p->path, password, false);

    return StoreError2QString(store->error);
}

QString WelcomeScreenBridge::create(const QString password)
{
    // deletes itself
    StoreScreen *store = new StoreScreen(_p->path, password, true);

    return StoreError2QString(store->error);
}

void WelcomeScreenBridge::close()
{
    _p->parent->close();
}

void WelcomeScreenBridge::setLang(QString lang)
{
    QSettings settings;

    settings.setValue("lang", lang);
}

QString WelcomeScreenBridge::lang()
{
    QSettings settings;

    if ( !settings.contains("lang") ) {
        settings.setValue("lang", "en");
    }

    return settings.value("lang").toString();
}

QString WelcomeScreenBridge::StoreError2QString(Store::StoreError error)
{
    switch ( error ) {
        case Store::Success:
            return "Success";

        case Store::DoesntExistAndCreationIsNotPermitted:
            return "DoesntExistAndCreationIsNotPermitted";

        case Store::CantCreateCryptoObject:
            return "CantCreateCryptoObject";

        case Store::CantOpenStoreFile:
            return "CantOpenStoreFile";

        case Store::CantOpenFile:
            return "CantOpenFile";

        case Store::CantWriteToFile:
            return "CantWriteToFile";

        case Store::FileTooLarge:
            return "FileTooLarge";

        case Store::NoSuchFile:
            return "NoSuchFile";

        case Store::PartCorrupted:
            return "PartCorrupted";

        case Store::WrongCheckSum:
            return "WrongCheckSum";

        case Store::FileAlreadyExists:
            return "FileAlreadyExists";
    }
}

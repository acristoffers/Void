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
    _p->path = QFileDialog::getExistingDirectory( nullptr, QStringLiteral("Load Store"), QDir::homePath() );

    return !_p->path.isEmpty();
}

bool WelcomeScreenBridge::getNew()
{
    _p->path = QFileDialog::getSaveFileName( nullptr, QStringLiteral("Load Store"), QDir::homePath() );

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

    settings.setValue(QStringLiteral("lang"), lang);
}

QString WelcomeScreenBridge::lang()
{
    QSettings settings;

    if ( !settings.contains( QStringLiteral("lang") ) ) {
        settings.setValue( QStringLiteral("lang"), QStringLiteral("en") );
    }

    return settings.value( QStringLiteral("lang") ).toString();
}

QString WelcomeScreenBridge::StoreError2QString(Store::StoreError error)
{
    switch ( error ) {
        case Store::Success:
            return QStringLiteral("Success");

        case Store::DoesntExistAndCreationIsNotPermitted:
            return QStringLiteral("DoesntExistAndCreationIsNotPermitted");

        case Store::CantCreateCryptoObject:
            return QStringLiteral("CantCreateCryptoObject");

        case Store::CantOpenStoreFile:
            return QStringLiteral("CantOpenStoreFile");

        case Store::CantOpenFile:
            return QStringLiteral("CantOpenFile");

        case Store::CantWriteToFile:
            return QStringLiteral("CantWriteToFile");

        case Store::FileTooLarge:
            return QStringLiteral("FileTooLarge");

        case Store::NoSuchFile:
            return QStringLiteral("NoSuchFile");

        case Store::PartCorrupted:
            return QStringLiteral("PartCorrupted");

        case Store::WrongCheckSum:
            return QStringLiteral("WrongCheckSum");

        case Store::FileAlreadyExists:
            return QStringLiteral("FileAlreadyExists");
    }
}

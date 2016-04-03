#include "Store.h"

#include <iostream>

#include <QDir>
#include <QFile>

#include "Crypto.h"
#include "StoreFile.h"

struct StorePrivate
{
    QString                    path;
    std::unique_ptr<Crypto>    storeCrypto;
    std::unique_ptr<StoreFile> storeFile;
};

Store::Store(QString path, QString password, bool create) : QObject()
{
    _p.reset(new StorePrivate);
    _p->path = path;

    bool storeExists = QFile::exists(path + "/Store.void");

    if ( !storeExists && create ) {
        std::string pswd = password.toUtf8().toStdString();
        std::string salt = Crypto::generateRandom(16);
        std::string iv   = Crypto::generateRandom(16);
        _p->storeCrypto.reset( new Crypto(pswd, salt, iv) );

        if ( _p->storeCrypto->error != Crypto::Success ) {
            error       = CantCreateCryptoObject;
            cryptoError = _p->storeCrypto->error;
        }

        QDir::home().mkpath(path);

        _p->storeFile.reset( new StoreFile(path + "/Store.void") );
        _p->storeFile->setSalt(salt);
        _p->storeFile->setIV(iv);
    } else if ( storeExists ) {
        _p->storeFile.reset( new StoreFile(path + "/Store.void") );
        std::string pswd = password.toUtf8().toStdString();
        std::string salt = _p->storeFile->salt();
        std::string iv   = _p->storeFile->IV();
        _p->storeCrypto.reset( new Crypto(pswd, salt, iv) );

        if ( _p->storeCrypto->error != Crypto::Success ) {
            error       = CantCreateCryptoObject;
            cryptoError = _p->storeCrypto->error;
        }
    } else {
        error = DoesntExistAndCreationIsNotPermitted;
        return;
    }

    error = StoreError::Success;
}

Store::~Store() = default;

#include "Store.h"

#include <iostream>

#include <QDir>
#include <QFile>

#include "StoreFile.h"
#include "StoreFS.h"

struct StorePrivate
{
    QString                    path;
    std::unique_ptr<Crypto>    storeCrypto;
    std::unique_ptr<StoreFile> storeFile;
    std::unique_ptr<StoreFS>   storeFS;
};

Store::Store(const QString path, const QString password, const bool create) : QObject()
{
    _p.reset(new StorePrivate);
    _p->storeFS.reset( new StoreFS(path) );
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
        _p->storeFile->setCryptoParams( CryptoParams() );
    } else if ( storeExists ) {
        _p->storeFile.reset( new StoreFile(path + "/Store.void") );

        if ( _p->storeFile->error != StoreFile::Success ) {
            error = CantOpenStoreFile;
            return;
        }

        std::string  pswd         = password.toUtf8().toStdString();
        std::string  salt         = _p->storeFile->salt();
        std::string  iv           = _p->storeFile->IV();
        CryptoParams cryptoParams = _p->storeFile->cryptoParams();

        _p->storeCrypto.reset( new Crypto(pswd, salt, iv, cryptoParams) );
        if ( _p->storeCrypto->error != Crypto::Success ) {
            error       = CantCreateCryptoObject;
            cryptoError = _p->storeCrypto->error;
        }

        std::string data = _p->storeFile->data().toStdString();
        data = _p->storeCrypto->decrypt(data);
        QByteArray badata = QByteArray::fromStdString(data);
        _p->storeFS->load(badata);
    } else {
        error = DoesntExistAndCreationIsNotPermitted;
        return;
    }

    error = StoreError::Success;
}

Store::~Store() = default;

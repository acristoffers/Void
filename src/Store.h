#ifndef STORE_H
#define STORE_H

#include <QObject>
#include <QString>

#include "Crypto.h"

struct StorePrivate;

class Store : public QObject
{
    Q_OBJECT
public:
    enum StoreError {
        // Everything went OK
        Success,

        // You passed create=false and there is no Store on the given path
        DoesntExistAndCreationIsNotPermitted,

        // An error inside Crypto occured, lookup cryptoError
        CantCreateCryptoObject,

        // The file could not be opened
        CantOpenStoreFile
    }

    error;

    Crypto::CryptoError cryptoError;

    Store(const QString path, const QString password, const bool create = false);
    ~Store();
private:
    std::unique_ptr<StorePrivate> _p;
};
#endif // STORE_H

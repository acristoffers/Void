#ifndef STORE_H
#define STORE_H

#include <memory>

#include <QObject>
#include <QString>

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
        CantCreateCryptoObject
    };

    Store(QString path, QString password, bool create = false);
    ~Store();

    StoreError   error;
    unsigned int cryptoError;
private:
    std::unique_ptr<StorePrivate> _p;
};
#endif // STORE_H

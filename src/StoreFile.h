#ifndef STOREFILE_H
#define STOREFILE_H

#include <memory>

#include <QObject>
#include <QString>

#include "Crypto.h"

struct StoreFilePrivate;

class StoreFile : public QObject
{
    Q_OBJECT
public:
    enum StoreFileError {
        Success,
        CantOpenFile
    }

    error;

    StoreFile(QString path);
    ~StoreFile();

    void setCryptoParams(const CryptoParams params);
    void setData(const QByteArray data);
    void setIV(const std::string iv);
    void setSalt(const std::string salt);

    CryptoParams cryptoParams() const;
    QByteArray data() const;
    std::string IV() const;
    std::string salt() const;

private:
    std::unique_ptr<StoreFilePrivate> _p;
};
#endif // STOREFILE_H

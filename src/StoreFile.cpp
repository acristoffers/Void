#include "StoreFile.h"

#include <QByteArray>
#include <QDataStream>
#include <QFile>
#include <QMap>

struct StoreFilePrivate
{
    quint16                version = 1;
    std::unique_ptr<QFile> storeFile;
    std::string            salt;
    std::string            iv;
    QByteArray             data;
    CryptoParams           cryptoParams;

    void save();
    void load();
};

StoreFile::StoreFile(QString path) : QObject()
{
    _p.reset(new StoreFilePrivate);
    _p->storeFile.reset( new QFile(path) );
    if ( !_p->storeFile->open(QIODevice::ReadWrite) ) {
        error = CantOpenFile;
        return;
    }

    if ( _p->storeFile->size() > 0 ) {
        _p->load();
    }

    error = Success;
}

void StoreFile::setCryptoParams(const CryptoParams params)
{
    _p->cryptoParams = params;
    _p->save();
}

void StoreFile::setData(const QByteArray data)
{
    _p->data = data;
    _p->save();
}

void StoreFile::setIV(const std::string iv)
{
    _p->iv = iv;
    _p->save();
}

void StoreFile::setSalt(const std::string salt)
{
    _p->salt = salt;
    _p->save();
}

CryptoParams StoreFile::cryptoParams() const
{
    return _p->cryptoParams;
}

QByteArray StoreFile::data() const
{
    return _p->data;
}

std::string StoreFile::IV() const
{
    return _p->iv;
}

std::string StoreFile::salt() const
{
    return _p->salt;
}

StoreFile::~StoreFile() = default;

void StoreFilePrivate::save()
{
    storeFile->seek(0);
    QDataStream stream( storeFile.get() );
    stream.setVersion(QDataStream::Qt_5_6);

    QByteArray qsalt = QByteArray::fromStdString(salt);
    QByteArray qiv   = QByteArray::fromStdString(iv);
    stream << version
           << qsalt
           << qiv
           << (quint8) cryptoParams.digest
           << (quint8) cryptoParams.encryption
           << (quint8) cryptoParams.keyDerivationFunction
           << (quint8) cryptoParams.keyDerivationHash
           << cryptoParams.keyDerivationCost
           << data;
}

void StoreFilePrivate::load()
{
    QDataStream stream( storeFile.get() );

    stream.setVersion(QDataStream::Qt_5_6);

    QByteArray qsalt, qiv;
    quint8     digest, encryption, keyDerivationFunction, keyDerivationHash;

    stream >> version
    >> qsalt
    >> qiv
    >> digest
    >> encryption
    >> keyDerivationFunction
    >> keyDerivationHash
    >> cryptoParams.keyDerivationCost
    >> data;

    cryptoParams.digest                = (DigestType) digest;
    cryptoParams.encryption            = (EncType) encryption;
    cryptoParams.keyDerivationFunction = (KeyDerivationFunction) keyDerivationFunction;
    cryptoParams.keyDerivationHash     = (KeyDerivationHash) keyDerivationHash;

    salt = qsalt.toStdString();
    iv   = qiv.toStdString();
}

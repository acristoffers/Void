#include "StoreFile.h"

#include <QByteArray>
#include <QDataStream>
#include <QFile>
#include <QMap>

#include "Utils.h"

struct StoreFilePrivate
{
    std::unique_ptr<QFile> storeFile;
    std::string            salt;
    std::string            iv;
    QByteArray             data;

    void save();
    void load();
};

StoreFile::StoreFile(QString path) : QObject()
{
    _p.reset(new StoreFilePrivate);
    _p->storeFile.reset( new QFile(path) );
    _p->storeFile->open(QIODevice::ReadWrite);
    _p->load();
}

void StoreFile::setSalt(std::string salt)
{
    _p->salt = salt;
    _p->save();
}

void StoreFile::setIV(std::string iv)
{
    _p->iv = iv;
    _p->save();
}

void StoreFile::setData(QByteArray data)
{
    _p->data = data;
    _p->save();
}

std::string StoreFile::salt()
{
    return _p->salt;
}

std::string StoreFile::IV()
{
    return _p->iv;
}

QByteArray StoreFile::data()
{
    return _p->data;
}

StoreFile::~StoreFile() = default;

void StoreFilePrivate::save()
{
    storeFile->seek(0);
    QDataStream stream( storeFile.get() );

    QByteArray qsalt = QByteArray::fromStdString(salt);
    QByteArray qiv   = QByteArray::fromStdString(iv);
    stream << qsalt << qiv << data;
}

void StoreFilePrivate::load()
{
    QDataStream stream( storeFile.get() );

    QByteArray qsalt, qiv;

    stream >> qsalt >> qiv >> data;

    salt = qsalt.toStdString();
    iv   = qiv.toStdString();
}

#ifndef STOREDATASTRUCT_H
#define STOREDATASTRUCT_H

#include <QList>
#include <QMap>

#include "Crypto.h"

struct StoreFSDir;
struct StoreFSFile;

using StoreFSDirPtr  = std::shared_ptr<StoreFSDir>;
using StoreFSFilePtr = std::shared_ptr<StoreFSFile>;

struct StoreFSDir
{
    quint64               id;
    QString               name;
    QString               path;
    StoreFSDirPtr         parent;
    QList<StoreFSDirPtr>  subdirs;
    QList<StoreFSFilePtr> files;
};

struct StoreFSFile
{
    quint64       id;
    StoreFSDirPtr parent;

    QString                   name;
    QString                   path;
    quint64                   size;
    QMap<QString, QByteArray> metadata;

    QByteArray             key;
    QByteArray             iv;
    QByteArray             salt;
    QByteArray             digest;
    CryptoParams           params;
    QMap<quint32, QString> cryptoParts;
};

struct StoreFS
{
    StoreFS(const QString storePath);

    QByteArray serialize() const;

    void load(const QByteArray &data);

    StoreFSDirPtr  dir(const QString path) const;
    StoreFSFilePtr file(const QString path) const;

    QList<StoreFSDirPtr> subdirs(const QString path) const;

    QList<StoreFSFilePtr> subfiles(const QString path) const;

    StoreFSDirPtr makePath(const QString path);
    StoreFSDirPtr addSubdir(const QString name, const StoreFSDirPtr parent);

    StoreFSFilePtr addFile(const QString path, const QByteArray data);
    StoreFSFilePtr addFile(const QString filePath, const QString storePath);

    QByteArray decryptFile(const QString path);
    void       decryptFile(const QString storePath, const QString path);

    enum StoreFSError {
        Success,
        CantOpenFile,
        CantWriteToFile,
        CantCreateCryptoObject,
        FileTooLarge,
        NoSuchFile,
        PartCorrupted,
        WrongCheckSum
    }

    error;

private:
    // Files and dir share the same id space
    // the MSB for dirs is set and for files it's cleared
    static quint64 fileIdCounter;
    static quint64 dirIdCounter;

    QMap<quint64, QString>        idPathMap;
    QMap<QString, quint64>        pathIdMap;
    QMap<quint64, StoreFSDirPtr>  idDirMap;
    QMap<quint64, StoreFSFilePtr> idFileMap;

    StoreFSDirPtr root;

    QString storePath;
    quint32 version = 1;
};
#endif // STOREDATASTRUCT_H

#ifndef STOREDATASTRUCT_H
#define STOREDATASTRUCT_H

#include <QList>
#include <QMap>

#include "Crypto.h"

struct StoreFSDir;
struct StoreFSFile;
struct StoreFSPrivate;

using StoreFSDirPtr  = std::shared_ptr<StoreFSDir>;
using StoreFSFilePtr = std::shared_ptr<StoreFSFile>;

/**
 *  \brief Represents a Directory in the internal structure.
 */
struct StoreFSDir
{
    quint64               id;      /*!< Internal session id. */
    QString               name;    /*!< Name of the directory. */
    QString               path;    /*!< Path of this directory. Includes it's name. */
    StoreFSDirPtr         parent;  /*!< Pointer to parent node structure. */
    QList<StoreFSDirPtr>  subdirs; /*!< Subdirectories list. */
    QList<StoreFSFilePtr> files;   /*!< Files list. */
};

/**
 *  \brief Represents a File in the internal structure.
 */
struct StoreFSFile
{
    quint64       id;     /*!< Internal session id. */
    StoreFSDirPtr parent; /*!< Pointer to parent directory. */

    QString                   name;     /*!< Name of the file. */
    QString                   path;     /*!< Path of this file. Includes it's name. */
    quint64                   size;     /*!< Size of the unencrypted file in bytes. */
    QMap<QString, QByteArray> metadata; /*!< A list of metada. For general purpose. */

    QByteArray             key;         /*!< Key used to encrypt this file. */
    QByteArray             iv;          /*!< IV used to encrypt this file. */
    QByteArray             salt;        /*!< Salt used on digests. */
    QByteArray             digest;      /*!< Digest of the unencrypted file. */
    CryptoParams           params;      /*!< CryptoParams used to encrypt/digest the file. */
    QMap<quint32, QString> cryptoParts; /*!< Map of name of encrypted file parts. It's a map to guarantee order. Starts at 0. */
};

struct StoreFS
{
    StoreFS(const QString storePath);
    ~StoreFS();

    QByteArray serialize() const;

    void load(const QByteArray &data);

    StoreFSDirPtr  dir(const QString path) const;
    StoreFSDirPtr  dir(const quint64 id) const;
    StoreFSFilePtr file(const QString path) const;
    StoreFSFilePtr file(const quint64 id) const;

    QStringList allDirs() const;
    QStringList allEntries() const;
    QStringList allFiles() const;

    QList<StoreFSDirPtr> subdirs(const QString path) const;

    QList<StoreFSFilePtr> subfiles(const QString path) const;

    QList<quint64> entryBeginsWith(const QString) const;

    QList<quint64> entryEndsWith(const QString) const;

    QList<quint64> entryContains(const QString) const;

    QList<quint64> entryMatchRegExp(const QString) const;

    StoreFSDirPtr addSubdir(const QString name, const StoreFSDirPtr parent);
    StoreFSDirPtr makePath(const QString path);
    void          removeDir(const QString path);
    void          moveDir(const QString oldPath, const QString newPath);

    StoreFSFilePtr addFile(const QString path, const QByteArray data);
    StoreFSFilePtr addFile(const QString filePath, const QString storePath);
    QByteArray     decryptFile(const QString path);
    void           decryptFile(const QString storePath, const QString path);
    void           moveFile(const QString oldPath, const QString newPath);
    void           removeFile(const QString path);

    /**
     *  \brief Errors returned by StoreFS
     */
    enum StoreFSError : uint8_t {
        Success,                /*!< Success */
        CantCreateCryptoObject, /*!< An error occured inside Crypto. */
        CantOpenFile,           /*!< Could not open a file. */
        CantWriteToFile,        /*!< Could not write to a file. */
        FileTooLarge,           /*!< The file is too large. Use Store#decryptFile(const QString, const QString) instead */
        NoSuchFile,             /*!< File does not exist. */
        PartCorrupted,          /*!< The checksum of the part file did not match. Verify that you are using the same parameters used during creation. The file might be just corrupted. */
        WrongCheckSum,          /*!< The checksum of the whole file did not match. Verify that you are using the same parameters used during creation. One of the files might be just corrupted. */
        FileAlreadyExists       /*!< A destination file already exists. */
    }

    /**
     *  \brief Errors returned by StoreFS
     */
    error;

private:
    std::unique_ptr<StoreFSPrivate> _p;
};
#endif // STOREDATASTRUCT_H

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
    /**
     *  \brief Errors returned by Store
     */
    enum StoreError : uint8_t {
        Success,                              /*!< Success */
        DoesntExistAndCreationIsNotPermitted, /*!< You passed create=false and there is no Store on the given path. */
        CantCreateCryptoObject,               /*!< An error occured inside Crypto. \see Store#cryptoError */
        CantOpenStoreFile,                    /*!< The Store.void file could not be opened. */

        // From StoreFS
        CantOpenFile,     /*!< Could not open a file. */
        CantWriteToFile,  /*!< Could not write to a file. */
        FileTooLarge,     /*!< The file is too large. Use Store#decryptFile(const QString, const QString) instead */
        NoSuchFile,       /*!< File does not exist. */
        PartCorrupted,    /*!< The checksum of the part file did not match. Verify that you are using the same parameters used during creation. The file might be just corrupted. */
        WrongCheckSum,    /*!< The checksum of the whole file did not match. Verify that you are using the same parameters used during creation. One of the files might be just corrupted. */
        FileAlreadyExists /*!< A destination file already exists. */
    }

    /**
     *  \brief Errors ocurred by Store
     */
    error;

    /**
     *  \brief Errors ocurred in the Crypto object.
     */
    Crypto::CryptoError cryptoError;

    Store(const QString path, const QString password, const bool create = false);
    ~Store();

    void addFile(const QString storePath, const QByteArray data);
    void addFile(const QString filePath, const QString storePath);
    QByteArray decryptFile(const QString path);
    void decryptFile(const QString storePath, const QString path);
    void move(const QString oldPath, const QString newPath);
    void remove(const QString path);

    void makePath(const QString path);

    QStringList listAllDirectories() const;
    QStringList listAllEntries() const;
    QStringList listAllFiles() const;
    QStringList listSubdirectories(QString path) const;
    QStringList listFiles(QString path) const;
    QStringList listEntries(QString path) const;
    QStringList searchStartsWith(QString filter, quint8 type) const;
    QStringList searchEndsWith(QString filter, quint8 type) const;
    QStringList searchContains(QString filter, quint8 type) const;
    QStringList searchRegex(QString filter, quint8 type) const;

    QByteArray fileMetadata(const QString path, const QString key);
    void setFileMetadata(const QString path, const QString key, const QByteArray data);
    quint64 fileSize(const QString path);

private:
    std::unique_ptr<StorePrivate> _p;
};
#endif // STORE_H

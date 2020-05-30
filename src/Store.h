/*
 *  Copyright (c) 2015 Álan Crístoffer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#ifndef STORE_H
#define STORE_H

#include <memory>

#include <QObject>
#include <QString>

#include "Crypto.h"

struct StorePrivate;

class Store : public QObject
{
    Q_OBJECT
    Q_ENUMS(StoreError)
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

    Q_INVOKABLE void addFileFromData(const QString storePath, const QByteArray data);
    Q_INVOKABLE void addFile(const QString filePath, const QString storePath);
    Q_INVOKABLE QByteArray decryptFile(const QString path);
    Q_INVOKABLE void decryptFile(const QString storePath, const QString path);
    Q_INVOKABLE void move(const QString oldPath, const QString newPath);
    Q_INVOKABLE void remove(const QString path);

    Q_INVOKABLE void makePath(const QString path);

    Q_INVOKABLE QStringList listAllDirectories() const;
    Q_INVOKABLE QStringList listAllEntries() const;
    Q_INVOKABLE QStringList listAllFiles() const;
    Q_INVOKABLE QStringList listSubdirectories(QString path) const;
    Q_INVOKABLE QStringList listFiles(QString path) const;
    Q_INVOKABLE QStringList listEntries(QString path) const;
    Q_INVOKABLE QStringList searchStartsWith(QString filter, quint8 type) const;
    Q_INVOKABLE QStringList searchEndsWith(QString filter, quint8 type) const;
    Q_INVOKABLE QStringList searchContains(QString filter, quint8 type) const;
    Q_INVOKABLE QStringList searchRegex(QString filter, quint8 type) const;

    Q_INVOKABLE QByteArray fileMetadata(const QString path, const QString key);
    Q_INVOKABLE void setFileMetadata(const QString path, const QString key, const QByteArray data);
    Q_INVOKABLE quint64 fileSize(const QString path);

private:
    std::unique_ptr<StorePrivate> _p;
};

#endif // STORE_H

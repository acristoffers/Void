/*
 *  Copyright (c) 2015 Álan Crístoffer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */

#include "Store.h"

#include <iostream>

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QMimeDatabase>

#include "StoreFile.h"
#include "StoreFS.h"

/*!
 *  \class Store
 *  \brief Manages encrypted files in a "store".
 *
 *  Manages a Store of encrypted files. AES-256 encryption.
 *
 *  A Store is a folder containing a Store.void file, which contains
 *  metadata, and a bunch of 128 bytes long "randomly" named files.
 *
 *  The Store.void file is a structured file. It contains information about the files
 *  encrypted in the store such as it's real name, mimetype and which encrypted
 *  files compose the file and in which order. It's encrypted as well.
 *
 *  The files are encrypted into parts of 50MB (power of 2), in order to
 *  obscure it's real size. The names are a SHA512 sum of the unencrypted
 *  file's content and a random salt (per file), making it pretty much random
 *  itself.
 *
 */

/**
 *  \brief Store's private data struct.
 */
struct StorePrivate
{
    QString                    path;        /*!< Path to the Store directory in the file system. */
    std::unique_ptr<Crypto>    storeCrypto; /*!< Crypto object used to encrypt/decrypt the Store. */
    std::unique_ptr<StoreFile> storeFile;   /*!< StoreFile object of this Store. */
    std::unique_ptr<StoreFS>   storeFS;     /*!< StoreFS object of this Store. */

    void              save();
    Store::StoreError storeFSErrorToStoreError(StoreFS::StoreFSError);
};

/**
 *  \brief Constructs a Store object for an existing or new store.
 *
 *  \arg \c path The path to the store folder.
 *  \arg \c password The password used to create the store.
 *  \arg \c create Wether it should create the store case it doesn't exist.
 *
 *  \see Store#error
 */
Store::Store(const QString path, const QString password, const bool create) : QObject()
{
    _p.reset(new StorePrivate);
    _p->storeFS.reset( new StoreFS(path) );
    _p->path = path;

    error = StoreError::Success;

    bool storeExists = QFile::exists(path + "/Store.void");

    if ( !storeExists && create ) {
        std::string pswd = password.toUtf8().toStdString();
        std::string salt = Crypto::generateRandom(16);
        std::string iv   = Crypto::generateRandom(16);

        _p->storeCrypto.reset( new Crypto(pswd, salt, iv) );
        if ( _p->storeCrypto->error != Crypto::Success ) {
            error       = CantCreateCryptoObject;
            cryptoError = _p->storeCrypto->error;
            return;
        }

        QDir::home().mkpath(path);

        _p->storeFile.reset( new StoreFile(path + "/Store.void") );
        _p->storeFile->setSalt(salt);
        _p->storeFile->setIV(iv);
        _p->storeFile->setCryptoParams( CryptoParams() );
        _p->save();
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
            return;
        }

        std::string data = _p->storeFile->data().toStdString();
        _p->storeFile->setData( QByteArray() );

        data = _p->storeCrypto->decrypt(data);

        if ( _p->storeCrypto->error != Crypto::Success ) {
            error       = CantCreateCryptoObject;
            cryptoError = _p->storeCrypto->error;
            return;
        }

        data = qUncompress( reinterpret_cast<const unsigned char *> ( data.data() ), static_cast<int> ( data.size() ) ).toStdString();
        QByteArray bdata = QByteArray::fromStdString(data);
        data.clear();
        _p->storeFS->load(bdata);
    } else {
        error = DoesntExistAndCreationIsNotPermitted;
        return;
    }
}

/**
 *  \brief Adds a file to the store.
 *
 *  Encrypts and adds \c data as a file named \c storePath to the store.
 *  Store.void is saved with the new file information in case of success.
 *  Errors are reported through Store#error.
 *
 *  \arg \c storePath The path inside the store. Example: "/path/to/file.txt". Leading '/' is necessary.
 *  \arg \c data The contents of the file. Be awere of the 2GB limit imposed by QByteArray.
 *
 *  \see Store#addFile(const QString, const QString)
 *  \see Store#decryptFile(const QString)
 *  \see Store#decryptFile(const QString, const QString)
 *  \see Store#error
 */
void Store::addFile(const QString storePath, const QByteArray data)
{
    _p->storeFS->addFile(storePath, data);

    if ( _p->storeFS->error == StoreFS::Success ) {
        QMimeDatabase mimedb;
        QMimeType     mimetype = mimedb.mimeTypeForData(data);
        _p->storeFS->file(storePath)->metadata[QStringLiteral("mimetype")] = mimetype.name().toUtf8();

        _p->save();
    }

    error = _p->storeFSErrorToStoreError(_p->storeFS->error);
}

/**
 *  \brief Adds a file to the store.
 *
 *  Encrypts and adds \c filePath as a file named \c storePath to the store.
 *  Store.void is saved with the new file information in case of success.
 *  Errors are reported through Store#error.
 *
 *  \arg \c filePath Path of the file to be encrypted. No size limit.
 *  \arg \c storePath The path inside the store. Example: "/path/to/file.txt". Leading '/' is necessary.
 *
 *  \see Store#addFile(const QString, const QString)
 *  \see Store#decryptFile(const QString)
 *  \see Store#decryptFile(const QString, const QString)
 *  \see Store#error
 */
void Store::addFile(const QString filePath, const QString storePath)
{
    _p->storeFS->addFile(filePath, storePath);

    if ( _p->storeFS->error == StoreFS::Success ) {
        QMimeDatabase mimedb;
        QMimeType     mimetype = mimedb.mimeTypeForFile(filePath);
        _p->storeFS->file(storePath)->metadata[QStringLiteral("mimetype")] = mimetype.name().toUtf8();

        _p->save();
    }

    error = _p->storeFSErrorToStoreError(_p->storeFS->error);
}

/**
 *  \brief Decrypts the file in \c path and returns it's content.
 *
 *  Decrypts the file in \c path and returns a QByteArray with it's
 *  decrypted contents. Be awere of the size limit of QByteArray.
 *  Store#error will be set to Store#FileTooLarge if the
 *  file can't fit a QByteArray.
 *
 *  \arg \c path Path of the file to be decrypted.
 *
 *  \return The decrypted content.
 *
 *  \see Store#addFile(const QString, const QString)
 *  \see Store#addFile(const QString, const QByteArray)
 *  \see Store#decryptFile(const QString, const QString)
 *  \see Store#error
 */
QByteArray Store::decryptFile(const QString path)
{
    QByteArray data = _p->storeFS->decryptFile(path);

    error = _p->storeFSErrorToStoreError(_p->storeFS->error);
    return data;
}

/**
 *  \brief Decrypts the file in \c storePath into \c path.
 *
 *  Decrypts the file in \c storePath into \c path.
 *  Errors are reported through Store#error.
 *
 *  \arg \c storePath Path of the file to be decrypted.
 *  \arg \c path The path in the file system where the file will be decrypted.
 *
 *  \see Store#addFile(const QString, const QString)
 *  \see Store#addFile(const QString, const QByteArray)
 *  \see Store#decryptFile(const QString, const QString)
 *  \see Store#error
 */
void Store::decryptFile(const QString storePath, const QString path)
{
    _p->storeFS->decryptFile(storePath, path);
    error = _p->storeFSErrorToStoreError(_p->storeFS->error);
}

/**
 *  \brief Renames \c oldPath to \c newPath
 *
 *  Renames a file or directory in \c oldPath to \c newPath.
 *  Saves Store.void in case of success.
 *  Errors are reported through Store#error.
 *
 *  \arg \c oldPath Actual path of the file/directory.
 *  \arg \c newPath New path of the file/directory
 *
 *  \see Store#error
 */
void Store::move(const QString oldPath, const QString newPath)
{
    if ( _p->storeFS->file(oldPath) ) {
        _p->storeFS->moveFile(oldPath, newPath);

        if ( _p->storeFS->error == StoreFS::Success ) {
            _p->save();
        }

        error = _p->storeFSErrorToStoreError(_p->storeFS->error);
    } else if ( _p->storeFS->dir(oldPath) ) {
        _p->storeFS->moveDir(oldPath, newPath);

        if ( _p->storeFS->error == StoreFS::Success ) {
            _p->save();
        }

        error = _p->storeFSErrorToStoreError(_p->storeFS->error);
    } else {
        error = NoSuchFile;
    }
}

/**
 *  \brief Removes a file/directory tree from the store.
 *
 *  Removes the file/directory tree \c path from the
 *  store and deletes all the parts in the disk. Saves
 *  the changes to Store.void in case of success.
 *  Errors are reported through Store#error.
 *
 *  \arg \c path Path of the file to be removed.
 *
 *  \see Store#error
 */
void Store::remove(const QString path)
{
    if ( _p->storeFS->file(path) ) {
        _p->storeFS->removeFile(path);

        if ( _p->storeFS->error == StoreFS::Success ) {
            _p->save();
        }

        error = _p->storeFSErrorToStoreError(_p->storeFS->error);
    } else if ( _p->storeFS->dir(path) ) {
        _p->storeFS->removeDir(path);

        if ( _p->storeFS->error == StoreFS::Success ) {
            _p->save();
        }

        error = _p->storeFSErrorToStoreError(_p->storeFS->error);
    } else {
        error = NoSuchFile;
    }
}

/**
 *  \brief Makes a directory at path \c path, like `mkdir -p path`.
 *
 *  Directories are not saved in Store.void. Only files. Therefore,
 *  empty directories are lost when you reload the store.
 *
 *  \arg \c path Path to be created.
 *
 *  \see Store#error
 */
void Store::makePath(const QString path)
{
    _p->storeFS->makePath(path);
    error = _p->storeFSErrorToStoreError(_p->storeFS->error);
}

/**
 *  \brief Returns a list of paths of all directories in the Store.
 *
 *  \return The list of paths.
 */
QStringList Store::listAllDirectories() const
{
    return _p->storeFS->allDirs();
}

/**
 *  \brief Returns a list of paths of all entries in the Store.
 *
 *  \return The list of paths.
 */
QStringList Store::listAllEntries() const
{
    return _p->storeFS->allEntries();
}

/**
 *  \brief Returns a list of paths of all files in the Store.
 *
 *  \return The list of paths.
 */
QStringList Store::listAllFiles() const
{
    return _p->storeFS->allFiles();
}

/**
 *  \brief List subdirectories of path.
 *
 *  \arg \c path Path of direcotory to list subdirs.
 *
 *  \return List of directories in path.
 *
 *  \see Store#listFiles
 *  \see Store#listEntries
 */
QStringList Store::listSubdirectories(QString path) const
{
    QStringList list;

    QList<StoreFSDirPtr> ds = _p->storeFS->subdirs(path);
    for ( StoreFSDirPtr d : ds ) {
        list << d->path;
    }

    return list;
}

/**
 *  \brief List files in directory.
 *
 *  \arg \c path Path of direcotory to list files.
 *
 *  \return List of files in path.
 *
 *  \see Store#listSubdirectories
 *  \see Store#listEntries
 */
QStringList Store::listFiles(QString path) const
{
    QStringList list;

    QList<StoreFSFilePtr> fs = _p->storeFS->subfiles(path);
    for ( StoreFSFilePtr f : fs ) {
        list << f->path;
    }

    return list;
}

/**
 *  \brief List files and directories in directory.
 *
 *  \arg \c path Path of direcotory to list entries.
 *
 *  \return List of entries in path.
 *
 *  \see Store#listSubdirectories
 *  \see Store#listEntries
 */
QStringList Store::listEntries(QString path) const
{
    return listFiles(path) + listSubdirectories(path);
}

/**
 *  \brief List files and directories whose path starts with \c filter. "/" is never included.
 *
 *  \arg \c filter String to be matched.
 *  \arg \c type 0 for all, 1 for files only, 2 for directories only.
 *
 *  \return List of paths that match the \c filter.
 *
 *  \see Store#searchEndsWith
 *  \see Store#searchContains
 *  \see Store#searchRegex
 */
QStringList Store::searchStartsWith(QString filter, quint8 type) const
{
    QStringList entries;

    QList<quint64> ids = _p->storeFS->entryBeginsWith(filter);
    for ( quint64 id : ids ) {
        switch ( type ) {
            case 0:
                entries << _p->storeFS->path(id);
                break;

            case 1:
            {
                StoreFSFilePtr f = _p->storeFS->file(id);
                if ( f != nullptr ) {
                    entries << f->path;
                }
                break;
            }

            case 2:
            {
                StoreFSDirPtr d = _p->storeFS->dir(id);
                if ( d != nullptr ) {
                    entries << d->path;
                }
                break;
            }
        }
    }

    entries.removeAll("");

    return entries;
}

/**
 *  \brief List files and directories whose path ends with \c filter. "/" is never included.
 *
 *  \arg \c filter String to be matched.
 *  \arg \c type 0 for all, 1 for files only, 2 for directories only.
 *
 *  \return List of paths that match the \c filter.
 *
 *  \see Store#searchStartsWith
 *  \see Store#searchContains
 *  \see Store#searchRegex
 */
QStringList Store::searchEndsWith(QString filter, quint8 type) const
{
    QStringList entries;

    QList<quint64> ids = _p->storeFS->entryEndsWith(filter);
    for ( quint64 id : ids ) {
        switch ( type ) {
            case 0:
                entries << _p->storeFS->path(id);
                break;

            case 1:
            {
                StoreFSFilePtr f = _p->storeFS->file(id);
                if ( f != nullptr ) {
                    entries << f->path;
                }
                break;
            }

            case 2:
            {
                StoreFSDirPtr d = _p->storeFS->dir(id);
                if ( d != nullptr ) {
                    entries << d->path;
                }
                break;
            }
        }
    }

    entries.removeAll("");

    return entries;
}

/**
 *  \brief List files and directories whose path contains with \c filter. "/" is never included.
 *
 *  \arg \c filter String to be matched.
 *  \arg \c type 0 for all, 1 for files only, 2 for directories only.
 *
 *  \return List of paths that match the \c filter.
 *
 *  \see Store#searchStartsWith
 *  \see Store#searchContains
 *  \see Store#searchRegex
 */
QStringList Store::searchContains(QString filter, quint8 type) const
{
    QStringList entries;

    QList<quint64> ids = _p->storeFS->entryContains(filter);
    for ( quint64 id : ids ) {
        switch ( type ) {
            case 0:
                entries << _p->storeFS->path(id);
                break;

            case 1:
            {
                StoreFSFilePtr f = _p->storeFS->file(id);
                if ( f != nullptr ) {
                    entries << f->path;
                }
                break;
            }

            case 2:
            {
                StoreFSDirPtr d = _p->storeFS->dir(id);
                if ( d != nullptr ) {
                    entries << d->path;
                }
                break;
            }
        }
    }

    entries.removeAll("");

    return entries;
}

/**
 *  \brief List files and directories whose path matches the regex \c filter. "/" is never included.
 *
 *  \arg \c filter String to be matched.
 *  \arg \c type 0 for all, 1 for files only, 2 for directories only.
 *
 *  \return List of paths that match the \c filter.
 *
 *  \see Store#searchStartsWith
 *  \see Store#searchContains
 *  \see Store#searchRegex
 */
QStringList Store::searchRegex(QString filter, quint8 type) const
{
    QStringList entries;

    QList<quint64> ids = _p->storeFS->entryMatchRegExp(filter);
    for ( quint64 id : ids ) {
        switch ( type ) {
            case 0:
                entries << _p->storeFS->path(id);
                break;

            case 1:
            {
                StoreFSFilePtr f = _p->storeFS->file(id);
                if ( f != nullptr ) {
                    entries << f->path;
                }
                break;
            }

            case 2:
            {
                StoreFSDirPtr d = _p->storeFS->dir(id);
                if ( d != nullptr ) {
                    entries << d->path;
                }
                break;
            }
        }
    }

    entries.removeAll("");

    return entries;
}

/**
 *  \brief Returns metadata associated with this file.
 *
 *  The metadata system is meant to be used to save extra information about the file.
 *  By default the following fields are defined:
 *
 *  mimetype: mimetype of the file as returned by QMimeType#name.
 *
 *  \arg \c path File path.
 *  \arg \c key Metadata key.
 *
 *  \return The value associated with key \c key in the file metadata map.
 *
 *  \see Store#error
 *  \see Store#setFileMetadata
 *  \see QMimeDatabase#mimeTypeForName
 */
QByteArray Store::fileMetadata(const QString path, const QString key)
{
    error = Success;

    StoreFSFilePtr file = _p->storeFS->file(path);

    if ( file != nullptr ) {
        if ( file->metadata.contains(key) ) {
            return file->metadata[key];
        }
    } else {
        error = NoSuchFile;
    }

    return QByteArray();
}

/**
 *  \brief Sets metadata associated with this file.
 *
 *  The metadata system is meant to be used to save extra information about the file.
 *  Setting to an empty data will remove the key from the list.
 *  By default the following fields are defined:
 *
 *  mimetype: mimetype of the file as returned by QMimeType#name.
 *
 *  \arg \c path File path.
 *  \arg \c key Metadata key.
 *  \arg \c data The data to associate with the key.
 *
 *  \see Store#error
 *  \see Store#fileMetadata
 *  \see QMimeDatabase#mimeTypeForName
 */
void Store::setFileMetadata(const QString path, const QString key, const QByteArray data)
{
    error = Success;

    StoreFSFilePtr file = _p->storeFS->file(path);

    if ( file != nullptr ) {
        if ( data.isEmpty() ) {
            file->metadata.remove(key);
        } else {
            file->metadata[key] = data;
        }
    } else {
        error = NoSuchFile;
    }
}

/**
 *  \brief Returns the size of file in \c path.
 *
 *  \arg \c path Path of the file to lookup the size.
 *
 *  \return Size of file in \c path.
 *
 *  \see Store#error
 */
quint64 Store::fileSize(const QString path)
{
    error = Success;
    StoreFSFilePtr file = _p->storeFS->file(path);

    if ( file != nullptr ) {
        return file->size;
    }

    error = NoSuchFile;
    return 0;
}

/**
 *  \brief Default destructor.
 */
Store::~Store() = default;

/**
 *  \brief Saves the Store.void file.
 *
 *  Serializes, compresses and encrypts the store information,
 *  then save it to the Store.void file.
 *
 *  \see Crypto#encrypt
 *  \see StoreFile
 *  \see StoreFS
 */
void StorePrivate::save()
{
    std::string serialized = qCompress(storeFS->serialize(), 9).toStdString();

    serialized = storeCrypto->encrypt(serialized);

    if ( storeCrypto->error == Crypto::Success ) {
        storeFile->setData( QByteArray::fromStdString(serialized) );
    }
}

/**
 *  \brief Maps StoreFS#StoreFSError to Store#StoreError
 *
 *  \arg \c fserror The StoreFS#StoreFSError entry to be maped.
 *
 *  \return A Store#StoreError with same meaning as the StoreFS#StoreFSError.
 */
Store::StoreError StorePrivate::storeFSErrorToStoreError(StoreFS::StoreFSError fserror)
{
    switch ( fserror ) {
        case StoreFS::CantOpenFile:
            return Store::CantOpenFile;

        case StoreFS::CantWriteToFile:
            return Store::CantWriteToFile;

        case StoreFS::CantCreateCryptoObject:
            return Store::CantCreateCryptoObject;

        case StoreFS::FileTooLarge:
            return Store::FileTooLarge;

        case StoreFS::NoSuchFile:
            return Store::NoSuchFile;

        case StoreFS::PartCorrupted:
            return Store::PartCorrupted;

        case StoreFS::WrongCheckSum:
            return Store::WrongCheckSum;

        case StoreFS::FileAlreadyExists:
            return Store::FileAlreadyExists;

        case StoreFS::Success:
            return Store::Success;
    }
}

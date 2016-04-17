#include "Store.h"

#include <iostream>

#include <QDir>
#include <QFile>

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
 *  \brief Makes a directory at path \c path, like `mkdir -p \c path`.
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

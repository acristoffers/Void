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

#include "StoreFS.h"

#include <math.h>

#include <QDataStream>
#include <QFile>
#include <QRegularExpression>

#define MAX_PART_SIZE 52428800

/*!
 *  \class StoreFS
 *  \brief Manages the virtual "File System"
 *
 *  This class manages the file structure of the Store.
 *  It knows about the files and folders in it and manages
 *  the relationship between them. It has the basic functions
 *  from a file system, allowing to create, modify, rename/move
 *  and delete a file/folder. Folder structure only exists in the
 *  class, and is not serialized. This means that empty folders
 *  are lost on saving.
 *
 */

/**
 *  \brief StoreFS's private data structure
 */
struct StoreFSPrivate
{
    /**
     *  \brief Counter of file internal session ID.
     *
     *  Files and dir share the same id space the MSB
     *  for dirs is set and for files it's cleared.
     */
    static quint64 fileIdCounter;

    /**
     *  \brief Counter of directory internal session ID.
     *
     *  Files and dir share the same id space the MSB
     *  for dirs is set and for files it's cleared.
     */
    static quint64 dirIdCounter;

    QMap<quint64, QString>        idPathMap; /*!< Maps IDs to Paths */
    QMap<QString, quint64>        pathIdMap; /*!< Maps Paths to IDs */
    QMap<quint64, StoreFSDirPtr>  idDirMap;  /*!< Maps IDs to StoreFSDirPtr */
    QMap<quint64, StoreFSFilePtr> idFileMap; /*!< Maps IDs to StoreFSFilePtr */

    StoreFSDirPtr root; /*!< Root directory */

    QString storePath;   /*!< Path to the store folder. */
    quint32 version = 1; /*!< Version of the FS */
};

quint64 StoreFSPrivate::fileIdCounter = 0;
quint64 StoreFSPrivate::dirIdCounter  = ( static_cast<quint64> (1) ) << 63;

/**
 *  \brief Initializes the empty StoreFS object.
 *
 *  \arg \c storePath Path to the store directory.
 */
StoreFS::StoreFS(QString storePath)
{
    _p.reset(new StoreFSPrivate);
    _p->root     = std::make_shared<StoreFSDir> ();
    _p->root->id = _p->dirIdCounter++;

    _p->idPathMap[_p->root->id]   = _p->root->path;
    _p->pathIdMap[_p->root->name] = _p->root->id;
    _p->idDirMap[_p->root->id]    = _p->root;

    _p->storePath = storePath;

    error = Success;
}

/**
 *  \brief Default destructor.
 */
StoreFS::~StoreFS() = default;

/**
 *  \brief Serializes the StoreFS structure.
 *
 *  All \b FILE information is serialized, making it possible
 *  to save the structure to a file. This serialized blob is not
 *  encrypted nor copressed, you must do it yourself.
 *
 *  \return A QByteArray containing the serialized structure.
 *
 *  \see StoreFS#load
 *  \see StoreFS
 */
QByteArray StoreFS::serialize() const
{
    QByteArray  data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream.setVersion(QDataStream::Qt_5_6);

    stream << _p->version;

    auto keys = _p->idFileMap.keys();

    for ( auto key : keys ) {
        StoreFSFilePtr file = _p->idFileMap[key];
        stream << file->path
               << file->size
               << file->metadata
               << file->key
               << file->iv
               << file->salt
               << file->digest
               << file->cryptoParts
               << static_cast<quint8> (file->params.digest)
               << static_cast<quint8> (file->params.encryption)
               << static_cast<quint8> (file->params.keyDerivationFunction)
               << static_cast<quint8> (file->params.keyDerivationHash)
               << file->params.keyDerivationCost;
    }

    return data;
}

/**
 *  \brief Loads the serialized StoreFS struct back into memory.
 *
 *  It will delete the current structure. All file information is
 *  loaded back into memory in a StoreFSDir/StoreFSFile structure.
 *
 *  \arg \c data The serialized data to load.
 *
 *  \see StoreFS#serialize
 *  \see StoreFS
 */
void StoreFS::load(const QByteArray &data)
{
    error = Success;

    _p->fileIdCounter = 0;
    _p->dirIdCounter  = ( static_cast<quint64> (1) ) << 63;

    _p->root     = std::make_shared<StoreFSDir> ();
    _p->root->id = _p->dirIdCounter++;

    _p->idPathMap.clear();
    _p->pathIdMap.clear();
    _p->idDirMap.clear();
    _p->idFileMap.clear();

    _p->idPathMap[_p->root->id]   = _p->root->path;
    _p->pathIdMap[_p->root->name] = _p->root->id;
    _p->idDirMap[_p->root->id]    = _p->root;

    QDataStream stream(data);

    stream.setVersion(QDataStream::Qt_5_6);

    stream >> _p->version;

    while ( !stream.atEnd() ) {
        quint8 digest, encryption, keyDerivationFunction, keyDerivationHash;

        StoreFSFilePtr file(new StoreFSFile);
        file->id = _p->fileIdCounter++;

        stream >> file->path
        >> file->size
        >> file->metadata
        >> file->key
        >> file->iv
        >> file->salt
        >> file->digest
        >> file->cryptoParts
        >> digest
        >> encryption
        >> keyDerivationFunction
        >> keyDerivationHash
        >> file->params.keyDerivationCost;

        file->params.digest                = static_cast<DigestType> (digest);
        file->params.encryption            = static_cast<EncType> (encryption);
        file->params.keyDerivationFunction = static_cast<KeyDerivationFunction> (keyDerivationFunction);
        file->params.keyDerivationHash     = static_cast<KeyDerivationHash> (keyDerivationHash);

        _p->pathIdMap[file->path] = file->id;
        _p->idPathMap[file->id]   = file->path;
        _p->idFileMap[file->id]   = file;

        QStringList list = file->path.split("/");
        file->name = list.last();
        list.removeLast();
        file->parent = makePath( list.join("/") );
    }
}

/**
 *  \brief Returns a pointer to the StoreFSDir structure that represents \c path, if it exists.
 *
 *  \arg \c path The path of the directory.
 *
 *  \return The corresponding StoreFSDirPtr or nullptr if it does not exist.
 */
StoreFSDirPtr StoreFS::dir(QString path) const
{
    if ( (path == "/") || path.isEmpty() ) {
        return _p->root;
    }

    if ( path.endsWith("/") ) {
        path.remove( QRegularExpression("[/]+$") );
    }

    if ( _p->pathIdMap.contains(path) && _p->idDirMap.contains(_p->pathIdMap[path]) ) {
        return _p->idDirMap[_p->pathIdMap[path]];
    }

    return nullptr;
}

/**
 *  \brief Returns a pointer to the StoreFSDir structure that represents \c id, if it exists.
 *
 *  \arg \c id The id of the directory.
 *
 *  \return The corresponding StoreFSDirPtr or nullptr if it does not exist.
 */
StoreFSDirPtr StoreFS::dir(const quint64 id) const
{
    if ( !_p->idDirMap.contains(id) ) {
        return nullptr;
    }

    return _p->idDirMap[id];
}

/**
 *  \brief Returns a pointer to the StoreFSFile structure that represents \c path, if it exists.
 *
 *  \arg \c path The path of the file.
 *
 *  \return The corresponding StoreFSFilePtr or nullptr if it does not exist.
 */
StoreFSFilePtr StoreFS::file(QString path) const
{
    StoreFSFilePtr file;

    if ( _p->pathIdMap.contains(path) && _p->idFileMap.contains(_p->pathIdMap[path]) ) {
        file = _p->idFileMap[_p->pathIdMap[path]];
    }

    return file;
}

/**
 *  \brief Returns a pointer to the StoreFSFile structure that represents \c id, if it exists.
 *
 *  \arg \c path The id of the file.
 *
 *  \return The corresponding StoreFSFilePtr or nullptr if it does not exist.
 */
StoreFSFilePtr StoreFS::file(const quint64 id) const
{
    if ( !_p->idFileMap.contains(id) ) {
        return nullptr;
    }

    return _p->idFileMap[id];
}

QString StoreFS::path(quint64 id)
{
    error = Success;

    if ( _p->idPathMap.contains(id) ) {
        return _p->idPathMap[id];
    }

    error = NoSuchFile;
    return "";
}

/**
 *  \brief Returns a list of paths of all directories in the Store.
 *
 *  \return The list of paths.
 */
QStringList StoreFS::allDirs() const
{
    QStringList paths;
    auto        ids = _p->idDirMap.keys();

    for ( auto id : ids ) {
        paths << _p->idPathMap[id];
    }

    paths.removeAll("");
    paths << "/";

    return paths;
}

/**
 *  \brief Returns a list of paths of all entries in the Store.
 *
 *  \return The list of paths.
 */
QStringList StoreFS::allEntries() const
{
    QStringList paths = _p->pathIdMap.keys();

    paths.removeAll("");
    paths << "/";

    return paths;
}

/**
 *  \brief Returns a list of paths of all files in the Store.
 *
 *  \return The list of paths.
 */
QStringList StoreFS::allFiles() const
{
    QStringList paths;
    auto        ids = _p->idFileMap.keys();

    for ( auto id : ids ) {
        paths << _p->idPathMap[id];
    }

    return paths;
}

/**
 *  \brief Returns the subdirectories of \c path.
 *
 *  \arg \c path The path of the directory from which you want the subdirs.
 *
 *  \returns A list of StoreFSDirPtr of the subdirs.
 */
QList<StoreFSDirPtr> StoreFS::subdirs(QString path)
{
    error = Success;
    auto d = dir(path);

    if ( d == nullptr ) {
        error = NoSuchFile;
        return QList<StoreFSDirPtr> ();
    }

    return d->subdirs;
}

/**
 *  \brief Returns the files on \c path.
 *
 *  \arg \c path The path of the directory from which you want the files.
 *
 *  \returns A list of StoreFSFilePtr of the files.
 */
QList<StoreFSFilePtr> StoreFS::subfiles(QString path)
{
    error = Success;
    auto d = dir(path);

    if ( d == nullptr ) {
        error = NoSuchFile;
        return QList<StoreFSFilePtr> ();
    }

    return d->files;
}

/**
 *  \brief Returns a list of IDs of all entries starting with \c s.
 *
 *  \arg \c s The string to be matched.
 *
 *  \return A list of IDs of all entries starting with \c s.
 */
QList<quint64> StoreFS::entryBeginsWith(const QString s) const
{
    QList<quint64> ids;

    for ( auto key : _p->pathIdMap.keys() ) {
        if ( key.startsWith(s) ) {
            ids << _p->pathIdMap[key];
        }
    }

    return ids;
}

/**
 *  \brief Returns a list of IDs of all entries ending with \c s.
 *
 *  \arg \c s The string to be matched.
 *
 *  \return A list of IDs of all entries ending with \c s.
 */
QList<quint64> StoreFS::entryEndsWith(const QString s) const
{
    QList<quint64> ids;

    for ( auto key : _p->pathIdMap.keys() ) {
        if ( key.endsWith(s) ) {
            ids << _p->pathIdMap[key];
        }
    }

    return ids;
}

/**
 *  \brief Returns a list of IDs of all entries containing with \c s.
 *
 *  \arg \c s The string to be matched.
 *
 *  \return A list of IDs of all entries containing with \c s.
 */
QList<quint64> StoreFS::entryContains(const QString s) const
{
    QList<quint64> ids;

    for ( auto key : _p->pathIdMap.keys() ) {
        if ( key.contains(s) ) {
            ids << _p->pathIdMap[key];
        }
    }

    return ids;
}

/**
 *  \brief Returns a list of IDs of all entries matching the regex \c s.
 *
 *  \arg \c s The string to be matched.
 *
 *  \return A list of IDs of all entries matching the regex \c s.
 */
QList<quint64> StoreFS::entryMatchRegExp(const QString s) const
{
    QList<quint64>     ids;
    QRegularExpression r(s);

    for ( auto key : _p->pathIdMap.keys() ) {
        if ( r.match(key).hasMatch() ) {
            ids << _p->pathIdMap[key];
        }
    }

    return ids;
}

/**
 *  \brief Adds a directory named \c name to the folder \c parent.
 *
 *  Adding the directory directly to the list in the object makes no effect.
 *  All bookkeeping is done in this class.
 *
 *  \arg \c name Name of the new directory.
 *  \arg \c parent Parent directory.
 *
 *  \see StoreFS#makePath
 */
StoreFSDirPtr StoreFS::addSubdir(QString name, StoreFSDirPtr parent)
{
    error = Success;

    StoreFSDirPtr dir = this->dir(parent->path + "/" + name);

    if ( dir != nullptr ) {
        return dir;
    }

    if ( file(parent->path + "/" + name) != nullptr ) {
        error = FileAlreadyExists;
        return nullptr;
    }

    dir = std::make_shared<StoreFSDir> ();

    dir->id     = _p->dirIdCounter++;
    dir->name   = name;
    dir->path   = parent->path + "/" + name;
    dir->parent = parent;

    parent->subdirs << dir;

    _p->idPathMap[dir->id]   = dir->path;
    _p->pathIdMap[dir->path] = dir->id;
    _p->idDirMap[dir->id]    = dir;

    return dir;
}

/**
 *  \brief Makes a path if it doesnt exist, like `mkdir -p path`.
 *
 *  \arg \c path Path to be made. If it already exists, a pointer to the existing one is returned instead.
 *
 *  \return A pointer to the new folder.
 */
StoreFSDirPtr StoreFS::makePath(QString path)
{
    error = Success;

    if ( (path == "/") || path.isEmpty() ) {
        return _p->root;
    }

    QStringList   parts  = path.split("/");
    StoreFSDirPtr folder = _p->root;

    parts.removeFirst();

    for ( QString part : parts ) {
        for ( StoreFSDirPtr dir:folder->subdirs ) {
            if ( dir->name == part ) {
                folder = dir;
            }
        }

        if ( folder->name != part ) {
            folder = addSubdir(part, folder);
            if ( error == FileAlreadyExists ) {
                return nullptr;
            }
        }
    }

    return folder;
}

/**
 * \brief Removes a directory tree.
 *
 * \arg \c path to the folder to be removed.
 */
void StoreFS::removeDir(const QString path)
{
    error = Success;

    StoreFSDirPtr dir = this->dir(path);

    if ( dir == nullptr ) {
        return;
    }

    QList<quint64> ids = entryBeginsWith(dir->path + "/");
    qSort(ids);
    for ( quint64 id : ids ) {
        if ( id > ( ( static_cast<quint64> (1) ) << 63 ) ) {
            dir = this->dir(id);
            _p->idPathMap.remove(id);
            _p->idDirMap.remove(id);
            _p->pathIdMap.remove(dir->path);
            dir->parent->subdirs.removeOne(dir);
        } else {
            removeFile(_p->idPathMap[id]);
        }
    }

    dir = this->dir(path);
    if ( !dir->path.isEmpty() && (dir->parent != nullptr) ) {
        _p->idPathMap.remove(dir->id);
        _p->idDirMap.remove(dir->id);
        _p->pathIdMap.remove(dir->path);
        dir->parent->subdirs.removeOne(dir);
    }
}

/**
 *  \brief Moves a directory.
 *
 *  \arg \c oldPath Actual path of the folder.
 *  \arg \c newPath New path of the folder.
 */
void StoreFS::moveDir(const QString oldPath, const QString newPath)
{
    error = Success;

    StoreFSDirPtr d = dir(oldPath);
    if ( d == nullptr ) {
        return;
    }

    QList<quint64> ids = entryBeginsWith(d->path + "/");
    for ( quint64 id : ids ) {
        if ( id < ( ( static_cast<quint64> (1) ) << 63 ) ) {
            QString oldFilePath = _p->idPathMap[id];
            QString newFilePath = oldFilePath;
            newFilePath.replace(oldPath, newPath);
            moveFile(oldFilePath, newFilePath);
        }
    }

    removeDir(oldPath);
}

/**
 *  \brief Adds a file to the store.
 *
 *  Adds the \c data to the store as the file \c path. The file is encrypted
 *  before being added using a random key, iv and salt. It's divided into
 *  smaller parts of 50MB (50 * 2^20 bytes). Pay attention to the limit of
 *  2GB imposed by QByteArray.
 *
 *  \arg \c path Path where the file will be stored.
 *  \arg \c data The contents of the file.
 *
 *  \return A pointer to the new added file
 *
 *  \see StoreFS#error
 *  \see StoreFS#decryptFile
 *  \see Crypto
 *  \see Store
 */
StoreFSFilePtr StoreFS::addFile(QString path, QByteArray data)
{
    error = Success;

    if ( file(path) != nullptr ) {
        error = FileAlreadyExists;
        return nullptr;
    }

    QStringList pathList = path.split("/");
    QString     name     = pathList.last();

    pathList.removeLast();

    QString        parentPath = pathList.join("/");
    StoreFSDirPtr  parent     = makePath(parentPath);
    StoreFSFilePtr file(new StoreFSFile);

    std::string key  = Crypto::generateRandom(32);
    std::string iv   = Crypto::generateRandom(16);
    std::string salt = Crypto::generateRandom(16);

    Crypto c(key, iv);

    if ( c.error != Crypto::Success ) {
        error = CantCreateCryptoObject;
        return file;
    }

    parent->files << file;

    file->id     = _p->fileIdCounter++;
    file->name   = name;
    file->path   = path;
    file->parent = parent;
    file->size   = static_cast<quint64> ( data.size() );
    file->key    = QByteArray::fromStdString(key);
    file->iv     = QByteArray::fromStdString(iv);
    file->salt   = QByteArray::fromStdString(salt);

    _p->pathIdMap[file->path] = file->id;
    _p->idPathMap[file->id]   = file->path;
    _p->idFileMap[file->id]   = file;

    std::string wholeFileDigest;

    for ( unsigned int i = 0; i < floor(data.size() / MAX_PART_SIZE) + 1; i++ ) {
        std::string part       = data.mid(static_cast<int> (i * MAX_PART_SIZE), MAX_PART_SIZE).toStdString();
        std::string partDigest = Crypto::digest(part);
        std::string name       = partDigest + salt;

        name             = Crypto::stringToHex(Crypto::digest(name), "");
        wholeFileDigest += partDigest;
        wholeFileDigest  = Crypto::digest(wholeFileDigest);
        part             = c.encrypt(part);

        QFile partFile( _p->storePath + "/" + QString::fromStdString(name) );
        if ( partFile.open(QFile::WriteOnly) ) {
            file->cryptoParts[i] = QString::fromStdString(name);

            if ( partFile.write( QByteArray::fromStdString(part) ) == -1 ) {
                error = CantWriteToFile;
                break;
            }
        } else {
            error = CantOpenFile;
            break;
        }
    }

    file->digest = QByteArray::fromStdString(wholeFileDigest);

    return file;
}

/**
 *  \brief Adds a file to the store.
 *
 *  Adds the file \c filePath to the store as the file \c path. The file
 *  is encrypted before being added using a random key, iv and salt. It's
 *  divided into smaller parts of 50MB (50 * 2^20 bytes).
 *
 *  \arg \c filePath The path of the file to be added.
 *  \arg \c path Path where the file will be stored.
 *
 *  \return A pointer to the new added file
 *
 *  \see StoreFS#error
 *  \see StoreFS#decryptFile
 *  \see Crypto
 *  \see Store
 */
StoreFSFilePtr StoreFS::addFile(QString filePath, QString path)
{
    error = Success;

    if ( file(path) != nullptr ) {
        error = FileAlreadyExists;
        return nullptr;
    }

    QStringList pathList = path.split("/");
    QString     name     = pathList.last();

    pathList.removeLast();

    QString        parentPath = pathList.join("/");
    StoreFSDirPtr  parent     = makePath(parentPath);
    StoreFSFilePtr file(new StoreFSFile);

    std::string key  = Crypto::generateRandom(32);
    std::string iv   = Crypto::generateRandom(16);
    std::string salt = Crypto::generateRandom(16);

    Crypto c(key, iv);

    if ( c.error != Crypto::Success ) {
        error = CantCreateCryptoObject;
        return file;
    }

    QFile fileIn(filePath);
    if ( !fileIn.open(QIODevice::ReadOnly) ) {
        error = CantOpenFile;
        return file;
    }

    parent->files << file;

    file->id     = _p->fileIdCounter++;
    file->name   = name;
    file->path   = path;
    file->parent = parent;
    file->size   = static_cast<quint64> ( fileIn.size() );
    file->key    = QByteArray::fromStdString(key);
    file->iv     = QByteArray::fromStdString(iv);
    file->salt   = QByteArray::fromStdString(salt);

    _p->pathIdMap[file->path] = file->id;
    _p->idPathMap[file->id]   = file->path;
    _p->idFileMap[file->id]   = file;

    std::string wholeFileDigest;

    for ( unsigned int i = 0; i < floor(file->size / MAX_PART_SIZE) + 1; i++ ) {
        std::string part       = fileIn.read(MAX_PART_SIZE).toStdString();
        std::string partDigest = Crypto::digest(part);
        std::string digest     = partDigest + salt;
        std::string name       = Crypto::stringToHex(Crypto::digest(digest), "");

        wholeFileDigest += partDigest;
        wholeFileDigest  = Crypto::digest(wholeFileDigest);
        part             = c.encrypt(part);

        QFile partFile( _p->storePath + "/" + QString::fromStdString(name) );
        if ( partFile.open(QFile::WriteOnly) ) {
            file->cryptoParts[i] = QString::fromStdString(name);

            if ( partFile.write( QByteArray::fromStdString(part) ) == -1 ) {
                error = CantWriteToFile;
                break;
            }
        } else {
            error = CantOpenFile;
            break;
        }
    }

    file->digest = QByteArray::fromStdString(wholeFileDigest);

    return file;
}

/**
 *  \brief Decrypts the file at \c path.
 *
 *  Decrypts the file at \c path and returns it's content.
 *  Pay attention to the limit of 2GB imposed by QByteArray.
 *
 *  \arg \c path Path of the file to be decrypted.
 *
 *  \return A QByteArray containing the unencrypted contents of the file.
 *
 *  \see StoreFS#error
 *  \see StoreFS#addFile
 *  \see Store
 *  \see Crypto
 */
QByteArray StoreFS::decryptFile(QString path)
{
    error = Success;

    QByteArray data;

    StoreFSFilePtr file = this->file(path);

    if ( file == nullptr ) {
        error = NoSuchFile;
        return data;
    }

    // QByteArray is limited to 2GB
    // I'm using power of 10 because I have
    // no idea what Qt uses.
    if ( file->size > 2000000000 ) {
        error = FileTooLarge;
        return data;
    }

    Crypto c( file->key.toStdString(), file->iv.toStdString() );

    if ( c.error != Crypto::Success ) {
        error = CantCreateCryptoObject;
        return data;
    }

    std::string wholeFileDigest;

    for ( unsigned int i = 0; i < static_cast<unsigned int> ( file->cryptoParts.size() ); i++ ) {
        QFile part(_p->storePath + "/" + file->cryptoParts[i]);
        if ( !part.open(QIODevice::ReadOnly) ) {
            error = CantOpenFile;
            return QByteArray();
        }

        std::string partData = part.readAll().toStdString();
        partData = c.decrypt(partData);

        std::string partDigest = Crypto::digest(partData);
        std::string digest     = partDigest + file->salt.toStdString();

        digest           = Crypto::digest(digest, file->params);
        wholeFileDigest += partDigest;
        wholeFileDigest  = Crypto::digest(wholeFileDigest);

        if ( Crypto::stringToHex(digest, "") != file->cryptoParts[i].toStdString() ) {
            error = PartCorrupted;
            return QByteArray();
        }

        data += QByteArray::fromStdString(partData);
    }

    if ( file->digest != QByteArray::fromStdString(wholeFileDigest) ) {
        error = WrongCheckSum;
        return QByteArray();
    }

    return data;
}

/**
 *  \brief Decrypts the file at \c path.
 *
 *  Decrypts the file at \c storePath and into the file \c path.
 *
 *  \arg \c storePath Path of the file to be decrypted.
 *  \arg \c path Where in the disk the file should be saved.
 *
 *  \see StoreFS#error
 *  \see StoreFS#addFile
 *  \see Store
 *  \see Crypto
 */
void StoreFS::decryptFile(QString storePath, QString path)
{
    error = Success;

    StoreFSFilePtr file = this->file(storePath);

    if ( file == nullptr ) {
        error = NoSuchFile;
        return;
    }

    // QByteArray is limited to 2GB
    // I'm using power of 10 because I have
    // no idea what Qt uses.
    if ( file->size > 2000000000 ) {
        error = FileTooLarge;
        return;
    }

    Crypto c( file->key.toStdString(), file->iv.toStdString() );

    if ( c.error != Crypto::Success ) {
        error = CantCreateCryptoObject;
        return;
    }

    QFile outFile(path);
    if ( !outFile.open(QIODevice::WriteOnly) ) {
        error = CantOpenFile;
        return;
    }

    std::string wholeFileDigest;

    for ( unsigned int i = 0; i < static_cast<unsigned int> ( file->cryptoParts.size() ); i++ ) {
        QFile part(_p->storePath + "/" + file->cryptoParts[i]);
        if ( !part.open(QIODevice::ReadOnly) ) {
            error = CantOpenFile;
            return;
        }

        std::string partData = part.readAll().toStdString();
        partData = c.decrypt(partData);

        std::string partDigest = Crypto::digest(partData);
        std::string digest     = partDigest + file->salt.toStdString();

        digest           = Crypto::digest(digest, file->params);
        wholeFileDigest += partDigest;
        wholeFileDigest  = Crypto::digest(wholeFileDigest);

        if ( Crypto::stringToHex(digest, "") != file->cryptoParts[i].toStdString() ) {
            error = PartCorrupted;
            return;
        }

        if ( outFile.write( QByteArray::fromStdString(partData) ) == -1 ) {
            error = CantWriteToFile;
            return;
        }
    }

    if ( file->digest != QByteArray::fromStdString(wholeFileDigest) ) {
        error = WrongCheckSum;
    }
}

/**
 *  \brief Moves a file inside the store.
 *
 *  \arg \c oldPath Path of the file to be moved.
 *  \arg \c newPath Path where the file will be moved.
 *
 *  \see StoreFS#error
 */
void StoreFS::moveFile(const QString oldPath, const QString newPath)
{
    error = Success;

    StoreFSFilePtr file = this->file(oldPath);

    if ( file == nullptr ) {
        return;
    }

    if ( this->file(newPath) != nullptr ) {
        error = FileAlreadyExists;
        return;
    }

    QString newName       = newPath.split("/").last();
    QString newParentPath = newPath.mid( 0, newPath.lastIndexOf("/") );

    StoreFSDirPtr newParent = this->makePath(newParentPath);

    file->name = newName;
    file->path = newPath;
    file->parent->files.removeOne(file);
    file->parent = newParent;

    _p->pathIdMap.remove(oldPath);
    _p->pathIdMap[newPath]  = file->id;
    _p->idPathMap[file->id] = newPath;

    newParent->files << file;
}

/**
 *  \brief Removes a file from the store.
 *
 *  \arg \c path Path of the file to be removed.
 *
 *  \see StoreFS#error
 */
void StoreFS::removeFile(const QString path)
{
    error = Success;

    StoreFSFilePtr file = this->file(path);

    if ( file == nullptr ) {
        return;
    }

    file->parent->files.removeOne(file);

    _p->idPathMap.remove(file->id);
    _p->idFileMap.remove(file->id);
    _p->pathIdMap.remove(file->path);

    for ( QString partName : file->cryptoParts.values() ) {
        QFile::remove(_p->storePath + "/" + partName);
    }
}

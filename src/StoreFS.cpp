#include "StoreFS.h"

#include <math.h>

#include <QDataStream>
#include <QFile>

#define MAX_PART_SIZE 52428800

quint64 StoreFS::fileIdCounter = 0;
quint64 StoreFS::dirIdCounter  = ( (quint64) 1 ) << 63;

StoreFS::StoreFS(QString storePath)
{
    root.reset(new StoreFSDir);
    root->id = dirIdCounter++;

    idPathMap[root->id]   = root->path;
    pathIdMap[root->name] = root->id;
    idDirMap[root->id]    = root;

    this->storePath = storePath;

    error = Success;
}

QByteArray StoreFS::serialize() const
{
    QByteArray  data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream.setVersion(QDataStream::Qt_5_6);

    stream << version;

    auto keys = idFileMap.keys();

    for ( quint32 key : keys ) {
        StoreFSFilePtr file = idFileMap[key];
        stream << file->path
               << file->size
               << file->metadata
               << file->key
               << file->iv
               << file->salt
               << file->digest
               << file->cryptoParts
               << (quint8) file->params.digest
               << (quint8) file->params.encryption
               << (quint8) file->params.keyDerivationFunction
               << (quint8) file->params.keyDerivationHash
               << file->params.keyDerivationCost;
    }

    return data;
}

void StoreFS::load(const QByteArray &data)
{
    fileIdCounter = 0;
    dirIdCounter  = ( (quint64) 1 ) << 63;

    root.reset(new StoreFSDir);
    root->id = dirIdCounter++;

    idPathMap.clear();
    pathIdMap.clear();
    idDirMap.clear();
    idFileMap.clear();

    idPathMap[root->id]   = root->path;
    pathIdMap[root->name] = root->id;
    idDirMap[root->id]    = root;

    QDataStream stream(data);

    stream.setVersion(QDataStream::Qt_5_6);

    stream >> version;

    while ( !stream.atEnd() ) {
        quint8 digest, encryption, keyDerivationFunction, keyDerivationHash;

        StoreFSFilePtr file(new StoreFSFile);
        file->id = fileIdCounter++;

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

        file->params.digest                = (DigestType) digest;
        file->params.encryption            = (EncType) encryption;
        file->params.keyDerivationFunction = (KeyDerivationFunction) keyDerivationFunction;
        file->params.keyDerivationHash     = (KeyDerivationHash) keyDerivationHash;

        pathIdMap[file->path] = file->id;
        idPathMap[file->id]   = file->path;
        idFileMap[file->id]   = file;

        QStringList list = file->path.split("/");
        file->name = list.last();
        list.removeLast();
        file->parent = makePath( list.join("/") );
    }
}

StoreFSDirPtr StoreFS::dir(QString path) const
{
    if ( (path == "/") || path.isEmpty() ) {
        return root;
    }

    StoreFSDirPtr dir;

    if ( pathIdMap.contains(path) ) {
        dir = idDirMap[pathIdMap[path]];
    }

    return dir;
}

StoreFSFilePtr StoreFS::file(QString path) const
{
    StoreFSFilePtr file;

    if ( pathIdMap.contains(path) ) {
        file = idFileMap[pathIdMap[path]];
    }

    return file;
}

QList<StoreFSDirPtr> StoreFS::subdirs(QString path) const
{
    QList<StoreFSDirPtr> list;
    auto                 keys = pathIdMap.keys();

    for ( QString key : keys ) {
        if ( key.startsWith(path) && (idDirMap[pathIdMap[key]]->id >= root->id) ) {
            list << idDirMap[pathIdMap[key]];
        }
    }

    return list;
}

QList<StoreFSFilePtr> StoreFS::subfiles(QString path) const
{
    QList<StoreFSFilePtr> list;
    auto                  keys = pathIdMap.keys();

    for ( QString key : keys ) {
        if ( key.startsWith(path) && (idDirMap[pathIdMap[key]]->id < root->id) ) {
            list << idFileMap[pathIdMap[key]];
        }
    }

    return list;
}

StoreFSDirPtr StoreFS::makePath(QString path)
{
    if ( (path == "/") || path.isEmpty() ) {
        return root;
    }

    QStringList   parts  = path.split("/");
    StoreFSDirPtr folder = root;

    parts.removeFirst();

    for ( QString part : parts ) {
        for ( StoreFSDirPtr dir:folder->subdirs ) {
            if ( dir->name == part ) {
                folder = dir;
            }
        }

        if ( folder->name != part ) {
            folder = addSubdir(part, folder);
        }
    }

    return folder;
}

StoreFSDirPtr StoreFS::addSubdir(QString name, StoreFSDirPtr parent)
{
    StoreFSDirPtr dir(new StoreFSDir);

    dir->id     = dirIdCounter++;
    dir->name   = name;
    dir->path   = parent->path + "/" + name;
    dir->parent = parent;

    parent->subdirs << dir;

    idPathMap[dir->id]   = dir->path;
    pathIdMap[dir->name] = dir->id;
    idDirMap[dir->id]    = dir;

    return dir;
}

StoreFSFilePtr StoreFS::addFile(QString path, QByteArray data)
{
    error = Success;

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

    file->id     = fileIdCounter++;
    file->name   = name;
    file->path   = path;
    file->parent = parent;
    file->size   = data.size();
    file->key    = QByteArray::fromStdString(key);
    file->iv     = QByteArray::fromStdString(iv);
    file->salt   = QByteArray::fromStdString(salt);

    pathIdMap[file->path] = file->id;
    idPathMap[file->id]   = file->path;
    idFileMap[file->id]   = file;

    std::string wholeFileDigest;

    for ( int i = 0; i < floor(data.size() / MAX_PART_SIZE) + 1; i++ ) {
        std::string part       = data.mid(i * MAX_PART_SIZE, MAX_PART_SIZE).toStdString();
        std::string partDigest = Crypto::digest(part);
        std::string name       = partDigest + path.toStdString() + salt;

        name             = Crypto::stringToHex(Crypto::digest(name), "");
        wholeFileDigest += partDigest;
        wholeFileDigest  = Crypto::digest(wholeFileDigest);
        part             = c.encrypt(part);

        QFile partFile( storePath + "/" + QString::fromStdString(name) );
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

StoreFSFilePtr StoreFS::addFile(QString filePath, QString path)
{
    error = Success;

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

    file->id     = fileIdCounter++;
    file->name   = name;
    file->path   = path;
    file->parent = parent;
    file->size   = fileIn.size();
    file->key    = QByteArray::fromStdString(key);
    file->iv     = QByteArray::fromStdString(iv);
    file->salt   = QByteArray::fromStdString(salt);

    pathIdMap[file->path] = file->id;
    idPathMap[file->id]   = file->path;
    idFileMap[file->id]   = file;

    std::string wholeFileDigest;

    for ( int i = 0; i < floor(file->size / MAX_PART_SIZE) + 1; i++ ) {
        std::string part       = fileIn.read(MAX_PART_SIZE).toStdString();
        std::string partDigest = Crypto::digest(part);
        std::string digest     = partDigest + path.toStdString() + salt;
        std::string name       = Crypto::stringToHex(Crypto::digest(digest), "");

        wholeFileDigest += partDigest;
        wholeFileDigest  = Crypto::digest(wholeFileDigest);
        part             = c.encrypt(part);

        QFile partFile( storePath + "/" + QString::fromStdString(name) );
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

    for ( int i = 0; i < file->cryptoParts.size(); i++ ) {
        QFile part(this->storePath + "/" + file->cryptoParts[i]);
        if ( !part.open(QIODevice::ReadOnly) ) {
            error = CantOpenFile;
            return QByteArray();
        }

        std::string partData = part.readAll().toStdString();
        partData = c.decrypt(partData);

        std::string partDigest = Crypto::digest(partData);
        std::string digest     = partDigest + path.toStdString() + file->salt.toStdString();

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

    for ( int i = 0; i < file->cryptoParts.size(); i++ ) {
        QFile part(this->storePath + "/" + file->cryptoParts[i]);
        if ( !part.open(QIODevice::ReadOnly) ) {
            error = CantOpenFile;
            return;
        }

        std::string partData = part.readAll().toStdString();
        partData = c.decrypt(partData);

        std::string partDigest = Crypto::digest(partData);
        std::string digest     = partDigest + storePath.toStdString() + file->salt.toStdString();

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

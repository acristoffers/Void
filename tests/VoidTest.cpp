#include "VoidTest.h"

#include "Crypto.h"
#include "Store.h"
#include "StoreFile.h"
#include "StoreFS.h"

/*!
 *  \class VoidTest
 *  \brief Unity tests for the library part of the project
 */

/**
 *  \brief Tests that a Crypto object can be created and a key can be derived.
 */
void VoidTest::cryptoCreateObjectWithPassword()
{
    std::string password = "passw0rd";
    std::string salt     = "0123456789ABCDEF";
    std::string iv       = "FEDCBA9876543210";
    Crypto      c(password, salt, iv);

    QCOMPARE(c.error, Crypto::Success);
}

/**
 *  \brief Tests that a Crypto object can be created using an existing key and iv.
 */
void VoidTest::cryptoCreateObjectWithKey()
{
    std::string key = "0123456789ABCDEF";
    std::string iv  = "FEDCBA9876543210";
    Crypto      c(key, iv);

    QCOMPARE(c.error, Crypto::Success);
}

/**
 *  \brief Tests that Crypto#stringToHex does what it should.
 */
void VoidTest::cryptoToHex()
{
    std::string raw = {
        0xF, 0x0, 0xD, 0xE, 0xA, 0xD
    };

    std::string hex = Crypto::stringToHex(raw);

    QCOMPARE( QString::fromStdString(hex), QString("0F:00:0D:0E:0A:0D") );
}

/**
 *  \brief Tests that Crypto#cryptoToB64 does what it should.
 */
void VoidTest::cryptoToB64()
{
    std::string message = "Hello World";

    message = Crypto::toBase64(message);
    QCOMPARE( QString::fromStdString(message), QString("SGVsbG8gV29ybGQ=") );
}

/**
 *  \brief Tests that Crypto#fromBase64 does what it should.
 */
void VoidTest::cryptoFromB64()
{
    std::string message = "SGVsbG8gV29ybGQ=";

    message = Crypto::fromBase64(message);
    QCOMPARE( QString::fromStdString(message), QString("Hello World") );
}

/**
 *  \brief Tests that Crypto#encrypt encrypts correctly.
 */
void VoidTest::cryptoEncrypt()
{
    std::string key = "0123456789ABCDEF";
    std::string iv  = "FEDCBA9876543210";
    Crypto      c(key, iv);

    std::string message = "Hello World";

    message = c.encrypt(message);
    message = Crypto::stringToHex(message);

    QCOMPARE( QString::fromStdString(message), QString("8C:A9:6D:CC:19:2A:01:08:02:73:04:A5:C5:B7:FE:B0:09:2F:CC:3C:E3:3C:D3:11:C9:69:8C") );
}

/**
 *  \brief Tests that Crypto#decrypt decrypts correctly.
 */
void VoidTest::cryptoDecrypt()
{
    const unsigned char data[] = {
        0x8C, 0xA9, 0x6D, 0xCC, 0x19, 0x2A, 0x01, 0x08,
        0x02, 0x73, 0x04, 0xA5, 0xC5, 0xB7, 0xFE, 0xB0,
        0x09, 0x2F, 0xCC, 0x3C, 0xE3, 0x3C, 0xD3, 0x11,
        0xC9, 0x69, 0x8C
    };

    std::string message( reinterpret_cast<const char *> (data), sizeof(data) );

    std::string key = "0123456789ABCDEF";
    std::string iv  = "FEDCBA9876543210";
    Crypto      c(key, iv);

    message = c.decrypt(message);

    QCOMPARE( QString::fromStdString(message), QString("Hello World") );
}

/**
 *  \brief Tests that StoreFile can create and load the Store.void file.
 */
void VoidTest::storeFileCreateAndLoadStore()
{
    QDir::current().mkdir("void_store");
    StoreFile *sf = new StoreFile("void_store/Store.void");

    QCOMPARE(sf->error, StoreFile::Success);

    sf->setData("data");
    sf->setIV("iv");
    sf->setSalt("salt");

    delete sf;

    sf = new StoreFile("void_store/Store.void");

    QCOMPARE( sf->data(), QByteArray("data") );
    QCOMPARE( sf->IV(),   std::string("iv") );
    QCOMPARE( sf->salt(), std::string("salt") );

    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests that StoreFS#makePath does what is should.
 *
 *  Subtests are:
 *  - Make a path (should succeed).
 *  - Make a path complementing an existing path (should succeed).
 *  - Make a path over a file (should return the error StoreFS#FileAlreadyExists).
 */
void VoidTest::storeFSMakePath()
{
    QDir::current().mkdir("void_store");
    StoreFS sfs("void_store");

    sfs.makePath("/Users/Alan");
    sfs.makePath("/Users/Alan/Documents");

    QCOMPARE( sfs.dir("/Users")->path,                QString("/Users") );
    QCOMPARE( sfs.dir("/Users/Alan")->path,           QString("/Users/Alan") );
    QCOMPARE( sfs.dir("/Users/Alan/Documents")->path, QString("/Users/Alan/Documents") );

    QByteArray ba = "Hello World!";
    sfs.addFile("/file", ba);
    QCOMPARE(sfs.error, StoreFS::Success);
    sfs.makePath("/file");
    QCOMPARE(sfs.error, StoreFS::FileAlreadyExists);
    sfs.makePath("/file/otherFile");
    QCOMPARE(sfs.error, StoreFS::FileAlreadyExists);

    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests StoreFS#addFile(const QString, const QByteArray)
 */
void VoidTest::storeFSAddFile()
{
    QDir::current().mkdir("void_store");
    StoreFS sfs("void_store");

    QByteArray data = "Hello World";

    sfs.addFile("/hello.txt", data);
    StoreFSFilePtr file          = sfs.file("/hello.txt");
    QString        expected_name = "void_store/" + file->cryptoParts.first();

    QCOMPARE(sfs.error,                    StoreFS::Success);
    QCOMPARE(QFile::exists(expected_name), true);

    QFile::remove("void_store/Store.void");
    QFile::remove(expected_name);
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests StoreFS#addFile(const QString, const QString)
 */
void VoidTest::storeFSAddFileFromDisk()
{
    QDir::current().mkdir("void_store");
    StoreFS sfs("void_store");

    QByteArray data = "Hello World";

    data = data.repeated(500000);

    QFile f("void_store/hello.txt");
    f.open(QIODevice::WriteOnly);
    f.write(data);
    f.close();

    sfs.addFile( "void_store/hello.txt", QString("/hello.txt") );

    StoreFSFilePtr file          = sfs.file("/hello.txt");
    QString        expected_name = "void_store/" + file->cryptoParts.first();

    QCOMPARE(sfs.error,                    StoreFS::Success);
    QCOMPARE(QFile::exists(expected_name), true);

    sfs.decryptFile("/hello.txt", "void_store/hello2.txt");

    QFile f2("void_store/hello2.txt");
    f2.open(QIODevice::ReadOnly);
    QByteArray data2 = f2.readAll();
    f2.close();

    QCOMPARE(data, data2);

    QFile::remove("void_store/Store.void");
    QFile::remove("void_store/hello.txt");
    QFile::remove("void_store/hello2.txt");
    QFile::remove(expected_name);
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests StoreFS#removeFile
 */
void VoidTest::storeFSRemoveFile()
{
    QDir::current().mkdir("void_store");
    StoreFS sfs("void_store");

    QByteArray data = "Hello World";

    sfs.addFile("/hello.txt", data);
    QCOMPARE(sfs.error,                    StoreFS::Success);

    StoreFSFilePtr file = sfs.file("/hello.txt");
    QCOMPARE(file != nullptr,              true);

    QString expected_name = "void_store/" + file->cryptoParts.first();
    QCOMPARE(QFile::exists(expected_name), true);

    sfs.removeFile(file->path);

    file = sfs.file("/hello.txt");
    QCOMPARE(file == nullptr,              true);
    QCOMPARE(QFile::exists(expected_name), false);

    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests StoreFS#removeDir
 */
void VoidTest::storeFSRemoveDir()
{
    QDir::current().mkdir("void_store");
    StoreFS sfs("void_store");

    QByteArray data = "Hello World";

    sfs.addFile("/dir/hello.txt", data);
    QCOMPARE(sfs.error, StoreFS::Success);
    sfs.addFile("/dir/subdir/hello.txt", data);
    QCOMPARE(sfs.error, StoreFS::Success);
    sfs.addFile("/dir/hello2.txt", data);
    QCOMPARE(sfs.error, StoreFS::Success);

    QString name1 = sfs.file("/dir/hello.txt")->cryptoParts.first();
    QString name2 = sfs.file("/dir/subdir/hello.txt")->cryptoParts.first();
    QString name3 = sfs.file("/dir/hello2.txt")->cryptoParts.first();

    QCOMPARE(QFile::exists("void_store/" + name1), true);
    QCOMPARE(QFile::exists("void_store/" + name2), true);
    QCOMPARE(QFile::exists("void_store/" + name3), true);

    sfs.removeDir("/dir");

    QCOMPARE(sfs.dir("/dir") == nullptr,                   true);
    QCOMPARE(sfs.dir("/dir/subdir") == nullptr,            true);
    QCOMPARE(sfs.file("/dir/hello.txt") == nullptr,        true);
    QCOMPARE(sfs.file("/dir/subdir/hello.txt") == nullptr, true);
    QCOMPARE(sfs.file("/dir/hello2.txt") == nullptr,       true);
    QCOMPARE(QFile::exists("void_store/" + name1),         false);
    QCOMPARE(QFile::exists("void_store/" + name2),         false);
    QCOMPARE(QFile::exists("void_store/" + name3),         false);

    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests StoreFS#moveFile
 */
void VoidTest::storeFSRenameFile()
{
    QDir::current().mkdir("void_store");
    StoreFS sfs("void_store");

    QByteArray data = "Hello World";

    sfs.addFile("/hello.txt", data);
    QCOMPARE(sfs.error,                    StoreFS::Success);

    StoreFSFilePtr file = sfs.file("/hello.txt");
    QCOMPARE(file != nullptr,              true);

    QString expected_name = "void_store/" + file->cryptoParts.first();
    QCOMPARE(QFile::exists(expected_name), true);

    sfs.moveFile(file->path, "/Users/Alan/hello.txt");

    file = sfs.file("/Users/Alan/hello.txt");
    QCOMPARE(file != nullptr, true);

    file = sfs.file("/hello.txt");
    QCOMPARE(file == nullptr, true);

    QFile::remove("void_store/Store.void");
    QFile::remove(expected_name);
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests StoreFS#moveDir
 */
void VoidTest::storeFSRenameDir()
{
    QDir::current().mkdir("void_store");
    StoreFS sfs("void_store");

    QByteArray data = "Hello World";

    sfs.addFile("/dir/hello.txt", data);
    QCOMPARE(sfs.error, StoreFS::Success);
    sfs.addFile("/dir/subdir/hello.txt", data);
    QCOMPARE(sfs.error, StoreFS::Success);
    sfs.addFile("/dir/hello2.txt", data);
    QCOMPARE(sfs.error, StoreFS::Success);

    sfs.moveDir("/dir", "/folder");

    QCOMPARE(sfs.file("/folder/hello.txt") != nullptr,        true);
    QCOMPARE(sfs.file("/folder/subdir/hello.txt") != nullptr, true);
    QCOMPARE(sfs.file("/folder/hello2.txt") != nullptr,       true);
    QCOMPARE(sfs.file("/dir/hello.txt") == nullptr,           true);
    QCOMPARE(sfs.file("/dir/subdir/hello.txt") == nullptr,    true);
    QCOMPARE(sfs.file("/dir/hello2.txt") == nullptr,          true);

    sfs.removeDir("/");

    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests StoreFS#entryBeginsWith, StoreFS#entryEndsWith, StoreFS#entryContains and StoreFS#entryMatchRegExp.
 */
void VoidTest::storeFSFilters()
{
    QDir::current().mkdir("void_store");
    StoreFS sfs("void_store");

    QByteArray data = "Hello World";

    sfs.addFile("/dir/hello", data);
    QCOMPARE(sfs.error, StoreFS::Success);
    sfs.addFile("/dir/subdir/hello.txt", data);
    QCOMPARE(sfs.error, StoreFS::Success);
    sfs.addFile("/folder/subdir/hello2.txt", data);
    QCOMPARE(sfs.error,  StoreFS::Success);

    QList<quint64> ids = sfs.entryBeginsWith("/dir");
    QCOMPARE(ids.size(), 4);
    for ( quint64 id : ids ) {
        QStringList dpaths = {
            "/dir", "/dir/subdir"
        };

        QStringList fpaths = {
            "/dir/hello", "/dir/subdir/hello.txt"
        };

        if ( id > ( ( static_cast<quint64> (1) ) << 63 ) ) {
            QCOMPARE(dpaths.contains(sfs.dir(id)->path), true);
        } else {
            QCOMPARE(fpaths.contains(sfs.file(id)->path), true);
        }
    }

    ids = sfs.entryEndsWith("txt");
    QCOMPARE(ids.size(), 2);
    for ( quint64 id : ids ) {
        QStringList fpaths = {
            "/folder/subdir/hello2.txt", "/dir/subdir/hello.txt"
        };

        if ( id > ( ( static_cast<quint64> (1) ) << 63 ) ) {
            QCOMPARE(false, true);
        } else {
            QCOMPARE(fpaths.contains(sfs.file(id)->path), true);
        }
    }

    ids = sfs.entryContains("subdir");
    QCOMPARE(ids.size(), 4);
    for ( quint64 id : ids ) {
        QStringList dpaths = {
            "/dir/subdir", "/folder/subdir"
        };

        QStringList fpaths = {
            "/folder/subdir/hello2.txt", "/dir/subdir/hello.txt"
        };

        if ( id > ( ( static_cast<quint64> (1) ) << 63 ) ) {
            QCOMPARE(dpaths.contains(sfs.dir(id)->path), true);
        } else {
            QCOMPARE(fpaths.contains(sfs.file(id)->path), true);
        }
    }

    ids = sfs.entryMatchRegExp(".*hello[0-9]?.txt");
    QCOMPARE(ids.size(), 2);
    for ( quint64 id : ids ) {
        QStringList fpaths = {
            "/folder/subdir/hello2.txt", "/dir/subdir/hello.txt"
        };

        if ( id > ( ( static_cast<quint64> (1) ) << 63 ) ) {
            QCOMPARE(false, true);
        } else {
            QCOMPARE(fpaths.contains(sfs.file(id)->path), true);
        }
    }

    sfs.removeDir("/");

    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests StoreFS#allFiles, StoreFS#allDirs and StoreFS#allEntries
 */
void VoidTest::storeFSFetchAll()
{
    QDir::current().mkdir("void_store");
    StoreFS sfs("void_store");

    QByteArray data = "Hello World";

    sfs.addFile("/dir/hello", data);
    QCOMPARE(sfs.error, StoreFS::Success);
    sfs.addFile("/dir/subdir/hello.txt", data);
    QCOMPARE(sfs.error, StoreFS::Success);
    sfs.addFile("/folder/subdir/hello2.txt", data);
    QCOMPARE(sfs.error,                                   StoreFS::Success);

    QStringList paths = sfs.allDirs();
    QCOMPARE(paths.size(),                                5);
    QCOMPARE(paths.contains("/"),                         true);
    QCOMPARE(paths.contains("/dir"),                      true);
    QCOMPARE(paths.contains("/dir/subdir"),               true);
    QCOMPARE(paths.contains("/folder"),                   true);
    QCOMPARE(paths.contains("/folder/subdir"),            true);

    paths = sfs.allFiles();
    QCOMPARE(paths.size(),                                3);
    QCOMPARE(paths.contains("/dir/hello"),                true);
    QCOMPARE(paths.contains("/dir/subdir/hello.txt"),     true);
    QCOMPARE(paths.contains("/folder/subdir/hello2.txt"), true);

    paths = sfs.allEntries();
    QCOMPARE(paths.size(),                                8);
    QCOMPARE(paths.contains("/"),                         true);
    QCOMPARE(paths.contains("/dir"),                      true);
    QCOMPARE(paths.contains("/dir/subdir"),               true);
    QCOMPARE(paths.contains("/folder"),                   true);
    QCOMPARE(paths.contains("/folder/subdir"),            true);
    QCOMPARE(paths.contains("/dir/hello"),                true);
    QCOMPARE(paths.contains("/dir/subdir/hello.txt"),     true);
    QCOMPARE(paths.contains("/folder/subdir/hello2.txt"), true);

    sfs.removeDir("/");

    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests Store#Store
 */
void VoidTest::storeCreate()
{
    QString path     = QDir::current().filePath("void_store");
    QString password = "pswd";

    Store store(path, password, false);

    QCOMPARE(store.error,                            Store::DoesntExistAndCreationIsNotPermitted);

    Store store2(path, password, true);
    QCOMPARE(store2.error,                           Store::Success);
    QCOMPARE(QFile::exists("void_store/Store.void"), true);

    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests Store#addFile(const QString, const QByteArray)
 */
void VoidTest::storeAddFile()
{
    QString path     = QDir::current().filePath("void_store");
    QString password = "pswd";
    Store   store(path, password, true);

    QCOMPARE(store.error, Store::Success);

    QByteArray data = "Hello World";

    store.addFile("/hello.txt", data);
    QCOMPARE(store.error, Store::Success);
    data = store.decryptFile("/hello.txt");
    QCOMPARE(store.error, Store::Success);

    store.remove("/");

    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests Store#addFile(const QString, const QString)
 */
void VoidTest::storeAddFileFromDisk()
{
    QString path     = QDir::current().filePath("void_store");
    QString password = "pswd";
    Store   store(path, password, true);

    QCOMPARE(store.error, Store::Success);

    QByteArray data = "Hello World";
    data = data.repeated(500000);

    QFile f("void_store/hello.txt");
    f.open(QIODevice::WriteOnly);
    f.write(data);
    f.close();

    store.addFile( "void_store/hello.txt", QString("/hello.txt") );
    QCOMPARE(store.error, Store::Success);

    store.decryptFile("/hello.txt", "void_store/hello2.txt");
    QCOMPARE(store.error, Store::Success);

    QFile f2("void_store/hello2.txt");
    f2.open(QIODevice::ReadOnly);
    QByteArray data2 = f2.readAll();
    f2.close();

    QCOMPARE(data, data2);

    store.remove("/");
    QFile::remove("void_store/Store.void");
    QFile::remove("void_store/hello.txt");
    QFile::remove("void_store/hello2.txt");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests Store#move
 */
void VoidTest::storeRenameFile()
{
    QString path     = QDir::current().filePath("void_store");
    QString password = "pswd";
    Store   store(path, password, true);

    QByteArray data = "Hello World";

    store.addFile("/hello.txt", data);
    QCOMPARE(store.error,                     Store::Success);
    QCOMPARE(store.decryptFile("/hello.txt"), data);

    store.move("/hello.txt", "/dir/hello3.txt");
    QCOMPARE( store.error,                          Store::Success);
    QCOMPARE( store.decryptFile("/dir/hello3.txt"), data);
    QCOMPARE( store.decryptFile("/hello.txt"),      QByteArray() );
    QCOMPARE( store.error,                          Store::NoSuchFile);

    store.addFile("/dir/hello.txt", data);
    QCOMPARE(store.error,                         Store::Success);
    QCOMPARE(store.decryptFile("/dir/hello.txt"), data);
    store.addFile("/dir/hello2.txt", data);
    QCOMPARE(store.error,                          Store::Success);
    QCOMPARE(store.decryptFile("/dir/hello2.txt"), data);

    store.move("/dir", "/dir2");
    QCOMPARE( store.error,                           Store::Success);
    QCOMPARE( store.decryptFile("/dir2/hello.txt"),  data);
    QCOMPARE( store.decryptFile("/dir/hello.txt"),   QByteArray() );
    QCOMPARE( store.error,                           Store::NoSuchFile);
    QCOMPARE( store.decryptFile("/dir2/hello2.txt"), data);
    QCOMPARE( store.decryptFile("/dir/hello2.txt"),  QByteArray() );
    QCOMPARE( store.error,                           Store::NoSuchFile);
    QCOMPARE( store.decryptFile("/dir2/hello3.txt"), data);
    QCOMPARE( store.decryptFile("/dir/hello3.txt"),  QByteArray() );
    QCOMPARE( store.error,                           Store::NoSuchFile);

    store.remove("/");
    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests Store#listAllDirectories, Store#listAllEntries and Store#listAllFiles
 */
void VoidTest::storeFetchAll()
{
    QString path     = QDir::current().filePath("void_store");
    QString password = "pswd";
    Store   store(path, password, true);

    QByteArray data = "Hello World";

    store.addFile("/dir/hello", data);
    QCOMPARE(store.error, Store::Success);
    store.addFile("/dir/subdir/hello.txt", data);
    QCOMPARE(store.error, Store::Success);
    store.addFile("/folder/subdir/hello2.txt", data);
    QCOMPARE(store.error,                                 Store::Success);

    QStringList paths = store.listAllDirectories();
    QCOMPARE(paths.size(),                                5);
    QCOMPARE(paths.contains("/"),                         true);
    QCOMPARE(paths.contains("/dir"),                      true);
    QCOMPARE(paths.contains("/dir/subdir"),               true);
    QCOMPARE(paths.contains("/folder"),                   true);
    QCOMPARE(paths.contains("/folder/subdir"),            true);

    paths = store.listAllFiles();
    QCOMPARE(paths.size(),                                3);
    QCOMPARE(paths.contains("/dir/hello"),                true);
    QCOMPARE(paths.contains("/dir/subdir/hello.txt"),     true);
    QCOMPARE(paths.contains("/folder/subdir/hello2.txt"), true);

    paths = store.listAllEntries();
    QCOMPARE(paths.size(),                                8);
    QCOMPARE(paths.contains("/"),                         true);
    QCOMPARE(paths.contains("/dir"),                      true);
    QCOMPARE(paths.contains("/dir/subdir"),               true);
    QCOMPARE(paths.contains("/folder"),                   true);
    QCOMPARE(paths.contains("/folder/subdir"),            true);
    QCOMPARE(paths.contains("/dir/hello"),                true);
    QCOMPARE(paths.contains("/dir/subdir/hello.txt"),     true);
    QCOMPARE(paths.contains("/folder/subdir/hello2.txt"), true);

    store.remove("/");
    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests Store#fileMetadata and Store#setFileMetadata and see if [mimetype] is correctly assigned.
 */
void VoidTest::storeCheckMetadata()
{
    QString path     = QDir::current().filePath("void_store");
    QString password = "pswd";
    Store   store(path, password, true);

    QByteArray data = "#!/usr/bin/env ruby\n\nputs 'Hello World'\n";

    store.addFile("/hello.rb", data);
    QCOMPARE(store.error, Store::Success);

    QString       mimetypeString = store.fileMetadata("/hello.rb", "mimetype");
    QMimeDatabase mimedb;
    QMimeType     mimetype = mimedb.mimeTypeForName(mimetypeString);
    QCOMPARE(mimetype.inherits("application/x-ruby"), true);

    QString importantInformation = "k";
    store.setFileMetadata( "/hello.rb", "important", importantInformation.toUtf8() );
    QCOMPARE( store.fileMetadata("/hello.rb", "important"), QByteArray("k") );

    store.setFileMetadata( "/hello.rb", "important", QByteArray() );
    QCOMPARE( store.fileMetadata("/hello.rb", "important"), QByteArray() );

    store.fileMetadata("/hello2.rb", "important");
    QCOMPARE(store.error, Store::NoSuchFile);

    store.remove("/");
    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests Store#listFiles, Store#listSubdirectories and Store#listEntries
 */
void VoidTest::storeListEntries()
{
    QString path     = QDir::current().filePath("void_store");
    QString password = "pswd";
    Store   store(path, password, true);

    QByteArray data = "Hello World";

    store.addFile("/dir/hello", data);
    QCOMPARE(store.error, Store::Success);
    store.addFile("/dir/subdir/hello.txt", data);
    QCOMPARE(store.error, Store::Success);
    store.addFile("/dir/hello2.txt", data);
    QCOMPARE(store.error,                       Store::Success);

    QStringList paths = store.listSubdirectories("/dir");
    QCOMPARE(paths.size(),                      1);
    QCOMPARE(paths.contains("/dir/subdir"),     true);

    paths = store.listFiles("/dir");
    QCOMPARE(paths.size(),                      2);
    QCOMPARE(paths.contains("/dir/hello"),      true);
    QCOMPARE(paths.contains("/dir/hello2.txt"), true);

    paths = store.listEntries("/dir");
    QCOMPARE(paths.size(),                      3);
    QCOMPARE(paths.contains("/dir/subdir"),     true);
    QCOMPARE(paths.contains("/dir/hello"),      true);
    QCOMPARE(paths.contains("/dir/hello2.txt"), true);

    store.remove("/");
    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

/**
 *  \brief Tests Store#searchStartsWith, Store#searchEndsWith, Store#searchContains and Store#searchRegex
 */
void VoidTest::storeSearch()
{
    QString path     = QDir::current().filePath("void_store");
    QString password = "pswd";
    Store   store(path, password, true);

    QByteArray data = "Hello World";

    store.addFile("/dir/hello", data);
    QCOMPARE(store.error, Store::Success);
    store.addFile("/dir/subdir/hello.txt", data);
    QCOMPARE(store.error, Store::Success);
    store.addFile("/folder/subdir/hello2.txt", data);
    QCOMPARE(store.error, Store::Success);

    // searchStartsWith

    QStringList paths = store.searchStartsWith("/dir", 0);
    QCOMPARE(store.error,                             Store::Success);
    QCOMPARE(paths.size(),                            4);
    QCOMPARE(paths.contains("/dir"),                  true);
    QCOMPARE(paths.contains("/dir/subdir"),           true);
    QCOMPARE(paths.contains("/dir/subdir/hello.txt"), true);
    QCOMPARE(paths.contains("/dir/hello"),            true);

    paths = store.searchStartsWith("/dir", 1);
    QCOMPARE(store.error,                             Store::Success);
    QCOMPARE(paths.size(),                            2);
    QCOMPARE(paths.contains("/dir/subdir/hello.txt"), true);
    QCOMPARE(paths.contains("/dir/hello"),            true);

    paths = store.searchStartsWith("/dir", 2);
    QCOMPARE(store.error,                             Store::Success);
    QCOMPARE(paths.size(),                            2);
    QCOMPARE(paths.contains("/dir"),                  true);
    QCOMPARE(paths.contains("/dir/subdir"),           true);

    // searchEndsWith

    paths = store.searchEndsWith("dir", 0);
    QCOMPARE(store.error,                                 Store::Success);
    QCOMPARE(paths.size(),                                3);
    QCOMPARE(paths.contains("/dir"),                      true);
    QCOMPARE(paths.contains("/dir/subdir"),               true);
    QCOMPARE(paths.contains("/folder/subdir"),            true);

    paths = store.searchEndsWith("dir", 1);
    QCOMPARE(store.error,                                 Store::Success);
    QCOMPARE(paths.size(),                                0);

    paths = store.searchEndsWith("dir", 2);
    QCOMPARE(store.error,                                 Store::Success);
    QCOMPARE(paths.size(),                                3);
    QCOMPARE(paths.contains("/dir"),                      true);
    QCOMPARE(paths.contains("/dir/subdir"),               true);
    QCOMPARE(paths.contains("/folder/subdir"),            true);

    paths = store.searchEndsWith(".txt", 0);
    QCOMPARE(store.error,                                 Store::Success);
    QCOMPARE(paths.size(),                                2);
    QCOMPARE(paths.contains("/dir/subdir/hello.txt"),     true);
    QCOMPARE(paths.contains("/folder/subdir/hello2.txt"), true);

    paths = store.searchEndsWith(".txt", 1);
    QCOMPARE(store.error,                                 Store::Success);
    QCOMPARE(paths.size(),                                2);
    QCOMPARE(paths.contains("/dir/subdir/hello.txt"),     true);
    QCOMPARE(paths.contains("/folder/subdir/hello2.txt"), true);

    paths = store.searchEndsWith(".txt", 2);
    QCOMPARE(store.error,                                 Store::Success);
    QCOMPARE(paths.size(),                                0);

    // searchContains

    paths = store.searchContains("subdir", 0);
    QCOMPARE(store.error,                                 Store::Success);
    QCOMPARE(paths.size(),                                4);
    QCOMPARE(paths.contains("/dir/subdir"),               true);
    QCOMPARE(paths.contains("/folder/subdir"),            true);
    QCOMPARE(paths.contains("/dir/subdir/hello.txt"),     true);
    QCOMPARE(paths.contains("/folder/subdir/hello2.txt"), true);

    paths = store.searchContains("subdir", 1);
    QCOMPARE(store.error,                                 Store::Success);
    QCOMPARE(paths.size(),                                2);
    QCOMPARE(paths.contains("/dir/subdir/hello.txt"),     true);
    QCOMPARE(paths.contains("/folder/subdir/hello2.txt"), true);

    paths = store.searchContains("subdir", 2);
    QCOMPARE(store.error,                                 Store::Success);
    QCOMPARE(paths.size(),                                2);
    QCOMPARE(paths.contains("/dir/subdir"),               true);
    QCOMPARE(paths.contains("/folder/subdir"),            true);

    // searchRegex

    paths = store.searchRegex(".*hello.*", 0);
    QCOMPARE(store.error,                                 Store::Success);
    QCOMPARE(paths.size(),                                3);
    QCOMPARE(paths.contains("/dir/hello"),                true);
    QCOMPARE(paths.contains("/dir/subdir/hello.txt"),     true);
    QCOMPARE(paths.contains("/folder/subdir/hello2.txt"), true);

    paths = store.searchRegex(".*hello.*", 1);
    QCOMPARE(store.error,                                 Store::Success);
    QCOMPARE(paths.size(),                                3);
    QCOMPARE(paths.contains("/dir/hello"),                true);
    QCOMPARE(paths.contains("/dir/subdir/hello.txt"),     true);
    QCOMPARE(paths.contains("/folder/subdir/hello2.txt"), true);

    paths = store.searchRegex(".*hello.*", 2);
    QCOMPARE(store.error,                                 Store::Success);
    QCOMPARE(paths.size(),                                0);

    store.remove("/");
    QFile::remove("void_store/Store.void");
    QDir::current().rmdir("void_store");
}

QTEST_MAIN(VoidTest)

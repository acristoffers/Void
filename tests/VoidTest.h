#ifndef CRYPTOTEST_H
#define CRYPTOTEST_H

#include <QtTest/QtTest>

class VoidTest : public QObject
{
    Q_OBJECT
private slots:
    void cryptoCreateObjectWithPassword();
    void cryptoCreateObjectWithKey();
    void cryptoToHex();
    void cryptoToB64();
    void cryptoFromB64();
    void cryptoEncrypt();
    void cryptoDecrypt();

    void storeFileCreateAndLoadStore();

    void storeFSMakePath();
    void storeFSAddFile();
    void storeFSAddFileFromDisk();
    void storeFSRemoveFile();
    void storeFSRemoveDir();
    void storeFSRenameFile();
    void storeFSRenameDir();
    void storeFSFilters();
    void storeFSFetchAll();

    void storeCreate();
    void storeAddFile();
    void storeAddFileFromDisk();
    void storeRenameFile();
    void storeFetchAll();
    void storeCheckMetadata();
    void storeListEntries();
    void storeSearch();
};
#endif // CRYPTOTEST_H

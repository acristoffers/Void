#include "StoreFile.h"

#include <QByteArray>
#include <QDataStream>
#include <QFile>
#include <QMap>

/*!
 *  \class StoreFile
 *  \brief Manages the Store.void file
 *
 *  This class is responsible for loading and saving
 *  the Store.void file. It takes the necessary crypto
 *  information and the data returned by StoreFS and
 *  serializes and save everything to the file. It also
 *  reads the file and restore the information.
 *
 *  To release memory resources, data can be cleared.
 *  Pay attention so you don't lose information by setting
 *  something without data, which will trigger a save.
 *  It's meant to be initialized once and then only used
 *  to save data, not to change parameters.
 *
 */

/**
 *  \brief StoreFile's private data structure
 */
struct StoreFilePrivate
{
    quint16                version = 1;  /*!< Version of the Store.void file format implemented. */
    std::string            salt;         /*!< Salt used by the crypt operations */
    std::string            iv;           /*!< IV used by the crypt operations */
    QByteArray             data;         /*!< The serialized, compressed and encrypted data to be saved or that was loaded */
    CryptoParams           cryptoParams; /*!< CryptoParams object used by the crypt operations */
    std::unique_ptr<QFile> storeFile;    /*!< Pointer to the Store.void file */

    void save();
    void load();
};

/**
 *  \brief Constructs a new StoreFile object.
 *
 *  Opens the file at \c path and loads it if necessary.
 *  Errors are returned in StoreFile#error
 *
 *  \arg \c path The path of the Store.void file.
 */
StoreFile::StoreFile(QString path) : QObject()
{
    _p.reset(new StoreFilePrivate);
    _p->storeFile.reset( new QFile(path) );
    if ( !_p->storeFile->open(QIODevice::ReadWrite) ) {
        error = CantOpenFile;
        return;
    }

    if ( _p->storeFile->size() > 0 ) {
        _p->load();
    }

    error = Success;
}

/**
 *  \brief Sets CryptoParams and triggers a save.
 *
 *  \arg \c params The CryptoParams to be saved.
 */
void StoreFile::setCryptoParams(const CryptoParams params)
{
    _p->cryptoParams = params;
    _p->save();
}

/**
 *  \brief Sets data and triggers a save.
 *
 *  If \c data is empty, only clears the internal data
 *  object, without saving. Meant for memory management.
 *
 *  \arg \c data The data to be saved.
 */
void StoreFile::setData(const QByteArray data)
{
    if ( data.isEmpty() ) {
        _p->data.clear();
    } else {
        _p->data = data;
        _p->save();
    }
}

/**
 *  \brief Sets IV and triggers a save.
 *
 *  \arg \c iv The IV to be saved.
 */
void StoreFile::setIV(const std::string iv)
{
    _p->iv = iv;
    _p->save();
}

/**
 *  \brief Sets salt and triggers a save.
 *
 *  \arg \c salt The salt to be saved.
 */
void StoreFile::setSalt(const std::string salt)
{
    _p->salt = salt;
    _p->save();
}

/**
 *  \brief Returns the CryptoParams object in mememory.
 *
 *  \return The CryptoParams object in mememory.
 */
CryptoParams StoreFile::cryptoParams() const
{
    return _p->cryptoParams;
}

/**
 *  \brief Returns the data object in mememory.
 *
 *  \return The data object in mememory.
 */
QByteArray StoreFile::data() const
{
    return _p->data;
}

/**
 *  \brief Returns the IV object in mememory.
 *
 *  \return The IV object in mememory.
 */
std::string StoreFile::IV() const
{
    return _p->iv;
}

/**
 *  \brief Returns the salt object in mememory.
 *
 *  \return The salt object in mememory.
 */
std::string StoreFile::salt() const
{
    return _p->salt;
}

/**
 *  \brief Default destructor.
 */
StoreFile::~StoreFile() = default;

/**
 *  \brief Saves the information to the file.
 *
 *  Serializes \c version, \c salt, \c iv, \c data and
 *  \c cryptoParams saves it to \c storeFile.
 */
void StoreFilePrivate::save()
{
    storeFile->seek(0);
    QDataStream stream( storeFile.get() );
    stream.setVersion(QDataStream::Qt_5_6);

    QByteArray qsalt = QByteArray::fromStdString(salt);
    QByteArray qiv   = QByteArray::fromStdString(iv);
    stream << version
           << qsalt
           << qiv
           << static_cast<quint8> (cryptoParams.digest)
           << static_cast<quint8> (cryptoParams.encryption)
           << static_cast<quint8> (cryptoParams.keyDerivationFunction)
           << static_cast<quint8> (cryptoParams.keyDerivationHash)
           << cryptoParams.keyDerivationCost
           << data;
}

/**
 *  \brief Loads information from file.
 *
 *  Loads \c version, \c salt, \c iv, \c data
 *  and \c cryptoParams from \c storeFile.
 */
void StoreFilePrivate::load()
{
    QDataStream stream( storeFile.get() );

    stream.setVersion(QDataStream::Qt_5_6);

    QByteArray qsalt, qiv;
    quint8     digest, encryption, keyDerivationFunction, keyDerivationHash;

    stream >> version
    >> qsalt
    >> qiv
    >> digest
    >> encryption
    >> keyDerivationFunction
    >> keyDerivationHash
    >> cryptoParams.keyDerivationCost
    >> data;

    cryptoParams.digest                = static_cast<DigestType> (digest);
    cryptoParams.encryption            = static_cast<EncType> (encryption);
    cryptoParams.keyDerivationFunction = static_cast<KeyDerivationFunction> (keyDerivationFunction);
    cryptoParams.keyDerivationHash     = static_cast<KeyDerivationHash> (keyDerivationHash);

    salt = qsalt.toStdString();
    iv   = qiv.toStdString();
}

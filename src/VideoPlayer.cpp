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

#include "VideoPlayer.h"

#include <QMediaPlayer>
#include <QNetworkAccessManager>
#include <QRegularExpression>
#include <QSslCertificate>
#include <QSslCipher>
#include <QSslConfiguration>
#include <QSslKey>
#include <QSslSocket>
#include <QVideoWidget>

#define OPENSSL_NO_DEPRECATED
namespace OpenSSL
{
#include <openssl/bn.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509v3.h>
}

#include "Runner.h"
#include "Store.h"
#include "VideoPlayerWidget.h"

#ifdef Q_OS_MAC
 #include "MacOSKeychainManagement.h"
#endif

#define CA_SERIAL  0xDEADBEEF01
#define CLI_SERIAL 0xDEADBEEF02
#define SRV_SERIAL 0xDEADBEEF03

enum CertType {
    CA,
    Server,
    Client
};

struct VideoPlayerPrivate
{
    std::shared_ptr<Store> store;
    QSslConfiguration      serverSslConfig;
    QByteArray             data;
    QString                mimetype;
    QString                hash;

    QSslKey         generateRSAKey() const;
    QSslCertificate generateCertificate(const QSslKey &key, const QSslCertificate &signer, const QSslKey &signerKey, CertType type) const;
};

VideoPlayer::VideoPlayer(std::shared_ptr<Store> store) : QTcpServer()
{
    _p.reset(new VideoPlayerPrivate);
    _p->store = store;

    Runner *runner = new Runner([this]() {
        QList<QSslCipher> ciphers;
        for ( auto cipher : QSslSocket::supportedCiphers() ) {
            if ( cipher.name().contains("RSA") && cipher.name().contains("ECDHE") && cipher.name().contains("AES") ) {
                ciphers << cipher;
            }
        }

        const QSslKey rootKey = _p->generateRSAKey();
        const QSslCertificate rootCert = _p->generateCertificate(rootKey, QSslCertificate(), QSslKey(), CA);

        /*
         *  I believe that this would be ideal, but Qt won't allow me to use it.
         *  It keeps using a weird (null) certificate instead of this one.
         *
         *  const QSslKey clientKey = _p->generateRSAKey();
         *  const QSslCertificate clientCert = _p->generateCertificate(clientKey, rootCert, rootKey, Client);
         */

        const QSslKey serverKey = _p->generateRSAKey();
        const QSslCertificate serverCert = _p->generateCertificate(serverKey, rootCert, rootKey, Server);

#ifdef Q_OS_MAC
        RegisterCertificate(rootCert, "void.ca");
        RemoveCertificate("void.server");
#endif

        QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
        sslConfig.setAllowedNextProtocols(QList<QByteArray> () << QSslConfiguration::NextProtocolHttp1_1);
        sslConfig.setCaCertificates(QList<QSslCertificate> () << rootCert);
        sslConfig.setCiphers(ciphers);
        sslConfig.setLocalCertificate(serverCert);
        sslConfig.setPeerVerifyDepth(0);
        sslConfig.setPrivateKey(serverKey);
        sslConfig.setProtocol(QSsl::SecureProtocols);

        _p->serverSslConfig = sslConfig;

        QSslConfiguration::setDefaultConfiguration(sslConfig);
    });

    QThreadPool::globalInstance()->start(runner);
}

VideoPlayer::~VideoPlayer()
{
#ifdef Q_OS_MAC
    RemoveCertificate("void.ca");
    RemoveCertificate("void.server");
#endif
}

void VideoPlayer::play(const QString path)
{
    _p->data     = _p->store->decryptFile(path);
    _p->mimetype = _p->store->fileMetadata(path, "mimetype");

    if ( !isListening() ) {
        listen(QHostAddress::LocalHost);
    }

    QMediaPlayer      *player = new QMediaPlayer;
    VideoPlayerWidget *video  = new VideoPlayerWidget;
    QUrl              url;

    _p->hash = QCryptographicHash::hash(path.toUtf8(), QCryptographicHash::Sha3_512).toHex();

    url.setScheme("https");
    url.setHost( serverAddress().toString() );
    url.setPort( serverPort() );
    url.setPath("/" + _p->hash);

    connect(video, &VideoPlayerWidget::closing, [this, player, video]() {
        player->setMedia( QMediaContent() );

        _p->data.clear();
        _p->mimetype.clear();
        _p->hash.clear();

        player->deleteLater();
        video->deleteLater();
    });

    connect(player, &QMediaPlayer::stateChanged, [video](QMediaPlayer::State state) {
        if ( state == QMediaPlayer::PlayingState ) {
            video->play();
        } else {
            video->pause();
        }
    });

    connect(player, &QMediaPlayer::volumeChanged,     video,  &VideoPlayerWidget::setVolume);
    connect(player, &QMediaPlayer::positionChanged,   video,  &VideoPlayerWidget::setPosition);

    connect(video,  &VideoPlayerWidget::positionSet,  player, &QMediaPlayer::setPosition);
    connect(video,  &VideoPlayerWidget::volumeSet,    player, &QMediaPlayer::setVolume);
    connect(video,  &VideoPlayerWidget::playPressed,  player, &QMediaPlayer::play);
    connect(video,  &VideoPlayerWidget::pausePressed, player, &QMediaPlayer::pause);

    video->show();
    player->setMedia(url);
    player->setVideoOutput( video->videoWidget() );
    player->play();

    video->setVolume( static_cast<quint8> ( player->volume() ) );
}

QSslKey VideoPlayerPrivate::generateRSAKey() const
{
    using namespace OpenSSL;

    RSA    *rsa = RSA_new();
    BIGNUM *bn  = BN_new();
    BIO    *mem = BIO_new( BIO_s_mem() );

    BN_set_word(bn, 65537);
    RSA_generate_key_ex(rsa, 2048, bn, NULL);

    PEM_write_bio_RSAPrivateKey(mem, rsa, NULL, NULL, 0, NULL, NULL);

    BIO_flush(mem);

    BUF_MEM *bptr;
    BIO_get_mem_ptr(mem, &bptr);

    QByteArray keyData(bptr->data, bptr->length);

    BIO_free(mem);
    RSA_free(rsa);
    BN_free(bn);

    return QSslKey(keyData, QSsl::Rsa);
}

int add_ext(OpenSSL::X509 *cert, OpenSSL::X509 *issuer, int nid, char *value)
{
    using namespace OpenSSL;
    X509_EXTENSION *ex;
    X509V3_CTX     ctx;

    X509V3_set_ctx_nodb(&ctx);
    X509V3_set_ctx(&ctx, issuer, cert, NULL, NULL, 0);
    ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
    if ( !ex ) {
        return 0;
    }

    X509_add_ext(cert, ex, -1);
    X509_EXTENSION_free(ex);
    return 1;
}

QSslCertificate VideoPlayerPrivate::generateCertificate(const QSslKey &key, const QSslCertificate &signer, const QSslKey &signerKey, CertType type) const
{
    using namespace OpenSSL;

    QByteArray   pem   = key.toPem();
    BIO          *mem  = BIO_new( BIO_s_mem() );
    RSA          *rsa  = RSA_new();
    EVP_PKEY     *pkey = EVP_PKEY_new();
    X509         *x509 = X509_new();
    ASN1_UTCTIME *s    = ASN1_UTCTIME_new();

    BIO_write( mem, pem.data(), pem.size() );
    BIO_flush(mem);
    PEM_read_bio_RSAPrivateKey(mem, &rsa, NULL, NULL);
    EVP_PKEY_assign_RSA(pkey, rsa);

    X509_set_version(x509, 2);
    ASN1_INTEGER_set( X509_get_serialNumber(x509), ( type == CA ? CA_SERIAL : (type == Server ? SRV_SERIAL : CLI_SERIAL) ) );
    X509_gmtime_adj(s, -60 * 60);
    X509_set_notBefore(x509, s);
    X509_gmtime_adj(s, 60 * 60 * 24);
    X509_set_notAfter(x509, s);
    X509_NAME *name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, reinterpret_cast<const uchar *> ("BR"),              -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, reinterpret_cast<const uchar *> ("acristoffers.me"), -1, -1, 0);
    X509_set_pubkey(x509, pkey);

    add_ext( x509, x509, NID_subject_key_identifier, QString("hash").toLocal8Bit().data() );

    if ( signer.isNull() || signerKey.isNull() ) {
        X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, reinterpret_cast<const uchar *> ("void.ca"), -1, -1, 0);

        add_ext( x509, x509, NID_basic_constraints,  QString("critical,CA:TRUE").toLocal8Bit().data() );
        add_ext( x509, x509, NID_key_usage,          QString("critical,keyCertSign,cRLSign").toLocal8Bit().data() );
        add_ext( x509, x509, NID_netscape_cert_type, QString("sslCA").toLocal8Bit().data() );

        X509_set_issuer_name(x509, name);
        X509_sign( x509, pkey, EVP_sha512() );
    } else {
        X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, reinterpret_cast<const uchar *> (type == Client ? "void.client" : "void.server"), -1, -1, 0);

        add_ext( x509, x509, NID_basic_constraints,  QString("critical,CA:FALSE").toLocal8Bit().data() );
        add_ext( x509, x509, NID_key_usage,          QString("critical,digitalSignature,keyEncipherment,keyAgreement").toLocal8Bit().data() );
        add_ext( x509, x509, NID_netscape_cert_type, QString(type == Client ? "client" : "server").toLocal8Bit().data() );
        add_ext( x509, x509, NID_ext_key_usage,      QString(type == Client ? "clientAuth" : "serverAuth").toLocal8Bit().data() );

        if ( type == Server ) {
            add_ext( x509, x509, NID_subject_alt_name, QString("DNS:localhost,IP:127.0.0.1").toLocal8Bit().data() );
        }

        X509     *signerCert = X509_new();
        RSA      *signerRsa  = RSA_new();
        EVP_PKEY *signerPkey = EVP_PKEY_new();

        pem = signer.toPem();
        BIO_reset(mem);
        BIO_write( mem, pem.data(), pem.size() );
        PEM_read_bio_X509(mem, &signerCert, NULL, NULL);

        pem = signerKey.toPem();
        BIO_reset(mem);
        BIO_write( mem, pem.data(), pem.size() );
        PEM_read_bio_RSAPrivateKey(mem, &signerRsa, NULL, NULL);
        EVP_PKEY_assign_RSA(signerPkey, signerRsa);

        name = X509_get_subject_name(signerCert);
        X509_set_issuer_name(x509, name);
        X509_sign( x509, signerPkey, EVP_sha512() );

        EVP_PKEY_free(signerPkey);
        X509_free(signerCert);
    }

    BIO_reset(mem);
    PEM_write_bio_X509(mem, x509);

    BUF_MEM *bptr;
    BIO_get_mem_ptr(mem, &bptr);

    QByteArray certData(bptr->data, bptr->length);

    BIO_free(mem);
    EVP_PKEY_free(pkey);
    X509_free(x509);
    ASN1_UTCTIME_free(s);

    return QSslCertificate(certData);
}

void VideoPlayer::incomingConnection(qintptr handle)
{
    QSslSocket *socket = new QSslSocket(this);

    connect(socket, &QAbstractSocket::disconnected, socket, &QObject::deleteLater);
    connect(socket, &QIODevice::readyRead,          [this, socket]() {
        QString request = socket->readAll();

        bool partial = false;
        int min = 0;
        int max = _p->data.size() - 1;

        if ( socket->peerAddress() != QHostAddress(QHostAddress::LocalHost) ) {
            socket->write("HTTP/1.1 403 Forbidden\r\n\r\n");
            socket->flush();
            socket->close();
            return;
        }

        if ( !request.split("\r\n").first().contains(_p->hash) ) {
            socket->write("HTTP/1.1 404 Not Found\r\n\r\n");
            socket->flush();
            socket->close();
            return;
        }

        if ( request.contains("Range") ) {
            partial = true;
            QString rangeValue = request.split("\r\n").filter( QRegularExpression("Range:.*") ).first().split(":").last().trimmed();
            if ( !rangeValue.isEmpty() ) {
                QStringList minMax = rangeValue.split("=").last().split("-");
                min = minMax.first().toInt();
                max = minMax.last().isEmpty() ? max : minMax.last().toInt();
            }
        }

        if ( (max < min) || (max > _p->data.size() - 1) ) {
            min = 0;
            max = _p->data.size() - 1;
            partial = false;
        }

        QLocale locale(QLocale::English, QLocale::UnitedStates);

        if ( partial ) {
            socket->write( "HTTP/1.1 206 Partial Content\r\n");
            socket->write( ("Content-Range: bytes " + QString::number(min) + "-" + QString::number(max) + "/" + QString::number( _p->data.size() ) + "\r\n").toLocal8Bit() );
        } else {
            socket->write("HTTP/1.1 200 OK\r\n");
        }

        socket->write( "Server: void\r\n");
        socket->write( ("Date: " + locale.toString(QDateTime::currentDateTimeUtc(), "ddd, dd MMM yyyy hh:mm:ss") + " GMT\r\n").toLocal8Bit() );
        socket->write( ("Last-Modified: " + locale.toString(QDateTime::currentDateTimeUtc(), "ddd, dd MMM yyyy hh:mm:ss") + " GMT\r\n").toLocal8Bit() );
        socket->write( "Connection: keep-alive\r\n");
        socket->write( "Accept-Ranges: bytes\r\n");
        socket->write( ("Content-Type: " + _p->mimetype + "\r\n").toLocal8Bit() );
        socket->write( "X-Frame-Options: DENY\r\n");
        socket->write( ("Content-Length: " + QString::number(max - min + 1) + "\r\n").toLocal8Bit() );
        socket->write( "X-Content-Type-Options: nosniff\r\n\r\n");
        socket->write( _p->data.mid(min, max - min + 1) );
        socket->flush();
    });

    socket->setSslConfiguration(_p->serverSslConfig);

    if ( socket->setSocketDescriptor(handle) ) {
        socket->startServerEncryption();
    } else {
        socket->deleteLater();
    }
}

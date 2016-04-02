#include "crypto.h"

#include <algorithm>

#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/gcm.h>
#include <cryptopp/osrng.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>

using byte = unsigned char;

struct CryptoPrivate
{
    CryptoPP::SecByteBlock                   key(32);
    CryptoPP::PKCS12_PBKDF<CryptoPP::SHA256> pkcs12;
};

Crypto::Crypto(const std::string &password, const std::string &salt) noexcept
{
    _p.reset(new CryptoPrivate);

    _p->pkcs12.DeriveKey(_p->key, _p->key.size(), 0, reinterpret_cast<const byte *> ( password.data() ), password.size(), reinterpret_cast<const byte *> ( salt.data() ), salt.size(), 5000, 0);
}

Crypto::~Crypto() = default;

std::shared_ptr<std::string> Crypto::encrypt(const std::string &plaintext, const std::string &iv) const noexcept
{
    std::shared_ptr<std::string> cipher(new std::string);

    CryptoPP::GCM<CryptoPP::AES>::Encryption e;

    e.SetKeyWithIV( _p->key, _p->key.size(), reinterpret_cast<const byte *> ( iv.data() ), iv.size() );

    CryptoPP::AuthenticatedEncryptionFilter ef(e, new CryptoPP::StringSink(*cipher), false, 16);
    ef.ChannelPut( "", reinterpret_cast<const byte *> ( plaintext.data() ), plaintext.size() );
    ef.ChannelMessageEnd("");

    return cipher;
}

std::shared_ptr<std::string> Crypto::decrypt(const std::string &cipher, const std::string &iv) const noexcept
{
    std::shared_ptr<std::string> plaintext(new std::string);

    CryptoPP::GCM<CryptoPP::AES>::Decryption d;

    d.SetKeyWithIV( _p->key, _p->key.size(), reinterpret_cast<const byte *> ( iv.data() ), iv.size() );

    CryptoPP::AuthenticatedDecryptionFilter df(d, new CryptoPP::StringSink(*plaintext), false, 16);
    df.Put( reinterpret_cast<const byte *> ( cipher.data() ), cipher.size() );
    df.MessageEnd();

    return plaintext;
}

std::string Crypto::generateRandomString(unsigned size) noexcept
{
    byte *salt = new byte[size + 1];

    CryptoPP::AutoSeededX917RNG<CryptoPP::AES> rng;

    rng.GenerateBlock(salt, size);

    salt[size] = 0x0;
    std::string s = reinterpret_cast<char *> (salt);

    delete salt;

    return s;
}

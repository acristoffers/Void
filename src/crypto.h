#ifndef CRYPTO_H
#define CRYPTO_H

#include <memory>
#include <string>

struct CryptoPrivate;

enum EncType {
    AES_GCM_128,
    AES_GCM_256
};

enum DigestType {
    SHA256,
    SHA512
};

enum KeyDerivationFunction {
    PKCS5_PBKDF2
};

enum KeyDerivationHash {
    HMAC_SHA256,
    HMAC_SHA512
};

struct CryptoParams
{
    EncType               encryption            = AES_GCM_256;
    DigestType            digest                = SHA512;
    KeyDerivationFunction keyDerivationFunction = PKCS5_PBKDF2;
    KeyDerivationHash     keyDerivationHash     = HMAC_SHA512;
    unsigned int          keyDerivationCost     = 1000000;
};

struct Crypto
{
    enum CryptoError {
        Success,
        CantGetAlgID,
        CantGetSlot,
        CantGenerateKey,
        CantEncrypt,
        CantDecrypt
    };

    static std::string generateRandom(uint8_t size);
    static std::string digest( std::string &data, CryptoParams params = CryptoParams() );
    static std::string stringToHex(const std::string &input, std::string separator = ":");
    static std::string toBase64(const std::string &data);
    static std::string fromBase64(const std::string &data);

    Crypto( std::string &password, std::string &salt, std::string &iv, CryptoParams params = CryptoParams() );
    Crypto( std::string &key, std::string &iv, CryptoParams params = CryptoParams() );
    ~Crypto();

    std::string encrypt(std::string data);
    std::string decrypt(std::string data);

    std::string key();
    std::string iv();

    CryptoError error;
private:
    std::unique_ptr<CryptoPrivate> _p;
};
#endif

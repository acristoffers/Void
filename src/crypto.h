#ifndef CRYPTO_H
#define CRYPTO_H

#include <memory>
#include <string>

struct CryptoPrivate;

enum EncType : uint8_t {
    AES_GCM_128,
    AES_GCM_256
};

enum DigestType : uint8_t {
    SHA256,
    SHA512
};

enum KeyDerivationFunction : uint8_t {
    PKCS5_PBKDF2
};

enum KeyDerivationHash : uint8_t {
    HMAC_SHA256,
    HMAC_SHA512
};

struct CryptoParams
{
    EncType               encryption            = AES_GCM_256;
    DigestType            digest                = SHA512;
    KeyDerivationFunction keyDerivationFunction = PKCS5_PBKDF2;
    KeyDerivationHash     keyDerivationHash     = HMAC_SHA512;
    uint32_t              keyDerivationCost     = 1000000;
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
    }

    error;

    static std::string generateRandom(const uint8_t size);
    static std::string digest( const std::string &data, CryptoParams params = CryptoParams() );
    static std::string stringToHex(const std::string &input, const std::string separator = ":");
    static std::string toBase64(const std::string &data);
    static std::string fromBase64(const std::string &data);

    Crypto( const std::string password, const std::string salt, const std::string iv, const CryptoParams params = CryptoParams() );
    Crypto( const std::string key, const std::string iv, const CryptoParams params = CryptoParams() );
    ~Crypto();

    std::string encrypt(const std::string &data);
    std::string decrypt(const std::string &data);

    std::string key() const;
    std::string iv() const;

private:
    std::unique_ptr<CryptoPrivate> _p;
};
#endif

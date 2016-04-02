#ifndef CRYPTO_H
#define CRYPTO_H

#include <memory>

struct CryptoPrivate;

struct Crypto
{
    static std::string generateSalt(uint8_t size);

    Crypto(std::string &password, std::string &salt, std::string &iv);
    Crypto(std::string &key, std::string &iv);
    ~Crypto();

    std::string encrypt(std::string data);
    std::string decrypt(std::string data);

    std::string key();
    std::string iv();

private:
    std::unique_ptr<CryptoPrivate> _p;
};
#endif

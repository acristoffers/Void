#ifndef CRYPTO_H
#define CRYPTO_H

#include <memory>
#include <vector>

struct CryptoPrivate;

struct Crypto
{
    Crypto(const std::string &password, const std::string &salt) noexcept;
    ~Crypto();

    std::shared_ptr<std::string> encrypt(const std::string &plaintext, const std::string &iv) const noexcept;

    std::shared_ptr<std::string> decrypt(const std::string &cipher, const std::string &iv) const noexcept;

    static std::string generateRandomString(unsigned size) noexcept;

private:
    std::unique_ptr<CryptoPrivate> _p;
};
#endif

/*
 *  Copyright (c) 2015 Álan Crístoffer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#ifndef CRYPTO_H
#define CRYPTO_H

#include <memory>
#include <string>
#include <stdint.h>

/*! \file */

struct CryptoPrivate;

/**
 *  \brief Type of encryption.
 */
enum EncType : uint8_t {
    AES_GCM_128, /*!< AES-GCM 128 bits. */
    AES_GCM_256  /*!< AES-GCM 256 bits. */
};

/**
 *  \brief Type of digest.
 */
enum DigestType : uint8_t {
    SHA256, /*!< SHA256. */
    SHA512  /*!< SHA512. */
};

/**
 *  \brief Key derivation function.
 */
enum KeyDerivationFunction : uint8_t {
    PKCS5_PBKDF2 /*!< PKCS#5 PBKDF#2 */
};

/**
 *  \brief Digest used on key derivation.
 */
enum KeyDerivationHash : uint8_t {
    HMAC_SHA256, /*!< HMAC + SHA256 */
    HMAC_SHA512  /*!< HMAC + SHA512 */
};

/**
 *  \brief Parameters for Crypto operation.
 *
 *  It's highly recommended to use the defaults when
 *  creating new objects.
 */
struct CryptoParams
{
    EncType               encryption            = AES_GCM_256;  /*!< \see EncType */
    DigestType            digest                = SHA512;       /*!< \see DigestType */
    KeyDerivationFunction keyDerivationFunction = PKCS5_PBKDF2; /*!< \see KeyDerivationFunction */
    KeyDerivationHash     keyDerivationHash     = HMAC_SHA512;  /*!< \see KeyDerivationHash */
    uint32_t              keyDerivationCost     = 250000;       /*!< How many iterations the derivation function will use. */
};

struct Crypto
{
    /**
     *  \brief Errors returned by Crypto
     */
    enum CryptoError : uint8_t {
        Success,         /*!< Success. */
        CantGetAlgID,    /*!< NSS error. Could not generate an AlgID object. */
        CantGetSlot,     /*!< NSS error. Could not get a slot. */
        CantGenerateKey, /*!< NSS error. Could not generate a key. */
        CantEncrypt,     /*!< NSS error. Could not encrypt. */
        CantDecrypt      /*!< NSS error. Could not decrypt. */
    }

    /**
     *  \brief Errors returned by Crypto
     */
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

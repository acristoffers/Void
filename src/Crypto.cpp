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

#include "Crypto.h"

#include <iostream>
#include <math.h>

#include <blapit.h>
#include <nss.h>
#include <nssb64.h>
#include <pk11pub.h>
#include <prrng.h>
#include <secoid.h>

/**
 *  \class Crypto
 *  \brief Wraps Mozilla's NSS and eases the encryption process.
 *
 *  This class uses only standard c++ types and is meant to be a wrapper to NSS.
 *  It's not meant to offer every functionality of NSS, only those needed by the
 *  application, while being somewhat configurable to allow backward compatible
 *  changes in the future (to add better security, for example). To create brand
 *  new objects, it's recommended to use the default CryptoParams. When working
 *  with files already encrypted before, use the saved information.
 *
 */

/**
 *  \brief Wrapper for NSS::SECITEM_FreeItem
 */
void SECITEM_Free(SECItem *item)
{
    SECITEM_FreeItem(item, PR_TRUE);
}

using PK11SlotInfoPtr = std::unique_ptr<PK11SlotInfo, decltype(&PK11_FreeSlot)>;
using PK11SymKeyPtr   = std::unique_ptr<PK11SymKey, decltype(&PK11_FreeSymKey)>;
using SECItemPtr      = std::unique_ptr<SECItem, decltype(&SECITEM_Free)>;

/**
 *  \brief Crypto's private data structure
 *
 *  \see Crypto
 */
struct CryptoPrivate
{
    PK11SlotInfoPtr slot   = PK11SlotInfoPtr(nullptr, PK11_FreeSlot); /*!< NSS's slot. Used in most crypto operations. */
    PK11SymKeyPtr   SymKey = PK11SymKeyPtr(nullptr, PK11_FreeSymKey); /*!< Symmetric Key used to encrypt/decrypt. \see Crypto#encrypt \see Crypto#decrypt */
    SECItemPtr      IV     = SECItemPtr(nullptr, SECITEM_Free);       /*!< Initial Vector used to encrypt/decrypt. \see Crypto#encrypt \see Crypto#decrypt */

    unsigned int      keySize;           /*!< Size of the key in bytes. */
    uint32_t          keyDerivationCost; /*!< Number of iterations in the key generation function. \see Crypto#Crypto(const std::string,const std::string,const std::string,const CryptoParams) */
    SECOidTag         keyDeriveFunction; /*!< The NSS function to use to generate the key. */
    SECOidTag         keyDeriveMethod;   /*!< The NSS method to use to generate the key. */
    SECOidTag         keyDerivationHash; /*!< The NSS Hash to use to generate the key. */
    SECOidTag         digestType;        /*!< Hash used by Crypto#digest. */
    unsigned int      digestSize;        /*!< Size of output of hash used by Crypto#digest. */
    CK_MECHANISM_TYPE cypherMechanism;   /*!< Type of cypher used by Crypto#encrypt and Crypto#decrypt. */

    void parseParams(const CryptoParams params);
    static SECItemPtr stringToSECItemPtr(const std::string);

    static bool initialized; /*!< Wether NSS was initialized. */
    static void initNSS();
};

bool CryptoPrivate::initialized = false;

/**
 *  \brief Generates a blob of random data.
 *
 *  \arg \c size Size of the blob to generate.
 *
 *  \return The generated data as std::string.
 */
std::string Crypto::generateRandom(const uint8_t size)
{
    CryptoPrivate::initNSS();

    std::unique_ptr<uint8_t> random(new uint8_t[size]);

    auto slot = PK11_GetBestSlot(CKM_FAKE_RANDOM, nullptr);

    PK11_GenerateRandom(random.get(), size);
    PK11_FreeSlot(slot);

    return std::string(reinterpret_cast<const char*>(random.get()), size);
}

/**
 *  \brief Makes a checksum of \c data using the method choosen in \c params.
 *
 *  \arg \c data Data to be digested.
 *  \arg \c params CryptoParams containing the digest method.
 *
 *  \return The binary digest as a std::string (not an HEX representation).
 */
std::string Crypto::digest(const std::string &data, const CryptoParams params)
{
    CryptoPrivate _p;

    _p.parseParams(params);

    auto         data_c      = reinterpret_cast<const unsigned char*>(data.data());
    unsigned int digest_size = _p.digestSize;

    std::unique_ptr<unsigned char> digest(new unsigned char[digest_size]);

    auto context = PK11_CreateDigestContext(_p.digestType);

    PK11_DigestBegin(context);
    PK11_DigestOp(context, data_c, static_cast<unsigned int>(data.size()));
    PK11_DigestFinal(context, digest.get(), &digest_size, digest_size);
    PK11_DestroyContext(context, PR_TRUE);

    return std::string(reinterpret_cast<const char*>(digest.get()), digest_size);
}

/**
 *  \brief Converts a std::string binary blob to its HEX representation,
 *  separated by \c separator.
 *
 *  \arg \c input The blob to be represented as HEX.
 *  \arg \c separator The separator to be put between every 2 HEX digits.
 *
 *  \return The HEX representation of \c input.
 */
std::string Crypto::stringToHex(const std::string &input, const std::string separator)
{
    static const char* const lut = "0123456789ABCDEF";

    std::string output;
    bool        first = true;

    output.reserve((2 + separator.size()) * input.length());

    for (size_t i = 0; i < input.size(); ++i) {
        if (!first) {
            output += separator;
        }

        const unsigned char c = static_cast<const unsigned char>(input[i]);
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 0xF]);

        first = false;
    }

    return output;
}

/**
 *  \brief Encodes \c data to its base64 form.
 *
 *  \arg \c data Data to be represented as base64.
 *
 *  \return The base64 representation of \c data.
 */
std::string Crypto::toBase64(const std::string &data)
{
    CryptoPrivate::initNSS();

    auto        secItem   = CryptoPrivate::stringToSECItemPtr(data);
    char        *base64_c = NSSBase64_EncodeItem(nullptr, nullptr, 0, secItem.get());
    std::string base64(base64_c, strlen(base64_c));

    delete base64_c;
    return base64;
}

/**
 *  \brief Decodes the base64 \c data.
 *
 *  \arg \c data Base64 representation of some data.
 *
 *  \returns The decoded data.
 */
std::string Crypto::fromBase64(const std::string &data)
{
    CryptoPrivate::initNSS();

    SECItemPtr secItem(SECITEM_AllocItem(nullptr, nullptr, 0), SECITEM_Free);

    NSSBase64_DecodeBuffer(nullptr, secItem.get(), data.data(), static_cast<unsigned int>(data.size()));

    return std::string(reinterpret_cast<const char*>(secItem->data), secItem->len);
}

/**
 *  \brief Wraps Mozillas NSS and eases the encryption process.
 *
 *  Constructs a crypto object for encryption/decryption, deriving a key from \c
 *  password and \c salt and uses the method described in \c params. Initializes
 *  its CryptoPrivate with parameters from \c params.  This application always
 *  assume that your OS is trustworthy, as there is no way to hide something
 *  from a compromised OS.
 *  Uses Crypto#error to report errors.
 *
 *  \arg \c password The password used to generate the key.
 *  \arg \c salt The salt used to generate the key. This is public information
 *  and can be saved unprotected.
 *  \arg \c iv The Initial Vector that will be used during
 *  encryption/decryption.
 *  \arg \c params All the parameters that describe what kinds of encryptio,
 *  hash, keys, etc will be used by this object.
 *
 *  \see Crypto#encrypt
 *  \see Crypto#decrypt
 *  \see CryptoParams
 *  \see Crytpo#error
 */
Crypto::Crypto(const std::string password, const std::string salt, const std::string iv, const CryptoParams params)
{
    _p.reset(new CryptoPrivate);
    _p->parseParams(params);
    _p->initNSS();
    _p->slot.reset(PK11_GetBestSlot(_p->cypherMechanism, nullptr));

    auto PswdSec = _p->stringToSECItemPtr(password);
    auto SaltSec = _p->stringToSECItemPtr(salt);

    SECAlgorithmID *algID = PK11_CreatePBEV2AlgorithmID(
        _p->keyDeriveFunction,
        _p->keyDeriveMethod,
        _p->keyDerivationHash,
        static_cast<int>(_p->keySize),
        static_cast<int>(_p->keyDerivationCost), SaltSec.get());
    PK11SlotInfo *slot = PK11_GetBestSlot(_p->keyDeriveFunction, nullptr);

    if (algID == nullptr) {
        error = CantGetAlgID;
        return;
    }

    if (slot == nullptr) {
        error = CantGetSlot;
        return;
    }

    auto symKey = PK11_PBEKeyGen(slot, algID, PswdSec.get(), PR_FALSE, nullptr);

    PK11_FreeSlot(slot);
    SECOID_DestroyAlgorithmID(algID, PR_TRUE);

    if (symKey == nullptr) {
        error = CantGenerateKey;
        return;
    }

    _p->IV = _p->stringToSECItemPtr(iv);
    _p->SymKey.reset(symKey);

    error = Success;
}

/**
 *  \brief Wraps Mozillas NSS and eases the encryption process.
 *
 *  Constructs a crypto object for encryption/decryption, using the given key.
 *  Initializes its CryptoPrivate with parameters from \c params.  This
 *  application always assume that your OS is trustworthy, as there is no way to
 *  hide something from a compromised OS.
 *
 *  \arg \c key The key used to encrypt/decrypt. Must be at least as long as
 *  required by the method choosen in \c params.
 *  \arg \c iv The Initial Vector that will be used during
 *  encryption/decryption.
 *  \arg \c params All the parameters that describe what kinds of encryptio,
 *  hash, keys, etc will be used by this object.
 *
 *  \see Crypto#encrypt
 *  \see Crypto#decrypt
 *  \see CryptoParams
 */
Crypto::Crypto(const std::string key, const std::string iv, const CryptoParams params)
{
    _p.reset(new CryptoPrivate);
    _p->parseParams(params);
    _p->initNSS();
    _p->slot.reset(PK11_GetBestSlot(_p->cypherMechanism, nullptr));

    _p->IV = _p->stringToSECItemPtr(iv);
    auto secKey = _p->stringToSECItemPtr(key);
    auto symKey = PK11_ImportSymKey(_p->slot.get(),
                                    _p->cypherMechanism,
                                    PK11_OriginUnwrap,
                                    CKA_ENCRYPT,
                                    secKey.get(),
                                    nullptr);

    _p->SymKey.reset(symKey);

    error = Success;
}

/**
 *  \brief Default destructor.
 */
Crypto::~Crypto() = default;

/**
 *  \brief Encrypts the contens of \c data.
 *
 *  Encrypts \c data using the method choosen when constructing the object.
 *  Uses Crypto#error to report errors.
 *
 *  \arg \c data Data to be encrypted.
 *
 *  \return The encrypted data.
 *
 *  \see Crypto
 *  \see CryptoParams
 *  \see Crypto#decrypt
 *  \see Crypto#error
 */
std::string Crypto::encrypt(const std::string &data)
{
    CK_GCM_PARAMS gcmParams;
    SECItem       param;

    param.type          = siBuffer;
    param.data          = reinterpret_cast<unsigned char*>(&gcmParams);
    param.len           = sizeof(gcmParams);
    gcmParams.pIv       = _p->IV->data;
    gcmParams.ulIvLen   = _p->IV->len;
    gcmParams.pAAD      = nullptr;
    gcmParams.ulAADLen  = 0;
    gcmParams.ulTagBits = AES_BLOCK_SIZE * 8;

    unsigned int                   len = static_cast<unsigned int>(data.size() + 2 * _p->IV->len);
    std::unique_ptr<unsigned char> buffer(new unsigned char[len]);

    auto clear_text = reinterpret_cast<const unsigned char*>(data.data());

    SECStatus s = PK11_Encrypt(_p->SymKey.get(), _p->cypherMechanism, &param, buffer.get(), &len, len, clear_text, static_cast<unsigned int>(data.size()));

    if (s != SECSuccess) {
        error = CantEncrypt;
        return "";
    }

    error = Success;

    return std::string(reinterpret_cast<const char*>(buffer.get()), len);
}

/**
 *  \brief Decrypts the contens of \c data.
 *
 *  Decrypts \c data using the method choosen when constructing the object.
 *  Uses Crypto#error to report errors.
 *
 *  \arg \c data Data to be decrypted.
 *
 *  \return The decrypted data.
 *
 *  \see Crypto
 *  \see CryptoParams
 *  \see Crypto#encrypt
 *  \see Crypto#error
 */
std::string Crypto::decrypt(const std::string &data)
{
    CK_GCM_PARAMS gcmParams;
    SECItem       param;

    param.type          = siBuffer;
    param.data          = reinterpret_cast<unsigned char*>(&gcmParams);
    param.len           = sizeof(gcmParams);
    gcmParams.pIv       = _p->IV->data;
    gcmParams.ulIvLen   = _p->IV->len;
    gcmParams.pAAD      = nullptr;
    gcmParams.ulAADLen  = 0;
    gcmParams.ulTagBits = AES_BLOCK_SIZE * 8;

    unsigned int                   len = static_cast<unsigned int>(data.size() + 2 * _p->IV->len);
    std::unique_ptr<unsigned char> buffer(new unsigned char[len]);

    auto cipher = reinterpret_cast<const unsigned char*>(data.data());

    SECStatus s = PK11_Decrypt(_p->SymKey.get(), _p->cypherMechanism, &param, buffer.get(), &len, len, cipher, static_cast<unsigned int>(data.size()));

    if (s != SECSuccess) {
        error = CantDecrypt;
        return "";
    }

    error = Success;

    return std::string(reinterpret_cast<const char*>(buffer.get()), len);
}

/**
 *  \brief Returns the key used in this object as std::string. Available, but not recommended.
 *
 *  \return The symmetric key as std::string.
 */
std::string Crypto::key() const
{
    PK11_ExtractKeyValue(_p->SymKey.get());
    SECItem *key = PK11_GetKeyData(_p->SymKey.get());

    return std::string(reinterpret_cast<const char*>(key->data), key->len);
}

/**
 *  \brief Returns the IV used by this object.
 *
 *  \return The IV as std::string.
 */
std::string Crypto::iv() const
{
    return std::string(reinterpret_cast<const char*>(_p->IV->data), _p->IV->len);
}

/**
 *  \brief Fills CryptoPrivate fields based on configuration found in params.
 *
 *  \arg \c params CryptoParams struct containing all the configuration parameters.
 */
void CryptoPrivate::parseParams(const CryptoParams params)
{
    switch (params.encryption) {
        case AES_GCM_128:
            cypherMechanism = CKM_AES_GCM;
            keySize         = AES_128_KEY_LENGTH;
            keyDeriveMethod = SEC_OID_AES_128_CBC;
            break;

        case AES_GCM_256:
            cypherMechanism = CKM_AES_GCM;
            keySize         = AES_256_KEY_LENGTH;
            keyDeriveMethod = SEC_OID_AES_256_CBC;
            break;
    }

    switch (params.keyDerivationHash) {
        case HMAC_SHA256:
            keyDerivationHash = SEC_OID_HMAC_SHA256;
            break;

        case HMAC_SHA512:
            keyDerivationHash = SEC_OID_HMAC_SHA512;
            break;
    }

    switch (params.digest) {
        case SHA256:
            digestType = SEC_OID_SHA256;
            digestSize = SHA256_BLOCK_LENGTH;
            break;

        case SHA512:
            digestType = SEC_OID_SHA512;
            digestSize = SHA512_BLOCK_LENGTH;
            break;
    }

    keyDeriveFunction = SEC_OID_PKCS5_PBKDF2;
    keyDerivationCost = params.keyDerivationCost;
}

/**
 *  \brief Initializes NSS
 */
void CryptoPrivate::initNSS()
{
    if (!initialized) {
        NSS_NoDB_Init(".");
        initialized = true;

        uint8_t random[SEED_BLOCK_SIZE];
        auto    s    = PR_GetRandomNoise(random, SEED_BLOCK_SIZE);
        auto    slot = PK11_GetBestSlot(CKM_FAKE_RANDOM, nullptr);
        PK11_SeedRandom(slot, random, static_cast<int>(s));
    }
}

/**
 *  \brief Turns a std::string into a SECItem object and returns its SECItemPtr
 *  wrapper.
 *
 *  \arg \c str The string to be converted.
 *
 *  \return A SECItemPtr
 */
SECItemPtr CryptoPrivate::stringToSECItemPtr(std::string str)
{
    // Freed in destructor
    unsigned char *data = new unsigned char[str.size()];
    SECItemPtr    secItem(SECITEM_AllocItem(nullptr, nullptr, 0), SECITEM_Free);

    memcpy(data, str.data(), str.size());

    secItem->type = siBuffer;
    secItem->data = data;
    secItem->len  = static_cast<unsigned int>(str.size());

    return secItem;
}

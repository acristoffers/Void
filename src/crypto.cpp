#include "crypto.h"

#include <iostream>
#include <math.h>

#include <blapit.h>
#include <nss.h>
#include <nssb64.h>
#include <pk11pub.h>
#include <prrng.h>
#include <secoid.h>

void SECITEM_Free(SECItem *item)
{
    SECITEM_FreeItem(item, PR_TRUE);
}

using PK11SlotInfoPtr = std::unique_ptr<PK11SlotInfo, decltype( &PK11_FreeSlot)>;
using PK11SymKeyPtr   = std::unique_ptr<PK11SymKey, decltype( &PK11_FreeSymKey)>;
using SECItemPtr      = std::unique_ptr<SECItem, decltype( &SECITEM_Free)>;

struct CryptoPrivate
{
    PK11SlotInfoPtr slot   = PK11SlotInfoPtr(nullptr, PK11_FreeSlot);
    PK11SymKeyPtr   SymKey = PK11SymKeyPtr(nullptr, PK11_FreeSymKey);
    SECItemPtr      IV     = SECItemPtr(nullptr, SECITEM_Free);

    CK_MECHANISM_TYPE cypherMechanism;
    unsigned int      keySize;         // In bytes
    SECOidTag         keyDeriveFunction;
    SECOidTag         keyDeriveMethod;
    SECOidTag         keyDerivationHash;
    unsigned int      keyDerivationCost;
    SECOidTag         digestType;
    unsigned int      digestSize;
    void              parseParams(CryptoParams params);

    static bool initialized;
    void        initNSS();

    static SECItemPtr stringToSECItemPtr(std::string);
};

bool CryptoPrivate::initialized = false;

std::string Crypto::generateRandom(uint8_t size)
{
    if ( !CryptoPrivate::initialized ) {
        NSS_NoDB_Init(".");
        CryptoPrivate::initialized = true;
    }

    uint8_t random[size];

    auto s    = PR_GetRandomNoise(random, SEED_BLOCK_SIZE);
    auto slot = PK11_GetBestSlot(CKM_FAKE_RANDOM, nullptr);

    PK11_SeedRandom(slot, random, s);
    PK11_GenerateRandom(random, size);
    PK11_FreeSlot(slot);

    return std::string(reinterpret_cast<const char *> (random), size);
}

std::string Crypto::digest(std::string &data, CryptoParams params)
{
    CryptoPrivate _p;

    _p.parseParams(params);

    auto          data_c      = reinterpret_cast<const unsigned char *> ( data.data() );
    unsigned int  digest_size = _p.digestSize;
    unsigned char digest[digest_size];

    auto context = PK11_CreateDigestContext(_p.digestType);

    PK11_DigestBegin(context);
    PK11_DigestOp( context, data_c, data.size() );
    PK11_DigestFinal(context, digest, &digest_size, digest_size);
    PK11_DestroyContext(context, PR_TRUE);

    return std::string(reinterpret_cast<const char *> (digest), digest_size);
}

std::string Crypto::stringToHex(const std::string &input, std::string separator)
{
    static const char *const lut = "0123456789ABCDEF";

    std::string output;
    bool        first = true;

    output.reserve( ( 2 + separator.size() ) * input.length() );

    for ( size_t i = 0; i < input.size(); ++i ) {
        if ( !first ) {
            output += separator;
        }

        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);

        first = false;
    }

    return output;
}

std::string Crypto::toBase64(const std::string &data)
{
    auto        secItem   = CryptoPrivate::stringToSECItemPtr(data);
    char        *base64_c = NSSBase64_EncodeItem( nullptr, nullptr, 0, secItem.get() );
    std::string base64( base64_c, strlen(base64_c) );

    delete base64_c;
    return base64;
}

std::string Crypto::fromBase64(const std::string &data)
{
    SECItemPtr secItem(new SECItem, SECITEM_Free);

    NSSBase64_DecodeBuffer( nullptr, secItem.get(), data.data(), data.size() );

    return std::string(reinterpret_cast<const char *> (secItem->data), secItem->len);
}

Crypto::Crypto(std::string &password, std::string &salt, std::string &iv, CryptoParams params)
{
    _p.reset(new CryptoPrivate);
    _p->parseParams(params);
    _p->initNSS();

    auto PswdSec = _p->stringToSECItemPtr(password);
    auto SaltSec = _p->stringToSECItemPtr(salt);

    SECAlgorithmID *algID = PK11_CreatePBEV2AlgorithmID( _p->keyDeriveFunction, _p->keyDeriveMethod, _p->keyDerivationHash, _p->keySize, _p->keyDerivationCost, SaltSec.get() );
    PK11SlotInfo   *slot  = PK11_GetBestSlot(_p->keyDeriveFunction, nullptr);
    auto           symKey = PK11_PBEKeyGen(slot, algID, PswdSec.get(), PR_FALSE, nullptr);
    PK11_FreeSlot(slot);
    SECOID_DestroyAlgorithmID(algID, PR_TRUE);

    _p->IV = _p->stringToSECItemPtr(iv);
    _p->SymKey.reset(symKey);
}

Crypto::Crypto(std::string &key, std::string &iv, CryptoParams params)
{
    _p.reset(new CryptoPrivate);
    _p->parseParams(params);
    _p->initNSS();

    _p->IV = _p->stringToSECItemPtr(iv);
    auto secKey = _p->stringToSECItemPtr(key);
    auto symKey = PK11_ImportSymKey(_p->slot.get(), _p->cypherMechanism, PK11_OriginUnwrap, CKA_ENCRYPT, secKey.get(), nullptr);
    _p->SymKey.reset(symKey);
}

Crypto::~Crypto() = default;

std::string Crypto::encrypt(std::string data)
{
    CK_GCM_PARAMS gcmParams;
    SECItem       param;

    param.type          = siBuffer;
    param.data          = (unsigned char *) &gcmParams;
    param.len           = sizeof(gcmParams);
    gcmParams.pIv       = _p->IV->data;
    gcmParams.ulIvLen   = _p->IV->len;
    gcmParams.pAAD      = nullptr;
    gcmParams.ulAADLen  = 0;
    gcmParams.ulTagBits = AES_BLOCK_SIZE * 8;

    unsigned int  len = data.size() + 2 * _p->IV->len;
    unsigned char buffer[len];

    auto clear_text = reinterpret_cast<const unsigned char *> ( data.data() );

    PK11_Encrypt( _p->SymKey.get(), _p->cypherMechanism, &param, buffer, &len, len, clear_text, data.size() );

    return std::string(reinterpret_cast<const char *> (buffer), len);
}

std::string Crypto::decrypt(std::string data)
{
    CK_GCM_PARAMS gcmParams;
    SECItem       param;

    param.type          = siBuffer;
    param.data          = (unsigned char *) &gcmParams;
    param.len           = sizeof(gcmParams);
    gcmParams.pIv       = _p->IV->data;
    gcmParams.ulIvLen   = _p->IV->len;
    gcmParams.pAAD      = nullptr;
    gcmParams.ulAADLen  = 0;
    gcmParams.ulTagBits = AES_BLOCK_SIZE * 8;

    unsigned int  len = data.size() + 2 * _p->IV->len;
    unsigned char buffer[len];

    auto cipher = reinterpret_cast<const unsigned char *> ( data.data() );

    PK11_Decrypt( _p->SymKey.get(), _p->cypherMechanism, &param, buffer, &len, len, cipher, data.size() );

    return std::string(reinterpret_cast<const char *> (buffer), len);
}

std::string Crypto::key()
{
    PK11_ExtractKeyValue( _p->SymKey.get() );
    SECItem *key = PK11_GetKeyData( _p->SymKey.get() );
    return std::string(reinterpret_cast<const char *> (key->data), key->len);
}

std::string Crypto::iv()
{
    return std::string(reinterpret_cast<const char *> (_p->IV->data), _p->IV->len);
}

void CryptoPrivate::parseParams(CryptoParams params)
{
    switch ( params.encryption ) {
        case AES_GCM_128:
            cypherMechanism = CKM_AES_GCM;
            keySize         = AES_128_KEY_LENGTH;
            keyDeriveMethod = SEC_OID_AES_128_CBC;
            break;

        default:
            cypherMechanism = CKM_AES_GCM;
            keySize         = AES_256_KEY_LENGTH;
            keyDeriveMethod = SEC_OID_AES_256_CBC;
            break;
    }

    switch ( params.keyDerivationHash ) {
        case HMAC_SHA256:
            keyDerivationHash = SEC_OID_HMAC_SHA256;
            break;

        default:
            keyDerivationHash = SEC_OID_HMAC_SHA512;
            break;
    }

    switch ( params.digest ) {
        case SHA256:
            digestType = SEC_OID_SHA256;
            digestSize = SHA256_BLOCK_LENGTH;
            break;

        default:
            digestType = SEC_OID_SHA512;
            digestSize = SHA512_BLOCK_LENGTH;
            break;
    }

    keyDeriveFunction = SEC_OID_PKCS5_PBKDF2;
    keyDerivationCost = params.keyDerivationCost;
}

void CryptoPrivate::initNSS()
{
    if ( !initialized ) {
        NSS_NoDB_Init(".");
        initialized = true;
    }

    slot.reset( PK11_GetBestSlot(cypherMechanism, nullptr) );
}

SECItemPtr CryptoPrivate::stringToSECItemPtr(std::string str)
{
    // Freed in destructor
    unsigned char *data = new unsigned char[str.size()];
    SECItemPtr    secItem(new SECItem, SECITEM_Free);

    memcpy( data, str.data(), str.size() );

    secItem->type = siBuffer;
    secItem->data = data;
    secItem->len  = str.size();

    return secItem;
}

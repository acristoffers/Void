#include "crypto.h"

#include <iostream>
#include <math.h>
#include <string>

#include <nss.h>
#include <pk11pub.h>

#include <libscrypt.h>

#define KEY_LENGTH 32

struct CryptoPrivate
{
    CK_MECHANISM_TYPE cypherMechanism = CKM_AES_GCM;
    PK11SlotInfo      *slot           = nullptr;
    PK11SymKey        *SymKey         = nullptr;
    SECItem           key;
    SECItem           IV;

    static bool initialized;
    void        initNSS();
};

bool CryptoPrivate::initialized = false;

std::string Crypto::generateSalt(uint8_t size)
{
    uint8_t salt[size];

    libscrypt_salt_gen(salt, size);

    return std::string(reinterpret_cast<const char *> (salt), size);
}

Crypto::Crypto(std::string &password, std::string &salt, std::string &iv)
{
    _p.reset(new CryptoPrivate);
    _p->initNSS();

    // Salt being passed to function makes absolutely no difference,
    // so I'm salting the password here
    password += salt;

    auto pswd_b   = reinterpret_cast<const uint8_t *> ( password.data() );
    auto pswd_len = password.size();

    auto salt_b   = reinterpret_cast<const uint8_t *> ( password.data() );
    auto salt_len = password.size();

    uint8_t key[KEY_LENGTH];

    libscrypt_scrypt(pswd_b, pswd_len, salt_b, salt_len, pow(2, 16), 8, 16, key, KEY_LENGTH);

    unsigned char *key_data = new unsigned char[KEY_LENGTH];
    memcpy(key_data, key, KEY_LENGTH);
    memset(key, 0, KEY_LENGTH);

    _p->key.type = siBuffer;
    _p->key.data = key_data;
    _p->key.len  = KEY_LENGTH;

    unsigned char *iv_data = new unsigned char[iv.size()];
    memcpy( iv_data, iv.data(), iv.size() );

    _p->IV.type = siBuffer;
    _p->IV.data = iv_data;
    _p->IV.len  = iv.size();

    _p->SymKey = PK11_ImportSymKey(_p->slot, _p->cypherMechanism, PK11_OriginUnwrap, CKA_ENCRYPT, &_p->key, nullptr);
}

Crypto::Crypto(std::string &key, std::string &iv)
{
    _p.reset(new CryptoPrivate);
    _p->initNSS();

    unsigned char *key_data = new unsigned char[key.size()];
    memcpy( key_data, key.data(), key.size() );

    _p->key.type = siBuffer;
    _p->key.data = key_data;
    _p->key.len  = key.size();

    unsigned char *iv_data = new unsigned char[iv.size()];
    memcpy( iv_data, iv.data(), iv.size() );

    _p->IV.type = siBuffer;
    _p->IV.data = iv_data;
    _p->IV.len  = iv.size();

    _p->SymKey = PK11_ImportSymKey(_p->slot, _p->cypherMechanism, PK11_OriginUnwrap, CKA_ENCRYPT, &_p->key, nullptr);
}

Crypto::~Crypto() = default;

std::string Crypto::encrypt(std::string data)
{
    CK_GCM_PARAMS gcmParams;
    SECItem       param;

    param.type          = siBuffer;
    param.data          = (unsigned char *) &gcmParams;
    param.len           = sizeof(gcmParams);
    gcmParams.pIv       = _p->IV.data;
    gcmParams.ulIvLen   = _p->IV.len;
    gcmParams.pAAD      = nullptr;
    gcmParams.ulAADLen  = 0;
    gcmParams.ulTagBits = 16 * 8;

    unsigned int  len = data.size() + 2 * _p->IV.len;
    unsigned char buffer[len];

    auto clear_text = reinterpret_cast<const unsigned char *> ( data.data() );

    PK11_Encrypt( _p->SymKey, _p->cypherMechanism, &param, buffer, &len, len, clear_text, data.size() );

    return std::string(reinterpret_cast<const char *> (buffer), len);
}

std::string Crypto::decrypt(std::string data)
{
    CK_GCM_PARAMS gcmParams;
    SECItem       param;

    param.type          = siBuffer;
    param.data          = (unsigned char *) &gcmParams;
    param.len           = sizeof(gcmParams);
    gcmParams.pIv       = _p->IV.data;
    gcmParams.ulIvLen   = _p->IV.len;
    gcmParams.pAAD      = nullptr;
    gcmParams.ulAADLen  = 0;
    gcmParams.ulTagBits = 16 * 8;

    unsigned int  len = data.size() + 2 * _p->IV.len;
    unsigned char buffer[len];

    auto cipher = reinterpret_cast<const unsigned char *> ( data.data() );

    PK11_Decrypt( _p->SymKey, _p->cypherMechanism, &param, buffer, &len, len, cipher, data.size() );

    return std::string(reinterpret_cast<const char *> (buffer), len);
}

std::string Crypto::key()
{
    return std::string(reinterpret_cast<const char *> (_p->key.data), _p->key.len);
}

std::string Crypto::iv()
{
    return std::string(reinterpret_cast<const char *> (_p->IV.data), _p->IV.len);
}

void CryptoPrivate::initNSS()
{
    if ( !initialized ) {
        NSS_NoDB_Init(".");
        initialized = true;
    }

    slot = PK11_GetBestSlot(cypherMechanism, nullptr);
}

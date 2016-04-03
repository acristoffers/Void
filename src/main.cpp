#include <iostream>
#include <iomanip>

#include "crypto.h"

int main(int argc, char *argv[])
{
    std::string pass = "pass";
    std::string salt = Crypto::generateRandom(16);
    std::string iv   = Crypto::generateRandom(16);

    Crypto c(pass, salt, iv);

    std::cout << "salt:\t" << Crypto::stringToHex(salt) << std::endl;
    std::cout << "IV:\t" << Crypto::stringToHex( c.iv() ) << std::endl;
    std::cout << "key:\t" << Crypto::stringToHex( c.key() ) << std::endl;

    std::string message = "Hello World!";

    std::string base64 = Crypto::toBase64(message);

    std::cout << "msg:\t" << message << std::endl;

    std::cout << "b64:\t" << base64 << std::endl;
    std::cout << "bck:\t" << Crypto::fromBase64(base64) << std::endl;

    message = c.encrypt(message);

    std::cout << "enc:\t" << Crypto::stringToHex(message) << std::endl;

    message = c.decrypt(message);

    std::cout << "dec:\t" << message << std::endl;

    message = Crypto::digest(message);

    std::cout << "SHA:\t" << Crypto::stringToHex(message) << std::endl;

    return 0;
}

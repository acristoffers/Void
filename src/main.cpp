#include <iostream>
#include <iomanip>

#include "crypto.h"

std::string string_to_hex(const std::string &input)
{
    static const char *const lut = "0123456789ABCDEF";
    size_t                   len = input.length();

    std::string output;
    auto        first = true;

    output.reserve(2 * len);
    for ( size_t i = 0; i < len; ++i ) {
        if ( !first ) {
            output.push_back(':');
        }
        first = false;
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

int main(int argc, char *argv[])
{
    std::string pass = "pass";
    std::string salt = Crypto::generateSalt(16);
    std::string iv   = Crypto::generateSalt(16);

    Crypto c(pass, salt, iv);

    std::cout << "salt:\t" << string_to_hex(salt) << std::endl;
    std::cout << "IV:\t" << string_to_hex(iv) << std::endl;
    std::cout << "key:\t" << string_to_hex( c.key() ) << std::endl;

    std::string message = "Hello World!";

    std::cout << "msg:\t" << message << std::endl;

    message = c.encrypt(message);

    std::cout << "enc:\t" << string_to_hex(message) << std::endl;

    message = c.decrypt(message);

    std::cout << "dec:\t" << message << std::endl;

    return 0;
}

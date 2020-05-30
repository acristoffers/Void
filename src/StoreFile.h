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

#ifndef STOREFILE_H
#define STOREFILE_H

#include <memory>

#include <QString>

#include "Crypto.h"

struct StoreFilePrivate;

struct StoreFile
{
    /**
     *  \brief Errors returned by StoreFile
     */
    enum StoreFileError : uint8_t {
        Success,     /*!< Success */
        CantOpenFile /*!< The Store.void file could not be opened. */
    }

    /**
     *  \brief Errors returned by StoreFile
     */
    error;

    StoreFile(QString path);
    ~StoreFile();

    void setCryptoParams(const CryptoParams params);
    void setData(const QByteArray data);
    void setIV(const std::string iv);
    void setSalt(const std::string salt);

    CryptoParams cryptoParams() const;
    QByteArray   data() const;
    std::string  IV() const;
    std::string  salt() const;

private:
    std::unique_ptr<StoreFilePrivate> _p;
};

#endif // STOREFILE_H

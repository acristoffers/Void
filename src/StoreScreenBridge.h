/*
 *  Copyright (c) 2015 Álan Crístoffer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */

#ifndef STORESCREENBRIDGE_H
#define STORESCREENBRIDGE_H

#include <QObject>

#include "Store.h"
#include "StoreScreen.h"

struct StoreScreenBridgePrivate;

class StoreScreenBridge : public QObject
{
    Q_OBJECT
public:
    StoreScreenBridge(const QString &path, const QString &password, const bool create, StoreScreen* parent);
    ~StoreScreenBridge();

    Q_INVOKABLE void setLang(QString lang);
    Q_INVOKABLE QString lang();

    Store::StoreError error;
private:
    std::unique_ptr<StoreScreenBridgePrivate> _p;
};

#endif // STORESCREENBRIDGE_H

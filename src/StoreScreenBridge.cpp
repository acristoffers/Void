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

#include "StoreScreenBridge.h"

#include <QSettings>

struct StoreScreenBridgePrivate
{
    std::unique_ptr<Store> store;
};

StoreScreenBridge::StoreScreenBridge(const QString &path, const QString &password, const bool create) : QObject()
{
    _p.reset(new StoreScreenBridgePrivate);
    _p->store.reset( new Store(path, password, create) );
    error = _p->store->error;
}

StoreScreenBridge::~StoreScreenBridge() = default;

void StoreScreenBridge::setLang(QString lang)
{
    QSettings settings;

    settings.setValue("lang", lang);
}

QString StoreScreenBridge::lang()
{
    QSettings settings;

    if ( !settings.contains("lang") ) {
        settings.setValue("lang", "en");
    }

    return settings.value("lang").toString();
}
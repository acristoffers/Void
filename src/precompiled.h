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

#ifndef PRECOMPILED_H
#define PRECOMPILED_H
#ifndef __OBJC__
 #include <blapit.h>
 #include <nss.h>
 #include <nssb64.h>
 #include <pk11pub.h>
 #include <prrng.h>
 #include <secoid.h>

 #include <QByteArray>
 #include <QDataStream>
 #include <QDir>
 #include <QFile>
 #include <QList>
 #include <QMap>
 #include <QMimeDatabase>
 #include <QObject>
 #include <QRegularExpression>
 #include <QString>
 #include <QtTest/QtTest>

 #include <iostream>
 #include <math.h>
 #include <memory>
 #include <string>

 #include "Crypto.h"
 #include "Store.h"
 #include "StoreFS.h"
 #include "StoreFile.h"
#endif
#endif // PRECOMPILED_H

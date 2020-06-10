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

#include <QApplication>
#include <QIcon>
#include <QWebEngineSettings>
#include <QWebEngineUrlScheme>

#include "WelcomeScreen.h"
#include "VideoPlayer.h"

int main(int argc, char **argv)
{
    QWebEngineUrlScheme decryptScheme("decrypt");
    QWebEngineUrlScheme thumbScheme("thumb");
    decryptScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
    decryptScheme.setFlags(QWebEngineUrlScheme::Flag::LocalScheme);
    thumbScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
    thumbScheme.setFlags(QWebEngineUrlScheme::Flag::LocalScheme);
    QWebEngineUrlScheme::registerScheme(decryptScheme);
    QWebEngineUrlScheme::registerScheme(thumbScheme);

    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(QStringLiteral("qrc:/icon.png")));
    app.setApplicationDisplayName(QStringLiteral("Void"));
    app.setApplicationName(QStringLiteral("Void"));
    app.setApplicationVersion("1.0");
    app.setOrganizationDomain(QStringLiteral("acristoffers.me"));
    app.setOrganizationName(QStringLiteral("Alan Cristoffer"));

    auto fse = QWebEngineSettings::FullScreenSupportEnabled;
    QWebEngineSettings::globalSettings()->setAttribute(fse, false);

    // Deletes itself on close
    new WelcomeScreen;

    return app.exec();
}

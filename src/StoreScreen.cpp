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

#include "StoreScreen.h"

#include <QApplication>
#include <QContextMenuEvent>
#include <QDesktopWidget>
#include <QSettings>
#include <QStyle>
#include <QWebChannel>

#include "StoreScreenBridge.h"

struct StoreScreenPrivate
{
    std::unique_ptr<StoreScreenBridge> bridge;
    std::unique_ptr<QWebChannel>       channel;
};

StoreScreen::StoreScreen(const QString &path, const QString &password, const bool create) : QWebEngineView()
{
    _p.reset(new StoreScreenPrivate);
    _p->channel.reset(new QWebChannel);
    _p->bridge.reset( new StoreScreenBridge(path, password, create) );

    error = _p->bridge->error;
    if ( error != Store::Success ) {
        return;
    }

    page()->setBackgroundColor("#222222");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() & ~Qt::WindowFullscreenButtonHint);

    QSettings settings;
    if ( settings.contains("storeWindowGeometry") ) {
        restoreGeometry( settings.value("storeWindowGeometry").toByteArray() );
    } else {
        setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, QSize(1024, 768), qApp->desktop()->availableGeometry() ) );
        settings.setValue( "storeWindowGeometry", saveGeometry() );
    }

    load( QUrl("qrc:/html/store.htm") );

    page()->setWebChannel( _p->channel.get() );
    _p->channel->registerObject( QStringLiteral("store_bridge"), _p->bridge.get() );

    show();
}

StoreScreen::~StoreScreen() = default;

void StoreScreen::changeEvent(QEvent *e)
{
    // There is a bug that makes the screen go blank (gray actually)
    // when restoring from minimize. This works around that.
    if ( e->type() == QEvent::WindowStateChange ) {
        QWindowStateChangeEvent *event = static_cast<QWindowStateChangeEvent *> (e);

        if ( event->oldState() & Qt::WindowMinimized ) {
            hide();
            show();
        }
    }
}

void StoreScreen::closeEvent(QCloseEvent *)
{
    QSettings settings;

    settings.setValue( "storeWindowGeometry", saveGeometry() );
}

void StoreScreen::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
}

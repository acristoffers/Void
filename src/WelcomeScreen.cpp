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

#include "WelcomeScreen.h"

#include <QApplication>
#include <QContextMenuEvent>
#include <QDesktopWidget>
#include <QStyle>
#include <QWebChannel>

#include <WelcomeScreenBridge.h>

struct WelcomeScreenPrivate
{
    std::unique_ptr<WelcomeScreenBridge> bridge;
    std::unique_ptr<QWebChannel>         channel;
};

WelcomeScreen::WelcomeScreen() : QWebEngineView()
{
    _p.reset(new WelcomeScreenPrivate);
    _p->channel.reset(new QWebChannel);
    _p->bridge.reset( new WelcomeScreenBridge(this) );

    page()->setBackgroundColor("#222222");
    setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, QSize(400, 600), qApp->desktop()->availableGeometry() ) );
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() & ~Qt::WindowFullscreenButtonHint & ~Qt::WindowMaximizeButtonHint);
    setMaximumSize(400, 600);
    setMinimumSize(400, 600);

    load( QUrl("qrc:/html/ws.htm") );

    page()->setWebChannel( _p->channel.get() );
    _p->channel->registerObject( QStringLiteral("welcome_bridge"), _p->bridge.get() );

    show();
}

WelcomeScreen::~WelcomeScreen() = default;

void WelcomeScreen::changeEvent(QEvent *e)
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

void WelcomeScreen::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
}

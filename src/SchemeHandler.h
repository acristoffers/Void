#ifndef SCHEMEHANDLER_H
#define SCHEMEHANDLER_H

#include <QWebEngineUrlSchemeHandler>

class Store;
struct SchemeHandlerPrivate;

class SchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT
public:
    SchemeHandler(std::shared_ptr<Store> );
    ~SchemeHandler();
    void requestStarted(QWebEngineUrlRequestJob *) override;

private:
    std::unique_ptr<SchemeHandlerPrivate> _p;
};

#endif // SCHEMEHANDLER_H

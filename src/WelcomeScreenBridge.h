#ifndef WELCOMESCREENCHANELOBJECT_H
#define WELCOMESCREENCHANELOBJECT_H

#include <memory>

#include <QObject>

#include "Store.h"
#include "WelcomeScreen.h"

struct WelcomeScreenBridgePrivate;

class WelcomeScreenBridge : public QObject
{
    Q_OBJECT
public:
    WelcomeScreenBridge(WelcomeScreen *parent);
    ~WelcomeScreenBridge();

    Q_INVOKABLE bool getExisting();
    Q_INVOKABLE bool getNew();
    Q_INVOKABLE QString open(const QString password);
    Q_INVOKABLE QString create(const QString password);
    Q_INVOKABLE void close();

    static QString StoreError2QString(Store::StoreError);

private:
    std::unique_ptr<WelcomeScreenBridgePrivate> _p;
};

#endif // WELCOMESCREENCHANELOBJECT_H

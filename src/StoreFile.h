#ifndef STOREFILE_H
#define STOREFILE_H

#include <memory>

#include <QObject>
#include <QString>

struct StoreFilePrivate;

class StoreFile : public QObject
{
    Q_OBJECT
public:
    StoreFile(QString path);
    ~StoreFile();

    void setSalt(std::string salt);
    void setIV(std::string iv);
    void setData(QByteArray data);

    std::string salt();
    std::string IV();
    QByteArray data();

private:
    std::unique_ptr<StoreFilePrivate> _p;
};
#endif // STOREFILE_H

#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "opedb.h"
#include<QDir>
#include<QFile>
#include<QTimer>
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString getName();
    static MyTcpSocket& getInstance();
signals:
    void offline(MyTcpSocket* my);
public slots:
    void resMsg();
    void clientOffOnline();
    void sendFile();

private:
    QString na;
    QFile file;
    qint64 totalSize;
    qint64 resSize;
    bool isupload;
    bool shareFile;
    QTimer* timer;
};


#endif // MYTCPSOCKET_H

#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H
#include <QTcpServer>
#include <mytcpsocket.h>
#include <QList>
#include "protocol.h"
class mytcpserver : public QTcpServer
{
    Q_OBJECT
public:
    mytcpserver();
    static mytcpserver& getInstance();
    virtual void incomingConnection(qintptr socket);
    void resend(const char* userName,PDU* p);
    void resendAll(PDU* p);
public slots:
    void deleteSocket(MyTcpSocket* mySocket);
private:
    QList<MyTcpSocket*> list;
};

#endif // MYTCPSERVER_H

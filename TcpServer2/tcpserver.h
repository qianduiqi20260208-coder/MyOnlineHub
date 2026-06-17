#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTcpServer>

QT_BEGIN_NAMESPACE
namespace Ui { class TcpServer; }
QT_END_NAMESPACE

class TcpServer : public QWidget
{
    Q_OBJECT

public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    void loadConfig();

private:
    Ui::TcpServer *ui;
    QString ip;
    quint32 port;

};
#endif // TCPSERVER_H

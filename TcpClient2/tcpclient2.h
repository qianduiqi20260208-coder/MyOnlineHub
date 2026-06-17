#ifndef TCPCLIENT2_H
#define TCPCLIENT2_H

#include <QWidget>

#include <QDebug>
#include <QByteArray>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include<QBrush>
#include<QFileDialog>
QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient2; }
QT_END_NAMESPACE

class TcpClient2 : public QWidget
{
    Q_OBJECT

public:
    TcpClient2(QWidget *parent = nullptr);
    ~TcpClient2();

    void loadconfig();
    static TcpClient2& getInstance();
    QString getName();
    QString loginN();
    QString getPath();
    QTcpSocket* getSocket();
    QString getRandomString(int length);

private slots:
    //void on_sendButton_clicked();
    void showConnection();
    void resMsg();
    void on_loginButton_clicked();

    void on_registButton_clicked();

private:
    Ui::TcpClient2 *ui;
    QTcpSocket* socket;
    QString ip;
    int port;
    //获取名字
    QString loginName;
    QString curPath;

    quint64 totalSize;
    quint64 resSize;
    bool isDownload;
    QFile file;

};
#endif // TCPCLIENT2_H

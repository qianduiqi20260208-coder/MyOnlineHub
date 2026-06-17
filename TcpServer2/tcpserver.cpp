#include "tcpserver.h"
#include "ui_tcpserver.h"
#include <mytcpserver.h>
TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    mytcpserver::getInstance().listen(QHostAddress(ip),port);

}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    //定义文件对象
    QFile file(":/server.config");
    //打开文件，并且判断是否成功打开
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray q= file.readAll();
        //首先转换成标准字符串，然后转换成C字符串
        QString s=q.toStdString().c_str();
        //替换字符，并且分隔字符串
        s.replace("\r\n"," ");
        QStringList sl=s.split(" ");
        ip=sl.at(0);
        port=sl.at(1).toUShort();

        file.close();
    }
    else
    {
        QMessageBox::critical(this,"open config","open fail");
    }
}


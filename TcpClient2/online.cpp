#include "online.h"
#include "ui_online.h"
#include <tcpclient2.h>

online::online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::online)
{
    ui->setupUi(this);
}

online::~online()
{
    delete ui;
}

void online::showUsers(PDU *p)
{
    if(NULL==p)
        return;

    uint count=p->uiMsgLen/32;
    char ca[32];
    for(int i=0;i<count;i++)
    {
        memcpy(ca,(char*)p->caMsg+i*32,32);
        ui->listWidget->addItem(ca);
    }
}



void online::on_pushButton_clicked()
{
    QListWidgetItem * l=ui->listWidget->currentItem();
    //打印文本
    qDebug()<<l->text();
    QString userName=l->text();
    QString loginName=TcpClient2::getInstance().loginN();
    PDU* pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData,userName.toStdString().c_str(),userName.size());
    memcpy(pdu->caData+32,loginName.toStdString().c_str(),loginName.size());
     TcpClient2::getInstance().getSocket()->write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}


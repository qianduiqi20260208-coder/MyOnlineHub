#include "sharefilewidget.h"
#include "opewidget.h"
#include "protocol.h"
#include<QDebug>
#include "tcpclient2.h"
#include <QMessageBox>
shareFileWidget::shareFileWidget(QWidget *parent)
    : QWidget{parent}
{
    listWidget=opeWidget::getInstance().getFriend()->getFriendList();


    shareButton=new QPushButton("点击分享文件");
    hboxLayout=new QHBoxLayout;
    hboxLayout->addWidget(listWidget);
    hboxLayout->addWidget(shareButton);
    timer=new QTimer;
    setLayout(hboxLayout);
    connect(shareButton,SIGNAL(clicked(bool)),this,SLOT(shareFile()));
    connect(timer,SIGNAL(timeout()),this,SLOT(sendFile()));
}

void shareFileWidget::shareFile()
{
    QString a=opeWidget::getInstance().getBook()->getResFilePath();
    QString b=TcpClient2::getInstance().getName();

    //这里需要多进行一步操作，点击刷新好友列表，这样才能将好友的名字传递过来
    QString name=listWidget->currentItem()->text();


    QString path=TcpClient2::getInstance().getPath();
    //拼接字符串，用来获取要发送文件的大小
    QString c=path+'/'+b+'/'+a;


    f.setFileName(c);
    qint64 d=f.size();

    qDebug()<<"文件大小"<<d;
    QString filePath=path+'/'+name+'/'+a;
    qDebug()<<"文件的路径为："<<filePath;
    PDU* p=mkPDU(filePath.size()+1);
    sprintf(p->caData,"%lld",d);
    p->uiMsgType=ENUM_SHARE_FILE_REQUEST;
    strncpy((char*)p->caMsg,filePath.toStdString().c_str(),filePath.size());
    TcpClient2::getInstance().getSocket()->write((char*)p,p->uiPDULen);
    free(p);
    p=NULL;
    timer->start();
}

void shareFileWidget::sendFile()
{
    timer->stop();
    qDebug()<<"进入到分享文件的方法里...";

    qint64 size;
    if(f.open(QIODevice::ReadOnly))
    {

        char* buffer=new char[4096];
        while(true)
        {


            size=f.read(buffer,4096);
            if(size>0&&size<=4096)
            {
                TcpClient2::getInstance().getSocket()->write(buffer,size);
            }else if(size==0)
            {
                break;
            }else{
                QMessageBox::warning(this,"分享文件","读取文件失败");
            }

        }
        delete []buffer;
        buffer=NULL;
    }else{
        QMessageBox::critical(this,"分享文件","打开文件失败");
    }
    f.close();

}

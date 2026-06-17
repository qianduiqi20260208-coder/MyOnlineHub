#include "mytcpserver.h"

mytcpserver::mytcpserver()
{

}

mytcpserver &mytcpserver::getInstance()
{
    static mytcpserver instance;
    return instance;
}

void mytcpserver::incomingConnection(qintptr socket)
{
    qDebug()<<"new client connected";
    //在这里使用已经定义好的类
    MyTcpSocket* my=new MyTcpSocket;
    my->setSocketDescriptor(socket);
    list.append(my);
    //设置用户下线的信号
    connect(my,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deleteSocket(MyTcpSocket*)));
}

void mytcpserver::resend(const char *userName, PDU *p)
{
    if(NULL==userName||NULL==p)
    {
        return;
    }
    QString strName=userName;
    for(int i=0;i<list.size();i++)
    {
        //找到要发送的那个socket
        if(strName==list.at(i)->getName())
        {
            list.at(i)->write((char*)p,p->uiPDULen);
            break;
        }
    }

}
//转发给所有用户
void mytcpserver::resendAll(PDU *p)
{
    if(NULL==p)
        return;
    for(int i=0;i<list.size();i++)
    {
        list.at(i)->write((char*)p,p->uiPDULen);
    }

}

void mytcpserver::deleteSocket(MyTcpSocket *mySocket)
{
    QList<MyTcpSocket*>::iterator iter=list.begin();
    for(;iter!=list.end();iter++)
    {
        if(mySocket==*iter)
        {
            (*iter)->close();
            //*iter是迭代器指向的元素，也就是存储在list里的指针
            (*iter)->deleteLater();
            *iter=NULL;
            list.erase(iter);
            break;
        }
    }


}

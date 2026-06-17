#include "mythread.h"
#include "mytcpsocket.h"
myThread::myThread(QObject* parent)
{


}

void myThread::run()
{
    MyTcpSocket::getInstance().resMsg();
}

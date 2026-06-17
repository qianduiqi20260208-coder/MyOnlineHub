#include "mytcpsocket.h"
#include "protocol.h"
#include "mytcpserver.h"
MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this,SIGNAL(readyRead()),this,SLOT(resMsg()));
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffOnline()));
    isupload=false;
    shareFile=false;
    timer=new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(sendFile()));
}

QString MyTcpSocket::getName()
{
    return na;
}

MyTcpSocket &MyTcpSocket::getInstance()
{
    static MyTcpSocket instance;
    return instance;
}

void MyTcpSocket::clientOffOnline()
{
    //将na作为参数传进去
    OpeDb::getInstance().offOnline(na.toStdString().c_str());
    emit offline(this);
}

void MyTcpSocket::sendFile()
{
    timer->stop();
    quint64 size;
    char* buffer=new char[4096];
    while(true)
    {
        size=file.read(buffer,4096);
        if(size>0&&size<=4096)
        {
            write(buffer,size);

        }
        else if(size==0)
        {
            break;
        }

    }
    delete[] buffer;
    buffer=NULL;
    file.close();

}

void MyTcpSocket::resMsg()
{
    if(!isupload&&!shareFile)
    {
        qDebug()<<this->bytesAvailable();
        uint uiPDULen=0;

        this->read((char*)&uiPDULen,sizeof(uint));
        //计算出来实际消息的长度
        uint msgLen=uiPDULen-sizeof(PDU);
        //创建一块区域用来存放读入的数据
        PDU* pdu=mkPDU(msgLen);
        this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
        //判断属于哪一种类型的消息

        switch(pdu->uiMsgType)
        {
        case ENUM_MSG_TYPE_REGIST_REQUEST:
        {
            qDebug()<<"ENUM_MSG_TYPE_REGIST_REQUEST";
            //准备两个数组用来接收数据
            char caName[32]={'\0'};
            char caPwd[32]={'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            //再开辟一段内存区域用来响应
            PDU* p=mkPDU(0);
            p->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPONSE;
            bool ret=OpeDb::getInstance().handleRegist(caName,caPwd);
            //判断是否注册成功，根据结果返回对应的消息
            if(ret)
            {
                strcpy(p->caData,REGIST_OK);
            }
            else
            {
                strcpy(p->caData,REGIST_FAILED);
            }
            write((char*)p,p->uiPDULen);
            free(p);
            p=NULL;
            break;

        }
        case ENUM_MSG_TYPE_LOGIN_REQUEST:
        {
            //准备两个数组用来接收数据
            char caName[32]={'\0'};
            char caPwd[32]={'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            //再开辟一段内存区域用来响应
            PDU* p=mkPDU(0);
            p->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPONSE;
            bool ret=OpeDb::getInstance().handleLogin(caName,caPwd);
            //判断是否登录成功，根据结果返回对应的消息
            if(ret)
            {
                strcpy(p->caData,LOGIN_OK);
                na=caName;
            }
            else
            {
                strcpy(p->caData,LOGIN_FAILED);
            }
            write((char*)p,p->uiPDULen);
            free(p);
            p=NULL;
            break;
        }

        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
        {
            QStringList ls= OpeDb::getInstance().handleAllOnline();
            qDebug()<<ls.size();
            uint uiMsgLen=ls.size()*32;
            PDU* pdu=mkPDU(uiMsgLen);
            pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_RESPONSE;
            for(int i=0;i<ls.size();i++)
            {
                //将list内容拷贝到开辟好的内存区域内
                memcpy((char*)(pdu->caMsg)+i*32,ls.at(i).toStdString().c_str(),ls.at(i).size());
            }
            write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu=NULL;
            break;

        }
        case ENUM_MSG_TYPE_SEARCH_USER_REQUEST:
        {
            int r= OpeDb::getInstance().handleSearch(pdu->caData);
            PDU* pp=mkPDU(0);
            pp->uiMsgType=ENUM_MSG_TYPE_SEARCH_USER_RESPONSE;

            if(r==0)
            {
                strcpy(pp->caData,SEARCH_OFFLINE);
            }else if(r==1)
            {
                strcpy(pp->caData,SEARCH_ONLINE);
            }
            else if(r==-1)
            {
                strcpy(pp->caData,SEARCH_NO);
            }
            free(pp);
            pp=NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            char caUserName[32]={'\0'};
            char caLoginName[32]={'\0'};
            strncpy(caUserName,pdu->caData,32);
            strncpy(caLoginName,pdu->caData+32,32);
            PDU* p=NULL;
            int re=OpeDb::getInstance().addFriend(caUserName,caLoginName);
            //名字为空
            if(-1==re)
            {
                p=mkPDU(0);
                p->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
                strcpy(p->caData,UNKNOW_ERROR);
                write((char*)p,p->uiPDULen);
                free(p);
                p=NULL;
                //已经是好友
            }else if(0==re)
            {
                p=mkPDU(0);
                p->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
                strcpy(p->caData,EXISTED_FRIEND);
                write((char*)p,p->uiPDULen);
                free(p);
                p=NULL;
                //在线
            } else if(1==re)
            {

                mytcpserver::getInstance().resend(caUserName,pdu);
                //不在线
            }else if(2==re)
            {
                p=mkPDU(0);
                p->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
                strcpy(p->caData,ADD_FRIEND_OFFLINE);
                write((char*)p,p->uiPDULen);
                free(p);
                p=NULL;
            }
            //用户不存在
            else if(3==re)
            {
                p=mkPDU(0);
                p->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
                strcpy(p->caData,ADD_FRIEND_NOEXIST);
                write((char*)p,p->uiPDULen);
                free(p);
                p=NULL;
            }

            break;
        }
        //给申请好友的一方发送消息
        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:
        {
            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName, pdu->caData, 32);
            strncpy(caName, pdu->caData+32, 32);
            OpeDb::getInstance().handleAgree(caPerName, caName);


            mytcpserver::getInstance().resend(caName, pdu);
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
        {


            char caName[32] = {'\0'};
            strncpy(caName, pdu->caData+32, 32);
            mytcpserver::getInstance().resend(caName, pdu);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
        {
            char caName[32]={'\0'};
            strncpy(caName,pdu->caData,32);
            QStringList list=OpeDb::getInstance().flush(caName);
            int msgLen=list.size()*32;
            PDU* p=mkPDU(msgLen);
            p->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_RESPONSE;
            for(int i=0;i<list.size();i++)
            {
                strncpy((char*)p->caMsg+i*32,list[i].toStdString().c_str(),list.at(i).size());
            }
            write((char* )p,p->uiPDULen);
            free(p);
            p=NULL;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        {
            char friName[32]={'\0'};
            char name[32]={'\0'};
            strncpy(friName,pdu->caData,32);
            strncpy(name,pdu->caData+32,32);
            OpeDb::getInstance().delFriend(friName,name);
            PDU* p=mkPDU(0);
            p->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_RESPONSE;
            strcpy(p->caData,DELETE_FRIEND_SUCCESS);

            write((char*)p,p->uiPDULen);
            free(p);
            p=NULL;
            break;
        }
        case ENUM_MSG_TYPE_CHAT_ALONE_REQUEST:
        {
            //转发给对应的用户
            char caName[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);
            mytcpserver::getInstance().resend(caName, pdu);
            break;
        }
        case ENUM_MSG_TYPE_GROUP_REQUEST:
        {

            mytcpserver::getInstance().resendAll(pdu);
            break;
        }
        case ENUM_CREATE_DIR_REQUEST:
        {

            QString strCurPath=QString("%1").arg((char*)pdu->caMsg);
            char caNewDir[32]={'\0'};
            strncpy(caNewDir,pdu->caData,32);
            QString strNewPath=strCurPath+"\\"+caNewDir;
            qDebug()<<strNewPath;
            QDir dir(strNewPath);
            if(!dir.exists())
            {
                dir.mkdir(strNewPath);
            }

            break;
        }
        case ENUM_CREATE_DIR_CLICKED_REQUEST:
        {
            QString str=QString("%1/%2/%3").arg((char*)pdu->caMsg).arg(pdu->caData).arg(pdu->caData+32);
            QDir dir(str);
            PDU* p=mkPDU(0);
            p->uiMsgType=ENUM_CREATE_DIR_CLICKED_RESPONSE;
            if(!dir.exists())
            {
                dir.mkdir(str);
                strcpy(p->caData,CREATE_DIR_SUCCESS);
            }else if(dir.exists()){
                strcpy(p->caData,CREATE_DIR_FAIL);
            }
            write((char*)p,p->uiPDULen);
            free(p);
            p=NULL;
            break;
        }
        //刷新文件请求
        case ENUM_FLUSH_FILE_REQUEST:

            {
                qDebug()<<"进入服务端的case分支...";
                QString strCurPath=QString("%1").arg((char*)pdu->caMsg);
                char caNewDir[32]={'\0'};
                strcpy(caNewDir,pdu->caData);
                QString strNewPath=strCurPath+"\\"+caNewDir;

                QDir dir(strNewPath);

                if(dir.exists())
                {

                    QFileInfoList fileInfo;

                    PDU* p=mkPDU(sizeof(fileInformation)*dir.count());
                    p->uiMsgType=ENUM_FLUSH_FILE_RESPONSE;
                    //将数字封装成字符串发送出去

                    sprintf(p->caData,"%d",dir.count());
                    //遍历所有的文件和目录
                    for(int i=0;i<dir.count();i++)
                    {
                        fileInformation* fiIn=(fileInformation*)p->caMsg+i;
                        fileInfo=dir.entryInfoList();
                        //判断是目录还是文件，然后进行不同的封装
                        if(fileInfo.at(i).isFile())
                        {

                            fiIn->isFile=1;
                        }else if(fileInfo.at(i).isDir())
                        {

                            fiIn->isFile=0;
                        }

                        //将名字复制到对应的区域
                        memcpy(fiIn->info,fileInfo.at(i).fileName().toStdString().c_str(),fileInfo.at(i).fileName().size());

                    }
                    write((char*)p,p->uiPDULen);
                    free(p);
                    p=NULL;


                }

                break;
            }
        case ENUM_UPLOAD_FILE_REQUEST:
        {
            char* tmp=new char[pdu->uiMsgLen];
            memcpy(tmp,pdu->caMsg,pdu->uiMsgLen);
            QString strPath=QString("%1").arg(tmp);
            file.setFileName(strPath);
            delete []tmp;
            tmp=NULL;
            qint64 size=atoi(pdu->caData);
            qDebug()<<"上传文件的大小为："<<size;
            //判断是否打开成功，没有文件第一次会创建文件的
            if(file.open(QIODevice::WriteOnly))
            {
                totalSize=size;
                resSize=0;
                isupload=true;
            }
            break;
        }
        case ENUM_DOWNLOAD_FILE_REQUEST:
        {
            char* tmp=new char[pdu->uiMsgLen];
            memcpy(tmp,pdu->caMsg,pdu->uiMsgLen);
            QString strPath=QString("%1").arg(tmp);
            file.setFileName(strPath);
            quint64 size=file.size();
            qDebug()<<"(下载文件)文件的大小为:"<<size;
            delete []tmp;
            tmp=NULL;

            PDU* p=mkPDU(0);
            //如果文件成功打开，返回文件的大小
            if(file.open(QIODevice::ReadOnly))
            {
                sprintf(p->caData,"%lld",size);
                p->uiMsgType=ENUM_DOWNLOAD_FILE_RESPONSE;
                write((char*)p,p->uiPDULen);

                timer->start(3000);
            }else{
                strcpy(p->caData,DOWNLOAD_FILE_FAIL);
                p->uiMsgType=ENUM_DOWNLOAD_FILE_RESPONSE;
                write((char*)p,p->uiPDULen);

            }
            free(p);
            p=NULL;
            break;
        }
        //分享文件的请求
        case ENUM_SHARE_FILE_REQUEST:{
            char* ch=new char[pdu->uiMsgLen];
            memcpy(ch,pdu->caMsg,pdu->uiMsgLen);
            QString path=QString("%1").arg(ch);
            qDebug()<<"分享文件的目的地路径为："<<path;
            file.setFileName(ch);
            delete []ch;
            ch=NULL;

            qint64 size=atoi(pdu->caData);
            qDebug()<<"(分享文件)文件的大小:"<<size;
            if(file.open(QIODevice::WriteOnly))
            {

                totalSize=size;
                resSize=0;
                shareFile=true;
            }
            else{
                qDebug()<<"打开文件失败...";
                file.close();
            }
        }
        default:
            break;
        }

        free(pdu);
        pdu=NULL;
    }

    else if(isupload){

        QByteArray array=readAll();
        file.write(array);
        resSize+=array.size();
        qDebug()<<"写入数据的大小："<<array.size();
        //接收完毕发送OK，出现错误发送FAIL
        if(resSize==totalSize)
        {
            file.close();
            isupload=false;
            PDU* p=mkPDU(0);
            p->uiMsgType=ENUM_UPLOAD_FILE_RESPONSE;
            strcpy(p->caData,UPLOAD_FILE_OK);
            write((char*)p,p->uiPDULen);
            free(p);
            p=NULL;
        }else if(resSize>totalSize)
        {
            file.close();
            isupload=false;
            PDU* p=mkPDU(0);
            p->uiMsgType=ENUM_UPLOAD_FILE_RESPONSE;
            strcpy(p->caData,UPLOAD_FILE_FAIL);
             write((char*)p,p->uiPDULen);
            free(p);
            p=NULL;
        }

    }else if(shareFile)
    {

        QByteArray array=readAll();
        resSize+=array.size();
        qDebug()<<"以分享文件的字节数:"<<resSize;
        file.write(array);


        if(resSize==totalSize)
        {
            qDebug()<<"分享成功！";
            shareFile=false;
            file.close();
        }

        else if(resSize>totalSize)
        {
            qDebug()<<"分享失败";
            shareFile=false;
            file.close();
        }
    }

}

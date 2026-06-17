#include "tcpclient2.h"
#include "qdebug.h"
#include "ui_tcpclient2.h"
#include "protocol.h"
#include "opewidget.h"
#include <QMessageBox>
#include "book.h"
#include <QDateTime>

TcpClient2::TcpClient2(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient2)
{
    //将UI里设置好的窗口部件应用于当前窗口
    ui->setupUi(this);
    socket=new QTcpSocket;

    loadconfig();

    //这个路径的设置只是针对学习项目而非实用项目来设置的
    curPath=QDir::currentPath();

    socket->connectToHost(QHostAddress(ip),port);

    ui->label->setStyleSheet("color:white;");
    ui->label_2->setStyleSheet("color:white;");
    this->setAttribute(Qt::WA_StyledBackground, true);

    this->setStyleSheet("QLabel{color:white;} QWidget#TcpClient2{background-color:rgb(32, 36, 56);}");
    //用信号来判断是否连接成功
    connect(socket,SIGNAL(connected()),this,SLOT(showConnection()));
    //设置信号用来检测消息
    connect(socket,SIGNAL(readyRead()),this,SLOT(resMsg()));
    isDownload=false;
}
void TcpClient2::loadconfig()
{
    //定义文件对象
    QFile file(":/tt.config");
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
        QMessageBox::critical(nullptr,"open config","open fail");
    }

}
TcpClient2::~TcpClient2()
{
    delete ui;
}



TcpClient2 &TcpClient2::getInstance()
{
    static TcpClient2 tcp;
    return tcp;
}

QString TcpClient2::getName()
{
    return loginName;
}



QString TcpClient2::loginN()
{
    return loginName;
}

QString TcpClient2::getPath()
{
    return  curPath;
}

QTcpSocket *TcpClient2::getSocket()
{
    return socket;
}

//随机生成的字符串
QString TcpClient2::getRandomString(int length)
{
        srand(QDateTime::currentMSecsSinceEpoch());
        char* ch=new char[length+1];
        ch[length]={'\0'};
        const char chars[]="abcdefghijklmnopqrst";

        int chars_size=sizeof(chars);
        int random;
        for(int i=0;i<length;i++)
        {
        random=rand()%(chars_size-1);
        ch[i]=chars[random];
        }
        QString res(ch);
        delete []ch;
        ch=NULL;
        qDebug()<<"生成的随机的字符串"<<res;
        return res;
}



#if 0
void TcpClient2::on_sendButton_clicked()
{
    QString text=ui->userName->text();
    if(!text.isEmpty())
    {
        //申请空间
        PDU *pdu=mkPDU(text.size()+1);
        pdu->uiMsgType=8888;
        //将输入框中的数据拷贝到cdData中
        memcpy(pdu->caMsg,text.toStdString().c_str(),pdu->uiMsgLen);
        socket.write((char*)pdu,pdu->uiPDULen);

        //释放掉内存
        free(pdu);
        pdu=NULL;

    }
    else
    {
        QMessageBox::warning(this,"信息发送","发送内容不能为空");
    }



}

#endif
void TcpClient2::on_loginButton_clicked()
{
    QString name=ui->userName->text();
    QString pwd=ui->pwd->text();
    //开辟一段内存，用来存放获取到的数据
    if(!name.isEmpty()&&!pwd.isEmpty())
    {
        loginName=name;
        PDU* pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,name.toStdString().c_str(),32);
        strncpy(pdu->caData+32,pwd.toStdString().c_str(),32);
        //发送数据
        socket->write((char*)pdu,pdu->uiPDULen);
        //释放内存
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::critical(this,"登录提示","用户名或者密码不能为空");
    }

}



void  TcpClient2::showConnection()
{

    QMessageBox::information(this,"connect","connect success");
}

void  TcpClient2::resMsg()
{
    if(!isDownload)
    {
    uint uiPDULen=0;

    socket->read((char*)&uiPDULen,sizeof(uint));
    //计算出来实际消息的长度
    uint msgLen=uiPDULen-sizeof(PDU);
    //创建一块区域用来存放读入的数据
    PDU* pdu=mkPDU(msgLen);
    socket->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
    //判断属于哪一种类型的消息

    switch(pdu->uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_RESPONSE:
    {
        if(0==strcmp(pdu->caData,REGIST_OK))
        {

            QMessageBox::information(nullptr,"注册",REGIST_OK);
            opeWidget::getInstance().getBook()->registCreate();


        }

        else if(0==strcmp(pdu->caData,REGIST_FAILED))
        {
            QMessageBox::critical(nullptr,"注册",REGIST_FAILED);
        }

        break;

    }
    case ENUM_MSG_TYPE_LOGIN_RESPONSE:
    {
        if(0==strcmp(pdu->caData,LOGIN_OK))
        {
            QMessageBox::information(nullptr,"登录",LOGIN_OK);
            opeWidget::getInstance().show();
            TcpClient2::getInstance().hide();
        }
        else if(0==strcmp(pdu->caData,LOGIN_FAILED))
        {
            QMessageBox::critical(nullptr,"登录",LOGIN_FAILED);
        }
        break;

    }

    case ENUM_MSG_TYPE_ALL_ONLINE_RESPONSE:
    {
        opeWidget::getInstance().getFriend()->showAllUsers(pdu);
        break;

    }
    case ENUM_MSG_TYPE_SEARCH_USER_RESPONSE:
    {
        //判断各种情况，-1是没有此用户，1是在线，0是下线
        if(-1==strcmp(pdu->caData,SEARCH_NO))
        {
            QMessageBox::information(nullptr,"提示",QString("%1:not exist").arg(opeWidget::getInstance().getFriend()->searchName));
        }
        else if(1==strcmp(pdu->caData,SEARCH_ONLINE))
        {
             QMessageBox::information(nullptr,"提示",QString("%1:online").arg(opeWidget::getInstance().getFriend()->searchName));
        }
        else if(0==strcmp(pdu->caData,SEARCH_OFFLINE))
        {
             QMessageBox::information(nullptr,"提示",QString("%1:offline").arg(opeWidget::getInstance().getFriend()->searchName));
        }
        break;

    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE:
    {
        QMessageBox::information(nullptr,"提示",pdu->caData);

        break;

    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        char caName[32]={'\0'};
        strncpy(caName,pdu->caData+32,32);
        int re=QMessageBox::information(nullptr,"添加好友",QString("%1 want to add you as friend?").arg(caName),
QMessageBox::Yes,QMessageBox::No);
        PDU* p=mkPDU(0);
        memcpy(p->caData,pdu->caData,64);
        //处理同意好友请求还是拒绝好友请求
        if(QMessageBox::Yes==re)
        {
             p->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
        }
        else
        {
             p->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
        }
        socket->write((char*)p,p->uiPDULen);
        free(p);
        p=NULL;

        break;

    }
    //同意加好友的提示
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:
    {
        char caPerName[32] = {'\0'};
        memcpy(caPerName, pdu->caData, 32);
        QMessageBox::information(nullptr, "添加好友", QString("添加%1好友成功").arg(caPerName));
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
    {
        char caPerName[32] = {'\0'};
        memcpy(caPerName, pdu->caData, 32);
        QMessageBox::information(nullptr, "添加好友", QString("添加%1好友失败").arg(caPerName));
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPONSE:
    {
        opeWidget::getInstance().getFriend()->updateFriend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPONSE:
    {
        if(0==strcmp(pdu->caData,DELETE_FRIEND_SUCCESS))
        {
             QMessageBox::information(this,"删除好友",DELETE_FRIEND_SUCCESS);
        }
        opeWidget::getInstance().getFriend()->flush();
        break;
    }
    case ENUM_MSG_TYPE_CHAT_ALONE_REQUEST:
    {
        //把收到的消息展示出来
        opeWidget::getInstance().getFriend()->showMs(pdu);
        break;
    }
    case ENUM_MSG_TYPE_GROUP_REQUEST:
    {
        opeWidget::getInstance().getFriend()->showGroupMsg(pdu);
        break;
    }
    case ENUM_CREATE_DIR_CLICKED_RESPONSE:
    {
        //根据返回的结果去显示不同的内容
        if(0==strcmp(CREATE_DIR_SUCCESS,pdu->caData))
        {
              QMessageBox::information(this,"创建文件夹",CREATE_DIR_SUCCESS);
        }else if(0==strcmp(CREATE_DIR_FAIL,pdu->caData))
        {
              QMessageBox::information(this,"创建文件夹",CREATE_DIR_FAIL);
        }

        break;
    }
    case ENUM_FLUSH_FILE_RESPONSE:
    {
        opeWidget::getInstance().getBook()->updateFile(pdu);



        break;
    }
    case ENUM_UPLOAD_FILE_RESPONSE:
    {
        if(0==strcmp(UPLOAD_FILE_OK,pdu->caData))
        {
              QMessageBox::information(this,"上传文件",UPLOAD_FILE_OK);
        }
        else if(0==strcmp(UPLOAD_FILE_FAIL,pdu->caData))
              {
                  QMessageBox::information(this,"上传文件",UPLOAD_FILE_FAIL);
              }
        break;
    }
    case ENUM_DOWNLOAD_FILE_RESPONSE:
    {
        if(strcmp(pdu->caData,DOWNLOAD_FILE_FAIL)==0)
        {
                  QMessageBox::information(this,"下载文件",DOWNLOAD_FILE_FAIL);
        }
        else{
                  //准备接收数据
                  totalSize=atoi(pdu->caData);
                  resSize=0;
                  isDownload=true;

                  QString filePath=opeWidget::getInstance().getBook()->getResFilePath();
                  file.setFileName(filePath);
                  if(!file.open(QIODevice::WriteOnly))
                  {
                      QMessageBox::critical(this,"下载文件","打开文件失败");
                      isDownload=false;
                      file.close();

                  }
        }
        break;
    }
    default:
        break;
    }

    free(pdu);
    pdu=NULL;
    }

    else {


    QByteArray array=TcpClient2::getInstance().getSocket()->readAll();
    qDebug()<<array.size();
    resSize+=array.size();
    qDebug()<<resSize;
    file.write(array);

    if(resSize==totalSize)
    {
        QMessageBox::information(this,"下载文件","下载成功");
        isDownload=false;
        file.close();
    }else if(resSize>totalSize){

        QMessageBox::critical(this,"下载文件","下载失败");
                              isDownload=false;
                              file.close();
    }


    }


}


void TcpClient2::on_registButton_clicked()
{
    QString name=ui->userName->text();
    QString pwd=ui->pwd->text();
    //开辟一段内存，用来存放获取到的数据
    if(!name.isEmpty()&&!pwd.isEmpty())
    {
        loginName=name;
        PDU* pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,name.toStdString().c_str(),32);
        strncpy(pdu->caData+32,pwd.toStdString().c_str(),32);
        //发送数据
        socket->write((char*)pdu,pdu->uiPDULen);
        //释放内存
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::critical(this,"注册","用户名或者密码不能为空");
    }
}


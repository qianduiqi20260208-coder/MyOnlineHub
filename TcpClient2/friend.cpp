#include "friend.h"
#include "protocol.h"
#include "tcpclient2.h"
#include<QMessageBox>
Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    showMsg=new QTextEdit;
    friendList=new QListWidget;
    inputMsg=new QLineEdit;

    delFriend=new QPushButton("删除好友");
    flushFriend=new QPushButton("刷新好友");
    showOnlineFriend=new QPushButton("显示在线用户");
    searchUser=new QPushButton("查找用户");
    sendButton=new QPushButton("发送");
    privateChat=new QPushButton("私聊");

    QVBoxLayout* pRight=new QVBoxLayout;
    pRight->addWidget(delFriend);
    pRight->addWidget(flushFriend);
    pRight->addWidget(showOnlineFriend);
    pRight->addWidget(searchUser);
    pRight->addWidget(privateChat);

     //设置水平布局
         QHBoxLayout* pTop=new QHBoxLayout;
         pTop->addWidget(showMsg);
         pTop->addWidget(friendList);
         pTop->addLayout(pRight);

         QHBoxLayout* pMsg=new QHBoxLayout;
         pMsg->addWidget(inputMsg);
         pMsg->addWidget(sendButton);

     //总体上垂直布局
         on=new online;
         QVBoxLayout* pMain=new QVBoxLayout;
         pMain->addLayout(pTop);
         pMain->addLayout(pMsg);
         pMain->addWidget(on);
         on->hide();
         setLayout(pMain);
         connect(showOnlineFriend,SIGNAL(clicked(bool)),this,SLOT(showOnine()));
         connect(searchUser,SIGNAL(clicked(bool)),this,SLOT(showSearch()));
         connect(flushFriend,SIGNAL(clicked(bool)),this,SLOT(flush()));
         //设置删除好友的信号与槽
         connect(delFriend,SIGNAL(clicked(bool)),this,SLOT(delFri()));
         connect(privateChat,SIGNAL(clicked(bool)),this,SLOT(chatAlone()));
         //群聊
         connect(sendButton,SIGNAL(clicked(bool)),this,SLOT(group()));
}

void Friend::updateFriend(PDU *p)
{
         if(NULL==p)
             return;
         //计算一共有多少个好友
         char name[32]={'\0'};
         int size=p->uiMsgLen/32;
         for(int i=0;i<size;i++)
         {
             memcpy(name,(char*)p->caMsg+i*32,32);
             friendList->addItem(name);
         }
}

void Friend::showMs(PDU *p)
{
         if(NULL==p)
         return;

         char name[32]={'\0'};
         char* ms=new char[p->uiMsgLen+1];
         ms[p->uiMsgLen]={'\0'};
         strncpy(name,p->caData+32,32);
         strncpy(ms,(char*)p->caMsg,p->uiMsgLen);
         QString msg=QString("%1对你说：%2").arg(name).arg(ms);

         showMsg->append(msg);
         delete []ms;
}

void Friend::showGroupMsg(PDU *p)
{
         if(NULL==p)
         return;
         char* ms=new char[p->uiMsgLen+1];
         ms[p->uiMsgLen]={'\0'};
          strncpy(ms,(char*)p->caMsg,p->uiMsgLen);
         QString msg=QString("%1对所有人说：%2").arg(p->caData).arg(ms);

         showMsg->append(msg);
         delete []ms;
}

QListWidget *Friend::getFriendList()
{
         return friendList;
}

void Friend::showOnine()
{//如果隐藏就打开，如果打开就隐藏
         if(on->isHidden())
         {
             on->show();
             PDU* pdu=mkPDU(0);
             pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
             //write后面是一个指向指针的指针
             TcpClient2::getInstance().getSocket()->write((char*) pdu,pdu->uiPDULen);
             free(pdu);
             pdu=NULL;
         }
         else
         {
             on->hide();
         }
}

void Friend::showSearch()
{
         searchName=  QInputDialog::getText(this,"搜索","用户名");
         if(!searchName.isEmpty())
         {
             qDebug()<<searchName;
             PDU* p=mkPDU(0);
             strncpy((char*)p->caData,searchName.toStdString().c_str(),searchName.size());
             p->uiMsgType=ENUM_MSG_TYPE_SEARCH_USER_REQUEST;
             //发送数据
              TcpClient2::getInstance().getSocket()->write((char*)p,p->uiPDULen);
             free(p);
             p=NULL;
         }
}

void Friend::showAllUsers(PDU *p)
{
         if(NULL==p)
             return;
         on->showUsers(p);

}

void Friend::flush()
{    //获取名字，发送出去
         friendList->clear();
         QString strName=TcpClient2::getInstance().getName();
         PDU* p=mkPDU(0);
         p->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
         strncpy(p->caData,strName.toStdString().c_str(),strName.size());

         TcpClient2::getInstance().getSocket()->write((char*)p,p->uiPDULen);
         free(p);
         p=NULL;
}

void Friend::delFri()
{
         QListWidgetItem* lw= friendList->currentItem();
         QString st;
         //判断是否为空
         if(lw)
         {
              st=lw->text();
         }

         QString strName=TcpClient2::getInstance().getName();
         qDebug()<<st;
         PDU* p=mkPDU(0);
         p->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
         strncpy(p->caData,st.toStdString().c_str(),st.size());
         strncpy(p->caData+32,strName.toStdString().c_str(),strName.size());
         TcpClient2::getInstance().getSocket()->write((char*)p,p->uiPDULen);
         free(p);
         p=NULL;
}
//私聊功能
void Friend::chatAlone()
{
        //获取自己的名字和好友的名字，获取消息的内容
         QListWidgetItem* lw= friendList->currentItem();

         //判断是否为空
         if(lw==NULL)
         {
              QMessageBox::information(this,"私聊","请选择要私聊的好友");
              return;
         }
         QString st;
         st=lw->text().toUtf8();
         QString strName=TcpClient2::getInstance().getName().toUtf8();
         QString msg=inputMsg->text().toUtf8();
         inputMsg->clear();
         PDU* p=mkPDU(msg.size());
         p->uiMsgType=ENUM_MSG_TYPE_CHAT_ALONE_REQUEST;
         //将获取到的数据封装到自己定义的PDU里面
         strncpy(p->caData,st.toStdString().c_str(),st.size());
         strncpy(p->caData+32,strName.toStdString().c_str(),strName.size());
         strncpy((char*)p->caMsg,msg.toStdString().c_str(),msg.size());
         TcpClient2::getInstance().getSocket()->write((char*)p,p->uiPDULen);
         free(p);
         p=NULL;

         showMsg->append("你对"+st+"说："+msg);

}

void Friend::group()
{

         int friCount=friendList->count();
         QString msg=inputMsg->text();
         QString strName=TcpClient2::getInstance().getName();
         inputMsg->clear();
         PDU* p=mkPDU(msg.size());
         p->uiMsgType=ENUM_MSG_TYPE_GROUP_REQUEST;
         strncpy((char*)p->caMsg,msg.toStdString().c_str(),msg.size());
         strncpy(p->caData,strName.toStdString().c_str(),strName.size());
         TcpClient2::getInstance().getSocket()->write((char*)p,p->uiPDULen);
         free(p);
         p=NULL;
}

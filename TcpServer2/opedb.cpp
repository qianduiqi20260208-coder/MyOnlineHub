#include "opedb.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QDir>
OpeDb::OpeDb(QObject *parent)
    : QObject{parent}
{
    //建立连接
    database=QSqlDatabase::addDatabase("QSQLITE");

}

OpeDb &OpeDb::getInstance()
{
    static OpeDb op;
    return op;
}

void OpeDb::init()
{
    qDebug() << "currentPath:" << QDir::currentPath();
    database.setDatabaseName("cloud.db");

    if(!database.open())
    {
        QMessageBox::critical(NULL,"打开状态","打开数据库失败");
    }

}

bool OpeDb::handleRegist(const char *name, const char *pwd)
{
    if(NULL==name||pwd==NULL)
        return false;
    QSqlQuery query;
    QString str=QString("insert into userInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);

    bool ret = query.exec(str);
    if(ret == false)
    {
        qDebug()<<query.lastError().text();
        qDebug()<<database.lastError().text();

        return false;
    }
    return true;
}

bool OpeDb::handleLogin(const char *name, const char *pwd)
{
    if(NULL==name||pwd==NULL)
        return false;
    QSqlQuery query;
    //只有没有在线的用户才允许登录
    QString str=QString("select * from userInfo where name=\'%1\' and pwd=\'%2\' and online=0").arg(name).arg(pwd);
    query.exec(str);
    if(query.next())
    {
        QString s=QString("update userInfo set online=1 where name=\'%1\' and pwd=\'%2\'").arg(name).arg(pwd);
        query.exec(s);
        return true;

    }
    else
    {
        return false;
    }
}
//根据名字来查找用户，有三种可能，查不到，不在线，在线，所以返回值为int类型
int OpeDb::handleSearch(const char *name)
{
    QSqlQuery query;
    QString str=QString("select online from userInfo where name=\'%1\'").arg(name);
    query.exec(str);

    if(query.next())
    {
        int res=query.value(0).toInt();
        if(0==res)
        {
            return 0;
        }
        else if(1==res)
        {
            return 1;
        }
    }
    else
    {
        return -1;
    }

}
//用户下线的话需要更改相应的状态
void OpeDb::offOnline(const char *name)
{
    if(NULL==name)
        return;
    QSqlQuery query;
    QString str=QString("update userInfo set online=0 where name=\'%1\'").arg(name);
    query.exec(str);

}

void OpeDb::handleAgree(const char *userName, const char *loginName)
{
    if (NULL == userName || NULL == loginName)
    {
        return;
    }
    QString data = QString("insert into friend(id, friendId) values((select id from userInfo where name=\'%1\'), (select id from userInfo where name=\'%2\'))").arg(loginName).arg(userName);
    QSqlQuery query;

    if (query.exec(data)) {
        // 查询执行成功
        // 执行其他操作或处理结果
    } else {
        // 查询执行失败
        // 可以通过 query.lastError() 获取错误信息并进行处理
        qDebug() << "Query error: " << query.lastError().text();
    }



}



int OpeDb::addFriend(const char *userN, const char *loginN)
{
    //判断是不是空值
    if(NULL==userN||NULL==loginN)
    {
        return -1;
    }
    QString data=QString("select * from friend where (id=(select id from userInfo where name=\'%1\') and friendId=(select id from userInfo where name=\'%2\'))"
    "or (id=(select id from userInfo where name=\'%3\') and friendId=(select id from userInfo where name=\'%4\'))").arg(userN).arg(loginN).arg(loginN).arg(userN);
    QSqlQuery qu;
    qu.exec(data);
    //判断是否已经是好友
    if(qu.next())
    {
        return 0;
    }
    else

    {
        QSqlQuery query;
        QString str=QString("select online from userInfo where name=\'%1\'").arg(userN);
        query.exec(str);

        if(query.next())
        {
            int res=query.value(0).toInt();
            if(0==res)
            {
                return 2;//表示不在线
            }
            else if(1==res)
            {
                return 1;//表示可以加好友
            }
        }
        else
        {
            return 3;//查不到这个用户
        }

    }

}

QStringList OpeDb::handleAllOnline()
{
    QSqlQuery query;
    QString str=QString("select name from userInfo where online=1");
    query.exec(str);
    QStringList res;
    res.clear();

    while(query.next())
    {
        res.append(query.value(0).toString());
    }
    return res;
}

QStringList OpeDb::flush(const char *name)
{
    QStringList sl;
    QSqlQuery query;
    QString str=QString("select name from userInfo where id in (select friendId from friend where id=(select id from userInfo where name=\'%1\')) and online=1").arg(name);
    query.exec(str);
    while(query.next())
    {
        sl.append(query.value(0).toString());
    }

    str=QString("select name from userInfo where id in (select id from friend where friendId=(select id from userInfo where name=\'%1\')) and online=1").arg(name);
    query.exec(str);
    while(query.next())
    {
        sl.append(query.value(0).toString());
    }

    return sl;
}

void OpeDb::delFriend(const char *friName,const char* name)
{
    if(NULL==name||NULL==friName)
        return;
    QString str=QString("delete from friend where friendId=(select id from userInfo where name=\'%1\') and id=(select id from userInfo where name=\'%2\')").arg(friName).arg(name);
    QSqlQuery query;
    query.exec(str);

    str=QString("delete from friend where id=(select id from userInfo where name=\'%1\') and friendId=(select id from userInfo where name=\'%2\')").arg(friName).arg(name);
    query.exec(str);
}

#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
class OpeDb : public QObject
{
    Q_OBJECT
public:
    explicit OpeDb(QObject *parent = nullptr);
    static OpeDb& getInstance();
    void init();
    bool handleRegist(const char* name,const char* pwd);
    bool handleLogin(const char* name,const char* pwd);
    int handleSearch(const char* name);
    void offOnline(const char* name);
    void handleAgree(const char* userName,const char* loginName);

    //声明一个用来处理加好友的函数
    int addFriend(const char* userN,const char* loginN);
    QStringList handleAllOnline();
    QStringList flush(const char* name);
    //删除好友
    void delFriend(const char* friName,const char* name);
signals:
private:
    QSqlDatabase database;

};

#endif // OPEDB_H

#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "online.h"
#include "protocol.h"
#include <QInputDialog>
class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void updateFriend(PDU* p);
    void showMs(PDU* p);
    void showGroupMsg(PDU* p);
    QListWidget* getFriendList();

QString searchName;
signals:
public slots:
    void showOnine();
    void showSearch();
    void showAllUsers(PDU* p);
    void flush();
    void delFri();
    void chatAlone();
    void group();

private:
    QTextEdit * showMsg;
    QListWidget* friendList;
    QLineEdit* inputMsg;

    QPushButton* delFriend;
    QPushButton* flushFriend;
    QPushButton* showOnlineFriend;
    QPushButton* searchUser;
    QPushButton* sendButton;
    QPushButton* privateChat;
    online* on;
};

#endif // FRIEND_H

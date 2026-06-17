#ifndef ONLINE_H
#define ONLINE_H

#include <QWidget>
#include <QListWidgetItem>
#include "protocol.h"
namespace Ui {
class online;
}

class online : public QWidget
{
    Q_OBJECT

public:
    explicit online(QWidget *parent = nullptr);
    ~online();
    void showUsers(PDU* p);
private slots:
    void on_pushButton_clicked();

private:
    Ui::online *ui;
};

#endif // ONLINE_H

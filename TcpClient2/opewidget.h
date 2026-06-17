#ifndef OPEWIDGET_H
#define OPEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "friend.h"
#include "book.h"
#include <QStackedWidget>
class opeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit opeWidget(QWidget *parent = nullptr);
    static opeWidget& getInstance();
    Friend* getFriend();
    book* getBook();
signals:
private:
    QListWidget* lw;
    Friend *f;
    book* b;
    QStackedWidget * sw;
};

#endif // OPEWIDGET_H

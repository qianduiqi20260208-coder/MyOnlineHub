#include "opewidget.h"

opeWidget::opeWidget(QWidget *parent)
    : QWidget{parent}
{
    lw=new QListWidget(this);
    lw->addItem("好友");
    lw->addItem("图书");
    f=new Friend;
    b=new book;

    sw=new QStackedWidget;
    sw->addWidget(f);
    sw->addWidget(b);

    QHBoxLayout* pMain=new QHBoxLayout;
    pMain->addWidget(lw);
    pMain->addWidget(sw);
    setLayout(pMain);
    //设置对不同的条目点击呈现出不同的页面
    connect(lw,SIGNAL(currentRowChanged(int)),sw,SLOT(setCurrentIndex(int)));

}

opeWidget &opeWidget::getInstance()
{
    static opeWidget op;
    return op;
}

Friend *opeWidget::getFriend()
{
    return f;
}

book *opeWidget::getBook()
{
    return b;
}

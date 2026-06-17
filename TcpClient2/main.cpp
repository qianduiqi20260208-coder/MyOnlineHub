#include "tcpclient2.h"

#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TcpClient2::getInstance().show();

    return a.exec();
}

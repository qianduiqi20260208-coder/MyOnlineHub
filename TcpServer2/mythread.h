#ifndef MYTHREAD_H
#define MYTHREAD_H
#include<QThread>

class myThread :public QThread
{
    Q_OBJECT
public:
    myThread(QObject* parent);

    void run();
};

#endif // MYTHREAD_H

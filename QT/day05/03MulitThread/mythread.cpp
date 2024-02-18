#include "mythread.h"
#include <QDebug>

MyThread::MyThread(QObject *parent) : QThread(parent)
{

}

// 线程处理操作
void MyThread::run()
{
    qDebug() << "subthread: " << QThread::currentThread();
    for(int i=0; i<1000000000; ++i)
    {
        // qDebug() << i;
    }
    qDebug() << "+++++++++ OVER +++++++++";
    emit workDone();
}

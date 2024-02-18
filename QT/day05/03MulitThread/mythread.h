#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0);

    // 重写run函数
    void run();

//    void setValue(int array[]);

signals:
    // 自定义信号
    void workDone();

public slots:
private:
    int array[1024];
};

#endif // MYTHREAD_H

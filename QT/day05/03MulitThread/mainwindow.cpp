#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mythread.h"
#include <QTimer>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTimer * timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]()
    {
        static int num = 0;
        ui->lcdNumber->display(++num);
    });

    qDebug() << "mainThread: " << QThread::currentThread();

    MyThread* subThread = new MyThread(this); // 创建子线程对象
    // connect(this, &MainWindow::sigSendValue, subThread, )
    connect(ui->startBtn, &QPushButton::clicked, this, [=]()
    {
        // 启动定时器
        if(!timer->isActive())
        {
            timer->start(1000);
        }
        // 启动子线程
        if(!subThread->isRunning())
        {
            subThread->start(); // run函数开始工作
        }
    });

    connect(subThread, &MyThread::workDone, this, [=]()
    {
        QMessageBox::information(this, "info", "子线程处理完成...");
    });

    // 关闭窗口的时候, 停止子线程
    connect(this, &MainWindow::destroyed, this, [=]()
    {
        // 退出子线程
        subThread->quit();
        // 等待线程执行完毕
        subThread->wait();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

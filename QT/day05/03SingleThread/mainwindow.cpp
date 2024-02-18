#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDebug>

/*
 *  单线程的应用程序执行负责的数据处理, 会导致应用程序界面无响应的情况.
 *  解决方案:
 *      1. 使用多线程编程
 *      2. 主线程负责UI界面, 主线程也称之为UI线程
 *      3. 子线程负责数据处理
 *      4. 父子线程之间通信使用信号槽机制
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]()
    {
        // 定时器启动, lcd显示屏开始计数
        static int num = 0;
        ui->lcdNumber->display(++num);
    });

    connect(ui->startBtn, &QPushButton::clicked, this, [=]()
    {
        // 启动定时器
        if(!timer->isActive())
        {
            timer->start(1000); // 时间间隔1s
        }
        // 复杂的数据处理
        for(int i=0; i<200000; ++i)
        {
            qDebug() << i;
        }
        qDebug() << "+++++++++ OVER +++++++++";
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

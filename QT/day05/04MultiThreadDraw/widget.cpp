#include "widget.h"
#include "ui_widget.h"
#include <QThread>
#include "drawimage.h"
#include <QPainter>
#include <QDebug>

/*
 * 在窗口中有一个按钮,当点击按钮之后,
 * 在线程(子线程)中绘制一张图片,
 * 然后将绘制好的图片显示到当前窗口中。
 */
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setFixedSize(600, 600);

    QThread* sub = new QThread(this);  // 创建子线程对象
    DrawImage* drawer = new DrawImage; // 创建画图对象
    drawer->moveToThread(sub); // 画图对象移动到子线程中
    sub->start(); // 启动子线程
    connect(ui->draw, &QPushButton::clicked, drawer, &DrawImage::drawing); // 点击按钮, 子线程开始画图

    connect(drawer, &DrawImage::imageDone, this, [=](QImage image) // 捕捉子线程信号
    {
        m_img = image;
        update();   // 调用该函数paintEvent被调用
    });

    connect(this, &Widget::destroyed, this, [=]()
    {
        sub->quit();
        sub->wait();
        delete drawer;
    });

    qDebug() << "mainThread: " << QThread::currentThread();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *)
{
    // 将子线程画好的图片显示出来
    QPainter p(this);
    p.drawImage(100, 100, m_img);
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QUrl>
#include <QMovie>
#include <QTimer>   // 定时器类

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->text->setText("我是纯文本"); // QLabel 显示纯文本
    ui->html->setText("<a href='www.baidu.com'>百度一下</a>"); // html字符串
    //ui->html->setOpenExternalLinks(true); // 设置属性, 让链接跳转
    connect(ui->html, &QLabel::linkActivated, this, [=](const QString &link)
    {
        QDesktopServices::openUrl(QUrl(link));
    });

    QPixmap pix(":/Image/Luffy.png"); // 显示图片
    pix.scaled(100, 100); // scaled返回的是一个新对象
    ui->pic->setPixmap(pix.scaled(100, 100));

#if 1
    QMovie* movie = new QMovie(":/Image/mario.gif"); // 动态图片
    ui->gif->setMovie(movie);
    movie->start(); // 启动
    movie->setScaledSize(QSize(100, 100));
#else
    QMovie movie(":/Image/mario.gif");
    ui->gif->setMovie(&movie);
    movie.start();
    movie.setScaledSize(QSize(100, 100));
#endif

    QTimer* timer = new QTimer(this); // 创建定时器类对象
    connect(timer, &QTimer::timeout, this, [=]() // 当定时器的时间到达, 会发出一个信号
    {
       // 修改LCDNumber控件显示的内容
        static int num = 0;
        ui->lcdNumber->display(++num);
    });
    timer->start(10);  // 启动定时器 单位: ms

    ui->progressBar->setRange(0, 30); // 设置进度条的范围
    ui->progressBar->setValue(10);
    // 创建定时器类对象
    #if 0
    QTimer* timer1 = new QTimer(this);
    connect(timer1, &QTimer::timeout, this, [=]()
    {
        static int num = 30;/*ui->progressBar->value()*/;
        ui->progressBar->setValue(++num);
        if(num == 50)
        {
            num = 0;
        }
    });
    timer1->start(100);
    #endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

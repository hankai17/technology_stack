#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFont font("华文彩云", 32, 75, true); // 作用域为当前整个窗口 子窗口
    this->setFont(font);

}

MainWindow::~MainWindow()
{
    delete ui;
}

// 在当前窗口中画图
// paintEvent 不需要用户调用, 窗口需要刷新的时候自动被调用
/*
 *1. 窗口显示的时候
 *2. 最大化最小化的时候
 *3. 一部分覆盖重新显示的时候
 *4. 手动刷新: update() : QWidget
 */
void MainWindow::paintEvent(QPaintEvent *)
{
    //如何在窗口中画图: QPainter
    QPainter p(this);   // 指定绘图设备
#if 1
    QPen pen; // 创建新的画笔 // 设置图形的轮廓
    pen.setColor(Qt::green);
    pen.setWidth(5);
    pen.setStyle(Qt::DashLine);
    p.setPen(pen); // 将画笔设置给画家类

    QBrush brush(Qt::red, Qt::CrossPattern); // 画刷作用, 填充闭合区域
    p.setBrush(brush); // 设置给画家类
#endif

    p.drawPixmap(0, 0, this->width(), this->height(), QPixmap("e:\\Image\\loveLian.jpg")); // 画背景图 // 重绘
    p.drawLine(QPoint(100, 100), QPoint(400, 400)); // 划线
    p.drawEllipse(QPoint(250, 250), 100, 100);  // 画圆

    //QFont font("华文彩云", 32, 75, true); // 自定义字体 // 没有这个字体
    //p.setFont(font);

    p.drawText(50, 100, "让编程改变世界!"); // 输出文本
    qDebug() << "+++++++++++++++";
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDataStream>  // 重量级的流操作类, qt中支持的所有类对象都可以操作,
#include <QFile>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 使用QImage加载磁盘图片, 写到一txt文件中
    // 将txt文件中的内容读出来, 显示到一个label中

//    QImage img("E:\\Image\\Luffy.png");
    QImage img;
    img.load("E:\\Image\\Luffy.png");

    QFile file("E:\\mypic.txt");
    file.open(QIODevice::WriteOnly);
    QDataStream stream(&file);
    // 写数据
    stream << QString("你好, 世界!") << img;
    file.close();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_show_clicked()
{
    // 将txt文件中的内容读出来, 显示到一个label中
    QFile file("E:\\mypic.txt");
    file.open(QIODevice::ReadOnly);
    QDataStream stream(&file);    // 读文件
    QString str;
    QImage img;
    stream >> str >> img;
    file.close();

    QMessageBox::information(this, "info", str);

    // label中显示图片
    ui->label->setPixmap(QPixmap::fromImage(img));
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 写文件
    QFile file("E:\\aab.txt");
    file.open(QIODevice::WriteOnly);
    // QString -> QByteArray: toUtf8();
    // QByteArray -> char* : data();
    QString str = "hello, Qt QFile!!";
    file.write(str.toUtf8());
    file.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selFile_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "打开文件", "E:\\",
                                 "文本文件 (*.txt)");
    if(path.isEmpty())
    {
        return ;
    }
    // 路径设置到单行编辑框
    ui->filePath->setText(path);

    QTextCodec* codec = QTextCodec::codecForName("gbk");
    // 创建文件对象
    QFile file;
    file.setFileName(path);
    // 打开文件
    bool bl = file.open(QIODevice::ReadOnly);
    if(bl == false)
    {
        return;
    }
    // 读文件

    // QByteArray array = file.readAll();

    // 分行读
    QByteArray all;
    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        all += line;
    }

    // 设置到文本框里边
    // 格式转换
    QString mytext = codec->toUnicode(all);
    ui->textEdit->setText(mytext);

    file.close();
}

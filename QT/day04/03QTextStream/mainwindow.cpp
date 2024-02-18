#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>  // 文本流 -- 轻量级 int QString double float
#include <QFileDialog>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 使用QTextStream写文件
    QFile file("E:\\ababc.txt");
    file.open(QIODevice::WriteOnly);
    QTextStream stream;
    stream.setDevice(&file);
    // 写数据 char* QString
    stream << QString("学生爱学习") << 666 << QString("学习累了要睡觉") ;

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

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    // 创建流对象
    // 解码默认采用当前的操作系统的编码
    QTextStream stream(&file);
    // 设置解析的文件编码
    stream.setCodec("gbk");

    // 读出文件中所有的内容
    //QString all = stream.readAll();
    // 分行读
    QString all;
    while(!stream.atEnd())
    {
        QString line = stream.readLine();   // 读出的每一行内容不包含换行符
        all += line;
    }

    ui->textEdit->setText(all);

    file.close();
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboBox->addItem("周伯通");
    ui->comboBox->addItem("小龙女");

    QStringList list; // 添加多个
    list << "南帝" <<"北丐"<< "东邪"<<"西毒";
    ui->comboBox->addItems(list);

    connect(ui->comboBox, &QComboBox::currentTextChanged, this, [=](const QString &text)
    {
        qDebug() << text << ui->comboBox->currentIndex() << ui->comboBox->currentText();
    });

    // QTextEdit
    ui->textEdit->setText("<a href='www.baidu.com'>百度一下<a>");
    ui->textEdit->append(")))))))");
    ui->textEdit->toPlainText();    // 取纯文本
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionSave, &QAction::triggered, this, [=]()  // 菜单项也叫action //捕捉菜单项的信号 // objName是不同控件的唯一标识
    {
        qDebug() << "actionSave的save";
    });

    // 工具栏中添加按钮
    ui->mainToolBar->addWidget(new QPushButton("Hello, toolBar"));
    //ui->mainToolBar->insertWidget(1);

    // 状态栏
    ui->statusBar->addAction(ui->actionEfg);
    ui->statusBar->addWidget(new QPushButton("hello, statusBar"));
    ui->statusBar->addWidget(new QPushButton("hello"));

    // QAction添加图标
    ui->actionSaveAsABC->setIcon(QIcon(":/acb/Image/heart.png")); // 首先添加qrc资源文件 然后将图片添加进去
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSaveAsABC_triggered() // ui生成的代码
{
    qDebug() << "ui转到槽的saveAs";
}

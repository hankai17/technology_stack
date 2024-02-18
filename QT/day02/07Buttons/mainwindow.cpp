#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // QToolButton对象
    QToolButton* toolbtn = new QToolButton(this); // 在工具栏实现像"菜单栏下拉"那样的形似 // 即将菜单栏与button关联起来
    // 创建菜单对象
    QMenu* menu = new QMenu(this);
    // 添加QAction
    menu->addAction(ui->actionAa);
    menu->addAction(ui->actionBb);
    menu->addAction(ui->actionCc);
    // 将菜单设置给工具按钮
    toolbtn->setMenu(menu);
    // 工具按钮添加到工具栏
    ui->mainToolBar->addWidget(toolbtn);
    // 点按钮弹出菜单
    connect(toolbtn, &QToolButton::clicked, toolbtn, &QToolButton::showMenu);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_radioButton_clicked()
{
    qDebug() << "------------";
}

void MainWindow::on_checkBox_4_clicked()
{
    // 判断状态
    bool bl = ui->checkBox_4->isChecked();
    qDebug() << "status: " << bl;

    ui->checkBox->setChecked(true);
}

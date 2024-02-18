#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 添加数据库 -- mysql
    qDebug() << QSqlDatabase::drivers();
    QSqlDatabase base = QSqlDatabase::addDatabase("QMYSQL");
    // 配置
    base.setHostName("192.168.22.56");  // 主机IP
    base.setPort(3306);         // 设置端口
    base.setUserName("root");   // mysql登录用户名
    base.setPassword("root");   // mysql登录密码
    base.setDatabaseName("itcast"); // 使用的数据库的名字
    // 打开数据库
    if(!base.open())
    {
        // 失败弹出错误信息
        QMessageBox::critical(this, "error", base.lastError().text());
    }

    // QSqlTableModel 该模型类存储数据, 需要显示需要使用视图类QTableView
    // 创建模型对象
    QSqlTableModel *model = new QSqlTableModel(this);
    model->setTable("student");  // 设置数据库表名
    // 查询数据库
    model->select();
    // 将模型设置给视图
    ui->tableView->setModel(model);
    // 设置提交方式 -- 手动提交
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    connect(ui->submit, &QPushButton::clicked, this, [=]()
    {
        // 提交
        model->submitAll();
    });

    connect(ui->revert, &QPushButton::clicked, this, [=]()
    {
        // 撤销
       model->revertAll();
    });

    connect(ui->search, &QPushButton::clicked, this, [=]()
    {
        QString name = ui->lineEdit->text();
        // 设置过滤
        QString str = QString("name='%1'").arg(name);
        model->setFilter(str);
        // 重写查询
        model->select();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

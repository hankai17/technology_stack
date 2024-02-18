#include "widget.h"
#include "ui_widget.h"
#include <QSqlDatabase> // 数据库类, 主要用于添加配置数据库
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>    // sql错误信息类
#include <QSqlQuery>    // 数据库查询类

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
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

    // 数据库操作 添加 删除 修改 查询
    // 插入操作
    QSqlQuery query;
#if 0
    // 插入一条语句
    // query.exec("insert into people(name, age) values('张三丰', 100)");
    // 批处理操作
    query.prepare("insert into people(name, age) values(:name, :age)");
    // 绑定数据
    QStringList namelist;
    namelist << "东方不败" << "左冷禅" << "岳不群" << "林平之";
    query.bindValue(":name", namelist);
    // 年龄　int -> QString
    QStringList agelist;
    agelist << QString::number(45) << QString::number(40)
            << QString::number(35) << QString::number(23);
    query.bindValue(":age", agelist);
    // 执行批处理操作
    query.execBatch();
#endif
    // 数据查询
    query.exec("select * from people");
    // 遍历所有的记录
    while(query.next())
    {
        // 打印每个字段的值
#if 0
        qDebug() << query.value("id").toInt()
                 << query.value("name").toString()
                 << query.value("age").toInt();
#endif
        qDebug() << query.value(0).toInt()
                 << query.value(1).toString()
                 << query.value(2).toInt();
    }
}

Widget::~Widget()
{
    delete ui;
}

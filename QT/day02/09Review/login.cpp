#include "login.h"
#include "ui_login.h"
#include <QMessageBox>

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    // 设置密码格式
    ui->passwd->setEchoMode(QLineEdit::Password);
}

Login::~Login()
{
    delete ui;
}

void Login::on_login_clicked()
{
    // 获取编辑框中的内容
    // 用户名
    QString user = ui->userName->text();
    // 密码
    QString pwd = ui->passwd->text();

    if(user == "root" && pwd == "root")
    {
        // 登录
        this->accept();
    }
    else
    {
        QMessageBox::warning(this, "警告", "用户名或密码不正确");
        // 清空编辑框中的内容
        ui->userName->setText("");
        ui->passwd->clear();

        // 设置焦点
        ui->userName->setFocus();
    }
}

void Login::on_exit_clicked()
{
    this->reject();
}

#include "mydialog.h"
#include "ui_mydialog.h"
#include <QDebug>

MyDialog::MyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyDialog)
{
    ui->setupUi(this);
}

MyDialog::~MyDialog()
{
    qDebug() << "mydialgo 被干掉了。。。";
    delete ui;
}

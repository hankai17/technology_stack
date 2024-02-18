#include "mywidget.h"
#include "ui_mywidget.h"
#include <QAction>

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{
    ui->setupUi(this);

    // 初始化按钮
    QStringList list;
    list << "butterfly.png" << "up.png" << "down.png";
    ui->pushButton->setImage(list);
    ui->pushButton->setFixedSize(160, 130);

    //ui->widget->setImage(":/Image/butterfly.png");
}

MyWidget::~MyWidget()
{
    delete ui;
}

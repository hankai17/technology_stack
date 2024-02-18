#include "mybutton.h"
#include "ui_mybutton.h"

MyButton::MyButton(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyButton)
{
    ui->setupUi(this);
}

MyButton::~MyButton()
{
    delete ui;
}

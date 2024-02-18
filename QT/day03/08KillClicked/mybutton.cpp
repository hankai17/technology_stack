#include "mybutton.h"
#include <QMouseEvent>

MyButton::MyButton(QWidget *parent) : QPushButton(parent)
{

}

void MyButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        emit myclicked();
        return;
    }
    QPushButton::mousePressEvent(e);
}

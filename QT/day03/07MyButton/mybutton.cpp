#include "mybutton.h"
#include <QPainter>

MyButton::MyButton(QWidget *parent) : QPushButton(parent)
{

}

void MyButton::enterEvent(QEvent *)
{
    m_curImg = m_imgList.at(1);
    // 刷新按钮
    update();
}

void MyButton::leaveEvent(QEvent *)
{
    m_curImg = m_imgList.at(0);
    // 刷新按钮
    update();
}

void MyButton::mousePressEvent(QMouseEvent *ev)
{
    m_curImg = m_imgList.at(2);
    // 刷新按钮
    update();
}

void MyButton::mouseReleaseEvent(QMouseEvent *ev)
{
    m_curImg = m_imgList.at(1);
    // 刷新按钮
    update();
}

void MyButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(0, 0, width(), height(),
                 QPixmap(":/Image/" + m_curImg));
}

void MyButton::setImage(QStringList list)
{
    m_imgList = list;
    m_curImg = m_imgList.at(0);
    // 刷新按钮
    update();
}

void MyButton::setImage(QString normal, QString hover, QString pressed)
{
    m_imgList << normal << hover << pressed;
    update();
}

#include "mybuttonplus.h"
#include <QPainter>
#include <QMouseEvent>
#include <QBrush>

MyButtonPlus::MyButtonPlus(QWidget *parent) : QWidget(parent)
{
    m_startxStep = 8;
    m_startyStep = 8;
    m_widthStep = 16;
    m_heightStep = 16;
}

void MyButtonPlus::setImage(QString img)
{
    m_pix.load(img);
    setFixedSize(m_pix.size());
    update();
}

void MyButtonPlus::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.drawPixmap(0, 0, width(), height(), m_pix);
}

void MyButtonPlus::mousePressEvent(QMouseEvent *ev)
{
    // 按下鼠标左键按钮窗口放大显示
    if(ev->button() == Qt::LeftButton)
    {
        this->setGeometry(x()-m_startxStep, y()-m_startyStep,
                          width()+m_widthStep, height()+m_heightStep);
        emit pressed();
        update();
    }
}

void MyButtonPlus::mouseReleaseEvent(QMouseEvent *ev)
{
    // 释放鼠标键
    if(ev->button() == Qt::LeftButton)
    {
        this->setGeometry(x()+m_startxStep, y()+m_startyStep,
                          width()-m_widthStep, height()-m_heightStep);
        emit released();
        update();
    }
}

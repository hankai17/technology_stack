#include "mylabel.h"
#include <QMouseEvent>
#include <QDebug>

MyLabel::MyLabel(QWidget *parent) : QLabel(parent)
{

}

void MyLabel::mousePressEvent(QMouseEvent *ev)
{
    QString str = QString();
    if(ev->button() == Qt::LeftButton)
    {
        str = "Mouse Left Button: ";
    }
    else if(ev->button() == Qt::RightButton)
    {
        str = "Mouse Right Button: ";
    }
    QString text = QString("[%1]: position(%2, %3)").arg(str).arg(ev->x()).arg(ev->y());
    qDebug() << text;
}

/*
 *  事件分发器:
 *      作用:
 *          1. 将进入当前窗口的事件分类, 并且将事件分发给对应的事件处理函数
 *          2. 只分发事件, 不处理事件
 *          3. 我们可以利用这一特性对事件进行拦截, 或者在此处直接将事件处理(不建议这么做).
 */
bool MyLabel::event(QEvent *e)
{
    if(e->type() == QEvent::MouseButtonPress ||         // 鼠标单击
            e->type() == QEvent::MouseButtonDblClick)   // 鼠标双击
    {
        // 此处父类转子类, 为什么可以转换成功?
        // ----  因为QEvent类型的指针内部指向的QMouseEvent类型的对象, 所有可以直接转
        QMouseEvent* ev = (QMouseEvent*)e;
        if(ev->button() == Qt::LeftButton)
        {
            // return true // 默认事件已经被处理, 事件停止传递 // 按左键无任何响应
            return false; // 事件向上传到给父窗口
        }
    }
    // 处理鼠标单击和双击, 其他事件通过父类执行默认处理
    // 以下调用是必须的
    return QLabel::event(e);
}

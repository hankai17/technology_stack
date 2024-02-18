#include "mylabel.h"
#include <QEvent>

MyLabel::MyLabel(QWidget *parent) : QLabel(parent)
{

}

bool MyLabel::event(QEvent *e)
{
    if(e->type() == QEvent::MouseButtonPress)
    {
        //return true;    // 事件停止向下分发
        return false;   // 事件继续向下传递 -- 传递给父窗口
    }
    // 如果是其他事件, 让父类执行默认操作
    return QLabel::event(e);
}

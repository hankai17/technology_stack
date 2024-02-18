#include "widget.h"
#include "ui_widget.h"
#include <QMouseEvent>
#include <QDebug>

/*
 *  事件过滤器的使用方法:
 *      1. 给指定的控件安装事件过滤器
 *      2. 重写事件过滤器函数, 在该函数中过滤对应的事件
 *  事件过滤器处理事件的时机:
 *      在事件进入窗口之前将事件过滤掉
 */
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 给控件安装事件过滤器
    // 参数表示在当前窗口中给label中安装过滤器, 也就是让当前窗口过滤label的事件
    ui->label->installEventFilter(this);
}

Widget::~Widget()
{
    delete ui;
}

// 重写事件过滤器
bool Widget::eventFilter(QObject *obj, QEvent *ev)
{
    // 判断是哪一个控件
    if(obj == ui->label)
    {
        // 过滤鼠标 单击, 双击 事件
        if(ev->type() == QEvent::MouseButtonPress ||
                ev->type() == QEvent::MouseButtonDblClick)
        {
            // 判断是否是鼠标左键
            QMouseEvent* mouseEv = (QMouseEvent*)ev;
            if(mouseEv->button() == Qt::LeftButton)
            {
                QString text = QString("Mouse Left Button: Pos[%1, %2]")
                        .arg(mouseEv->x()).arg(mouseEv->y());
                qDebug() << text;
                // 事件停止传递
                return true;
            }
            else if(mouseEv->button() == Qt::RightButton)
            {
                // 事件传递给被拦击的窗口的的父窗口, 也就是label的父窗口
                //return false;
            }
        }
    }
    // 其他事件执行默认处理
    return QWidget::eventFilter(obj, ev);
}

void Widget::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::RightButton)
    {
        qDebug() << "Current is Widget, Right button is clicked, pos: " << ev->pos();
    } else if (ev->button() == Qt::LeftButton) {
        qDebug() << "Current is Widget, Left button is clicked, pos: " << ev->pos();
    }
}

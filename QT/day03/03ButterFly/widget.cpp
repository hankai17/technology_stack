#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QDesktopWidget>   // 这个类可以得到屏幕的矩形区域
#include <time.h>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    setMouseTracking(true); // 设置自动追踪鼠标移动
    m_up.load(":/Image/up.png"); // 加载图片
    m_down.load(":/Image/down.png");
    setFixedSize(m_up.size()); // 发虚 // 设置窗口和图片同样大小
    setWindowFlags(Qt::FramelessWindowHint); // 去边框
    setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明
    m_cur = m_up;

#if 1
    QTimer* timer = new QTimer(this);
    qsrand(time(NULL));
    // 获取一个QDesktopWidget类型的对象
    // qApp->desktop();
    QDesktopWidget* desk = QApplication::desktop();
    connect(timer, &QTimer::timeout, this, [=]()
    {
        static bool flag = true; // 切换图片
        m_cur = flag ? m_up : m_down;
        flag = !flag;
#if 1
        int x = qrand() % 30 + this->geometry().topLeft().x(); // 窗口的移动
        int y = qrand() % 30 + this->geometry().topLeft().y();
        if(x > desk->screenGeometry().bottomRight().x()) // 边界判断 // 出了右边界, 从左边界回来
        {
            x = 0;
        }
        if(y > desk->screenGeometry().bottomLeft().y()) // 出了下边界, 从上边界回来
        {
            y = 0;
        }
        move(x, y);
#endif
        update(); // 手动刷新窗口 // 调用该函数paintEvent会被调用
    });
    // 启动定时器
    timer->start(200);
#endif
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(0, 0, width(), height(), m_cur);
}

//mouseReleaseEvent
void Widget::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::RightButton) // 判定当前是哪一个键 -- 返回的是一个瞬间的状态
    {
        this->close();
    }
    if(ev->button() == Qt::LeftButton) // 如果是左键, 计算窗口左上角, 和当前按钮位置的距离
    {
        m_pt = ev->globalPos() - this->frameGeometry().topLeft();
    }
}

void Widget::mouseMoveEvent(QMouseEvent *ev)
{
    if(ev->buttons() & Qt::LeftButton) // 移动是持续的状态, 需要使用buttons
    {
        move(ev->globalPos() - m_pt);
    }
   // qDebug() << ev->globalPos();
}

void Widget::enterEvent(QEvent *ev)
{
    qDebug() << "++++++++++";
}

void Widget::leaveEvent(QEvent *ev)
{
    qDebug() << "============";
}

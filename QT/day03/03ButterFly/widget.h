#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *);
    // 鼠标按下
    void mousePressEvent(QMouseEvent* ev);
    // 鼠标移动
    void mouseMoveEvent(QMouseEvent* ev);
    // 进入窗口瞬间
    void enterEvent(QEvent* ev);
    // 离开窗口瞬间
    void leaveEvent(QEvent* ev);

private:
    Ui::Widget *ui;

    QPixmap m_up;
    QPixmap m_down;
    QPixmap m_cur;

    QPoint m_pt;
};

#endif // WIDGET_H

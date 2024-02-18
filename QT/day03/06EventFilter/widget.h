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

    // 事件过滤器
    bool eventFilter(QObject *, QEvent *);
    void mousePressEvent(QMouseEvent* ev);

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H

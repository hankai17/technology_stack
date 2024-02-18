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
    // 窗口重绘函数
    void paintEvent(QPaintEvent *);

private:
    Ui::Widget *ui;

    QImage m_img;
};

#endif // WIDGET_H

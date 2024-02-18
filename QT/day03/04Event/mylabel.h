#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>

class MyLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MyLabel(QWidget *parent = 0);

protected:

    // 拦截鼠标事件
    bool event(QEvent *e);

public slots:
};

#endif // MYLABEL_H

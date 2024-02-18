#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>

class MyLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MyLabel(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *ev);
    bool event(QEvent *e);

public slots:
};

#endif // MYLABEL_H

#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QPushButton>

class MyButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyButton(QWidget *parent = 0);

signals:
    void myclicked();
public slots:
protected:
    void mousePressEvent(QMouseEvent* e);
};

#endif // MYBUTTON_H

#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QWidget>

namespace Ui {
class MyButton;
}

class MyButton : public QWidget
{
    Q_OBJECT

public:
    explicit MyButton(QWidget *parent = 0);
    ~MyButton();

private:
    Ui::MyButton *ui;
};

#endif // MYBUTTON_H

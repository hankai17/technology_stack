#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "newspaper.h"
#include "reader.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    void slotPrint();

private:
    Newspaper* paper;
    Reader* reader;
};

#endif // WIDGET_H

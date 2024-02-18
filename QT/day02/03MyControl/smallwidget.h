#ifndef SMALLWIDGET_H
#define SMALLWIDGET_H

#include <QWidget>

namespace Ui {
class SmallWidget;
}

class SmallWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SmallWidget(QWidget *parent = 0);
    ~SmallWidget();

    // 设置数据
    void setValue(int value);
    // 取数据
    int value();

private:
    Ui::SmallWidget *ui;
};

#endif // SMALLWIDGET_H

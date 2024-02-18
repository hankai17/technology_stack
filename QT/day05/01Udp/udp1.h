#ifndef UDP1_H
#define UDP1_H

#include <QWidget>
#include <QUdpSocket>

namespace Ui {
class Udp1;
}

class Udp1 : public QWidget
{
    Q_OBJECT

public:
    explicit Udp1(QWidget *parent = 0);
    ~Udp1();

private:
    Ui::Udp1 *ui;

    QUdpSocket* udp1;
};

#endif // UDP1_H

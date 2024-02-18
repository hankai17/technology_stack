#ifndef MYBUTTONPLUS_H
#define MYBUTTONPLUS_H

#include <QWidget>

class MyButtonPlus : public QWidget
{
    Q_OBJECT
public:
    explicit MyButtonPlus(QWidget *parent = 0);

    void setImage(QString img);

protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent* );
    void mouseReleaseEvent(QMouseEvent*);

signals:
    void pressed();
    void released();
public slots:
private:
    int m_startxStep;
    int m_startyStep;
    int m_widthStep;
    int m_heightStep;
    QPixmap m_pix;
};

#endif // MYBUTTONPLUS_H

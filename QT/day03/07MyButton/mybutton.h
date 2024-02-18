#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QPushButton>

class MyButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyButton(QWidget *parent = 0);


    void setImage(QStringList list);
    void setImage(QString normal, QString hover, QString pressed);


signals:

public slots:
protected:
    void enterEvent(QEvent* );
    void leaveEvent(QEvent*);
    void mousePressEvent(QMouseEvent* ev);
    void mouseReleaseEvent(QMouseEvent* ev);
    void paintEvent(QPaintEvent*);

private:
    // 保存图片
    QStringList m_imgList;
    // 当前状态图片
    QString m_curImg;
};

#endif // MYBUTTON_H

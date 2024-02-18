#ifndef DRAWIMAGE_H
#define DRAWIMAGE_H

#include <QObject>
#include <QImage>

class DrawImage : public QObject
{
    Q_OBJECT
public:
    explicit DrawImage(QObject *parent = 0);

    // 自定义画图函数
    void drawing();

signals:
    void imageDone(QImage img);
public slots:
};

#endif // DRAWIMAGE_H

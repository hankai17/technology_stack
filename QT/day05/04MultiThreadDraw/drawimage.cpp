#include "drawimage.h"
#include <QPainter>
#include <QPen>
#include <QDebug>
#include <QThread>
#include <QMessageBox>

DrawImage::DrawImage(QObject *parent) : QObject(parent)
{

}

void DrawImage::drawing()
{
    // QMessageBox::aboutQt(NULL);
    qDebug() << "subthread: " << QThread::currentThread();
    // 画图 -- 画图设备
    // QWidget, QPixmap, QImage, QBitmap, QPicture
    // 创建画布
    QImage img(600, 600, QImage::Format_ARGB32);
    // 创建画家对象
    QPainter p(&img);

    // 设置画笔
    p.setPen(QPen(Qt::red, 10));
    // 设置画刷
    p.setBrush(QBrush(Qt::blue));
    // 话多边形
    QPoint points[] = {
        QPoint(qrand()%500, qrand()%500),
        QPoint(qrand()%500, qrand()%500),
        QPoint(qrand()%500, qrand()%500),
        QPoint(qrand()%500, qrand()%500),
        QPoint(qrand()%500, qrand()%500),
        QPoint(qrand()%500, qrand()%500)
    };
    p.drawPolygon(points, 6);

    // 画好之后发送信号, 通知主线程
    emit imageDone(img);
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QPen>
#include <QPicture>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#if 0
    // 在QPixmap中画图
    QPixmap pix(300, 300);  // 创建一个画布, 大小为300*300
    QPainter p(&pix);
    // 画布自己填充颜色
    pix.fill();
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(10);
    p.setPen(pen);
    p.setBrush(QBrush(Qt::green));
    p.drawEllipse(QPoint(150, 150), 100, 100);
    //xxxx
    // 画完之后, 保存图片
    pix.save("E:\\pix.png");
#endif
#if 0
    // QImage中画图 -- 可以画出北京透明的图片
    QImage img(300, 300, QImage::Format_ARGB32); // 创建一个画布, 大小为300*300
    QPainter p(&img);

    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(10);
    p.setPen(pen);
    p.setBrush(QBrush(Qt::green));
    p.drawEllipse(QPoint(150, 150), 100, 100);

    // 画完之后, 保存图片
    img.save("E:\\img.png");
#endif
#if 1
    QPicture pic;
    // 指定绘图设备的第二种方式
    QPainter p; // 1. 创建一个空的画家类对象
    // 2. 通过begin函数告诉画家类绘图设备是谁
    p.begin(&pic);

    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(10);
    p.setPen(pen);
    p.setBrush(QBrush(Qt::green));
    p.drawEllipse(QPoint(150, 150), 100, 100);

    // 3. 告诉画家类, 图画完了
    p.end();

    // 画完之后, 保存图片
    pic.save("E:\\pic.xxx");
#endif

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter p;
    p.begin(this);

    // 窗QPicture对象
    QPicture pic;
    pic.load("E:\\pic.xxx");
    // 将pic画到窗口上
    p.drawPicture(100, 100, pic);

    p.end();
}

#ifndef READER_H
#define READER_H

#include <QObject>
#include <QDebug>

class Reader : public QObject
{
    Q_OBJECT
public:
    explicit Reader(QObject *parent = 0) : QObject(parent) {}
    void readNewpaper() {    // 槽函数
        qDebug() << "我要去买报纸了....";
    }
    void readNewpaper1(QString name) {
        qDebug() << "新报纸的名字: " << name;
    }

public slots:    // 如果使用qt4 的方式进行信号槽的链接, 槽函数必须使用slots关键字声明
    void readNewpaper1(QString name, QString text) {
        qDebug() << "新报纸的名字: " << name << ", 报纸简介: " << text;
    }
signals:
};

#endif // READER_H

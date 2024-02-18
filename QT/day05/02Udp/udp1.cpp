#include "udp1.h"
#include "ui_udp1.h"
#include <QMessageBox>

Udp1::Udp1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Udp1)
{
    ui->setupUi(this);
    this->setWindowTitle("========");

    // 初始化套接字
    udp1 = new QUdpSocket(this);
    // 绑定端口, 可以不绑定, 如果不想接受数据
    udp1->bind(9999);

    // 接收数据, 如果有数据到达, 会发信号 readyRead() -- QIODevice类的信号
    connect(udp1, &QUdpSocket::readyRead, this, [=]()
    {
        // 获取即将接收的数据报的长度
        qint64 size = udp1->pendingDatagramSize();
        QByteArray array(size, 0);
        // 接收数据
        // QByteArray -> char* 使用data()
        udp1->readDatagram(array.data(), size);
        // 显示到历史记录框
        ui->record->append("Udp2 Say: " + array);
    });

    connect(ui->send, &QPushButton::clicked, this, [=]()
    {
        // 获取发送内容
        QString msg = ui->msg->toPlainText(); // 获取QTextEdit中的纯文本内容
        // 发送数据
        // qint64 ret = udp1->writeDatagram(msg.toUtf8(), QHostAddress("127.0.0.1"), 8989);

        // 广播
        // 192.168.22.255
        // 广播只能适用于局域网范围内
        // qint64 ret = udp1->writeDatagram(msg.toUtf8(), QHostAddress("255.255.255.255"), 8989);

        // 组播
        // 1. 需要使用组播地址
        // 2. 如果想接收组播消息, 需要加入到组播地址中
        qint64 ret = udp1->writeDatagram(msg.toUtf8(), QHostAddress("224.0.1.10"), 8989);

        if(ret == -1)
        {
            QMessageBox::critical(this, "error", "数据发送失败!");
            return;
        }
        // 数据显示
        ui->record->append("Me Say: " + msg);

        // 清空消息框
        ui->msg->clear();
        ui->msg->setFocus();
    });
}

Udp1::~Udp1()
{
    delete ui;
}

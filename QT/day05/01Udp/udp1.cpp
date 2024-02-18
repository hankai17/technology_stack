#include "udp1.h"
#include "ui_udp1.h"
#include <QMessageBox>

Udp1::Udp1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Udp1)
{
    ui->setupUi(this);

    udp1 = new QUdpSocket(this); // 初始化套接字
    udp1->bind(QHostAddress::AnyIPv4, 8989); // 绑定端口, 可以不绑定, 如果不想接受数据
    udp1->joinMulticastGroup(QHostAddress("224.0.1.10")); // 如果想收到组播消息, 需要加入到组播地址

    // 接收数据, 如果有数据到达, 会发信号 readyRead() -- QIODevice类的信号
    connect(udp1, &QUdpSocket::readyRead, this, [=]()
    {
        qint64 size = udp1->pendingDatagramSize(); // 获取即将接收的数据报的长度
        QByteArray array(size, 0);        
        udp1->readDatagram(array.data(), size); // 接收数据 // QByteArray -> char* 使用data()
        ui->record->append("Udp2 Say: " + array); // 显示到历史记录框
    });

    connect(ui->send, &QPushButton::clicked, this, [=]()
    {
        QString msg = ui->msg->toPlainText(); // 获取QTextEdit中的纯文本内容
        qint64 ret = udp1->writeDatagram(msg.toUtf8(), QHostAddress("127.0.0.1"), 9999);
        if(ret == -1)
        {
            QMessageBox::critical(this, "error", "数据发送失败!");
            return;
        }
        ui->record->append("Me Say: " + msg);
        ui->msg->clear();
        ui->msg->setFocus();
    });
}

Udp1::~Udp1()
{
    delete ui;
}

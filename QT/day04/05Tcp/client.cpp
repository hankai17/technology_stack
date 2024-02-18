#include "client.h"
#include "ui_client.h"
#include <QHostAddress>

Client::Client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    // 创建套接字
    m_sock = new QTcpSocket(this);
    // 连接服务器
    m_sock->connectToHost(QHostAddress("127.0.0.1"), 9898);
    // 如果想知道是否连接成功
    connect(m_sock, &QTcpSocket::connected, this, [=]()
    {
        // 跟服务器说一句话
        m_sock->write("难道真的需要补一刀吗?");
        ui->record->setTextColor(Qt::red);
        ui->record->append("Me Say: 难道真的需要补一刀吗?");
    });

    // 客户接受服务器数据
    connect(m_sock, &QTcpSocket::readyRead, this, [=]()
    {
        QString msg = m_sock->readAll();
        ui->record->setTextColor(Qt::green);
        ui->record->append("Server Say: " + msg);
    });

}

Client::~Client()
{
    delete ui;
}

void Client::on_send_clicked()
{
    // 获取内容
    QString msg = ui->msg->toPlainText();
    // 发送
    ui->record->setTextColor(Qt::red);
    ui->record->append("Me Say: " + msg);
    m_sock->write(msg.toUtf8());

    ui->msg->clear();
    ui->msg->setFocus();
}

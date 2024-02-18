#include "server.h"
#include "ui_server.h"

Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);

    // 创建套接字 -- 用户监听
    m_server = new QTcpServer(this);
    // 设置监听
    m_server->listen(QHostAddress("127.0.0.1"), 9898);
    //
//    m_server->waitForNewConnection(8);
    connect(m_server, &QTcpServer::newConnection, this, [=]()
    {
       // 接收连接请求
        // 调用这句话相当于跟客户端建立了连接
        m_client = m_server->nextPendingConnection();
        // 通信
        // 发数据给客户端
        m_client->write("你好客户端, 我ij是岳不群!");
        ui->record->setTextColor(Qt::red);
        ui->record->append("Me Say: 你好客户端, 我是岳不群!");

        // 信号槽连接的时候, 必须保证信号发出者和信号接受者对象是被实例化的
        connect(m_client, &QTcpSocket::readyRead, this, [=]()
        {
            QString str =  m_client->readAll();
            ui->record->setTextColor(Qt::green);
            ui->record->append("Client Say: " + str);
        });
    });
}

Server::~Server()
{
    delete ui;
}

void Server::on_send_clicked()
{
    // 获取内容
    QString msg = ui->msg->toPlainText();
    // 发送
    ui->record->setTextColor(Qt::red);
    ui->record->append("Me Say: " + msg);
    m_client->write(msg.toUtf8());

    ui->msg->clear();
    ui->msg->setFocus();
}

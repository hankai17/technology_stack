#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>   // 用于监听
#include <QTcpSocket>   // 用于通信

namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

private slots:
    void on_send_clicked();

private:
    Ui::Server *ui;

    //　服务器端需要几个套接字：２
    // 1. 用于监听, 2. 通信
    QTcpServer* m_server;
    QTcpSocket* m_client;
};

#endif // SERVER_H

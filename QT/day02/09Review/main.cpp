#include "mainwindow.h"
#include <QApplication>
#include "login.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 登录对话框窗口对象
    Login login;
    // 模态显示
    int ret = login.exec();
    if(ret == QDialog::Accepted)
    {
        // 显示主窗口
        MainWindow w;
        w.show();
        return a.exec();
    }
    return 0;
}

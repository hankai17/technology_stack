#include "MyWidget.h"   // 窗口头文件
#include <QApplication> // 应用程序类

int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);  // 应用程序类: 1. 只有一个应用程序类对象 2. 进行消息循环, 捕捉并且处理事件
    MyWidget w;
    w.show();                    // 显示窗口
    return a.exec();             // 进行消息循环, 捕捉并且处理事件
}

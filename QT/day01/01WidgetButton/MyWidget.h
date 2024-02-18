#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QPushButton>

class MyWidget : public QWidget // 1. 默认只显示一个空窗口
{
    Q_OBJECT                    // 如果使用qt的信号槽机制 必须添加该宏 类必须从QObject派生
public:
    MyWidget(QWidget *parent = 0);
    ~MyWidget();
private:
    QPushButton* m_btn;         // 2. 添加按钮
};

#endif // MYWIDGET_H

/*
1）帮助文件：F1 （光标在函数名字或类名上，按 F1 即可跳转到对应帮助文档，查看其详细用法）
2）ESC 切换到编辑模式
3）.h 文件和对应.cpp 文件切换：F4
4）函数声明和定义(函数调用和定义)切换：F2
5）代码注释取消注释：Ctrl + / （选中代码再按快捷键）
6）字体变大变小：Ctrl + 鼠标滚轮向上向下
7）移动代码：选中所要移动的代码 -> 按住 ctrl + shift -> 键盘方向键
8）查找替换关键字：ctrl + f
9）快速打开代码所在目录：编辑模式下 -> 选中项目文件 -> 右击 -> 选择“显示包含的目录”，即可显示项目所在目录（此方法同样可以打开代码文件所在目录）
10) Ctrl+I   自动对齐
11) 编译 && 调试
            Ctrl+B        编译工程
            Ctrl+R        运行工程
            F5            开始调试
            Shift+F5      停止调试
            F9            设置和取消断点
            F10           单步前进
            F11           单步进入函数
            Shift + F11   单步跳出函数
*/

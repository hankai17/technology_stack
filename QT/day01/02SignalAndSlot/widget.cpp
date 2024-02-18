#include "widget.h"
#include <QPushButton>
#include <QDebug>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QPushButton* btn = new QPushButton("Quit", this);
    connect(btn, &QPushButton::clicked, this, &Widget::close);  // btn用于发信号 widget用于收信号 由此可以通过help找QPushButton
                                                                // Widget相应的发信号函数 接受信号的槽函数 // 我以为传参是静态函数 实则非也 有点像std::bind
    QPushButton* print = new QPushButton("印刷报纸", this);
    print->move(100, 100);
#if 1 // 自定义信号槽
     paper = new Newspaper(this);
     reader = new Reader(this);
     //connect(print, &QPushButton::clicked, paper, &Newspaper::hasNespaper);     // clicked参数为bool类型 hasNespaper为无参类型 so可以匹配
     connect(print, &QPushButton::clicked, this, &Widget::slotPrint); // 信号发送给信号 通过参数传递
     connect(paper, &Newspaper::hasNespaper, reader, &Reader::readNewpaper); // 信号发送给槽函数 // 如果使用qt4 的方式进行信号槽的链接, 槽函数必须使用slots关键字声明
#endif
#if 0
     paper = new Newspaper("melon", "jujube", this);
     reader = new Reader(this);
     connect(print, &QPushButton::clicked, this, &Widget::slotPrint);
     //connect(paper, &Newspaper::hasNespaper1, reader, &Reader::readNewpaper1);    // 编译失败:  不知道用哪个重载函数
     void (Newspaper::*n1)(QString) = Newspaper::hasNespaper1;
     void (Reader::*r1)(QString) = Reader::readNewpaper1;
     void (Newspaper::*n2)(QString, QString) = Newspaper::hasNespaper1;
     void (Reader::*r2)(QString, QString) = Reader::readNewpaper1;
     connect(paper, n1, reader, r1); // 手动指定
     connect(paper, n2, reader, r2);
#endif
#if 0
     paper = new Newspaper("melon", "jujube", this);
     reader = new Reader(this);
     connect(print, &QPushButton::clicked, this, [=]()
     {
         paper->send();
     });
     // 使用Qt4的方式处理信号槽
     // connect(Sender, SIGNAL(sigFunc(int, QString)), Rerever, SLOT(slotFunc(int, QString)));
     connect(paper, SIGNAL(hasNespaper1(QString,QString)),  // 运行时出错 找不到函数hasNespaper100 // 实现机制应该是虚函数
             reader, SLOT(readNewpaper1(QString,QString)));
#endif
}

Widget::~Widget()
{

}

void Widget::slotPrint()
{
    paper->send();
}

/*
 *  信号函数和槽函数如何使用?
 *  信号:
 *      1. 返回为void
 *      2. 只有函数声明, 没有函数定义
 *      3. 定义信号的时候需要使用关键字: signals
 *      4. 信号可以重载
 *      5. 自己发信号: emit
 *  槽函数: -- 跟信号对应的一个函数
 *      1. Qt5中类的成员函数, 全局函数, 静态函数, lambda表达式
 *      2. 没有返回值: void
 *      3. 可以重载
 *      4. 槽函数的参数需要跟信号的参数一一对应
 *          void mysig(int, double);
 *
 *          void myslot(int, double);   // ok
 *          void myslot(double, int);   //error
 *          void myslot(int);   // ok
 *          void myslot(double) // error
 *          信号的参数作用: 传递数据
 *          槽函数的参数: 接收信号传递的数据
 *      5. 槽函数的参数个数 <= 信号的参数个数
 */

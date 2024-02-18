#include "MyWidget.h"
#include "MyButton.h"

// 构造函数用于初始化
MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)
{
    m_btn = new QPushButton;
    m_btn->setParent(this);
    m_btn->setText("mainBtn");
    m_btn->move(100, 100);
    m_btn->setFixedSize(100, 100);

    QPushButton* subBtn = new QPushButton("subBtn", m_btn);
    subBtn->move(50, 50);
    subBtn->setFixedSize(50, 50);           // Qt坐标原点: 左上角, x向右递增, y向下递增 窗口使用的坐标体系是相对于其父窗口来说的.

    MyButton* myBtn = new MyButton(this);
    //MyButton* myBtn = new MyButton();       // QT中的内存回收机制: 1类直接或间接从QObject派生 2创建对象的时候, 需要给对象指定父亲
    myBtn->setText("myBtn---11111111111111");
    myBtn->show();
    QPushButton* normalBtn = new QPushButton("normalBtn", this);
}

MyWidget::~MyWidget()
{
}

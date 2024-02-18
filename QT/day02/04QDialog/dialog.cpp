#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include "mydialog.h"
#include <QMessageBox>  // 提示对话框类， 使用静态成员函数
#include <QFileDialog>  // 使用静态成员函数

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // QDialog两种状态 1. 模态 √ 2. 非模态
    connect(ui->modal, &QPushButton::pressed, this, [=]()
    {
        QDialog dlg;
        dlg.exec(); // 显示模态对话框 // 模态窗口会阻塞代码向下执行， 关闭窗口之后， 代码继续执行
        qDebug() << "++++++++++++++";
    });

    connect(ui->noModal, &QPushButton::pressed, this, [=]()
    {
        #if 0
        QDialog dlg;
        dlg.show(); // 显示非模态对话框 // 一闪而过
        qDebug() << "++++++++++++++";
        #endif
        MyDialog* dlg = new MyDialog(this);  // 延长生命周期
        dlg->setAttribute(Qt::WA_DeleteOnClose); // 设置属性 当窗口关闭则进行自动销毁 否则的话只有当整个项目结束才销毁
        dlg->show(); // 显示非模态对话框

        QWidget* wg = new QWidget;
        wg->setAttribute(Qt::WA_DeleteOnClose); // dialog从widget派生所以 widget也可以设置这个属性
        wg->setWindowTitle("aaaa");
        wg->show();
        qDebug() << "++++++++++++++";
    });

    connect(ui->msg, &QPushButton::clicked, this, [=]()
    {
        int ret = QMessageBox::critical(this, "错误", "这是一个错误",
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No);
        // 判断返回值b
        if(ret == QMessageBox::No)
        {
            QMessageBox::aboutQt(this, "++++");
        }
    });

    connect(ui->fileDlg, &QPushButton::clicked, this, [=]()
    {
       QString str = QFileDialog::getOpenFileName(this, "打开文件", "E:\\", // 点按钮, 显示打开文件对话框
                                                  "Pic (*.txt)");
       qDebug() << str;
    });
}

Dialog::~Dialog()
{
    delete ui;
}

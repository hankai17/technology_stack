#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    // 窗口刷新函数
    void paintEvent(QPaintEvent *);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

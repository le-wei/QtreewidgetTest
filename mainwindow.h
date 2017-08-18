#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"myqtreewidget.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

//private slots:
//    void on_mYQTREEWIDGET_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
//    MYQTREEWIDGET * treeWidget;
};

#endif // MAINWINDOW_H

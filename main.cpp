#include "myqtreewidget.h"
#include <QApplication>
#include<QTreeWidget>
#include<QTreeWidgetItem>
#include<QList>
#include<QDebug>
#include <QContextMenuEvent>
#include<QTimer>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;

//    w.show();
    DarwinTreeWidget * qtreew = new DarwinTreeWidget();

    QStringList headlist;
    headlist.append("name");
    headlist.append("age");
    headlist.append("class");
    //qtreew->setHeaderLabels(headlist);
    qtreew->setHead(headlist);
    QTreeWidgetItem *item;
    for(int i=0;i<10;i++)
    {
        QStringList list;
        list.append("a"+QString::number(i, 10));
        list.append(QString::number(i, 10));
        list.append("class"+QString::number(i, 10));
        list.append(QString::number(NORMAL,10));
        list.append(QString::number(NORMAL,10));
        item=new QTreeWidgetItem((list));
        qtreew->insertItemList(item);
    }
    qtreew->createTree();
    qtreew->show();
    return a.exec();
}

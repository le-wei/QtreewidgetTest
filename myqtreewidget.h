#ifndef MYQTREEWIDGET_H
#define MYQTREEWIDGET_H

#include<QTreeWidget>
#include <QContextMenuEvent>
#include<QHash>
#include<QTreeWidgetItem>

#define NORMAL 0
#define DELETE 1
#define EDITED  2
#define ITEMEDITED  3
class DarwinTreeWidget : public QTreeWidget
{
    Q_OBJECT
    Q_PROPERTY(QString m_SFileName READ getFileName WRITE setFileName)
public:
    DarwinTreeWidget(QWidget *parent = 0);
    ~DarwinTreeWidget();
    void setFileName(QString);
    QString getFileName()const;
    bool itemComparison(const QTreeWidgetItem *,const QTreeWidgetItem *);
   void contextMenuEvent(QContextMenuEvent *);
   void insertUptadeTable(QTreeWidgetItem *item);
   void removeUptadeTable(QTreeWidgetItem *item);
   void setDate();
   void insertItemList(QTreeWidgetItem * item);
   void insertItemList(QStringList list);
   void createTree();
   void allPull(QHash<QString, QTreeWidgetItem *> &serveItemList);
   void partPull(QHash<QString, QTreeWidgetItem *> &serveItemList);
   void delete_Item(QTreeWidgetItem * item);
   void setHead(QStringList head);
   void setKey(int key);
   void deleteItem(QHash<QString, QTreeWidgetItem *> &serveItemList);
   void keyPressEvent(QKeyEvent *event);
   QHash<QString,QList<QString>> getUptadeTable();
   void getItem();

public slots:
    void on_edit_function();
    void on_revokededit_function( );
    void on_delete_function( );
    void on_currentItemChanged_function(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_timer_function();
    void on_itemChanged_function(QTreeWidgetItem*item, int);


private:
    QTimer* time;
    QString m_SFileName;
    bool   isEdit;
    int m_key;
    QMenu* menu;
    QHash<QString,QTreeWidgetItem *> m_HHistoryTable;
    QHash<QString,QList<QString>>m_HUptadeTable;
    QHash<QString,QTreeWidgetItem *>m_itemList;
    QHash<QString,QTreeWidgetItem *> m_HConflictList;
    static int num;


};

#endif

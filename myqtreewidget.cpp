#include "myqtreewidget.h"
#include<QTreeWidget>
#include<QDebug>
#include<QMenu>
#include<QTimer>

DarwinTreeWidget::DarwinTreeWidget(QWidget *parent) :
    QTreeWidget(parent),menu(NULL)
{

  connect(this,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(on_currentItemChanged_function(QTreeWidgetItem*,QTreeWidgetItem*)));
    isEdit = true;
    m_key = 0;
    time = new QTimer(this);
    connect(time,SIGNAL(timeout()),this,SLOT(on_timer_function()));
    time->start(30000);
    connect(this,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(on_itemChanged_function(QTreeWidgetItem*,int)));
}
DarwinTreeWidget::~DarwinTreeWidget()
{
}
void DarwinTreeWidget::setFileName(QString FileName)
{
    this->m_SFileName = FileName;
}
QString DarwinTreeWidget::getFileName()const
{
    return this->m_SFileName;
}
void DarwinTreeWidget::setHead(QStringList head)
{
    this->setHeaderLabels(head);
}
int DarwinTreeWidget::num=0;


bool DarwinTreeWidget::itemComparison(const QTreeWidgetItem *item1,const QTreeWidgetItem * item2)
{
    int item1columnCount = item1->columnCount();
    int item2columnCount = item2->columnCount();
    if( item1columnCount != item2columnCount )
    {
        return false;
    }
    for(int i =0;i<item1columnCount-2;i++)
    {
        if(!(item1->text(i) == item2->text(i)))
        {
 //           qDebug()<<"item1 != item2"<<endl;
            return false;
        }
    }
    return true;

}

//把当前修改的数据以StringList的方式插入到更新列表中以便用户提取列表组包发出
void DarwinTreeWidget::insertUptadeTable(QTreeWidgetItem *item)
{
    if(item !=NULL)
    {
        int colun = item->columnCount()-1;
        QList<QString> stringList;
        for(int i=0;i<colun;i++)
        {
            stringList.append(item->text(i));
        }
        this->m_HUptadeTable.insert(item->text(0),stringList);
    }
}
void DarwinTreeWidget::removeUptadeTable(QTreeWidgetItem *item)
{
    if(item != NULL)
    {
        this->m_HUptadeTable.remove(item->text(m_key));
    }
}
QHash<QString,QList<QString>> DarwinTreeWidget::getUptadeTable()
{
    foreach (QTreeWidgetItem* item, this->m_HHistoryTable) {
        delete item;
    }
    this->m_HHistoryTable.clear();
    foreach (QTreeWidgetItem *item, this->m_HConflictList) {
        delete item;
    }
    this->m_HConflictList.clear();
    QList<QString> keys=this->m_HUptadeTable.keys();
    foreach (QString key, keys) {
        QTreeWidgetItem *item = this->m_itemList.value(key);
        if(item!=NULL)
        {
            item->setText(item->columnCount()-2,QString::number(NORMAL,10));
        }
    }
    QHash<QString,QList<QString>> tem = this->m_HUptadeTable;
    this->m_HUptadeTable.clear();
    return tem;

}
void DarwinTreeWidget::allPull(QHash<QString, QTreeWidgetItem *>& serveItemList)
{
    if( 0==serveItemList.size() )
    {
        return;
    }
    QList<QString> keys = m_itemList.keys();
    qDebug()<<"keys number"<<keys.size();
    foreach (QString key, keys) {
        QTreeWidgetItem * itemtem =serveItemList.value(key);
        QTreeWidgetItem *item = m_itemList.value(key);
        int col = item->columnCount();
        bool ok;
        if(itemtem==NULL)
        {
  //          qDebug()<<key;
            switch (item->text(col-2).toInt(&ok,10)) {
            case NORMAL:
                this->delete_Item( m_itemList.take(key));
                break;
            case DELETE:
                this->delete_Item( m_itemList.take(key));
                this->m_HUptadeTable.remove(key);
                break;
            case EDITED:
                // this->m_HConflictList.insert(key,item);
                 QTreeWidgetItem * tem = this->m_HHistoryTable.take(key);
                 if(tem!=NULL)
                 {
                     delete tem;
                 }
                 QStringList listtem;
                 listtem.append(key);
                 listtem.append(QString::number(DELETE,10));
                 this->m_HHistoryTable.insert(key,new QTreeWidgetItem(listtem));
                 this->m_HConflictList.insert(key,new QTreeWidgetItem(listtem));
                break;
            }
        }
        else
        {
            switch (item->text(col-2).toInt(&ok,10)) {
            case NORMAL:
                if(! this->itemComparison(item,itemtem))
                {
                    for(int i=0;i<col;i++)
                    {
                        item->setText(i,itemtem->text(i));
                    }
                }
                delete serveItemList.take(key);
                break;
            case DELETE:
                this->m_HHistoryTable.insert(key,new QTreeWidgetItem(*itemtem));
                this->m_HConflictList.insert(key,new QTreeWidgetItem(*itemtem));
                delete serveItemList.take(key);
                break;
            case EDITED:              
                this->m_HConflictList.insert(key,new QTreeWidgetItem(*itemtem));
                QTreeWidgetItem * tem = this->m_HHistoryTable.take(key);
                if(tem!=NULL)
                {
                   delete tem;
                }
                this->m_HHistoryTable.insert(key,new QTreeWidgetItem(*itemtem));
                delete serveItemList.take(key);
                break;
            }
        }
    }
    if(serveItemList.size()!=0)
    {
        foreach (QTreeWidgetItem* item, serveItemList) {
            this->m_itemList.insert(item->text(m_key),new QTreeWidgetItem(*item));
            delete item;
        }
        serveItemList.clear();

    }
    qDebug()<<"server size"<<serveItemList.size();
    this->createTree();
}
void DarwinTreeWidget::partPull(QHash<QString, QTreeWidgetItem *> &serveItemList)
{
    if( 0==serveItemList.size())
    {
        return;
    }
    QList<QString> keys = serveItemList.keys();
    foreach (QString key, keys) {
       QTreeWidgetItem*item = this->m_itemList.value(key);
       QTreeWidgetItem *tem = serveItemList.take(key);
       if(item==NULL)
       {
            this->m_itemList.insert(key,tem);

       }else
       {
           int coln = item->columnCount();
           if(item->text(coln-2)==QString::number(NORMAL,10))
           {
               for(int i=0;i<coln-2;i++)
               {
                   item->setText(i,tem->text(i));
               }
               delete tem;
           }
           else
           {
               QTreeWidgetItem* tmp = this->m_HHistoryTable.take(key);

               if(tmp!=NULL)
               {
                   delete tmp;
               }
                this->m_HHistoryTable.insert(key,new QTreeWidgetItem(*tem));
               this->m_HConflictList.insert(key,new QTreeWidgetItem(*tem));
           }
       }
       delete tem;
    }
    this->createTree();
}

void DarwinTreeWidget::deleteItem(QHash<QString, QTreeWidgetItem *> &serveItemList)
{
    if(serveItemList.size() == 0)
    {
        return;
    }
    QList<QString> keys = serveItemList.keys();
    foreach (QString key, keys) {
        QTreeWidgetItem*item = this->m_itemList.take(key);
        QTreeWidgetItem* itemtem = serveItemList.take(key);
        if(item!=NULL)
        {
            this->delete_Item(item);
        }
        delete itemtem;
    }
}

void DarwinTreeWidget::setKey(int key)
{
    this->m_key = key;
}

void DarwinTreeWidget::insertItemList(QTreeWidgetItem *item)
{
    this->m_itemList.insert(item->text(m_key),item);
}
void DarwinTreeWidget::insertItemList(QStringList list)
{
    list.append(QString::number(NORMAL,10));
    list.append(QString::number(NORMAL,10));
    QTreeWidgetItem* item = new QTreeWidgetItem(list);
    this->m_itemList.insert(item->text(m_key),item);
}
void DarwinTreeWidget::createTree()
{
    foreach (QTreeWidgetItem *item, m_itemList) {
        this->addTopLevelItem(item);
        int colun = item->columnCount()-2;
        if(item->text(colun)==QString::number(NORMAL,10))
        {
            for(int i=0;i<colun;i++)
            {
                item->setBackgroundColor(i,QColor("#ffffff"));
            }
        }
    }

}

void DarwinTreeWidget::delete_Item(QTreeWidgetItem * item)
{
    if(item==NULL)
        return;
    if(item->childCount()>1)
    {
        for(int i=0;i<item->childCount();i++)
        {
            delete_Item(item->child(i));
        }
    }else {
        this->m_itemList.remove(item->text(m_key));
        delete item;
    }
}

void DarwinTreeWidget::on_delete_function()
{
    QTreeWidgetItem * item = this->currentItem();
    if(item == NULL)
    {
        return;
    }
    if(item->text(item->columnCount()-2)==QString::number(NORMAL,10) ||item->text(item->columnCount()-2)==QString::number(EDITED,10) )
    {
        if(item->text(item->columnCount()-2)==QString::number(EDITED,10))
        {
            item->setText(item->columnCount()-2,QString::number(DELETE,10));
            QTreeWidgetItem * itemtem = this->m_HHistoryTable.take(item->text(m_key));
            if(itemtem==NULL)
            {
                ;
            }
            else if(itemtem->text(1)==QString::number(DELETE,10))
            {
                delete itemtem;
                QTreeWidgetItem* tmp = this->m_HConflictList.take(item->text(m_key));
                if(tmp!=NULL)
                {
                    delete tmp;
                }
                this->m_HUptadeTable.remove(item->text(m_key));
                 this->delete_Item(item);
                 return;

             }
            else if(itemComparison(item,itemtem))
            {
                delete itemtem;
            }
            else
            {
                int column= item->columnCount()-2;
                for(int i=0;i<column;i++)
                {
                    item->setText(i,itemtem->text(i));
                }
            }

            this->insertUptadeTable(item);
            for(int i=0;i<item->columnCount()-2;i++)
            {
                item->setBackgroundColor(i,QColor("#ff9900"));
            }
        }else
        {
            item->setText(item->columnCount()-2,QString::number(DELETE,10));
            this->insertUptadeTable(item);
            for(int i=0;i<item->columnCount()-2;i++)
            {
                item->setBackgroundColor(i,QColor("#ff9900"));
            }
        }

    //    qDebug()<<"delete Current "<<this->m_HUptadeTable.count(item->text(m_key));
    //    qDebug()<<"delete Hostory "<<this->m_HHistoryTable.count(item->text(m_key));
    //    qDebug()<<"delete itemlist "<<this->m_itemList.value(item->text(m_key))->text(item->columnCount()-2);
    }
    else if(item->text(item->columnCount()-2)==QString::number(DELETE,10))
    {
        item->setText(item->columnCount()-1,QString::number(NORMAL,10));
        item->setText(item->columnCount()-2,QString::number(NORMAL,10));
        this->m_HUptadeTable.remove(item->text(m_key));
        QTreeWidgetItem *tmp = this->m_HHistoryTable.take(item->text(m_key));
        if(tmp!=NULL)
        {
            QTreeWidgetItem *tmp2 = this->m_HConflictList.take(item->text(m_key));
            if(tmp2!=NULL)
            {
                delete tmp2;
            }
            int clon = item->columnCount();
            for(int i=0;i<clon-2;i++)
            {
                item->setText(i,tmp->text(i));
            }
            delete tmp;

        }
        for(int i=0;i<item->columnCount()-2;i++)
        {
            item->setBackgroundColor(i,QColor("#ffffff"));
        }
  //      qDebug()<<"remove delete Current "<<this->m_HUptadeTable.count(item->text(m_key));
   //     qDebug()<<"remove delete Hostory "<<this->m_HHistoryTable.count(item->text(m_key));
    }

}
void DarwinTreeWidget::on_edit_function()
{
   QTreeWidgetItem *item = this->currentItem();
   if(item==NULL)
   {
       return;
   }
   int index = this->currentColumn();
   if(m_key==index)
           return;
   this->openPersistentEditor(item,index);

   if(item->text(item->columnCount()-2)==QString::number(NORMAL,10) || item->text(item->columnCount()-2)==QString::number(DELETE,10))
   {
       QTreeWidgetItem *item2 = new QTreeWidgetItem(*item);
       m_HHistoryTable.insert(item->text(m_key),item2);
       item->setText(item->columnCount()-2,QString::number(EDITED,10));

   }
   item->setText(item->columnCount()-1,QString::number(EDITED,10));
   this->clearSelection();

}

void DarwinTreeWidget::on_revokededit_function()
{
    QTreeWidgetItem *item = this->currentItem();
    if(item==NULL)
    {
        return;
    }
    QTreeWidgetItem *itemtem = this->m_HHistoryTable.take(item->text(m_key));
    QTreeWidgetItem *itemConflict = this->m_HConflictList.take(item->text(m_key));
    if(itemtem!=NULL)
    {
        int coln = item->columnCount();
        for(int i=0;i<coln;i++)
        {
            item->setText(i,itemtem->text(i));
            item->setBackgroundColor(i,QColor("#ffffff"));
        }
        delete itemtem;
    }
    if(itemConflict!=NULL)
    {
        delete itemConflict;
    }
    this->m_HUptadeTable.remove(item->text(m_key));
    item->setText(item->columnCount()-2,QString::number(NORMAL,10));
    item->setText(item->columnCount()-1,QString::number(NORMAL,10));
}

void DarwinTreeWidget::keyPressEvent(QKeyEvent *event)
{

    int colun=this->columnCount();
    QTreeWidgetItem *item = this->currentItem();

    qDebug()<<item->text(colun);
    if(item==NULL)
        return;

    if (event->key() == Qt::Key_Return && item->text(item->columnCount()-1)==QString::number(EDITED,10))
    {
        item->setText(item->columnCount()-1,QString::number(ITEMEDITED,10));
    }

}

void DarwinTreeWidget::on_itemChanged_function(QTreeWidgetItem*item,int n)
{

    if(item!=NULL)
    {

        if(item!=NULL && item->text(item->columnCount()-1)==QString::number(ITEMEDITED,10))
        {
            qDebug()<<"saveModify";
            QTreeWidgetItem *itemhost = this->m_HHistoryTable.value(item->text(m_key));
            for(int i=0;i<item->columnCount();i++)
            {
                qDebug()<<item->text(i);
            }
            if(!this->itemComparison(item,itemhost))
            {
                item->setText(item->columnCount()-1,QString::number(EDITED,10));
                emit currentItemChanged(item,item);
            }

        }
    }


}
void DarwinTreeWidget::getItem()
{
     QTreeWidgetItem *item = this->currentItem();
     if(item!=NULL)
     {
        for(int i=0;i<item->columnCount();i++)
        {
            qDebug()<<item->text(i);
        }
     }
}
void DarwinTreeWidget::on_currentItemChanged_function(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
 //   qDebug()<<"on_currentItemChanged_function current ";
//    for(int i=0;i<current->columnCount();i++)
//    {
//        qDebug()<<current->text(i);
//    }
//    qDebug()<<"on_currentItemChanged_function current end";

    if(previous != NULL && previous->text(previous->columnCount()-1)==QString::number(EDITED,10))
    {
        int colun=previous->columnCount();
        qDebug()<<"on_currentItemChanged_function previous ";
        for(int i=0;i<previous->columnCount();i++)
        {
            qDebug()<<previous->text(i);
        }
     //    qDebug()<<"on_currentItemChanged_function previous end";
        QTreeWidgetItem* item = this->m_HHistoryTable.value(previous->text(m_key),NULL);
        if(item!=NULL)
        {
            if(this->itemComparison(item,previous))
            {
                QTreeWidgetItem* item = this->m_HHistoryTable.take(previous->text(m_key));
                delete item;
                this->m_HUptadeTable.remove(previous->text(m_key));
                previous->setText(colun-2,QString::number(NORMAL,10));
           //     qDebug()<<"item is true Histor"<<this->m_HHistoryTable.count(previous->text(m_key));
           //     qDebug()<<"item is true Current"<<this->m_HUptadeTable.count(previous->text(m_key));
                for(int i=0;i<previous->columnCount();i++)
                {
                    previous->setBackgroundColor(i,QColor("#ffffff"));
                }
            }
            else
            {

                for(int i=0;i<previous->columnCount()-2;i++)
                {
                    previous->setBackgroundColor(i,QColor("#33ffff"));
                }
          //      qDebug()<<"m_HHistoricalOperation";
//                 for(int i=0;i<item->columnCount();i++)
//                 {
//                     qDebug()<<item->text(i);
//                 }
            //    qDebug()<<"m_HHistoricalOperation end";
                 previous->setText(previous->columnCount()-2,QString::number(EDITED,10));
                 this->insertUptadeTable(previous);
             //    qDebug()<<"is not true Historic"<<this->m_HHistoryTable.count(previous->text(m_key));
             //    qDebug()<<"is not true"<<this->m_HUptadeTable.count(previous->text(m_key));
            }
        }
        for(int i=0;i<colun-2;i++)
        {
            this->closePersistentEditor(previous,i);
        }
        previous->setText(colun-1,QString::number(NORMAL,10));

    }

}
void DarwinTreeWidget::setDate()
{
    qDebug()<<"setDate";
    if(num==4)
    {
        for(int i=0;i<1000;i++)
        {
            if(i%2)
            {
               // QTreeWidgetItem* item2 = this->m_itemList["a"+QString::number(i, 10)];

    //            if(item2!=NULL&&item2->text(item2->columnCount()-1)=="NORMAL")
    //            {
    //                    this->m_itemList["a"+QString::number(i, 10)]->setText(1,QString::number(i+500+num, 10));
    //                    this->m_itemList["a"+QString::number(i, 10)]->setText(2,"class"+QString::number(i+500+num, 10));
    //                this->m_itemList["a"+QString::number(i, 10)]->setText(3,QString::number(i+500+num, 10));


    //            }else
    //            {
    //                qDebug()<<"null"<<i;
    //            }
                QTreeWidgetItem* item2 = this->m_itemList.take("a"+QString::number(i, 10));
                delete_Item(item2);
            }
        }

    }else
    {
        qDebug()<<"add";
        QStringList list;
        list.append("hahah");
        list.append("hehehe");
        list.append("heihei");
        list.append("NORMAL");
        list.append("NORMAL");
        QTreeWidgetItem * item = new QTreeWidgetItem(list);
        this->m_itemList.insert(item->text(m_key),item);
        this->createTree();
    }


    //this->createTree();
    num +=100;
    QTreeWidgetItem *item = this->m_itemList.value("hahha");
    if(item==NULL)
    {
        qDebug()<<"hahaha is null";
    }else
    {
        qDebug()<<"hahaha is not null";
    }

}
void DarwinTreeWidget::contextMenuEvent(QContextMenuEvent * event)
{

    QTreeWidgetItem * itemlist = this->currentItem();
    if(itemlist==NULL)
    {
        qDebug("place");
    }
    else if(this->isEdit)
    {
       menu = new QMenu();
       if(itemlist->text(itemlist->columnCount()-2) == QString::number(EDITED,10))
       {
            connect( menu->addAction("revoked edit"),SIGNAL(triggered()),this,SLOT(on_revokededit_function()));
       }
       connect(menu->addAction("edit"),SIGNAL(triggered()),this,SLOT(on_edit_function()));
       if(itemlist->text(itemlist->columnCount()-2) == QString::number(DELETE,10))
       {
           connect( menu->addAction("revoked delete"),SIGNAL(triggered()),this,SLOT(on_delete_function()));
       }else
       {
           connect( menu->addAction("delete"),SIGNAL(triggered()),this,SLOT(on_delete_function()));
       }


       menu->exec(QCursor::pos());
       delete menu;
       menu = NULL;
    }

}

void DarwinTreeWidget::on_timer_function()
{
    qDebug()<<"time out erent";
//    if(time->isActive())
//        time->stop();
     QTreeWidgetItem *item;
     QHash<QString,QTreeWidgetItem *> ha;
    for(int i=0;i<15;i++)
    {
        if(i!=2&&i!=5&&i!=8)
        {
            QStringList list;
            list.append("a"+QString::number(i, 10));
            list.append(QString::number(i+1, 10));
            list.append("class"+QString::number(i+1, 10));
            list.append(QString::number(NORMAL,10));
            list.append(QString::number(NORMAL,10));
            item=new QTreeWidgetItem((list));
            ha.insert("a"+QString::number(i, 10),item);
        }


    }
//QList<QString> keys = this->m_itemList.keys();
//foreach (QString key, keys) {
//    QTreeWidgetItem *t = ha.value(key);
//    if(t==NULL)
//    {
//        qDebug()<<key;
//    }

//}

    this->allPull(ha);
//this->partPull(ha);
    qDebug()<<"m_HHistoryTable"<<this->m_HHistoryTable.size();
    foreach (QTreeWidgetItem* item, this->m_HHistoryTable) {
        for(int i =0;i<item->columnCount();i++)
        {
            qDebug()<<"m_HHistoryTable:"<<item->text(i);
        }
    }

    qDebug()<<"m_HConflictList"<<this->m_HConflictList.size();
    foreach (QTreeWidgetItem* item, this->m_HConflictList) {
        for(int i =0;i<item->columnCount();i++)
        {
            qDebug()<<"m_HConflictList"<<item->text(i);
        }
    }
    qDebug()<<"m_HUptadeTable"<<this->m_HUptadeTable.size();
    foreach (QList<QString> list, this->m_HUptadeTable) {
        foreach (QString s, list) {
            qDebug()<<"m_HUptadeTable"<<s;
        }
    }
}


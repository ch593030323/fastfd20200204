#include "lds_treewidget.h"
#include <QMouseEvent>

lds_treeWidget::lds_treeWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    rootitem=0;
    stack_key.push_back("0");
    this->setAlternatingRowColors(true);
    //    this->setAnimated(true);
}

void lds_treeWidget::deleteAllItem()
{
    for(int index = 0; index< topLevelItemCount(); index++)
        deleteItem(this->topLevelItem(index));
}

void lds_treeWidget::select_beforerow()
{
    QString key = "0";
    if(stack_key.count() == 2) {
        key = stack_key.last();
    }

    selectItemBykey(key);
}

QString lds_treeWidget::key()
{
    QString k;
    foreach(int deep, treedeepvect) {
        k += QString::number(deep)+"_";
    }
    if(!k.isEmpty()) k.chop(1);
    return k;
}

void lds_treeWidget::selectItemBykey(const QString &key)
{
    treedeepvect.clear();
    QStringList keys = key.split("_");
    foreach(const QString &d, keys) {
        treedeepvect.append(d.toInt());
    }
    setcurdeep();
}


void lds_treeWidget::setItemSizeHint(int column, QSize size)
{
    for(int index = 0; index< topLevelItemCount(); index++)
        setItemSizeHint0(this->topLevelItem(index), column, size);
}

void lds_treeWidget::setItemSizeHint0(QTreeWidgetItem *item, int column, QSize size)
{
    for(int r=0,r_count=item->childCount();r<r_count;r++) {
        if(item->child(r)) {
            if(item->child(r)->childCount() > 0) {
                setItemSizeHint0(item->child(r), column, size);
            } else {
                item->child(r)->setSizeHint(column, size);
            }
        }
    }
    item->setSizeHint(column, size);
}

int lds_treeWidget::deepofitem(QTreeWidgetItem *item)
{
    int deep = 0;
    if(item) {
        while(item->parent()) {
            deep++;
            item=item->parent();
        }
    }
    return deep;
}

void lds_treeWidget::set_0_Index(int index)
{
    treedeepvect.clear();
    treedeepvect << 0;
    treedeepvect <<index;
    setcurdeep();
}

int lds_treeWidget::get_0_Index()
{
    if(treedeepvect.count() >=2) {
        if(treedeepvect[0]==0) {
            return treedeepvect[1];
        }
    }
    return -1;
}

void lds_treeWidget::restore()
{
    //011
    setcurdeep();
}

void lds_treeWidget::setcurdeep()
{
    QTreeWidgetItem *fitem=rootitem;
    if(fitem == 0)
        fitem = this->headerItem();
    if(fitem) {
        for(int c=0,c_count=treedeepvect.count();c<c_count;c++) {
            if(c==0) {
                fitem=this->topLevelItem(treedeepvect[c]);
            } else {
                if(fitem)
                    fitem=fitem->child(treedeepvect[c]);
            }
        }
        this->setCurrentItem(fitem);
        this->expandItem(fitem);

        savedeep();
    }
}

void lds_treeWidget::deleteItem(QTreeWidgetItem *item)
{
    for(int r=0,r_count=item->childCount();r<r_count;r++) {
        if(item->child(r)) {
            if(item->child(r)->childCount() > 0) {
                deleteItem(item->child(r));
            } else {
                delete item->child(r);
            }
        }
    }
    delete item;
}

void lds_treeWidget::savedeep()
{
    treedeepvect.clear();
    QTreeWidgetItem *fitem=this->currentItem();
    while(fitem) {
        if(!fitem->parent()) break;
        treedeepvect.push_front(fitem->parent()->indexOfChild(fitem));
        fitem=fitem->parent();
    }
    treedeepvect.push_front(indexOfTopLevelItem(fitem));

    while(stack_key.count() > 1) stack_key.pop_front();
    stack_key.push_back(key());
}

void lds_treeWidget::savedeepEmitFinished()
{
    savedeep();
    emit signalSavedFinished();
}

//void expanded(const QModelIndex &index);
//void collapsed(const QModelIndex &index);
void lds_treeWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidgetItem*item= itemAt(event->pos());
    QTreeWidgetItem*item0= this->currentItem();
    QModelIndex cur=this->currentIndex();

    if(item
            &&item0
            &&item==item0) {
        this->setCurrentItem(item);
        if(this->isExpanded(cur))
            //            emit collapsed(cur);
            this->collapse(cur);
        else
            //            emit expanded(cur);
            this->expand(cur);
    } else {
        if(item)this->setCurrentItem(item);
    }
    emit pressed(cur);
    savedeepEmitFinished();
}

void lds_treeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QTreeWidget::mouseReleaseEvent(event);
}




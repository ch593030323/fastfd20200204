#include "ftableview_standmodel_virtual.h"
#include <QtDebug>
#include <QTableView>
#include <QTimer>
#include <QEventLoop>
#include <QApplication>
#include <QStateMachine>
#include <QDir>

ftableview_standmodel_virtual::ftableview_standmodel_virtual(ftableview_standmodel_virtual::TYPE t)
    :QStandardItemModel(0)
{
    _type = t;
    indexBegin();
    selectionmodel = 0;
    valuestep_pre = valuestep = 0;
    user_role_equal_max = Qt::UserRole;
}

bool ftableview_standmodel_virtual::model_data_set(int row, int column, const QVariant &value, int role)
{
    return this->setData(this->index(row, column), value, role);
}

bool ftableview_standmodel_virtual::model_data_set(int row, int column, const QMap<int, QVariant> &map)
{
    for(QMap<int, QVariant>::const_iterator k = map.begin(); k != map.end(); k ++) {
        if(!model_data_set(row, column, k.value(), k.key())) {
                return false;
        }
    }
    return true;
}

QVariant ftableview_standmodel_virtual::model_data(int row, int column, int role) const
{
    return this->data(this->index(row, column), role);
}

void ftableview_standmodel_virtual::set_row_column(int row_count, int column_count)
{
    this->removeRows(0, this->rowCount());
    this->removeColumns(0, this->columnCount());//中餐下的地图模式，若没有清除所有列的话，则是不会tableview 是会有滚动条的
    for(int row = 0; row < row_count; row++) {
        QList<QStandardItem*> items;
        for(int column = 0; column < column_count; column ++) {
            items << new QStandardItem;
        }
        this->appendRow(items);
    }
}

void ftableview_standmodel_virtual::setSelectionModel(QItemSelectionModel *m)
{
    if(selectionmodel == 0) {
        selectionmodel = m;
    } else {
#ifdef QT_DEBUG
        qDebug() << __FILE__ << __LINE__ << "selectionModel existed! ";
        QObject *o = 0;
        o->children();
#endif
    }
}

Qt::ItemFlags ftableview_standmodel_virtual::flags(const QModelIndex &index) const
{
    if(data(index, Qt::UserRole+1).isValid()) {//key
        return QStandardItemModel::flags(index);
    }
    return Qt::NoItemFlags;
}

void ftableview_standmodel_virtual::updatedata(int r, int c, const QMap<int, QVariant> &map)
{
    clearData(r, c);

    for(QMap<int/*role*/, QVariant>::const_iterator d = map.constBegin();
        d != map.constEnd();
        d++) {
        model_data_set(r, c, d.value(), d.key());
    }
}

/*一般情况下：
edta是显示的数据
udata的索引值
ddata 表示的是图片*/


void ftableview_standmodel_virtual::updatedata(int r, int c, const QVariant &u, const QVariant &u1, const QVariant &u2)
{
    clearData(r, c);

    model_data_set(r, c, u, Qt::UserRole);//显示的文字
    model_data_set(r, c, u1, Qt::UserRole+1);//索引值
    model_data_set(r, c, u2, Qt::UserRole+2);//图片路径
}

void ftableview_standmodel_virtual::updatedata(int r, int c, const QSqlRecord &record)
{
    clearData(r, c);

    int  count = record.count();
    for(int d = 0; d < count; d++) {
        model_data_set(r, c, record.value(d),  Qt::UserRole + d);//显示的文字
    }
}

void ftableview_standmodel_virtual::updatep2pdelay()
{
    emit signalstepstop();
    emit signalstepstart();
}

void ftableview_standmodel_virtual::indexBegin()
{
    curindex = 0;
    preindex = 0;
}

void ftableview_standmodel_virtual::clearData(int r, int c)
{
    for(int k = Qt::UserRole; k <= user_role_equal_max; k++) {
        model_data_set(r, c, QVariant(), k);
    }
//    user_role_equal_max = Qt::UserRole;
}

bool ftableview_standmodel_virtual::setData(const QModelIndex &index, const QVariant &value, int role)
{
    user_role_equal_max = qMax(user_role_equal_max, role);
    return QStandardItemModel::setData(index, value, role);
}

void ftableview_standmodel_virtual::disablePageRecord()
{
    b_page_record = false;
}

void ftableview_standmodel_virtual::removePageRecordDeep(int deep)
{
    page_record_skip_deep << deep;
}

bool ftableview_standmodel_virtual::hasSelected()
{
    if(0 == selectionmodel) {
#ifdef QT_DEBUG
        qDebug() << "selectionmodel is 0, error!";
        QObject *o = 0;
        o->children();
 #endif
        return false;
    }

    page_record_refresh();
    return page_record.count() > 0;
}

void ftableview_standmodel_virtual::clearSelected()
{
    if(0 == selectionmodel) {
#ifdef QT_DEBUG
        qDebug() << "selectionmodel is 0, error!";
        QObject *o = 0;
        o->children();
 #endif
        return;
    }

    selectionmodel->clearSelection();
    page_record.clear();
}

QVariantList ftableview_standmodel_virtual::currentSelected(int deep)
{
    page_record_refresh();
    return page_record_value(deep);

}

const QMap<QString, QVariantList> &ftableview_standmodel_virtual::currentSelected()
{
    page_record_refresh();
    return page_record;
}

QStringList ftableview_standmodel_virtual::currentSelected_usr_1()
{
    QStringList rets;
    page_record_refresh();
    foreach(const QVariant &d, page_record_value(1)) {
        rets << d.toString();
    }
    return rets;
}

void ftableview_standmodel_virtual::page_record_refresh()
{
    if(0 == selectionmodel) {
#ifdef QT_DEBUG
        qDebug() << "selectionmodel is 0, error!";
        QObject *o = 0;
        o->children();
 #endif
        return;
    }
    //手动不触发select record
    if(false == b_page_record)
        return;

    for(int row = 0; row < this->rowCount(); row ++) {
        for(int column = 0; column < this->columnCount(); column ++) {
            QString usr_1 = model_data(row, column, Qt::UserRole + 1).toString();
            if(usr_1 == "-1" //上一页
                    || usr_1 == "-2"//下一页
                    || usr_1 == "-3"//+
                    ) {
                continue;
            }
            //remove
            page_record.remove(usr_1);

            //insert
            if(selectionmodel->isSelected(this->index(row, column))) {
                for(int d = Qt::UserRole; d <= user_role_equal_max; d++) {
                    page_record[usr_1] << model_data(row, column, d);
                }
            }
        }
    }
}

void ftableview_standmodel_virtual::page_record_restore()
{
    if(0 == selectionmodel) {
#ifdef QT_DEBUG
        qDebug() << "selectionmodel is 0, error!" << this->metaObject()->className();
        QObject *o = 0;
        o->children();
 #endif
        return;
    }
    selectionmodel->clearSelection();
    //手动不触发select record
    if(false == b_page_record)
        return;

    for(int row = 0; row < this->rowCount(); row++) {
        for(int column = 0; column < this->columnCount(); column++) {
            QString usr_1 = model_data(row, column, Qt::UserRole + 1).toString();
            if(page_record.contains(usr_1)) {//find key
                selectionmodel->select(this->index(row, column), QItemSelectionModel::Select);//select
                selectionmodel->setCurrentIndex(this->index(row, column), QItemSelectionModel::SelectCurrent);
                for(int count = page_record.value(usr_1).count(), role = 0;
                    role < count && !page_record_skip_deep.contains(role);
                    role ++) {
                    model_data_set(row, column, page_record.value(usr_1).value(role)/*userdeep*/, Qt::UserRole + role);
                }
            }
        }
    }
}

bool ftableview_standmodel_virtual::page_record_set_data(const QString &usr_1, const QVariant &value, int role)
{
    int deep = role - Qt::UserRole;//3 2 1 0
    if(page_record.contains(usr_1)) {
        QVariantList &dvalue = page_record[usr_1];
        if(deep < dvalue.count()) {
            dvalue[deep] = value;
        }
        return true;
    }
    return false;
}

bool ftableview_standmodel_virtual::page_record_remove(const QString &usr_1)
{
    page_record.remove(usr_1);
    return true;
}

QVariantList ftableview_standmodel_virtual::page_record_value(int deep)
{
    QVariantList retdata;
    for(QMap<QString /*user + 1*/,  QVariantList /*user + 0->n*/>::iterator k = page_record.begin(); k != page_record.end(); k++) {
        retdata << k.value().value(deep);
    }
    return retdata;
}

void ftableview_standmodel_virtual::refreshcur()
{
    curindex = preindex;
    togenerateNext();
}

int ftableview_standmodel_virtual::step() const
{
    return valuestep;
}

void ftableview_standmodel_virtual::setstep(int value)
{
    valuestep = value;
    int columncount = this->columnCount();
    int addvalue = (valuestep_pre <= value? 1:-1);
    for(; valuestep_pre != value; valuestep_pre += addvalue) {
        if(addvalue == 1) {
            int r = valuestep_pre/columncount;
            int c = valuestep_pre%columncount;
            QVariant d = model_data(r, c, Qt::UserRole+2);
            if(d.canConvert<QString>()) {
                QPixmap p(imagedishdir+QDir::separator()+ d.toString());
                if(!p.isNull()) {
                    model_data_set(r, c, p, Qt::UserRole+2);
                }
            }
        }
    }
}

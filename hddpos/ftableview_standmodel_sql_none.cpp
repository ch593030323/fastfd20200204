#include "ftableview_standmodel_sql_none.h"
#include <QItemSelectionModel>
#include <QtDebug>
#include "lds.h"
#include <QTimer>

QVariant ftableview_standmodel_sql_none::list_data(const QList<ftableview_standmodel_sql_none::noneData> &list, const QString &usr_1, int role)
{
    int k = list_index(list, usr_1);
    if(k != -1) {
        return list[k].value(role);
    }
    return QVariant();
}

void ftableview_standmodel_sql_none::list_remove(QList<ftableview_standmodel_sql_none::noneData> &list, const QString &usr_1)
{
    int k = list_index(list, usr_1);
    if(k != -1) {
        list.removeAt(k);
    }
}

int ftableview_standmodel_sql_none::list_index(const QList<ftableview_standmodel_sql_none::noneData> &list, const QString &usr_1)
{
    for(int k = 0; k < list.count(); k ++) {
        if(list[k].value(Qt::UserRole + 1).toString() == usr_1) {
            return k;
        }
    }
    return -1;
}

QList<ftableview_standmodel_sql_none::noneData> ftableview_standmodel_sql_none::stringlist2datalist(const QStringList &list)
{
    QList<ftableview_standmodel_sql_none::noneData> list_data;
    for(int k = 0; k < list.count(); k++) {
        list_data << ftableview_standmodel_sql_none::noneData(list[k],
                                                              k);
    }
    return list_data;
}

ftableview_standmodel_sql_none::ftableview_standmodel_sql_none() :
    ftableview_standmodel_virtual(TYPE_list)
{
}

void ftableview_standmodel_sql_none::setPrepareList(const QList<ftableview_standmodel_sql_none::noneData> &list)
{
    _data_list = list;
    page_record.clear();//做法会保留第二页的部分导致,点菜时总是有默认做法
}

void ftableview_standmodel_sql_none::setPrepareList(const QList<ftableview_standmodel_sql_none::noneData> &list, const QStringList &select_usr_1_list)
{
    setPrepareList(list);
    foreach(const QString& usr_1, select_usr_1_list) {
        select(usr_1);
    }

    //    for(int k = 0; k < list.count(); k++) {
    //        QString usr_1 = list[k].value(Qt::UserRole + 1).toString();
    //        if(select_usr_1_list.contains(usr_1)) {
    //            page_record.insert(usr_1, list[k].values());
    //        }
    //    }
}

void ftableview_standmodel_sql_none::setModelList(const QList<ftableview_standmodel_sql_none::noneData> &list)
{
    for(int row = 0; row < this->rowCount(); row ++) {
        for(int column = 0; column < this->columnCount(); column ++) {
            int k = row * this->columnCount() + column;
            if(k < list.count())
                this->model_data_set(row, column, list[k].map());
        }
    }
}

void ftableview_standmodel_sql_none::select(const QString &usr_1)
{
    for(int k = 0; k < _data_list.count(); k++) {
        if(usr_1 == _data_list[k].value(Qt::UserRole + 1).toString()) {
            page_record.insert(usr_1, _data_list[k].values());
            break;
        }
    }
}

void ftableview_standmodel_sql_none::set_list_data(const QVariant &u1, const QVariant &value, int role)
{
    for(int k = 0; k < _data_list.count(); k++) {
        if(_data_list[k].value(Qt::UserRole + 1) == u1) {
            _data_list[k].setValue(role, value);
        }
    }
}

const QList<ftableview_standmodel_sql_none::noneData> &ftableview_standmodel_sql_none::data_list()
{
    return _data_list;
}

void ftableview_standmodel_sql_none::initDelay(int mesc)
{
    QTimer::singleShot(mesc, this, SLOT(toinit()));
}

void ftableview_standmodel_sql_none::toinit()
{
    indexBegin();
    generateNext();
    //
    if(page_record.count() > 0) {
        while(!atEnd()) {
            for(int row = 0; row < this->rowCount(); row ++) {
                for(int column = 0; column < this->columnCount(); column ++) {
                    if(page_record.contains(model_data(row, column, Qt::UserRole + 1).toString())) {
                        goto end;
                    }
                }
            }
            generateNext();
        }
    }
end:
    return;
}

QVariantList ftableview_standmodel_sql_none::get_value_unselect(int role)
{
    QVariantList unselects;
    QStringList selects = currentSelected_usr_1();
    for(int k = 0; k < _data_list.count(); k++) {
        if(selects.contains(_data_list[k].value(Qt::UserRole + 1).toString())) {

        } else {
            unselects << _data_list[k].value(role);
        }
    }
    return unselects;
}


bool ftableview_standmodel_sql_none::atEnd()
{
    return curindex >= _data_list.count();
}

void ftableview_standmodel_sql_none::enableEndAppendNew(bool enabled)
{
    b_endAppendNew = enabled;
}

QMap<int, QVariant> ftableview_standmodel_sql_none::getMapByKey(const QString &usr_1)
{
    for(int k = 0; k < _data_list.count(); k++) {
        if(_data_list[k].value(Qt::UserRole + 1).toString() == usr_1) {
            return _data_list[k].map();
        }
    }
    return QMap<int, QVariant>();
}

QVariant ftableview_standmodel_sql_none::value(const QModelIndex &index, const QString &field_name)
{
    const ftableview_standmodel_sql_none *m = qobject_cast<const ftableview_standmodel_sql_none *> (index.model());

    if(m->_data_list.count() == 0)
        return QVariant();
    const QList<noneData::rowData> &d = m->_data_list[0].content;
    for(int k = 0; k < d.count(); k ++) {
        if(d[k].field_name == field_name) {
            return m->data(index, d[k].role);
        }
    }
    return QVariant();
}

QString ftableview_standmodel_sql_none::keyvalue(const QModelIndex &index)
{
    const ftableview_standmodel_sql_none *m = qobject_cast<const ftableview_standmodel_sql_none *> (index.model());

    return m->data(index, Qt::UserRole + 1).toString();
}

void ftableview_standmodel_sql_none::generateNext()
{
    int updownum = 0;
    int index = 0;
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total;
    bool hasnext = false;
    if(curindex > 0) {
        updatedata(0,0,QObject::tr("上一页"), -1,-1);
        updownum ++;
        index ++;
        total = vcount * hcount-1;
    } else {
        total = vcount * hcount;
    }

    //7
    if(_data_list.count() > (curindex + total)) {
        hasnext = true;
        updownum ++;
    }

    togenerate(index,updownum,hasnext);
    //2
}

void ftableview_standmodel_sql_none::togenerate(int index, int updownum, bool hasnext)
{
    //点菜线程显示停止
    //    tostop();
    //=================================
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total = vcount * hcount;

    preindex = curindex;

    //    _data_list.mid(curindex, total -  updownum);
    for(int k = 0, count = total - updownum; k < count; k++) {
        if((curindex) >= (_data_list.count())) {
            break;
        }
        updatedata(index/hcount, index%hcount, _data_list[curindex].map());
        index++;
        curindex ++;
    }

    //1填充满了，而且有新增，则最后一个改为》》
    //2填充没有慢，而且有新增，则最后一个为New
    if(b_endAppendNew) {
        if(false == hasnext && index == total) {//1
            hasnext = true;
            index  --;
            curindex  --;
        } else if(false == hasnext &&index < total) {//2
            updatedata(index/hcount, index%hcount, "", "-3", "");
            index++;
            curindex ++;
        }
    }
    if(hasnext && index < total) {
        updatedata(index/hcount, index%hcount, QObject::tr("下一页"), -2, -2);
        index++;
    }
    for(;index < total; index++) {
        updatedata(index/hcount, index%hcount, "", QVariant(),  "");//主要是usr_1的值为null
    }
    ///还原选区
    page_record_restore();
}

void ftableview_standmodel_sql_none::togenerateNext()
{
    ///刷新当前页的select
    page_record_refresh();
    generateNext();
}

void ftableview_standmodel_sql_none::togeneratePre()
{
    ///刷新当前页的select
    page_record_refresh();

    int updownum = 0;
    int index = 0;
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total = vcount * hcount;
    bool hasnext = true;
    preindex = preindex - total + 2;
    updatedata(vcount-1, hcount-1, QObject::tr("下一页"), -2, -2);
    updownum++;
    if(preindex == 1) {
        preindex = 0;
    }
    if(preindex > 0) {
        updatedata(0, 0, QObject::tr("上一页"), -1, -1);
        updownum++;
        index++;
    }
    curindex  =preindex;

    togenerate(index,updownum,hasnext);
}



ftableview_standmodel_sql_none::noneData &ftableview_standmodel_sql_none::noneData::append(int role, const QVariant &value)
{
    rowData d;
    d.role = role;
    d.field_name = "UserRole + " + QString::number(role - Qt::UserRole);
    d.value = value;
    //
    for(int k = 0; k < content.count(); k ++) {
        if(content[k].role == d.role) {
            content[k].value = d.value;
            return *this;
        }
    }
    //
    content << d;
    return *this;
}

ftableview_standmodel_sql_none::noneData &ftableview_standmodel_sql_none::noneData::append(const QString &field_name, const QVariant &value)
{
    //
    int min_role = Qt::UserRole - 1;
    for(int k = 0; k < content.count(); k ++) {
        if(content[k].field_name == field_name) {
            content[k].value = value;
            return *this;
        }
        min_role = qMax(min_role, content[k].role);
    }
    min_role ++;
    //
    rowData d;
    d.field_name = field_name;
    d.value = value;
    d.role = min_role;
    content << d;
    return *this;
}

QVariant ftableview_standmodel_sql_none::noneData::value(int role) const
{
    for(int k = 0; k < content.count(); k ++) {
        if(content[k].role == role) {
            return content[k].value;
        }
    }
    return QVariant();
}

QVariant ftableview_standmodel_sql_none::noneData::value(const QString &field_name) const
{
    for(int k = 0; k < content.count(); k ++) {
        if(content[k].field_name == field_name) {
            return content[k].value;
        }
    }
    return QVariant();
}

QString ftableview_standmodel_sql_none::noneData::keyvalue() const
{
    return value(Qt::UserRole + 1).toString();
}

QVariantList ftableview_standmodel_sql_none::noneData::values() const
{
    QVariantList valuelist;
    if(content.count() == 0) {
        return valuelist;
    }
    foreach(const rowData &d, content) {
        int c = d.role - Qt::UserRole + 1;
        while(valuelist.count() < c) {
            valuelist << QVariant();
        }
        valuelist[c - 1] = d.value;
    }
    return valuelist;
}

QMap<int, QVariant> ftableview_standmodel_sql_none::noneData::map() const
{
    QMap<int, QVariant> m;
    for(int k = 0; k < content.count(); k ++) {
        m.insert(content[k].role, content[k].value);
    }
    return m;
}

void ftableview_standmodel_sql_none::noneData::init(const QVariant &key_value)
{
    content.clear();
    append(Qt::UserRole + 1, key_value);
}

void ftableview_standmodel_sql_none::noneData::print()
{
    qDebug() << "------begin";
    for(int k = 0; k < content.count(); k ++) {
        qDebug() << content[k].field_name << content[k].value;
    }
    qDebug() << "------end";
}

ftableview_standmodel_sql_none::noneData::noneData()
{

}

ftableview_standmodel_sql_none::noneData::noneData(const QVariant &u, const QVariant &u1)
{
    append(Qt::UserRole, u);
    append(Qt::UserRole + 1, u1);
}

ftableview_standmodel_sql_none::noneData::noneData(const QVariant &u, const QVariant &u1, const QVariant &u2)
{
    append(Qt::UserRole, u);
    append(Qt::UserRole + 1, u1);
    append(Qt::UserRole + 2, u2);
}

void ftableview_standmodel_sql_none::noneData::setValue(int role, const QVariant &u)
{
    append(role, u);
}

void ftableview_standmodel_sql_none::noneData::setValue(const QString &field_name, const QVariant &u)
{
    append(field_name, u);
}

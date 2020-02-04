#include "ftableview_standmodel_sql_table.h"
#include "lds_query_hddpos.h"
#include <QSqlRecord>
#include <QtDebug>
#include <QFile>
#include<QTableView>
#include<QApplication>

ftableview_standmodel_sql_table::ftableview_standmodel_sql_table() :
    ftableview_standmodel_virtual(TYPE_list)
{
    sqlargarg = " limit %1,%2";
}

QList<ftableview_standmodel_sql_table::checkData> ftableview_standmodel_sql_table::get_value_select()
{
    QList<checkData> checkdata;
    ///记录的是preindex的map
    page_record_refresh();
    //
    QVariantList datalist = page_record_value(1);
    QVariantList datalist_7 = page_record_value(7);
    for(int k = 0,count = datalist.count(); k < count; k++) {
        checkdata += checkData(datalist[k].toString(), datalist_7[k].toString());
    }
    return checkdata;
}

QList<QVariantList> ftableview_standmodel_sql_table::get_value_select_deep(QList<int> deeps)
{
    ///记录的是preindex的map
    page_record_refresh();
    QList<QVariantList> data_rect;
    for(int k = 0; k < deeps.count(); k++) {
        data_rect += page_record_value(deeps[k]);
    }
    return data_rect;
}

void ftableview_standmodel_sql_table::enableEndAppendNew(bool enabled)
{
    b_endAppendNew = enabled;
}


void ftableview_standmodel_sql_table::togenerate(int index, int updownum, bool hasnext)
{
    //点菜线程显示停止
    //    tostop();
    //=================================
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total = vcount * hcount;
    if(total == 0) return;

    lds_query_hddpos  query;

    preindex = curindex;
    query.execSelect(sql_cur+ sqlargarg.arg(curindex).arg(total - updownum));
    while(query.next()) {
        updatedata(index/hcount, index%hcount,query.record());
        clearCheckStatus(index/hcount, index%hcount);
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
    ///~
    if(hasnext && index < total) {
        updatedata(index/hcount, index%hcount, QObject::tr("下一页"), "-2", "");
        clearCheckStatus(index/hcount, index%hcount);
        index++;
    }
    for(;index < total; index++) {
        updatedata(index/hcount, index%hcount, "", QVariant(),  "");//主要是usr_1的值为null
        clearCheckStatus(index/hcount, index%hcount);
    }

    ///还原选区
    page_record_restore();
}

void ftableview_standmodel_sql_table::togenerateNext()
{
    ///刷新当前页的select
    page_record_refresh();

    lds_query_hddpos  query;
    int updownum = 0;
    int index = 0;
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total;
    bool hasnext = false;
    if(curindex > 0) {
        updatedata(0,0,QObject::tr("上一页"),"-1","");
        clearCheckStatus(0, 0);
        updownum ++;
        index ++;
        total = vcount * hcount-1;
    } else {
        total = vcount * hcount;
    }
    query.execSelect(sql_cur + sqlargarg.arg(curindex+total).arg(1));
    if(query.next()) {
        hasnext = true;
        updownum ++;
    }

    togenerate(index,updownum,hasnext);
}

void ftableview_standmodel_sql_table::togeneratePre()
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
    updatedata(vcount-1, hcount-1, QObject::tr("下一页"), "-2", "");
    clearCheckStatus(vcount-1, hcount-1);
    updownum++;
    if(preindex == 1) {
        preindex = 0;
    }
    if(preindex > 0) {
        updatedata(0,0, QObject::tr("上一页"), "-1", "");
        clearCheckStatus(0, 0);
        updownum++;
        index++;
    }
    curindex  =preindex;

    togenerate(index,updownum,hasnext);
}

void ftableview_standmodel_sql_table::toinit()
{
    toinit0();
}

void ftableview_standmodel_sql_table::toinitBySqlother(const QString &no)
{
    //1
    key_sqlother = no;
    //2
    sql_cur = sqlother.arg(no);
    toinit0();
}

void ftableview_standmodel_sql_table::toinit0()
{
    if(sql_cur.endsWith(";")) {
        qDebug() << "_________" << "sql endswith ';',  please manual check";
    }
    indexBegin();
    togenerateNext();
}

void ftableview_standmodel_sql_table::clearCheckStatus(int r, int c)
{
    this->setData(this->index(r,c), QVariant(), Qt::UserRole+7/*check*/);
}

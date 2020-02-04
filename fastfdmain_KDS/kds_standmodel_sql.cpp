#include "kds_standmodel_sql.h"
#include "lds_query_db.h"
#include "lds.h"
#include <QtDebug>
#include <QFile>
#include<QTableView>
#include<QApplication>
#include<QTimer>


kds_standmodel_sql::kds_standmodel_sql(ftableview_standmodel_virtual::TYPE t)
    :ftableview_standmodel_virtual(t)
{
    sqlargarg = " limit %1,%2";
    generateCount = 0;
}

void kds_standmodel_sql::toinit()
{
    curindex = 0;
    preindex = 0;
    togenerateNext();
}

void kds_standmodel_sql::togenerate(int index, int updownum, bool hasnext)
{
    //点菜线程显示停止
    //    tostop();
    //=================================
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total = vcount * hcount;
    lds_query_db  query;
    preindex = curindex;
    query.execSelect(sql_cur + sqlargarg.arg(curindex).arg(total - updownum));
    while(query.next()) {
        updatedata(index/hcount, index%hcount,
                   query.record());//一开始只是保存图片的名称，之后转换的时候会将内容切换秤图片，delegate读取直接读取图片信息
        index++;
        curindex ++;
    }
    if(hasnext && index < total) {
        updatedata(index/hcount, index%hcount, QObject::tr("下一页"), "-2", "");
        index++;
    }
    for(;index < total; index++) {
        updatedata(index/hcount, index%hcount, "", QVariant(),  "");//主要是usr_1的值为null
    }
    ///还原选区
    page_record_restore();
}

void kds_standmodel_sql::togenerateNext()
{
    ///刷新当前页的select
    page_record_refresh();

    lds_query_db  query;
    int updownum = 0;
    int index = 0;
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total;
    bool hasnext = false;
    if(curindex > 0) {
        updatedata(0,0, QObject::tr("上一页"), "-1", "");
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
    //2
}

void kds_standmodel_sql::togeneratePre()
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
    updownum++;
    if(preindex == 1) {
        preindex = 0;
    }
    if(preindex > 0) {
        updatedata(0, 0, QObject::tr("上一页"), "-1", "");
        updownum++;
        index++;
    }
    curindex  =preindex;

    togenerate(index,updownum,hasnext);
}

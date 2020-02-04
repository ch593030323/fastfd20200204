#include "ftableview_standmodel_sql.h"
#include "lds_query_hddpos.h"
#include "lds.h"
#include <QtDebug>
#include <QFile>
#include<QTableView>
#include<QApplication>
#include<QTimer>

ftableview_standmodel_sql::ftableview_standmodel_sql(TYPE t)
    :ftableview_standmodel_virtual(t)
{
    //    imageupdatethread = new imageupdate(this);
    //    sql= QString("select ch_dishno, vch_dishname from cey_bt_dish");
    sqlargarg = " limit %1,%2";
    generateCount = 0;
}

void ftableview_standmodel_sql::enableEndAppendNew(bool enabled)
{
    b_endAppendNew = enabled;
}

void ftableview_standmodel_sql::initDelay(int mesc)
{
    QTimer::singleShot(mesc, this, SLOT(toinit()));
}

void ftableview_standmodel_sql::togenerate(int index, int updownum, bool hasnext)
{
    //点菜线程显示停止
    //    tostop();
    //=================================
    int vcount = this->rowCount();
    int hcount = this->columnCount();
    int total = vcount * hcount;
    lds_query_hddpos  query;
    preindex = curindex;
    query.execSelect(sql_cur + sqlargarg.arg(curindex).arg(total - updownum));
    while(query.next()) {
        updatedata(index/hcount, index%hcount,
                   query.record());//一开始只是保存图片的名称，之后转换的时候会将内容切换秤图片，delegate读取直接读取图片信息
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
        index++;
    }
    for(;index < total; index++) {
        updatedata(index/hcount, index%hcount, "", QVariant(),  "");//主要是usr_1的值为null
    }
    ///还原选区
    page_record_restore();
    //=================================
    //    tostart();
    if(_type != TYPE_icon) return;
    if(false == lds::sysconf->value("system_setting/qt_show_dish_pic", true).toBool()) return;
    updatep2pdelay();
}

void ftableview_standmodel_sql::togenerateNext()
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

void ftableview_standmodel_sql::togeneratePre()
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

void ftableview_standmodel_sql::toinit()
{
    clearCache();
    QString no;//初始化
    QString matchkey = QString("%1%2").arg(no).arg(__LINE__);
    if(curgoing == matchkey) return;
    curgoing = matchkey;
    toinit0();
}

void ftableview_standmodel_sql::toinitBySqlother(const QString &no)
{
    //
    QString matchkey = QString("%1%2").arg(no).arg(__LINE__);
    if(curgoing == matchkey) return;
    curgoing = matchkey;
    sql_cur = sqlother.arg(no);
    toinit0();
}

void ftableview_standmodel_sql::toinitBySqlother2(const QString &no)
{
    //
    QString matchkey = QString("%1%2").arg(no).arg(__LINE__);
    if(curgoing == matchkey) return;
    curgoing = matchkey;
    sql_cur = sqlother2.arg(no);

    toinit0();
}

void ftableview_standmodel_sql::toinitBySqlother3(const QString &no)
{
    //
    QString matchkey = QString("%1%2").arg(no).arg(__LINE__);
    if(curgoing == matchkey) return;
    curgoing = matchkey;
    sql_cur = sqlother3.arg(no);

    toinit0();
}

void ftableview_standmodel_sql::toinitBySqlother4()
{
    //
    QString no;//初始化
    QString matchkey = QString("%1%2").arg(no).arg(__LINE__);
    if(curgoing == matchkey) return;
    curgoing = matchkey;
    sql_cur = sqlother4;
    toinit0();
}

void ftableview_standmodel_sql::clearCache()
{
    curgoing.clear();
}

void ftableview_standmodel_sql::toinit0()
{
    if(sql_cur.endsWith(";")) {
        qDebug("sql endswith ';',  please manual check");
    }
    curindex = 0;
    preindex = 0;
    togenerateNext();
}

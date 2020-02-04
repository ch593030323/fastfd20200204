#include "w_rpt_report_frame_btn_pageindex.h"
#include <QtDebug>
#include "lds_query_hddpos.h"
#include <QFile>

w_rpt_report_frame_btn_pageindex::w_rpt_report_frame_btn_pageindex(QWidget *parent)
    : lds_toolbar_button(parent)
{
}

QString w_rpt_report_frame_btn_pageindex::get_desc(int index)
{
    if(!isValid(index)) return "";
    //
    return QString("%1/%2(%3)").arg(get_cur_page(index)).arg(d_list[index].pagecount).arg(d_list[index].rowcount);
}

QString w_rpt_report_frame_btn_pageindex::get_limit_sql_update_desc(int index)
{
    setText(get_desc(index));
    if(d_list[index].onepagestep == -1) {
        return d_list[index].sql;
    }
    return d_list[index].sql + QString("  limit %1,%2 ").arg(d_list[index].curstep).arg(d_list[index].onepagestep);
}

QString w_rpt_report_frame_btn_pageindex::get_limit_sql(int index)
{
    return d_list[index].sql;
}

int w_rpt_report_frame_btn_pageindex::get_cur_page(int index)
{
    if(!isValid(index)) return 0;
    if(0 == d_list[index].onepagestep) return 0;
    //
    return d_list[index].curstep/d_list[index].onepagestep+1;
}

int w_rpt_report_frame_btn_pageindex::get_rowcount(int index)
{
    return d_list[index].rowcount;
}

int w_rpt_report_frame_btn_pageindex::get_onepagestep(int index)
{
    return d_list[index].onepagestep;
}

bool w_rpt_report_frame_btn_pageindex::isValid(int index)
{
    if(index < 0 || index >= d_list.count()) return false;

    return d_list[index].isValid();
}

bool w_rpt_report_frame_btn_pageindex::up(int index)
{
    if(!isValid(index)) return false;
    //
    int step =  d_list[index].curstep - d_list[index].onepagestep;
    if(!can_page_move(index, step)) return false;
    //
    d_list[index].curstep = step;
    emit signal_step_change();

    return true;
}

bool w_rpt_report_frame_btn_pageindex::down(int index)
{
    if(!isValid(index)) return false;
    //
    int step =  d_list[index].curstep + d_list[index].onepagestep;
    if(!can_page_move(index, step)) return false;
    //
    d_list[index].curstep =step;
    emit signal_step_change();

    return true;
}

void w_rpt_report_frame_btn_pageindex::setpage(int index, int page_value)
{
    if(!isValid(index)) return;
    //
    int step = page_value * d_list[index].onepagestep;
    if(!can_page_move(index, step)) return;
    //
    d_list[index].curstep = step;
    emit signal_step_change();
}

void w_rpt_report_frame_btn_pageindex::sum_clear(int index)
{
    d_list[index].total_list.clear();
}

void w_rpt_report_frame_btn_pageindex::sum_append(int index, const QString &fieldname)
{
    total_desc_value_Data  d;
    d.fieldname = fieldname;
    d_list[index].total_list.append(d);
}

void w_rpt_report_frame_btn_pageindex::sum_genrate(int index)
{
    QString sql_select ;
    foreach(const total_desc_value_Data &d, d_list[index].total_list) {
        sql_select += "sum("+d.fieldname +"),";
    }
    sql_select.chop(1);
    sql_select = "select "+ sql_select;
    lds_query_hddpos  query;
    query.execSelect(sql_select + " from(" + d_list[index].sql +")t" );
    while(query.next()) {
        int qcount = query.recordCount();
        while(qcount --) {
            d_list[index].total_list[qcount].value = query.recordValue(qcount).toFloat();
        }
    }
}

QList<w_rpt_report_frame_btn_pageindex::total_desc_value_Data> w_rpt_report_frame_btn_pageindex::get_sum_list_total_data(int index)
{
    //
    if(!isValid(index)) return QList<w_rpt_report_frame_btn_pageindex::total_desc_value_Data> ();
    //
    return d_list[index].total_list;
}

void w_rpt_report_frame_btn_pageindex::insert_map_widget_visiable(int index, QWidget *w, bool isvisiabled)
{
    d_list[index].map_widget_visiable.insert(w, isvisiabled);
}

void w_rpt_report_frame_btn_pageindex::update_map_widget_visiable(int index)
{
    //
    QMapIterator<QWidget */*widget*/, bool /*isvisiabled*/> mapitr(d_list[index].map_widget_visiable);
    while(mapitr.hasNext()) {
        mapitr.next();
        mapitr.key()->setVisible(mapitr.value());
    }
}

void w_rpt_report_frame_btn_pageindex::init(int index, int rowcount, int onepagestep, const QString &sql)
{
    int pagecount = rowcount;
    if(onepagestep > 0) {
        if(pagecount%onepagestep > 0) {
            pagecount += onepagestep;
        }
        pagecount = pagecount/onepagestep;
    }
    d_list[index].curstep = 0;
    d_list[index].pagecount = pagecount;
    d_list[index].onepagestep = onepagestep;
    d_list[index].sql = sql;
    d_list[index].rowcount = rowcount;
}

void w_rpt_report_frame_btn_pageindex::init(int index)
{
    d_list[index].init();
    setText(get_desc(index));
}

void w_rpt_report_frame_btn_pageindex::insert_d(int index)
{
    d_list.insert(index, step_count_Data());
}

bool w_rpt_report_frame_btn_pageindex::can_page_move(int index, int step)
{
    if(step == -1) step = d_list[index].curstep;
    int page = step/d_list[index].onepagestep+1;
    //
    if(!isValid(index)) return false;
    if(page < 1) return false;
    if(page > d_list[index].pagecount) return false;
    return true;
}

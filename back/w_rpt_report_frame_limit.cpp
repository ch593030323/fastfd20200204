#include "w_rpt_report_frame_limit.h"
#include "lds_query_hddpos.h"
#include <QtDebug>

w_rpt_report_frame_limit::w_rpt_report_frame_limit()
{
    init();
}

void w_rpt_report_frame_limit::insertFieldName(const QString &orgName, const QString &refName)
{
    select_sql_pair_list.append( QPair<QString, QString> (orgName,  refName));
}

QString w_rpt_report_frame_limit::refName(const QString &orgName)
{
    for(int k = 0; k < select_sql_pair_list.count(); k++) {
        if(select_sql_pair_list[k].first == orgName)
            return select_sql_pair_list[k].second;
    }
    return "";
}

QString w_rpt_report_frame_limit::orgName(const QString &refName)
{
    for(int k = 0; k < select_sql_pair_list.count(); k++) {
        if(select_sql_pair_list[k].second == refName)
            return select_sql_pair_list[k].first;
    }
    return "";
}

void w_rpt_report_frame_limit::init()
{
    _onepagecount = -1;
    _pagecount = -1;
    _page = -1;
    select_sql_pair_list.clear();
    select_sql = "";
}

double w_rpt_report_frame_limit::field_toal(const QString &refName)
{
    lds_query_hddpos  query;
    return lds_query_hddpos::selectValue(query, QString(" select sum(%1)  ").arg(orgName(refName)) + from_sql).toDouble();
}

int w_rpt_report_frame_limit::curpage()
{
    return _page;
}

int w_rpt_report_frame_limit::pagecount()
{
    if(_pagecount == -1) {
        lds_query_hddpos  query;
        _pagecount = lds_query_hddpos::selectValue(query, " select count(0)  " + from_sql).toInt();
    }
    return _pagecount;
}

void w_rpt_report_frame_limit::previous()
{
    _page --;
    if(_page < 0)
        _page = 0;
}

void w_rpt_report_frame_limit::next()
{
    _page ++;
    if(_page > pagecount() -1 )
        _page = pagecount() -1 ;
}

QString w_rpt_report_frame_limit::execSql()
{
    QString sql = "select ";
    for(int k = 0; k < select_sql_pair_list.count(); k++) {
        sql += select_sql_pair_list[k].first;
        if(select_sql_pair_list[k].second.length() > 0) {
            sql += QString(" as `%1`, ").arg(select_sql_pair_list[k].second);
        } else {
            sql += ",";
        }
    }
    sql.chop(1);
     return sql + " " + from_sql + QString(" limit %1, %2").arg(_page * _onepagecount).arg(_onepagecount);
}

void w_rpt_report_frame_limit::set_onepagecount(int onepagecount)
{
    _onepagecount = onepagecount;
}

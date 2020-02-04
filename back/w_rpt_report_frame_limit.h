#ifndef W_RPT_REPORT_FRAME_LIMIT_H
#define W_RPT_REPORT_FRAME_LIMIT_H

#include <QObject>
#include <QStringList>
#include<QPair>

class w_rpt_report_frame_limit
{
public:
    explicit w_rpt_report_frame_limit();

    void insertFieldName(const QString & orgName , const QString &refName = "");
    QString from_sql_arg;
    QString from_sql;

    void init();
    double field_toal(const QString &refName);
    int curpage();
    int pagecount();

    void previous();
    void next();
    QString execSql();

    void set_onepagecount(int onepagecount);
private:
    QString refName(const QString & orgName);
    QString orgName(const QString & refName);

    int _onepagecount;
    int _pagecount;
    int _page;
    QList<QPair<QString, QString> > select_sql_pair_list;
    QString  select_sql;
};

#endif // W_RPT_REPORT_FRAME_LIMIT_H

#ifndef W_RPT_REPORT_H
#define W_RPT_REPORT_H
#include <QStringList>
#include <QTableView>
#include "lds_query_hddpos.h"
#include <QHeaderView>
#include <QPushButton>
#include <QMenu>
#include <QStandardItemModel>
#include "backheader.h"
#include "lds_messagebox.h"
#include <QSqlQueryModel>
class QDateTimeEdit;
//datetimeModel
class datetimeModel:public QSqlQueryModel{
    Q_OBJECT
public:
    struct TABTYPE{//tabwidget离的格式，用于记录各种key的headers， sql， filters
        TABTYPE() {}
        TABTYPE(const QStringList &_headers,const QString &_sql,const QStringList &_filters=QStringList(), const QSet<int> &_except_c=QSet<int>()):
            headers(_headers),
            sql(_sql),
            filters(_filters),
            except_c(_except_c) {}

        QString title;
        QStringList headers;
        QString sql;
        QStringList filters;
        QSet<int> except_c;
    };
    TABTYPE dta;
public:
    explicit datetimeModel(QObject *parent = 0);
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;
};
//datetimeModel
class datetimestandModel:public QStandardItemModel{
    Q_OBJECT
public:
public:
    explicit datetimestandModel(QObject *parent = 0);
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
};
class w_rpt_report: public QDialog {
    Q_OBJECT
public:
    explicit w_rpt_report(QWidget *parent = 0);
    static int frame_left_fixed_width;
    typedef datetimeModel::TABTYPE TABTYPE;


    //返回结果集 【0】【1】；保证sql语句的执行列数的大于等于2
    //querymodelsetfilters("select * from cey_bt_table")
    static QStringList querymodelsetfilters(const QString &sql);
    //排序方法之适用与 taname0 - tabname1 之类的递增的头
    //" select "
    //" 'tabname0'=typeno,"
    //" 'tabname1'=(select vch_paymodename from cey_bt_paymode where cey_bt_paymode.ch_paymodeno=t.ch_paymodeno),"
    //。。。
    //" )t ORDER BY tabname%4"
    // .arg(w_rpt_report::querymodelordersort(ui->tableView->tableView))
    static QString querymodelordersort(QTableView *table);//排序方法之适用与 taname0 - tabname1 之类的递增的头
    //根据mysql  实现获取两段时间间隔的分钟总和
    static qlonglong sqltime_mindiff(const QDateTime &dt_begin, const QDateTime &da_end);

    static void reportBeginDtSet(QDateTimeEdit *dtedit);
};

#endif // W_RPT_REPORT_H

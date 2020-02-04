#include "w_rpt_report.h"
#include <QDateTimeEdit>
#include <QtDebug>

int w_rpt_report::frame_left_fixed_width=199;

datetimeModel::datetimeModel(QObject *parent):QSqlQueryModel(parent) {

}

QVariant datetimeModel::data(const QModelIndex &item, int role) const
{
    if(role==Qt::DisplayRole) {
        if(QSqlQueryModel::data(item, role).type()==QVariant::DateTime) {
            return QSqlQueryModel::data(item, role).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        }
        if(this->headerData(item.column(), Qt::Horizontal,  Qt::DisplayRole).toString().endsWith(QObject::tr("率"))
                ||this->headerData(item.column(), Qt::Horizontal,  Qt::DisplayRole).toString().endsWith(QObject::tr("百分比"))
                ||this->headerData(item.column(), Qt::Horizontal,  Qt::DisplayRole).toString().endsWith(QObject::tr("比例"))
                ) {
            return   QString::number(QSqlQueryModel::data(item, role).toFloat()*100) +"%";
        }
    }
    return QSqlQueryModel::data(item, role);
}

Qt::ItemFlags datetimeModel::flags(const QModelIndex &index) const
{
    if(index.data().isValid()) {
        return QSqlQueryModel::flags(index);
    }
    return Qt::NoItemFlags;
}

datetimestandModel::datetimestandModel(QObject *parent):QStandardItemModel(parent)
{

}

QVariant datetimestandModel::data(const QModelIndex &item, int role) const
{
    QVariant d = QStandardItemModel::data(item, role);
    if(role==Qt::DisplayRole) {
        if(d.type()==QVariant::DateTime) {
            return d.toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        }
        if(this->headerData(item.column(), Qt::Horizontal,  Qt::DisplayRole).toString().endsWith(QObject::tr("率"))
                ||this->headerData(item.column(), Qt::Horizontal,  Qt::DisplayRole).toString().endsWith(QObject::tr("百分比"))
                ||this->headerData(item.column(), Qt::Horizontal,  Qt::DisplayRole).toString().endsWith(QObject::tr("比例"))
                ) {

            return   QString().sprintf("%.02f", d.toFloat()*100) +"%";
        }
    }
    return d;
}


w_rpt_report::w_rpt_report(QWidget *parent)
{

}

QStringList w_rpt_report::querymodelsetfilters(const QString &sql)
{
    QStringList rets;
    rets << QObject::tr("全部");
    lds_query_hddpos  query;
    query.execSelect(sql);
    while(query.next()) {
        rets << QString("%1(%2)")
                .arg(query.recordValue(1).toString())
                .arg(query.recordValue(0).toString());
    }
    return rets;
}

QString w_rpt_report::querymodelordersort(QTableView *table)
{
    QString ret="0 ASC";
    if(table->horizontalHeader()) {
        ret=QString("%1 %2").arg(qMax(0, table->horizontalHeader()->sortIndicatorSection()))
                .arg(table->horizontalHeader()->sortIndicatorOrder()==Qt::AscendingOrder? "ASC" : "DESC");
    }
    return ret;
}

qlonglong w_rpt_report::sqltime_mindiff(const QDateTime &dt_begin, const QDateTime &da_end)
{
    lds_query_hddpos  query;
    query.execSelect(QString("select ABS(round(TIME_TO_SEC(TIMEDIFF('%1',  '%2'))/60)) as minutediff ")
                     .arg(dt_begin.toString("yyyy-MM-dd hh:mm:ss"))
                     .arg(da_end.toString("yyyy-MM-dd hh:mm:ss"))
                     );
    query.next();
    return query.recordValue("minutediff").toLongLong();
}

void w_rpt_report::reportBeginDtSet(QDateTimeEdit *dtedit)
{
    QString int_id_max = lds_query_hddpos::selectValue(" select max(int_id) from cey_u_handover_master ").toString();
    QDateTime  dt_end = lds_query_hddpos::selectValue(QString(" select dt_end from cey_u_handover_master where int_id = '%1' ").arg(int_id_max)).toDateTime();
    if(dt_end.isValid()) {
        dtedit->setDateTime(dt_end);
    } else {
        dtedit->setDateTime(QDateTime(lds_query_hddpos::selectNow().date(), QTime(0, 0, 0)));
    }
}

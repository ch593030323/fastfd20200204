#ifndef W_RPT_REPORT_PROMOTIONREPORT_H
#define W_RPT_REPORT_PROMOTIONREPORT_H

#include <QDialog>
#include <QMap>
#include <QModelIndex>
#include "w_rpt_report.h"

class QSqlQueryModel;
class Ui_w_rpt_report_promotionreport;
class QTableView;
class w_rpt_report_promotionreport : public QDialog
{
    Q_OBJECT
public:
    explicit w_rpt_report_promotionreport(QWidget *parent = 0);
    ~w_rpt_report_promotionreport();
    void tab1sqlinit();
    void tab2sqlinit();
    void tab3sqlinit();
    void tab4sqlinit();
    void tab5sqlinit();

private slots:
    void toselect();
    void toexit();
    void tocom12(const QString &/*text*/);

private:
    Ui_w_rpt_report_promotionreport *ui;
    QMap<QString , w_rpt_report::TABTYPE> tab1info;
    QSqlQueryModel *querymodel;

    QMap<QString , w_rpt_report::TABTYPE> tab2info;
    QSqlQueryModel *querymodel_2;

    QMap<QString , w_rpt_report::TABTYPE> tab3info;
    QSqlQueryModel *querymodel_3;

    QMap<QString , w_rpt_report::TABTYPE> tab4info;
    QSqlQueryModel *querymodel_4;

    QMap<QString , w_rpt_report::TABTYPE> tab5info;
    QSqlQueryModel *querymodel_5;
};

#endif // W_RPT_REPORT_PROMOTIONREPORT_H

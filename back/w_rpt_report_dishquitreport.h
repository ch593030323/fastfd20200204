#ifndef W_RPT_REPORT_DISHQUITREPORT_H
#define W_RPT_REPORT_DISHQUITREPORT_H

#include <QDialog>
#include "w_rpt_report.h"
class Ui_w_rpt_report_cashreport_Dialog;
class w_rpt_report_dishquitreport : public QDialog
{
    Q_OBJECT
public:
    explicit w_rpt_report_dishquitreport(QWidget *parent = 0);
    void tab1sqlinit();

private slots:
    void toselect();
    void toexit();
    void tocom12(const QString &text);
private:
    Ui_w_rpt_report_cashreport_Dialog *ui;
    QMap<QString , w_rpt_report::TABTYPE> tab1info;
    QSqlQueryModel *querymodel;
    
};

#endif // W_RPT_REPORT_DISHQUITREPORT_H

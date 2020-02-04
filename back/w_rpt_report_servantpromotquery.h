#ifndef W_RPT_REPORT_SERVANTPROMOTQUERY_H
#define W_RPT_REPORT_SERVANTPROMOTQUERY_H

#include <QDialog>
#include <QMap>
#include <QDateTime>
#include "w_rpt_report.h"
#include "w_rpt_report_frame.h"

class Ui_w_rpt_report_servantpromotquery_Dialog;
class w_rpt_report_servantpromotquery : public QDialog
{
    Q_OBJECT
public:
    explicit w_rpt_report_servantpromotquery(QWidget *parent = 0);
    ~w_rpt_report_servantpromotquery();
private slots:
    void toselect();
    void toexit();
    void toprint();
    void toexport();
private:
    Ui_w_rpt_report_servantpromotquery_Dialog *ui;
    report_querymodel *querymodel;
};

#endif // W_RPT_REPORT_SERVANTPROMOTQUERY_H

#ifndef W_RPT_REPORT_TABLEBUSINESSREPORT_H
#define W_RPT_REPORT_TABLEBUSINESSREPORT_H

#include <QDialog>
#include "w_rpt_report.h"
#include "lds_model_sqlstandardmodel.h"

class Ui_w_rpt_report_tablebusinessreport_Dialog;
class w_rpt_report_tablebusinessreport : public QDialog
{
    Q_OBJECT
public:
    explicit w_rpt_report_tablebusinessreport(QWidget *parent = 0);
    
    void tab1sqlinit();

private slots:
    void toselect();
    void toexit();
    void tocom12(int index);
private:
    Ui_w_rpt_report_tablebusinessreport_Dialog *ui;
    QMap<QString , w_rpt_report::TABTYPE> tab1info;
    lds_model_sqlstandardmodel *querymodel;
    
};

#endif // W_RPT_REPORT_TABLEBUSINESSREPORT_H

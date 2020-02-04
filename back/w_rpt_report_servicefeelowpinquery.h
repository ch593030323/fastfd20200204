#ifndef W_RPT_REPORT_SERVICEFEELOWPINQUERY_H
#define W_RPT_REPORT_SERVICEFEELOWPINQUERY_H

#include <QDialog>
#include <QMap>
#include <QModelIndex>
#include "w_rpt_report.h"
#include "w_rpt_report_frame.h"

class QSqlQueryModel;
class Ui_w_rpt_report_servicefeelowpinquery_dialog;
class QTableView;
class w_rpt_report_servicefeelowpinquery : public QDialog
{
    Q_OBJECT
public:
    explicit w_rpt_report_servicefeelowpinquery(QWidget *parent = 0);
    ~w_rpt_report_servicefeelowpinquery();

public slots:
    void toselect();
    void toexit();
    void toprint();
private:
    Ui_w_rpt_report_servicefeelowpinquery_dialog *ui;
    report_querymodel *querymodel;
};

#endif // W_RPT_REPORT_SERVICEFEELOWPINQUERY_H

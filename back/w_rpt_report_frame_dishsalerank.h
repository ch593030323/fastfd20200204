#ifndef W_RPT_REPORT_FRAME_DISHSALERANK_H
#define W_RPT_REPORT_FRAME_DISHSALERANK_H

#include <QWidget>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_dishsalerank;
}

class w_rpt_report_frame_dishsalerank : public w_rpt_report_frame
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_dishsalerank(QWidget *parent = 0);
     ~w_rpt_report_frame_dishsalerank();
private slots:
    void toselect();


    virtual void toexport();
    virtual void toprint();
private:
    Ui::w_rpt_report_frame_dishsalerank *ui_help;
    QMap<QString, QVariant> filter_map;
    QString tablename;
    QString sql_keyname;
    QString sql_valuename;
};

#endif // W_RPT_REPORT_FRAME_DISHSALERANK_H

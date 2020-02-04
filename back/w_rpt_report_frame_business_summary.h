#ifndef W_RPT_REPORT_FRAME_BUSINESS_SUMMARY_H
#define W_RPT_REPORT_FRAME_BUSINESS_SUMMARY_H

#include <QDialog>
#include "w_rpt_report_frame.h"

namespace Ui {
class w_rpt_report_frame_business_summary;
}

class w_rpt_report_frame_business_summary : public QDialog
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_business_summary(QWidget *parent = 0);
    ~w_rpt_report_frame_business_summary();

    void showLoginerContent();
private slots:
    void right_check_and_init();

    void to_query();
    void to_export();
    void to_print();
    void to_exit();
private:
    Ui::w_rpt_report_frame_business_summary *ui;

    report_querymodel *querymodel;
    report_standmodel *standmodel2;
    lds_bool_default_false is_show_login_data;
};

#endif // W_RPT_REPORT_FRAME_BUSINESS_SUMMARY_H

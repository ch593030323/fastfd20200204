#ifndef W_RPT_REPORT_UI_H
#define W_RPT_REPORT_UI_H

#include <QDialog>

namespace Ui {
class w_rpt_report_ui;
}

class w_rpt_report_ui : public QDialog
{
    Q_OBJECT

public:
    explicit w_rpt_report_ui(QWidget *parent = 0);
    ~w_rpt_report_ui();

private:
    Ui::w_rpt_report_ui *ui;
};

#endif // W_RPT_REPORT_UI_H

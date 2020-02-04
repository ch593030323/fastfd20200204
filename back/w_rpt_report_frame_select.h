#ifndef W_RPT_REPORT_FRAME_SELECT_H
#define W_RPT_REPORT_FRAME_SELECT_H

#include <QDialog>
#include <QSqlQueryModel>


namespace Ui {
class w_rpt_report_frame_select;
}

class w_rpt_report_frame_select : public QDialog
{
    Q_OBJECT

public:
    explicit w_rpt_report_frame_select(const QString &sql, QWidget *parent = 0);
    ~w_rpt_report_frame_select();
    QString retv;
private slots:
    void took();
    void tocancel();
    void tosetq();
private:
    Ui::w_rpt_report_frame_select *ui;
    QSqlQueryModel *querymodel;
    QString _sql;
};

#endif // W_RPT_REPORT_FRAME_SELECT_H

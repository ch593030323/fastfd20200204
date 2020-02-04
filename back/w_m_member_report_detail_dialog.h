#ifndef W_M_MEMBER_REPORT_DETAIL_DIALOG_H
#define W_M_MEMBER_REPORT_DETAIL_DIALOG_H

#include <QDialog>
#include "w_rpt_report_frame.h"

class Ui_w_m_member_report_detail_Dialog;
class w_m_member_report_detail_Dialog : public QDialog
{
    Q_OBJECT
public:
    explicit w_m_member_report_detail_Dialog(const QString &vch_memberno, const QDateTime &dtFrom, const QDateTime &dtFo, QWidget *parent = 0);

    ~w_m_member_report_detail_Dialog();
signals:

public slots:

    void torefresh();

private:
    Ui_w_m_member_report_detail_Dialog *ui;
    report_querymodel *querymodel;
    QString vch_memberno;
    QDateTime dtFrom;
    QDateTime dtFo;
};

#endif // W_M_MEMBER_REPORT_DETAIL_DIALOG_H

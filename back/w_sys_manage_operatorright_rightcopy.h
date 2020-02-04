#ifndef W_SYS_MANAGE_OPERATORRIGHT_RIGHTCOPY_H
#define W_SYS_MANAGE_OPERATORRIGHT_RIGHTCOPY_H

#include "w_rpt_report_frame.h"
class Ui_w_sys_manage_operatorright_rightcopy_Dialog;
class w_sys_manage_operatorright_rightcopy : public QDialog
{
    Q_OBJECT
public:
    explicit w_sys_manage_operatorright_rightcopy(const QString &vch_operID, const QString &vch_operator, QWidget *parent = 0);
    QString target;
private slots:
    void tocopy();
    void toexit();
private:
    Ui_w_sys_manage_operatorright_rightcopy_Dialog *ui;
    report_querymodel *querymodel;
    QString vch_operID;
};

#endif // W_SYS_MANAGE_OPERATORRIGHT_RIGHTCOPY_H

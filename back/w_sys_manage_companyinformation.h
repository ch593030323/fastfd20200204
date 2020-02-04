#ifndef W_SYS_MANAGE_COMPANYINFORMATION_H
#define W_SYS_MANAGE_COMPANYINFORMATION_H

//公司信息
#include <QDialog>
class Ui_w_sys_manage_companyinformation_Dialog;
class lds_model_sqltablemodel;
class QDataWidgetMapper;
class w_sys_manage_companyinformation : public QDialog
{
    Q_OBJECT
public:
    explicit w_sys_manage_companyinformation(QWidget *parent = 0);
    
private slots:
    bool tosave();
    void toexit();
private:
    Ui_w_sys_manage_companyinformation_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
    QDataWidgetMapper *mapper;

};

#endif // W_SYS_MANAGE_COMPANYINFORMATION_H

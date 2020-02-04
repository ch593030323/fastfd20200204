#ifndef WAIMAI_PAY_H
#define WAIMAI_PAY_H

#include <QDialog>
#include <QStandardItemModel>
#include "public_mainprinter.h"
#include "w_scr_dish.h"

namespace Ui {
class waimai_pay;
}

class waimai_pay : public QDialog
{
    Q_OBJECT
    
public:
    explicit waimai_pay(const QString &xiaofei,const QString &zhekou,const QString &zengsong,const QString &moling,const QString &shuishou,const QString &billno,const QString &waimaihao,QWidget *parent);
    ~waimai_pay();
    void init_control();
    void init_data();
    bool btn_t_f;
private slots:
    void on_pushButton_21_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_13_clicked();//付款

    void on_lineEdit_9_textChanged(const QString &arg1);

    void print_lost(const QString & str);

    void pay_print();

    void on_pushButton_11_clicked(); //<<

    void on_pushButton_7_clicked();//100

    void on_pushButton_4_clicked();//50

private:
    Ui::waimai_pay *ui;
    QString _xiaofei;
    QString _zhekou;
    QString _zengsong;
    QString _shuishou;
    QString _moling;
    QString _billno;
    QString _waimaihao;
    QString shifoujiazhang;
    QStandardItemModel *table_model;
    float kafujine;
    QString fukuanpingzheng;
//    public_mainprinter *printer;
};

#endif // WAIMAI_PAY_H

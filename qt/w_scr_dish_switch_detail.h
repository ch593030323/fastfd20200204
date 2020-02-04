#ifndef W_SCR_DISH_SWITCH_DETAIL_H
#define W_SCR_DISH_SWITCH_DETAIL_H

#include <QDialog>
#include "commonheader.h"
#include <QDateTime>

namespace Ui {
class w_scr_dish_switch_detail;
}

class w_scr_dish_switch_detail : public QDialog
{
    Q_OBJECT
public:
    struct pay_detail_DATA{
        QString payno;
        QString payname;
        double payamount;
    };
public:
    static void print_by_int_id(int int_id);
public:
    explicit w_scr_dish_switch_detail(QWidget *parent);
    ~w_scr_dish_switch_detail();
    QString new_operid;
    QString new_operpwd;//for relogin
    void initData();
    bool commitData(QString &errstring, int &int_id, bool isForPrevious = true);
    void refresh_tablewidget(int int_id);
private:
    void updateKeyFix4hanzi(QList< pay_detail_DATA > &ldec_paymode_otherlist);
    void updateKeyFix4hanzi(QList< QPair<QString, double> > &ldec_paymode_otherlist);
    QString updateKeyFix4hanzi(const QString &name);
    static QVariant get_cey_sys_operator(const QString& operid, const QString &valuename);
    static void set_cey_sys_operator(const QString& operid, const QString &valuename, const QVariant &var);
    bool handover_oper_check();
    int get_max_temp_int_id_insert_cey_u_handover_master(bool *ok = 0);
    QString handover_tmp_vch_memo();
private slots:
    void tofinish();
    void toswitch();
    void toexit();
    void tosaveupload(bool f);
    void to_set_print_count();
    void to_set_cash_register();
    void tosaveprintdetail(bool f);
    void tosave_cash_fee_keep_to_accept(bool f);
    void toprint();
    void tofetch();
    void tofetchrecord();
    void update_shift_memo();
    void update_temp_handover();//这里的temp指的都是取款和一些临时商品的数据

    void toInitCommitData();
    void toSetMemo();

    void toExport();
    void toEmailEdit();
    void toEmail();

    void right_check_and_init();
private:
    QFont resizeFont(QFont font, int pointSize);

    QString exportCSV();

    QString ifEmpty(const QString &str, const QString &def_value);
private:
    Ui::w_scr_dish_switch_detail *ui;
    blockIns printins;
    QStringList switch_sqllist;
    QDateTime date0;
    QDateTime date1;
    int int_id;

};

#endif // W_SCR_DISH_SWITCH_DETAIL_H

#ifndef W_SCR_DISH_MAIN_H
#define W_SCR_DISH_MAIN_H

#include <QDialog>
#include "fexpandmain_model_sqltablemodel_data.h"
#include "public_dialog.h"
#include "lds_roundeddialog_rect.h"
#include <QTableView>
#include <QPointer>
#include "w_scr_dish_main_pay_info.h"
#include <QPointer>
#include "lds_tableview_virtual.h"
#include "ftableview_standmodel_sql.h"
#include "ftableview_standmodel_sql_none.h"
#include "lds_label_doublespinbox.h"
#include "label_click.h"
#include "ftableview_delegate.h"
#include "w_scr_dish_pay_widget.h"

class ftableview_standmodel_virtual;
class QFrame;
class lds_roundeddialog_rect_align;
class w_scr_dish_pay;
class ftableview;
class fexpandmain_tableview;
class QLabel;
class fexpandmain_dish_btn;
class lds_lineedit;
class public_pushbutton_pay;
class public_pushbuton_rt_check;
class lds_roundeddialog_title_frame;

class w_scr_dish_main : public lds_roundeddialog_rect0
{
    Q_OBJECT
public:
    enum windowType{
        windowTypeKuaiCan,
        windowTypeZhongCan,
    };

    //paybill 效果 结算后,本单u_table ,u_orderdish,checkout_master,赋予ch_payno
    //                    分单结算, 若是全部付款,则分单付款的同时,留下的空的桌台也要付款
    //                    快餐,暂时不管


    static void initLogo(QLabel *label_logo ,QLabel *label_text);

    QString cur_ch_billno();

protected:
    explicit w_scr_dish_main(const QString &ch_tableno,  const QString &ch_billno, windowType window_type, QWidget *parent = 0);
protected:
    void initLogo();
    void torefresh_dish();
    void torefresh_dish_sql();
    void torefresh_control(const QString &ch_billno);
    void torefresh_control_Q000();

    int autoDiscount(QString &discount_desc, int &int_discount_value);

    bool tablemodelSelect(const QString &ch_billno, QWidget *m);
    bool tablemodelSelect(const QString &ch_billno);

    void tablemodel_setData_updateRow(int row, const QString &field_name, const QVariant &value);
protected slots:
    void refreshChBillno();
    void payBillFinished();

    void toAllbillcancel();
    void tochange();//换菜
    void toquitdish();
    void toquitdish2();
    void toExitbill();

    void todetail();
    void topresent();
    void tomhangpay();
    void toTakebill();
    void toTakeweight();

    void topaybill();
    void topaybillreturn();
    void topaybillmember();

    void topaybillSelectPayMode();
    void toUpdatePayBillButton();
    void toTuichu();
    void toMRead();
    void towaimai();
    void towaimaipay();

    void tosysyingyedanju();
    void toLanguageSwitch();
    void toDishNameSwitch(const QString &key);
    void toLanguageSet();
    void toDishnameSet();
    void tosysyingyeqingkuang();
    void tosysjiaoban();
    void tosyschongzhi();
    void tosysqukuan();
    void tosysxiugaimima();
    void tosysswitchback();
    void tosyskaiqianxiang();
    void tochangyongcai();
    void tosyssuoping();
    void tosysguqing();
    void toRestaurant();
    void toScanbar(const QString &text);
    void tosub();
    void toadd();
    void tochangenum();
    void todishup();
    void todishdown();

    void showBianMa();

    void toLongPressUpdateDishImage(const QModelIndex &index);
    void toLongPressUpdateDishImageAdd();
    void toLongPressTypeSet();

    void to_suit_fold(const QModelIndex &index);

    //商品的修改删除新增
    void todishclick(const QString &ch_dishno);
    void todishclickgroup(const QStringList &ch_dishno_list);
    bool to_dish_del(const int row, QString *errstring = 0);
    bool to_dish_del(const QModelIndex &index);

    bool add_dish_by_scanbar(const QString &pay_barcode);

    ///中餐

    void toprint();//落单

    void tocuicaipiece();//单个催起
    void tocuicaitable();//整桌
    void tocuicaitype();//类别

    void toguaqi();

    void tojiaodanpiece();
    void tojiaodantable();
    void tojiaodantype();

    void toDishTmp();
    void toPaySomeDish();
    void toWaiDai();
    void toTelComing(const QString &telNo = "");

    void to_moblie_payselect();
    void to_moblie_paybill();
    void to_cash_paybill();

    void toQuickNum();
    void toshowPayInfo();


    void toWine();

    void toqr_code_order_over_view();

    void toDiscountBill();

    void toTaxExec();
protected:
    bool paybill(w_scr_dish_pay_widget::operatMode operat_mode);
    void removeAllInsertDish();
    bool numcook_check_commit(const fexpandmain_model_sqltablemodel * numcook_model, int row, QString &errstring);
    void numcook_commit(const fexpandmain_model_sqltablemodel * numcook_model, int row);
    void numcook_commit(const fexpandmain_model_sqltablemodel * numcook_model, int row, bool commited);

    ///~中餐
    bool get_num_price_when_dishclick(const QString &ch_dishno, double &num_price, int &int_discount, bool &time_promotion);
    bool can_promotion();
    void dish_add(int f, bool autoScale);//-1 1
    //商品的修改删除新增
protected slots:
    void toCLearPayInfo();
    void to_payfor_info(const QString &ch_payno, int paytype);
    void to_payfor_info_ch_billno(const QString &ch_billno, int paytype);
    void toSwitchSkin();
    void toScaleDishPic();
protected:
    void update_type_subtype_view();

    ///中餐
    bool printdish_tran_saction(QString &errstring);//落单
    bool try_commit_data_update_num_cost(QString &errstring, QStringList &insert_int_flowid_list);
    bool update_num_cost(QString &errstring);
    ///~中餐

    void keyPressEvent(QKeyEvent *e);
protected:
    QString ch_tableno;
    QString ch_billno;

    ///虚函数
protected slots:
    virtual void menuhide();
    virtual void updateTakebillState();
    virtual void updateTakebillDesc();
    virtual void retranslateView();
    virtual void loadData();
    virtual void orderdishRowChanged(const QModelIndex &, const QModelIndex &) ;
    virtual void updatememberchecked();
    virtual void updateTotal(double total);
    virtual bool getWeightValue(int row, double &w);
    virtual bool dishAppend(const QString &ch_dishno, const vch_print_memoData &vch_print_memo, double num_price, int int_discount, const bool time_promotion);
    ///~虚函数


    ///外部的控件
protected:
    void loadControl(ftableview *tableView_dish,
                     ftableview *tableView_type,
                     ftableview *tableView_subtype,
                     fexpandmain_tableview *tableView,

                     QLabel *label_logo,
                     QLabel *label_text,
                     public_pushbutton_pay *pushButton_pay,

                     lds_label_doublespinbox *widget_total,
                     lds_label_doublespinbox *widget_change,

                     QLabel *label_total,
                     lds_roundeddialog_title_frame *frame_deskinfo,

                     fexpandmain_dish_btn *frame_dish_bottom2
                     );
protected:
    QPointer<ftableview> tableView_dish;
    QPointer<ftableview> tableView_type;
    QPointer<ftableview> tableView_subtype;

    QPointer<QLabel> label_logo;
    QPointer<QLabel> label_text;
    QPointer<public_pushbutton_pay> pushButton_pay;

    QPointer<fexpandmain_tableview> tableView;
    QPointer<w_scr_dish_main_pay_info> pay_info_widget;

    windowType window_type;

    fexpandmain_model_sqltablemodel_data *tablemodel;
    ftableview_standmodel_sql *model_type;
    ftableview_standmodel_sql *model_subtype;
    ftableview_standmodel_sql *model_dish;

    QPointer<lds_label_doublespinbox> widget_total;
    QPointer<lds_label_doublespinbox> widget_change;
    QPointer<QLabel> label_total;
    QPointer<lds_roundeddialog_title_frame> frame_deskinfo;

    QPointer<fexpandmain_dish_btn> frame_dish_bottom2;
    ///～外部的控件
private:
    QLineEdit *barcode_scan;
    QLineEdit *get_barcode_scan();
};

class w_scr_dish_main_auto_exit_multiSelection{
public:
    w_scr_dish_main_auto_exit_multiSelection(QTableView *tableview) {
        row = tableview->currentIndex().row();
        this->tableview = tableview;
        this->tableview->setSelectionMode(QTableView::MultiSelection);
    }
    ~w_scr_dish_main_auto_exit_multiSelection() {
        this->tableview->setSelectionMode(QTableView::ExtendedSelection);
        this->tableview->clearSelection();
        this->tableview->selectRow(row);
    }
private:
    QTableView *tableview;
    int row;
};

#endif // W_SCR_DISH_MAIN_H

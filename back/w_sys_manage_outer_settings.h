#ifndef W_SYS_MANAGE_OUTER_SETTINGS_H
#define W_SYS_MANAGE_OUTER_SETTINGS_H

#include <QDialog>
#include <QModelIndex>
#include <QSet>
#include "public_dialog.h"
#include <QTimer>
#include "w_bt_dish_kitchenprinter_printerset_port.h"
#include "electroniccountprice_resolve.h"

class lds_model_sqltablemodel;
class QStandardItemModel;
class ftableview_standmodel_sql_none;
namespace Ui {
class w_sys_manage_outer_settings;
}

class w_sys_manage_outer_settings : public public_Dialog
{
    Q_OBJECT

public:
    static bool supportDesktopSize();
    static void setSystemSize(QWidget *parent);
public:
    explicit w_sys_manage_outer_settings(QWidget *parent = 0, int fisrtIndex = 0);
    ~w_sys_manage_outer_settings();
    virtual void retranslateView();
private slots:
    void toupdateSection(const QModelIndex &index);
    virtual void took();
    void tocancel();
    void tobillset();
    void tobillset_txt();
    void todrawerdef();
    void todrawertest();
    void toprintermanager();
    void tokexiantest();
    void toshow_kexianruninfo(int index);
    void tofupingreset();
    void tofuping_adv();
    void tofuping_dish();
    void tojijia_test();
    void tojijia_CAL();
    void tojijia_type_check(int checked);

    void towxsavestart(bool f);
    void towxdel();
    void towxdel2();
    void towxnew();
    void towxnew2();
    void towxinit();
    void towxstartjz();
    void towxclose();
    void towxsave();

    void tolabel_print();
    void tolabel_print_naicai_test();
    void tobar_scale();

    void toimport();

    void tomobile_connect_max_value();

    void toSaaSCheck(int index);
    void toSaaSCheck(bool check);

    void tocolorPicker();

    void to_read_tel_test(); //电话盒
    void to_read_tel_readdata();

    void toLabelDishCheckUn();
    void toLabelDishCheckAll();

    void tofuping_4_3cun(int index);
    void tofuping_screen_index(int index);

    void toEmailSet();

    void toshowkitchen_print();
    void toshowkitchen_view();

    void to_label_printer_win();
private:
    QTimer *read_tel_timer;
protected:
    bool saveData(int index, bool issave = false);
    void setEditText(QComboBox *com, const QString &editText);
    bool kitchen_plan_check(const QString &kt_kitchen_type);
private:
    QSet<int> index_set;

    QStandardItemModel *wx_model;
    QStandardItemModel *wx_model_2;
    QString wx_dir;
    void wx_init_jizhan();
    void wx_init_diancaiji();
    bool wxsavejizhan();
    bool wxsavediancaiji();

    enum modelType{
        model_normal_print = 0,
        model_kitchen_print,
        model_label_print,
        model_screen_print,
        model_price_scale,

        model_bar_scale,
        model_wireless_machine,
        model_tablet,
        model_email,
    };
    QMap<modelType, int> map_view_stack;
public:
    Ui::w_sys_manage_outer_settings *ui;
    ftableview_standmodel_sql_none * backlist_model;
    lds_model_sqltablemodel *tablemodel_dish_subtype;
    ElectronicCountPrice_resolve *countprice;
    w_bt_dish_kitchenprinter_printerset_port *frame_label_path_select_help;
    Printer_POS_SERIAL jijia_key_scpos_printer;
};

#endif // W_SYS_MANAGE_OUTER_SETTINGS_H

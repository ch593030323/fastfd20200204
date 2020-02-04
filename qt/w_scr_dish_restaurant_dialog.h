#ifndef W_SCR_DISH_RESTAURANT_DIALOG_H
#define W_SCR_DISH_RESTAURANT_DIALOG_H


#include <QDialog>
#include "public_logindialog.h"
#include "frontviewdialog_virtual.h"
#include "restauranttableview.h"
#include "public_dialog.h"
#include "lds_roundeddialog_rect.h"
#include "lds_model_sqlstandardmodel.h"
#include "w_m_member_telcoming_table.h"

class Ui_w_scr_dish_restaurant_dialog;
class ftableview_standmodel_sql_table;
class QTimeLine;
class lds_roundeddialog_rect_align;
class public_pushbuton_rt_check;
class table_management_graphicsview;
class lds_pushbutton_wrap;
class lds_roundeddialog_title;
class lds_toolbar_button;
class lds_widgetgoing;
class btnlanguageswicth;
class public_pushbuton_likerb_pur;
class ftableview_standmodel_sql;


class w_scr_dish_restaurant_dialog : public lds_roundeddialog_rect0
{
    Q_OBJECT
public:
    static bool cey_bt_table_bar_replace(const QString &ch_tableno, int int_div_id, const QString &ch_billno, QString *errstring = 0);
    static bool cey_bt_table_bar_replace(lds_query_hddpos  query, const QString &ch_tableno, int int_div_id, const QString &ch_billno);
    static bool cey_bt_table_bar_existed(const QString &ch_tableno, int int_div_id);
public:
    explicit w_scr_dish_restaurant_dialog(QWidget *parent = 0);
    ~w_scr_dish_restaurant_dialog();

    void retranslateView();
public:
    static bool tablemessageOrder(QWidget *parent,
                                  const QString &ch_tableno,
                                  const QString &sql ,
                                  bool is_div,
                                  const w_m_member_telComing_table::telComingData &member_from_tel,
                                  bool isRequestOrder
                                  );
public slots:
    void toFirstGoIn();
    void torefresh(int cur, int total);
    void torefresh_data();

protected slots:
    void tocanceldesk(const QString &ch_tableno);//消台
    void tolockstate();//锁台
    void toopencash();
    void toother();
    void toexit();
    void totabletransfer(const QString &ch_tableno);
    void toswitch();

    void tomrecharge();
    void tobusiness_status();
    void tolockscreen();
    void tocurtable_status();
    void tobusinessdata_query();
    void topwdchange();

    void totablemessageOrder(const QModelIndex &index, const QString &sql, bool is_div);
    void totablemessageOrder(const QString &ch_tableno, const QString &sql, bool is_div);

    void totablemessage(const QModelIndex &index);
    void totablemessage(const QString &ch_tableno);

    void totablemessageList(const QModelIndex &index);
    void totable_ch_areano(const QString &ch_areano);
    void toswitchback();//后台

    void to_waimaipay();

    void topreorder();//预订
    void toparameter();
    void tomhangpay();

    void todishcopy(const QString &ch_tableno);
    void totableCombine(const QString &ch_tableno);//并台

    void update_print_name_id_state(const QString &int_id, uchar state);
    void update_print_name_id_error(const QString &int_id);

    void todishtransfer(const QString &ch_tableno);
    void toshowprintstate();

    void tosysguqing();

    void toLanguageSwitch();
    void toLanguageSet();
    void toFastfdMode();

    void widget_going_run(int cur, int total);
    void toTelComing(const QString &telNo = "");

    void toLongPressTableTimeOut(const QModelIndex &index);
    void toLongPressTableChange(const QModelIndex &index);
    void toLongPressTableNew();
    void toLongPressTableAreaNew(const QModelIndex &index);

    void to_mode_list_dish_update(const QModelIndex &index);
    void to_mode_list_dish_click(const QModelIndex &index);

    void to_mode_map_edit_table(const QModelIndex &index);
    void to_mode_map_edit_table(const QString &ch_tableno);

protected:
    QString modeltypeSql();
    QString modeltypeFilter();
    void closeEvent(QCloseEvent *e);
protected slots:
    void toupdate_section_by_map();
    void to_restaurant_mode_list(bool execPress);
    void to_restaurant_mode_table(bool execPress);
    void to_restaurant_mode_map(bool execPress);
    void to_map_mode_edit_and_save(bool execPress);


    void to_mode_map_edit_and_save();
    void to_mode_table_deselect(const QString &ch_tableno);
    void toWine();

    void toqr_code_polling(QNetworkReply *reply);
    void toqr_code_order_over_view();
protected:
    void restaurant_mode_select();//餐桌和地图模式切换
    void map_mode_switch();//设计\和完成之间的切换
    void update_desk_info();
    void map_desk_show_code();
    void map_select_by_table(const QString &ch_tableno);
    void table_select_by_map();
    void map_editmode_update_table_select();
    bool is_map_editmode();
    bool is_list();
    bool table_opera_when_btn_is_down(const QString &ch_tableno);
    void qr_code_request(const QString &state);


    void keyPressEvent(QKeyEvent *e);
protected:
    Ui_w_scr_dish_restaurant_dialog *ui;

    lds_bool_default_false restaurant_desk_save_file_is_load;

    QString _curtypeno;
    ftableview_standmodel_sql_table *modeltable;
    lds_model_sqlstandardmodel *modeltable_list_dish;
    ftableview_standmodel_sql *modeltype;
    QString ordertable_msg;
    QDateTime ordertable_msg_dt;
    int ordertable_msg_loop;

    QStandardItemModel *standmodel_print_state;
    lds_bool_default_false restaurant_model_is_init;

    QSet<QString> set_print_name_id_error;
    QWidget pushButton_map_model_edit_and_save_popup;
    // qr_code_polliing
    int qr_code_polliing_count;//
    bool qr_code_polling_has_pop;
    //
    QNetworkAccessManager *manager;
};
#endif // W_SCR_DISH_RESTAURANT_DIALOG_H

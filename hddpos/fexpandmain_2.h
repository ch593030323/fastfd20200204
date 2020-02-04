#ifndef FEXPANDMAIN_2_H
#define FEXPANDMAIN_2_H
#include <QDialog>
#include <QItemDelegate>
#include <QTableView>
#include "lds_model_sqltablemodel.h"
#include <QMouseEvent>
#include "fexpandmain_tableview.h"
#include "ftableview_standmodel_sql.h"
#include <QStyledItemDelegate>
#include "lds_query_hddpos.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "n_func.h"
#include "ftableview.h"
#include "restauranttableview.h"
#include "public_short_cut_map.h"
#include "frontviewdialog_virtual.h"
#include "w_bt_dish_back.h"

class w_scr_dish_main_pay_info;
class lds_roundeddialog_rect_align;
class w_scr_dish_num_cook_Dialog;
class w_scr_dish_pay;

namespace Ui {
class fexpandmain_2;
}

class fexpandmain_2 : public frontViewDialog_virtual
{
    Q_OBJECT
public:
    enum ReadMemType {
        rejected = -1,
        cleared = 0,
        readed = 1,
    };

    static void showPayinfo(
            QWidget *tableview,
            w_scr_dish_main_pay_info *pay_info_widget,
            const QString &ch_payno,
            int pay_type,
            bool ispromotion,
            const QString &discount_desc
            );

    static QString get_sub_vch_print_memo(const QString &org, const QString &sub);

    //数估清不足，则返回false，否则true
    static bool guqing_check(QString &msg, const QString &ch_dishno, double off);
    //数估清不足，则返回false，否则true
    static void guqing_try_clear();
    ///数据库更新失败，则报错，否则返回true
    static bool guqing_update(const QString &ch_dishno, double off/*可以为负数*/);
    static bool guqing_try_check_P(lds_model_sqltablemodel *tablemodel,  QString &msg, int rowP, double off);
    static bool material_insert(lds_query_hddpos  &query, const QString &ch_billno, int int_id);

    static void updateDishImage(QWidget *parent, const QModelIndex &index, ftableview_standmodel_virtual *model_dish, const QString &ch_sub_typeno, const QString &ch_groupno = "");
    static void updateDishImage_add(QWidget *parent, ftableview_standmodel_virtual *model_dish, const QString &ch_sub_typeno, const QString &ch_groupno = "");

    static ReadMemType m_read_exec(QWidget *parent, fexpandmain_model_sqltablemodel_data *tablemodel, const QString &localMemberNo = "");

    static bool cancelTable(const QString &ch_tableno, int sqr, QString &errstring);
    static bool cancelTableCheck(const QString &ch_tableno, int sqr, QString &errstring);
    static bool cancelTable_transaction(const QString &ch_tableno, int sqr, QString &errstring);
    static void wineTake(QWidget *parent);

public:
    explicit fexpandmain_2(QObject *parent = 0);
    ~fexpandmain_2();
};

#endif // FEXPANDMAIN_2_H

#include "w_sys_manage_outer_settings_label.h"
#include "w_st_good_input_select_dish.h"
#include "n_func.h"
#include "transparentcenterdialog.h"

w_sys_manage_outer_settings_label::w_sys_manage_outer_settings_label(QWidget *parent) :
    labelprint_main_dialog(parent)
{
}

void w_sys_manage_outer_settings_label::togoodadd()
{
    if(is_label_mode) {
        labelprint_main_dialog::togoodadd();
        return;
    }
    ///标签打印机导入的表头是什么，生成的itemlist的一行对应的就是什么
    ///重载步骤：
    /// 1调用其他窗口，生成对应的itemlist，
    /// 2调用goodsadd(itemslist);
    //1

    QList<QStringList> itemslist;
    QString dt_str = n_func::f_get_sysdatetime_str();

    w_st_good_input_select_dish dialog(this);
    dialog.setWindowTitle(QObject::tr("选择商品"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        foreach(int row, dialog.ret_row_list) {
            itemslist.append(QStringList()
                             << "1"
                             << dialog.get_tablemodel_value(row, "vch_dishname").toString()
                             << dialog.get_tablemodel_value(row, "ch_dishno").toString()
                             << QString::number(dialog.get_tablemodel_value(row, "num_price").toDouble(), 'f', 2)
                             << dt_str
                             << ""
                             << ""
                             )
                    ;
        }
    }
    //2
    goodsadd(itemslist);
}

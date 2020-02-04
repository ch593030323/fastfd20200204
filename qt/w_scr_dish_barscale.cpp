#include "w_scr_dish_barscale.h"
#include "lds_query_hddpos.h"
#include "labelprint_goodinfo_add_dialog.h"
#include "lds_query_hddpos.h"
#include "lds_messagebox.h"
#include "transparentcenterdialog.h"
#include "w_st_good_input_select_dish.h"

w_scr_dish_barscale::w_scr_dish_barscale(QWidget *parent) :
    barscale_main_dialog(parent)
{
}

void w_scr_dish_barscale::togoodadd()
{
    QString filter = "  vch_barcode <> '' ";
    switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("选择"), QObject::tr("称重商品"), QObject::tr("非称重商品"), QObject::tr("所有商品"), QObject::tr("取消"))) {
    case 0://称重商品
        filter += " and ch_weight = 'Y' ";
        break;
    case 1://非称重商品
        filter += " and ch_weight <> 'Y' ";
        break;
    case 2://所有商品
        break;
    default:
        return;
    }

    w_st_good_input_select_dish dialog(this);
    dialog.filter_by_other(filter);
    dialog.setWindowTitle(QObject::tr("选择"));
    if(QDialog::Accepted ==transparentCenterDialog(&dialog).exec()) {
        QList<BARSCAEL_ITEMS> items;
        foreach(int k, dialog.ret_row_list) {
            items.append(BARSCAEL_ITEMS(
                             ""/*plu为空*/,
                             dialog.get_tablemodel_value(k, "vch_dishname").toString(),
                             dialog.get_tablemodel_value(k, "vch_barcode").toString(),
                             dialog.get_tablemodel_value(k, "num_price").toString(),
                             ""));

        }
        //    }
            goodsadd(items);
    }

    return;
    //1
//    QList<BARSCAEL_ITEMS> items;
//    labelprint_goodinfo_add_dialog dialog(this);
//    dialog.model->setHorizontalHeaderLabels(QStringList() << QObject::tr("商品名") << QObject::tr("商品代码") << QObject::tr("价格")<<QObject::tr("有效期"));
//    lds_query_hddpos  query;
//    query.execSelect("select vch_dishname, vch_barcode, num_price,'' as dt_validdate from cey_bt_dish where vch_barcode <> '' ");
//    while(query.next()) {
//        dialog.model->appendRow(QList<QStandardItem*>()
//                                <<new QStandardItem(query.recordValue("vch_dishname").toString())
//                                <<new QStandardItem(query.recordValue("vch_barcode").toString())
//                                <<new QStandardItem(query.recordValue("num_price").toString())
//                                <<new QStandardItem(query.recordValue("dt_validdate").toString())
//                                );
//    }
//    //2
//    if(QDialog::Accepted==transparentCenterDialog(&dialog).exec()) {
//        QModelIndexList indexlist=dialog.ui->tableView->selectionModel()->selectedIndexes();
//        QSet<int> index_list;
//        foreach(QModelIndex index, indexlist) {
//            index_list.insert(index.row());
//        }
//        foreach(int index, index_list) {
//            items.append(BARSCAEL_ITEMS(
//                             ""/*plu为空*/,
//                             dialog.model->item(index, 0)->text(),
//                             dialog.model->item(index, 1)->text(),
//                             dialog.model->item(index, 2)->text(),
//                             "")
//                         );
//        }
//    }
//    goodsadd(items);
}

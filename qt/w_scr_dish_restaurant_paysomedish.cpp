#include "w_scr_dish_restaurant_paysomedish.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "lds_messagebox.h"
#include "public_printby_ch_billno.h"
#include "public_sql.h"

w_scr_dish_restaurant_paysomedish::w_scr_dish_restaurant_paysomedish(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_bt_dish_unitset_Dialog)
{
    ui->setupUi(this);
}

w_scr_dish_restaurant_paysomedish::~w_scr_dish_restaurant_paysomedish()
{
    delete ui;
}


void w_scr_dish_restaurant_paysomedish::topaysomedish_preprint()
{
    const fexpandmain_model_sqltablemodel_data *model = static_cast<fexpandmain_model_sqltablemodel_data *>(ui->tableView->model());
    if(!model->model_is_dirty()) {
        QString int_flowids;
        QList<int> list = ui->tableView->getSelectedRowListDesc();
        foreach(int row, list) {
            int_flowids += model->model_data(row, "int_flowid").toString()+",";
        }
        if(!int_flowids.isEmpty()) {
            int_flowids.chop(1);
        }
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("是否打印预打单"), QObject::tr("是"), QObject::tr("否"))) {
            public_printby_ch_billno printer(model->cur_ch_billno());
            printer.toprint_preprint(printer.defpath(), QObject::tr("预打单"), QString("int_flowid in (%1)").arg(int_flowids));
            public_sql::preprinttablemap.insert(model->cur_ch_billno(), true);
            return;
        }
    }
}

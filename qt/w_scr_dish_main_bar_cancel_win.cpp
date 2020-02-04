#include "w_scr_dish_main_bar_cancel_win.h"
#include "ui_w_scr_dish_main_bar_cancel_win.h"
#include "w_st_good_input_select_dish.h"
#include "lds_messagebox.h"
#include <QTimer>
#include "lds_query_hddpos.h"
#include <QtDebug>
#include "n_func.h"
#include"public_sql.h"
#include "getchinesespell.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QSqlError>
#include "lds_tableview_delegate_time.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "printer_0.h"
#include "backheader.h"
#include "lds.h"
#include "public_sql.h"
#include "lds_tableview_delegate_doublespinbox.h"

w_scr_dish_main_bar_cancel_win::w_scr_dish_main_bar_cancel_win(const QString &vch_handle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_main_bar_cancel_win)
{
    ui->setupUi(this);
    this->vch_handle = vch_handle;

    tablemodel = new sqlTableModel(this);
    tablemodel->setTable("cey_bt_table_bar_wine");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_dishno", QObject::tr("名称"));
    tablemodel->save_set_header("num_dish", QObject::tr("数量"), 3);
    tablemodel->save_set_header("dt_enddate", QObject::tr("有效期至"), 3);
    tablemodel->save_set_header("vch_operID", QObject::tr("存酒人"));
    tablemodel->save_set_header("vch_memo", QObject::tr("备注"), true);
    tablemodel->enable_vheaderdata_dirty_translate();

    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_dishno"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel, "cey_bt_dish","ch_dishno","vch_dishname"
                                                ));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("dt_enddate"), new lds_tableview_delegate_time(QVariant::Date, this));
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    lds_tableview_delegate_doublespinbox *d = new lds_tableview_delegate_doublespinbox(this);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("num_dish"), d);
    //connect
    connect(ui->pushButton_add, SIGNAL(clicked()), this, SLOT(toadd()));
    connect(ui->pushButton_del, SIGNAL(clicked()), this, SLOT(todel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_take_wine, SIGNAL(clicked()), this, SLOT(totakewine()));
    connect(ui->pushButton_print, SIGNAL(clicked()), this, SLOT(toprint()));

    QTimer::singleShot(10, this, SLOT(torefresh()));
}

w_scr_dish_main_bar_cancel_win::~w_scr_dish_main_bar_cancel_win()
{
    delete ui;
}

void w_scr_dish_main_bar_cancel_win::took()
{
    saveData();
}

void w_scr_dish_main_bar_cancel_win::tocancel()
{
    if(tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!saveData()) {
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }
    this->reject();
}

void w_scr_dish_main_bar_cancel_win::todel()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0 ) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"), QObject::tr("确定"), QObject::tr("取消"))) {
        tablemodel->removeRow(row);
    }
}

void w_scr_dish_main_bar_cancel_win::toadd()
{
    w_st_good_input_select_dish dialog(this);
    dialog.setWindowTitle(QObject::tr("添加商品"));
    if(QDialog::Accepted ==transparentCenterDialog(&dialog).exec()) {
        int row = tablemodel->rowCount();
        int row_old = row;
        foreach(int ret_row, dialog.ret_row_list) {
            tablemodel->insertRow(row);
            tablemodel->model_data_set(row, "vch_guest", ui->lineEdit_vch_guest->text());
            tablemodel->model_data_set(row, "vch_handle", ui->lineEdit_vch_handle->text());
            tablemodel->model_data_set(row, "ch_dishno", dialog.get_tablemodel_value(ret_row, "ch_dishno"));
            tablemodel->model_data_set(row, "vch_operID", lds::gs_operid);
            tablemodel->model_data_set(row, "dt_operdate", n_func::f_get_sysdatetime());
            tablemodel->model_data_set(row, "dt_enddate",  n_func::f_get_sysdatetime().addMonths(1).date());
        }
        ui->tableView->setCurrentIndex(tablemodel->model_index(row_old, "num_dish"));
    }

    ui->tableView->resizeColumnsToContents();
}

void w_scr_dish_main_bar_cancel_win::torefresh()
{
    tablemodel->setFilter(QString("vch_handle = '%1' order by dt_enddate desc")
                          .arg(vch_handle)
                          );
    ui->tableView->resizeColumnsToContents();
}

void w_scr_dish_main_bar_cancel_win::totakewine()
{
    QString vch_dishname = QObject::tr("名称");
    QString num_dish_taken_str = QObject::tr("取出");
    QString num_dish_remain_str = QObject::tr("剩余");
    int vch_dishname_len = 17;
    int num_dish_taken_str_len = 7;
    int num_dish_remain_str_len = 7;
    int print_row = 0;
    blockIns printins;
    printins.append(backheader::printer.drawText("a00,s11", QObject::tr("取酒") +"\n","a10,s00"));
    backheader::backheader_printer_insert_company(printins, "a10");
    printins.append(backheader::printer.drawText("", ui->label_vch_guest->text()+":"+ui->lineEdit_vch_guest->text()+"\n"));
    printins.append(backheader::printer.drawText("", ui->label_vch_handle->text()+":"+ui->lineEdit_vch_handle->text()+"\n"));
    printins.append(backheader::printer.drawText("-"));
    printins.append(backheader::printer.drawText("",
                                                 vch_dishname + QString(vch_dishname_len - lds::size_g_of_u(vch_dishname), ' ') +
                                                 QString(num_dish_taken_str_len - lds::size_g_of_u(num_dish_taken_str), ' ') + num_dish_taken_str +
                                                 QString(1, ' ') +
                                                 QString(num_dish_remain_str_len - lds::size_g_of_u(num_dish_remain_str), ' ') + num_dish_remain_str + "\n"));
    ///////
    QString errstring;
    QList<int> row_selected = ui->tableView->getSelectedRowListAsc();
    if(row_selected.count() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    //一键取出所选商品
    if(row_selected.count() > 0) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("一键取出所选商品"), QObject::tr("确定"), QObject::tr("分开取"), QObject::tr("取消"))) {
        case 0://一键取出所选商品
        {
            lds_query_hddpos  query;
            lds_query_hddpos::tran_saction();
            foreach(int row, row_selected) {
                double num_dish = tablemodel->model_data(row, "num_dish").toDouble();
                double num_dish_taken = num_dish;

                QString vch_dishname = tablemodel->tablename_kvmap_value("cey_bt_dish",
                                                                         tablemodel->model_data(row, "ch_dishno").toString(),
                                                                         "ch_dishno",
                                                                         "vch_dishname").toString();
                //update
                if(false == query.execUpdate("cey_bt_table_bar_wine",
                                             "num_dish", qrtFunc("num_dish - "+QString::number(num_dish_taken)),
                                             qrtEqual("int_flowID", tablemodel->model_data(row, "int_flowID").toString()))) {
                    lds_query_hddpos::roll_back();
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("取酒") + ":" + vch_dishname + "\n"+ query.recordError());
                    return;
                }

                num_dish_taken_str = QString::number(num_dish_taken);
                num_dish_remain_str = QString::number(num_dish - num_dish_taken);
                printins.append(backheader::printer.drawText("",
                                                             vch_dishname + QString(vch_dishname_len - lds::size_g_of_u(vch_dishname), ' ') +
                                                             QString(num_dish_taken_str_len - lds::size_g_of_u(num_dish_taken_str), ' ') + num_dish_taken_str +
                                                             QString(1, ' ') +
                                                             QString(num_dish_remain_str_len - lds::size_g_of_u(num_dish_remain_str), ' ') + num_dish_remain_str + "\n"));
                print_row ++;

                //delete
                if(num_dish == num_dish_taken) {
                    if(false == query.execDelete("cey_bt_table_bar_wine",  QString(" int_flowID = %1 ")
                                                 .arg(tablemodel->model_data(row, "int_flowID").toString()))) {
                        lds_query_hddpos::roll_back();
                        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("取酒") + ":" + vch_dishname + "\n"+ query.recordError());
                        return;
                    }
                }
            }
            lds_query_hddpos::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            torefresh();
            goto print;
            return;
        }
            break;
        case 2://取消
            return;
        }
    }

    foreach(int row, row_selected) {
        double num_dish = tablemodel->model_data(row, "num_dish").toDouble();
        QString vch_dishname = tablemodel->tablename_kvmap_value("cey_bt_dish",
                                                                 tablemodel->model_data(row, "ch_dishno").toString(),
                                                                 "ch_dishno",
                                                                 "vch_dishname").toString();
        //vch_handle
        double num_dish_taken = 0;
        lds_dialog_input inputdialog(this);
        inputdialog.setWindowTitle(QObject::tr("取酒") + ":" + vch_dishname);
        inputdialog.ui->label->setText(QObject::tr("数量"));
        inputdialog.ui->lineEdit->setText(QString::number(num_dish));
        if(QDialog::Accepted == transparentCenterDialog(&inputdialog).exec()) {
            num_dish_taken = inputdialog.ui->lineEdit->text().toDouble();
            if(num_dish_taken > num_dish) {
                lds_messagebox::warning(this,MESSAGE_TITLE_VOID,  QObject::tr("取酒") + ":" + vch_dishname + "\n"+ QObject::tr("当前数量不能超过最大数量"));
                continue;
            }
            if(num_dish_taken <= 0) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("取酒") + ":" + vch_dishname + "\n"+ QObject::tr("当前数量不能小于等于0"));
                continue;
            }

            lds_query_hddpos  query;
            lds_query_hddpos::tran_saction();
            //update
            if(false == query.execUpdate("cey_bt_table_bar_wine",
                                         "num_dish", qrtFunc("num_dish - " + QString::number(num_dish_taken)),
                                         qrtEqual("int_flowID", tablemodel->model_data(row, "int_flowID").toString()))) {
                lds_query_hddpos::roll_back();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("取酒") + ":" + vch_dishname + "\n"+ query.recordError());
                continue;
            }

            num_dish_taken_str = QString::number(num_dish_taken);
            num_dish_remain_str = QString::number(num_dish - num_dish_taken);
            printins.append(backheader::printer.drawText("",
                                                         vch_dishname + QString(vch_dishname_len - lds::size_g_of_u(vch_dishname), ' ') +
                                                         QString(num_dish_taken_str_len - lds::size_g_of_u(num_dish_taken_str), ' ') + num_dish_taken_str +
                                                         QString(1, ' ') +
                                                         QString(num_dish_remain_str_len - lds::size_g_of_u(num_dish_remain_str), ' ') + num_dish_remain_str + "\n"));
            print_row ++;
            //delete
            if(num_dish == num_dish_taken) {
                if(false == query.execDelete("cey_bt_table_bar_wine", QString("  int_flowID = %1 ")
                                             .arg(tablemodel->model_data(row, "int_flowID").toString()))) {
                    lds_query_hddpos::roll_back();
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("取酒") + ":" + vch_dishname + "\n"+ query.recordError());
                    continue;
                }
            }
            lds_query_hddpos::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,  QObject::tr("取酒") + ":" + vch_dishname + "\n"+ QObject::tr("操作成功"));
        }
    }
    torefresh();

print:
    if(print_row > 0) {
        printins.append(backheader::printer.drawText("-"));
        printins.append(backheader::printer.drawText("", QObject::tr("操作员") + ":" + lds::gs_operid + "\n"));
        printins.append(backheader::printer.drawText("", QObject::tr("操作时间") + ":" + n_func::f_get_sysdatetime_str() + "\n"));
        printins.append(backheader::printer.drawTextEnd("1B6405 1D5601", ""));//走纸5x， 切纸

        backheader::reslove_write(printins);
    }
}

void w_scr_dish_main_bar_cancel_win::toprint()
{
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    QString vch_dishname = QObject::tr("名称");
    QString num_dish = QObject::tr("数量");
    QString dt_enddate = QObject::tr("有效期至");
    int vch_dishname_len = 14;
    int num_dish_len = 7;
    int dt_enddate_len = 10;
    blockIns printins;
    printins.append(backheader::printer.drawText("a00,s11", QObject::tr("存酒") +"\n","a10,s00"));
    backheader::backheader_printer_insert_company(printins, "a10");
    printins.append(backheader::printer.drawText("", ui->label_vch_guest->text()+":"+ui->lineEdit_vch_guest->text()+"\n"));
    printins.append(backheader::printer.drawText("", ui->label_vch_handle->text()+":"+ui->lineEdit_vch_handle->text()+"\n"));
    printins.append(backheader::printer.drawText("-"));
    printins.append(backheader::printer.drawText("",
                                                 vch_dishname + QString(vch_dishname_len - lds::size_g_of_u(vch_dishname), ' ') +
                                                 QString(num_dish_len - lds::size_g_of_u(num_dish), ' ') + num_dish +
                                                 QString(1, ' ') +
                                                 dt_enddate + QString(dt_enddate_len - lds::size_g_of_u(dt_enddate), ' ')
                                                 + "\n"));
    printins.append(backheader::printer.drawText("-"));
    for(int row = 0; row < tablemodel->rowCount(); row ++)  {
        //逾期失效
        QString vch_dishname = tablemodel->tablename_kvmap_value("cey_bt_dish",
                                                                 tablemodel->model_data(row, "ch_dishno").toString(), "ch_dishno", "vch_dishname").toString();
        QString num_dish = tablemodel->model_data(row, "num_dish").toString();
        QString dt_enddate = tablemodel->model_data(row, "dt_enddate").toDateTime().toString("yyyy-MM-dd");
        QString vch_memo = tablemodel->model_data(row, "vch_memo").toString();
        printins.append(backheader::printer.drawText("",
                                                     vch_dishname + QString(vch_dishname_len - lds::size_g_of_u(vch_dishname), ' ') +
                                                     QString(num_dish_len - lds::size_g_of_u(num_dish), ' ') + num_dish +
                                                     QString(1, ' ') +
                                                     dt_enddate + QString(dt_enddate_len - lds::size_g_of_u(dt_enddate), ' ')
                                                     + "\n"));
        if(vch_memo.trimmed().length() > 0) {
            printins.append(backheader::printer.drawText("",vch_memo + "\n"));
        }
    }
    printins.append(backheader::printer.drawText("-"));
    printins.append(backheader::printer.drawText("", QObject::tr("操作员") + ":" + lds::gs_operid + "\n"));
    printins.append(backheader::printer.drawText("", QObject::tr("操作时间") + ":" + n_func::f_get_sysdatetime_str() + "\n"));
    printins.append(backheader::printer.drawText("", QObject::tr("过期失效") + "\n"));
    printins.append(backheader::printer.drawTextEnd("1B6405 1D5601", ""));//走纸5x， 切纸

    backheader::reslove_write(printins);
}

bool w_scr_dish_main_bar_cancel_win::saveData()
{
    for(int row = 0; row < tablemodel->rowCount(); row ++) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
            if(tablemodel->model_data(row, "num_dish").toDouble() <= 0) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QString::number(row+1) + QObject::tr("行") +"\n"+ QObject::tr("商品数量不能小于等于0"));
                ui->tableView->clearSelection();
                ui->tableView->setCurrentIndex(tablemodel->model_index(row, "num_dish"));
                return false;
            }
        }
    }
    lds_query_hddpos::tran_saction();
    if(tablemodel->trySubmitAll()) {
        lds_query_hddpos::com_mit();
        ui->tableView->resizeColumnsToContents();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    return false;
}


w_scr_dish_main_bar_cancel_win_pend::w_scr_dish_main_bar_cancel_win_pend(const QString &vch_handle, const QString &vch_guest, QWidget *parent)
    : w_scr_dish_main_bar_cancel_win(vch_handle, parent)
{
    ui->lineEdit_vch_handle->setText(vch_handle);
    ui->lineEdit_vch_guest->setText(vch_guest);
    ui->pushButton_take_wine->hide();
    this->setWindowTitle(QObject::tr("存酒"));
}


w_scr_dish_main_bar_cancel_win_take::w_scr_dish_main_bar_cancel_win_take(const QString &vch_handle, QWidget *parent)
    : w_scr_dish_main_bar_cancel_win(vch_handle, parent)
{
    tablemodel->save_set_header("num_dish", QObject::tr("数量"), 0);
    tablemodel->save_set_header("vch_memo", QObject::tr("备注"), 0);

    ui->lineEdit_vch_handle->setText(vch_handle);
    ui->tableView->setSelectionMode(QTableView::MultiSelection);
    ui->pushButton_ok->hide();
    ui->pushButton_add->hide();

    lds_query_hddpos  query;
    query.execSelect(QString("SELECT  vch_guest from cey_bt_table_bar_wine  where vch_handle = '%1' limit 1")
                     .arg(vch_handle));
    query.next();
    ui->lineEdit_vch_guest->setText(query.recordValue("vch_guest").toString());
    this->setWindowTitle(QObject::tr("取酒"));
}


w_scr_dish_main_bar_cancel_win::sqlTableModel::sqlTableModel(QObject *parent)
    : lds_model_sqltablemodel(parent)
{
    tablename_kvmap_insert_sql("mysql_now", "select  'cur_time', now()", false, SELECT_AUTO_REFRESH_KEY);
}

w_scr_dish_main_bar_cancel_win::sqlTableModel::~sqlTableModel()
{

}

QVariant w_scr_dish_main_bar_cancel_win::sqlTableModel::data(const QModelIndex &idx, int role) const
{
    QVariant d = lds_model_sqltablemodel::data(idx, role);
    if(Qt::TextColorRole == role) {
        if(idx.column() == this->fieldIndex("dt_enddate")) {
            if(this->tablename_kvmap_value("mysql_now", "cur_time").toDate() > data(idx).toDate()) {
                return QColor("red");
            }
            if(this->tablename_kvmap_value("mysql_now", "cur_time").toDate() == data(idx).toDate()) {
                return QColor("#ff546b");
            }
            if(this->tablename_kvmap_value("mysql_now", "cur_time").toDate() < data(idx).toDate()) {
                return QColor("blue");
            }
        }
    }

    return d;
}

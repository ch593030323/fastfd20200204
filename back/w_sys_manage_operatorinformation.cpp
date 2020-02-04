#include "w_sys_manage_operatorinformation.h"
//操作员信息
#include "ui_w_bt_dish_unitset_dialog.h"
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>
#include "w_sys_manage.h"
#include "lds_messagebox.h"
#include <QSqlError>
#include <QTimer>
#include "backheader.h"
#include "lds_tableview_delegate_check.h"
#include "ui_w_sys_manage_operatorinformation_dialog.h"
#include "lds_model_sqltablemodel.h"

w_sys_manage_operatorinformation::w_sys_manage_operatorinformation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_sys_manage_operatorinformation_Dialog)
{
    ui->setupUi(this);
    ui->label_discount->setText(QObject::tr("折扣下限"));
    if(public_sql::discount_off_mode()) {
        ui->label_discount->setText(QObject::tr("折扣上限"));
    }
    ui->lineEdit_vch_password->setEchoMode(QLineEdit::Password);
    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_sys_operator");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("vch_operID", QObject::tr("编码"));
    tablemodel->save_set_header("vch_operator", QObject::tr("姓名"));
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    //! datawidget
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->addMapping(ui->lineEdit_int_discount,tablemodel->fieldIndex("int_discount"));
    mapper->addMapping(ui->lineEdit_num_free,tablemodel->fieldIndex("num_free"));
    mapper->addMapping(ui->lineEdit_num_present,tablemodel->fieldIndex("num_present"));
    mapper->addMapping(ui->lineEdit_vch_operator,tablemodel->fieldIndex("vch_operator"));
    mapper->addMapping(ui->lineEdit_vch_operid,tablemodel->fieldIndex("vch_operid"));
    mapper->addMapping(ui->lineEdit_vch_password,tablemodel->fieldIndex("vch_password"));
    mapper->addMapping(ui->lineEdit_num_deduct_rate,tablemodel->fieldIndex("num_deduct_rate"));

    mapper->addMapping(ui->checkBox_ch_flag,tablemodel->fieldIndex("ch_flag"), "checked1");//将bool 转换为Y N
    mapper->addMapping(ui->checkBox_ch_free_flag,tablemodel->fieldIndex("ch_free_flag"), "checked1");//将bool 转换为Y N
    mapper->addMapping(ui->checkBox_ch_waiter,tablemodel->fieldIndex("ch_waiter"), "checked1");//将bool 转换为Y N

    //! toolbar
    connect(ui->pushButton_new, SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->pushButton_del, SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->pushButton_save, SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView,SIGNAL(selectChanged(int)),mapper,SLOT(setCurrentIndex(int)));

    //
    tablemodel->select();
    ui->tableView->selectRow(0);
}

void w_sys_manage_operatorinformation::tonew()
{
    int rowcount=tablemodel->rowCount();
    tablemodel->insertRow(rowcount);

    tablemodel->model_data_set(rowcount, "ch_flag", "Y");
    tablemodel->model_data_set(rowcount, "num_free", "500");
    if(public_sql::discount_off_mode()) {
        tablemodel->model_data_set(rowcount, "int_discount", 100);
    } else {
        tablemodel->model_data_set(rowcount, "int_discount", 0);
    }
    tablemodel->model_data_set(rowcount, "vch_operid", tablemodel->max_column_add_1_dec("vch_operid", 1001));
    tablemodel->model_data_set(rowcount, "vch_operator", tablemodel->model_data(rowcount, "vch_operid"));
    tablemodel->model_data_set(rowcount, "vch_password", tablemodel->model_data(rowcount, "vch_operid"));

    ui->tableView->selectRow(rowcount);
    ui->lineEdit_vch_operator->setFocus();
    ui->lineEdit_vch_operator->selectAll();
}

void w_sys_manage_operatorinformation::todel()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    lds_query_hddpos  query;
    query.execSelect(QString(" select vch_operid from cey_u_checkout_master where vch_operid = '%1' limit 1").arg(tablemodel->model_data(row, "vch_operid").toString()));
    if(query.next()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("有营业数据无法删除"));
        return;
    }
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"), QObject::tr("是"), QObject::tr("否"))) {
        return;
    }
    tablemodel->removeRow(ui->tableView->currentIndex().row());
    tablemodel->trySubmitAll();
    ui->tableView->restore(0);
}

void w_sys_manage_operatorinformation::tosave()
{
    int row = ui->tableView->currentIndex().row();
    saveData();
    ui->tableView->selectRow(row);
}

void w_sys_manage_operatorinformation::toexit()
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

bool w_sys_manage_operatorinformation::saveData()
{
    for(int row = 0,rowcount = tablemodel->rowCount();row<rowcount;row++) {
        if(tablemodel->model_is_dirty(row)
                && tablemodel->model_data(row,  0).toString().startsWith("0")) {
            if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID,
                                            QObject::tr("行数") + QString::number(row+1) + "\n" + QObject::tr("编号开头为0，若使用点菜宝则禁止编号开头为0，是否继续"), QObject::tr("继续"), QObject::tr("取消"))) {
                return false;
            }
        }
    }
    QString errstring;
    if(tablemodel->trySubmitAll(&errstring)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    return false;
}

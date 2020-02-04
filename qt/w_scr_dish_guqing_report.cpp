#include "w_scr_dish_guqing_report.h"
#include "ui_w_scr_dish_guqing_report.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "public_sql.h"
#include "lds_tableview_delegate_check.h"
#include "lds.h"
#include "lds_query_hddpos.h"
#include "fexpandmain_2.h"


w_scr_dish_guqing_report::w_scr_dish_guqing_report(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_guqing_report)
{
    ui->setupUi(this);
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_bt_dish");
    tablemodel->save_set_header("ch_dishno", QObject::tr("编号"));
    tablemodel->save_set_header("vch_dishname", QObject::tr("名称"));
    tablemodel->save_set_header("ch_guqing_flag", QObject::tr("估清设置"), true);
    tablemodel->save_set_header("num_guqing_num", QObject::tr("估清量"), true);

    ui->tableView->setModel(tablemodel);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_guqing_flag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_guqing_flag"), ui->tableView, true));


    ui->checkBox_showall->setChecked(lds::sysconf->value("w_scr_dish_guqing_report/showall", false).toBool());

    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_save, SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->checkBox_showall, SIGNAL(toggled(bool)),this,SLOT(tosave_showall(bool)));


    QString filter = "Y";
    if(ui->checkBox_showall->isChecked()) {
        filter = "";
    }
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_scr_dish_guqing_report::~w_scr_dish_guqing_report()
{
    delete ui;
}

void w_scr_dish_guqing_report::tosave()
{
    saveData();
}

void w_scr_dish_guqing_report::toexit()
{
    this->reject();
}

void w_scr_dish_guqing_report::tosave_showall(bool f)
{
    fexpandmain_2::guqing_try_clear();

    lds::sysconf->setValue("w_scr_dish_guqing_report/showall", f);

    QString filter = "Y";
    if(f) {
        filter = "";
    }

    if(tablemodel->model_is_dirty()) {
        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"))) {
            if(!saveData()) return;
        }
    }
    tablemodel->setFilter(QString("ifnull(ch_guqing_flag, '')  like '%1%' and (ch_dishno like '%2%' or vch_spell like '%2%')  order by ch_guqing_flag desc").arg(filter).arg(ui->lineEdit_filter->text().toUpper()));
//    ui->tableView->setColumnWidth(0, 60);
//    ui->tableView->setColumnWidth(1, 300);
//    ui->tableView->setColumnWidth(2, 60);
//    ui->tableView->setColumnWidth(3, 60);
    ui->tableView->resizeColumnsToContents();

}

bool w_scr_dish_guqing_report::saveData()
{
    lds_query_hddpos::tran_saction();

    if(tablemodel->trySubmitAll()) {
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        return true;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    return false;
}

void w_scr_dish_guqing_report::on_lineEdit_filter_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    tosave_showall(ui->checkBox_showall->isChecked());
}

void w_scr_dish_guqing_report::torefresh()
{
    tosave_showall(ui->checkBox_showall->isChecked());
}

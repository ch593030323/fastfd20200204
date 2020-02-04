#include "w_m_member_telcoming.h"
#include "ui_w_m_member_telcoming.h"
#include "w_m_member.h"
#include "w_m_member_list.h"
#include "w_m_member_telcoming_table.h"
#include <QTimer>
#include "w_scr_dish_main.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QDir>
#include "lds_scr_table.h"

w_m_member_telComing::w_m_member_telComing(bool isQ000, const QString &vch_tel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_m_member_telComing)
{
    ui->setupUi(this);

    vch_tel_for_first = vch_tel;
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setTable("t_m_member");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("vch_memberno", QObject::tr("会员号"));
    tablemodel->save_set_header("vch_member", QObject::tr("会员名称"));
    tablemodel->save_set_header("vch_tel", QObject::tr("电话"));

    tablemodel->save_set_header("ch_state", QObject::tr("状态"));
    tablemodel->save_set_header("ch_cardflag", QObject::tr("持卡状态"));
    tablemodel->save_set_header("dt_limit", QObject::tr("有效期"));

    ui->tableView->setModel(tablemodel);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_state"), new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_STATE()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_cardflag"), new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_FLAG()));
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    //
    ui->label->setText(QObject::tr("未处理的电话"));

    model_tel = new QStandardItemModel(this);
    model_tel->setHorizontalHeaderLabels(QStringList() << QObject::tr("电话") << QObject::tr("时间"));
    ui->tableView_tel->setModel(model_tel);
    ui->tableView_tel->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_tel->setSelectionMode(QTableView::SingleSelection);

    connect(ui->pushButton_add, SIGNAL(clicked()), this, SLOT(toaddm()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(toorder()));
    connect(ui->pushButton_cancel,  SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->tableView_tel, SIGNAL(clicked(QModelIndex)), this, SLOT(torefreshM()));
    connect(ui->pushButton_tel_del, SIGNAL(clicked()), this, SLOT(toTelDel()));
    connect(ui->pushButton_Q000_mread, SIGNAL(clicked()), this, SLOT(toQ000_mread()));
    connect(ui->pushButton_refresh, SIGNAL(clicked()), this, SLOT(torefresh()));
    connect(public_sql::save_login_object(), SIGNAL(signal_TelNoComing(QString)), this, SLOT(torefresh()));

    //
    ui->pushButton_Q000_mread->hide();
    ui->pushButton_ok->hide();
    if(isQ000) {//读会员直接开台
        ui->pushButton_Q000_mread->show();
        ui->checkBox->setText(ui->pushButton_Q000_mread->text() + "," + QObject::tr("删除电话记录"));
    } else {//落单
        ui->pushButton_ok->show();
        ui->checkBox->setText(ui->pushButton_ok->text() + "," + QObject::tr("删除电话记录"));
    }
    ui->checkBox->setChecked(lds::sysconf->value("w_m_member_telComing/delRecord", true).toBool());
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(toSaveDelRecord(bool)));
    QTimer::singleShot(100, this, SLOT(torefresh()));

    this->setWindowTitle(QObject::tr("来电信息"));
}

w_m_member_telComing::~w_m_member_telComing()
{
    delete ui;
}

void w_m_member_telComing::toaddm()
{
    int row = ui->tableView_tel->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    w_m_member_list_new dialog(this, tablemodel, ui->tableView);
    dialog.updateVch_tel_handtel(model_tel->item(row, 0)->text());
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void w_m_member_telComing::toorder()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有会员被选中"));
        return;
    }
    w_m_member_telComing_table::telComingData member_from_tel;
    member_from_tel.vch_memberno = tablemodel->model_data(row, "vch_memberno").toString();
    member_from_tel.vch_tel =  ui->tableView_tel->model_data(ui->tableView_tel->currentIndex().row(), 0/*vch_tel*/).toString();

    w_m_member_telComing_table dialog(member_from_tel, this);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {

        ret_ch_tableno = dialog.ret_ch_tableno;
        ret_vch_memberno = tablemodel->model_data(row, "vch_memberno").toString();
        ret_tel = model_tel->item(ui->tableView_tel->currentIndex().row(), 0)->text();

        if(lds::sysconf->value("w_m_member_telComing/delRecord", true).toBool()) {
#ifdef QT_DEBUG
#else
            delTelFileRow(ui->tableView_tel->currentIndex().row());
#endif
        }
        this->accept();
    }
}

void w_m_member_telComing::toexit()
{
    this->reject();
}

void w_m_member_telComing::torefresh()
{
    int row_tel = ui->tableView_tel->currentIndex().row();
    //tel
    //check tel existed
    QFile file(lds::localdataPath + "/TelNoComing.txt");
    if(false == file.exists()) {
        //create
         file.open(QFile::WriteOnly);
         file.close();
    }
    if(false == file.open(QFile::ReadOnly)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, file.errorString());
        return;
    }
    model_tel->removeRows(0, model_tel->rowCount());
    while(!file.atEnd()) {
        QString tel = file.readLine().trimmed();
        if(tel.length() > 0) {
            model_tel->appendRow(QList<QStandardItem *>()
                                 << new QStandardItem(tel.split(",").value(0))
                                 << new QStandardItem(tel.split(",").value(1))
                                 );
            if(vch_tel_for_first.length() > 0 && tel == vch_tel_for_first) {
                row_tel = model_tel->rowCount() - 1;
            }
        }
    }
    ui->tableView_tel->selectRow(row_tel);
    ui->tableView_tel->resizeColumnsToContents();
    vch_tel_for_first = "";
    //
    //model
    torefreshM();
}

void w_m_member_telComing::toTelDel()
{
    int row = ui->tableView_tel->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除") + model_tel->item(row, 0)->text(), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }

    delTelFileRow(row);
    //
    torefresh();
}

void w_m_member_telComing::torefreshM()
{
    if(model_tel->rowCount() > 0) {
        int row_tel = ui->tableView_tel->currentIndex().row();
        if(row_tel < 0) {ui->tableView_tel->selectRow(0); row_tel = 0;}
        QString tel = model_tel->item(row_tel, 0)->text();
        tablemodel->setFilter(QString(" vch_tel = '%1' ")
                              .arg(tel));
        ui->tableView->resizeColumnsToContents();
        ui->tableView->selectRow(0);
    } else {
        tablemodel->setFilter("1 = 2");
    }
}

void w_m_member_telComing::toQ000_mread()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有会员被选中"));
        return;
    }

    ret_ch_tableno = lds_scr_table::Q000;
    ret_vch_memberno = tablemodel->model_data(row, "vch_memberno").toString();
    ret_tel = model_tel->item(ui->tableView_tel->currentIndex().row(), 0)->text();
    this->accept();
}

void w_m_member_telComing::toSaveDelRecord(bool f)
{
    lds::sysconf->setValue("w_m_member_telComing/delRecord", f);
}

bool w_m_member_telComing::delTelFileRow(int row_tel)
{
    //del file
    QFile file(lds::localdataPath + "/TelNoComing.txt");
    if(false == file.open(QFile::WriteOnly)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, file.errorString());
        return false;
    }
    QByteArray tels;
    for(int k = 0; k < model_tel->rowCount(); k ++) {
        if( k == row_tel) continue;
        tels += model_tel->item(k, 0)->text() .toLocal8Bit() +","+model_tel->item(k, 1)->text().toLocal8Bit() + "\n";
    }
    file.write(tels);
    file.close();
    return true;
}

bool w_m_member_telComing::delTelFileTelNo(const QString &tel)
{
    //del file
    QFile file(lds::localdataPath + "/TelNoComing.txt");
    if(false == file.open(QFile::WriteOnly)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, file.errorString());
        return false;
    }
    QByteArray tels;
    for(int k = 0; k < model_tel->rowCount(); k ++) {
        if(model_tel->item(k, 0)->text() == tel) continue;
        tels += model_tel->item(k, 0)->text() .toLocal8Bit() +","+model_tel->item(k, 1)->text().toLocal8Bit() + "\n";
    }
    file.write(tels);
    file.close();
    return true;
}

#include "public_cika_read_dialog.h"
#include "n_func.h"
#include "w_m_member_list.h"
#include <QStandardItemModel>
#include "lds_query_hddpos.h"
#include "w_sys_manage_cloudsync.h"
#include <QTimer>
#include "ui_lds_dialog_input_dialog.h"

public_cika_read_Dialog::public_cika_read_Dialog(QWidget *parent) :
    lds_dialog_input(parent),
    standmodel(0)
{
    ui->label->setText(QObject::tr("请输入卡号"));
    if(n_func::f_m_get_parameter(this,"input_memberno", "0") !="1") {
        ui->lineEdit->enableTimeClear();
    }
}

public_cika_read_Dialog::~public_cika_read_Dialog()
{
}

QString public_cika_read_Dialog::getcardno()
{
    return took_cardno;
}

void public_cika_read_Dialog::setmemberlist()
{
    openDetail();
    standmodel = new QStandardItemModel;
    standmodel->setHorizontalHeaderLabels(QStringList() << QObject::tr("会员名")<< QObject::tr("手机号")<< QObject::tr("会员号"));
    ui->tableView->setModel(standmodel);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    _sql = "select vch_member, vch_handtel, vch_memberno  from t_m_member  where ch_state = '1' and ch_cardflag ='Y' "
            "and (vch_memberno like '%1%' or vch_handtel like '%%1%') limit 0,5";
    updatemodel();
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(updatemodel()));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(toapp(QModelIndex)));

    delay_set_focus();
}

void public_cika_read_Dialog::updatemodel()
{
    if(standmodel) {
        standmodel->removeRows(0, standmodel->rowCount());
        if(w_m_member_list::is_yun_member) {
            QList<yunMemberSelectInfo > selects;
            w_sys_manage_cloudsync::get_yun_member_select_pre100(this, ui->lineEdit->text(), selects);
            foreach(const yunMemberSelectInfo &s, selects) {
                QList<QStandardItem*> items;
                items.append(new QStandardItem(s.mname));
                items.append(new QStandardItem(s.mhandtel));
                items.append(new QStandardItem(s.mcardno));
                standmodel->appendRow(items);
            }
        } else {
            lds_query_hddpos  query;
            query.execSelect(_sql.arg(ui->lineEdit->text()));
            while(query.next()) {
                QList<QStandardItem*> items;
                items.append(new QStandardItem(query.recordValue("vch_member").toString()));
                items.append(new QStandardItem(query.recordValue("vch_handtel").toString()));
                items.append(new QStandardItem(query.recordValue("vch_memberno").toString()));
                standmodel->appendRow(items);
            }
        }
        ui->tableView->selectRow(0);
    }
}

void public_cika_read_Dialog::toapp(const QModelIndex &index)
{
    if(index.isValid()) {
        ui->lineEdit->setText(standmodel->item(index.row(), 2)->text());
        took();
    }
}

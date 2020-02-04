#include "w_rpt_report_tablebusinessreport.h"
//桌台营业统计
#include "ui_w_rpt_report_tablebusinessreport_dialog.h"
#include "n_func.h"
#include "backheader.h"
#include "lds_tableview.h"
#include <QtDebug>

w_rpt_report_tablebusinessreport::w_rpt_report_tablebusinessreport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_rpt_report_tablebusinessreport_Dialog)
{
    //! init
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(23,59,59)));
    tab1sqlinit();

    querymodel=new lds_model_sqlstandardmodel(this);

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);ui->tableView->setModel(querymodel);
    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("查询")<<QObject::tr("退出"), Qt::AlignTop);
    connect(ui->widget->index_widget(QObject::tr("查询")),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->comboBox_11,SIGNAL(currentIndexChanged(int)),this,SLOT(tocom12(int)));
    connect(ui->tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(toselect()));

    //! after
    ui->comboBox_11->addItems(tab1info.keys());
    ui->comboBox_13->addItem(QObject::tr("全部"), "");
    ui->comboBox_13->addItemsBySql(" select 'Q0', 'Q000' union select  ch_tableno, vch_tablename from cey_bt_table  ");
    //stackedWidget header
    ui->comboBox->addItem(QObject::tr("桌台营业统计"));
}

void w_rpt_report_tablebusinessreport::tab1sqlinit()
{
    tab1info.insert(QObject::tr("区域"),
                    w_rpt_report::TABTYPE(QStringList()<<QObject::tr("区域")<<QObject::tr("桌号")<<QObject::tr("桌名")<<QObject::tr("开台次数")<<QObject::tr("比率")<<QObject::tr("人数")<<QObject::tr("比率")<<QObject::tr("实际金额")<<QObject::tr("比率")<<QObject::tr("人均"),
                                          QString (
                                              "         SELECT "
                                              "         ch_areano, "
                                              "         ch_tableno, "
                                              "         vch_tablename,  "
                                              "         opentable,  "
                                              "         opentable / (Select "
                                              "         count(a.ch_tableno) "
                                              "         from "
                                              "         cey_u_table a, "
                                              "         cey_u_checkout_master b "
                                              "         where "
                                              "         a.ch_payno = b.ch_payno "
                                              "         and b.ch_state = 'Y' "
                                              "         and b.dt_operdate >= '%1' "
                                              "         and b.dt_operdate <= '%2') as 'ropentable', "
                                              "         person,  "
                                              "         person / (Select "
                                              "         IFNULL(sum(a.int_person), 0) "
                                              "         from "
                                              "         cey_u_table a, "
                                              "         cey_u_checkout_master b "
                                              "         where "
                                              "         a.ch_payno = b.ch_payno "
                                              "         and b.ch_state = 'Y' "
                                              "         and b.dt_operdate >= '%1' "
                                              "         and b.dt_operdate <= '%2') as 'rperson', "
                                              "         amount,  "
                                              "         amount / (Select "
                                              "         IFNULL(sum(aa.num_realamount), 0) "
                                              "         from "
                                              "         cey_u_checkout_detail aa, "
                                              "         cey_u_checkout_master bb, "
                                              "         cey_bt_paymode cc "
                                              "         where "
                                              "         aa.ch_payno = bb.ch_payno "
                                              "         and bb.ch_state = 'Y' "
                                              "         and aa.ch_paymodeno = cc.ch_paymodeno "
                                              "         and cc.ch_incomeflag = 'Y' "
                                              "         and bb.dt_operdate >= '%1' "
                                              "         and bb.dt_operdate <= '%2') as 'ramount', "
                                              "         (case person when 0 then 0  else amount / person   end) as 'per_person' "
                                              "         FROM "
                                              "         (SELECT "
                                              "         ch_areano, "
                                              "         ch_tableno,  "
                                              "         vch_tablename,  "
                                              "         IFNULL((Select "
                                              "         count(a.ch_tableno) "
                                              "         from "
                                              "         cey_u_table a, cey_u_checkout_master b "
                                              "         where "
                                              "         a.ch_payno = b.ch_payno "
                                              "         and b.ch_state = 'Y' "
                                              "         and b.dt_operdate >= '%1' "
                                              "         and b.dt_operdate <= '%2' "
                                              "         and ZZ.ch_tableno = a.ch_tableno), 0) as 'opentable', "
                                              "         IFNULL((Select "
                                              "         IFNULL(sum(a.int_person), 0) "
                                              "         from "
                                              "         cey_u_table a, cey_u_checkout_master b "
                                              "         where "
                                              "         a.ch_payno = b.ch_payno "
                                              "         and b.ch_state = 'Y' "
                                              "         and b.dt_operdate >= '%1' "
                                              "         and b.dt_operdate <= '%2' "
                                              "         and ZZ.ch_tableno = a.ch_tableno), 0) as 'person', "
                                              "         IFNULL((Select "
                                              "         IFNULL(sum(aa.num_realamount), 0) "
                                              "         from "
                                              "         cey_u_checkout_detail aa, cey_u_checkout_master bb, cey_bt_paymode cc "
                                              "         where "
                                              "         aa.ch_payno = bb.ch_payno "
                                              "         and bb.ch_state = 'Y' "
                                              "         and bb.ch_tableno = ZZ.ch_tableno "
                                              "         and aa.ch_paymodeno = cc.ch_paymodeno "
                                              "         and cc.ch_incomeflag = 'Y' "
                                              "         and bb.dt_operdate >= '%1' "
                                              "         and bb.dt_operdate <= '%2'), 0) as 'amount' "
                                              "         FROM "
                                              "         (select "
                                              "         ch_areano, ch_tableno, vch_tablename "
                                              "         from "
                                              "         cey_bt_table union select 'Q0', 'Q000', 'Q000') ZZ "
                                              "         where "
                                              "         ZZ.ch_areano like '%3%' "
                                              "         and ZZ.ch_tableno like %4'%' "
                                              "         group by ch_areano,  ch_tableno) t "
                                              )
                                          )
                    );




    tab1info.insert(QObject::tr("桌类"),
                    w_rpt_report::TABTYPE(QStringList()<<QObject::tr("桌类")<<QObject::tr("桌号")<<QObject::tr("桌名")<<QObject::tr("开台次数")<<QObject::tr("比率")<<QObject::tr("人数")<<QObject::tr("比率")<<QObject::tr("实际金额")<<QObject::tr("比率")<<QObject::tr("人均"),
                                          QString (
                                              "      SELECT    ch_typeno,    "
                                              "      ch_tableno,    "
                                              "      vch_tablename,    "
                                              "      opentable,    "
                                              "      opentable / (SELECT    "
                                              "                   sum(1)    "
                                              "                   from    "
                                              "                   cey_u_table aa,    "
                                              "                   cey_u_checkout_master bb    "
                                              "                   where    "
                                              "                   aa.ch_payno = bb.ch_payno    "
                                              "      and bb.ch_state = 'Y'    "
                                              "      and bb.dt_operdate >= '%1'    "
                                              "      and bb.dt_operdate <= '%2') as 'tabname4',    "
                                              "      person as 'tabname5',    "
                                              "      person / (SELECT    "
                                              "                sum(cc.int_person)    "
                                              "                from    "
                                              "                cey_u_table cc,    "
                                              "                cey_u_checkout_master dd    "
                                              "                where    "
                                              "                cc.ch_payno = dd.ch_payno    "
                                              "      and dd.ch_state = 'Y'    "
                                              "      and dd.dt_operdate >= '%1'    "
                                              "      and dd.dt_operdate <= '%2') as 'tabname6',    "
                                              "      amount as 'tabname7',    "
                                              "      amount / (Select    "
                                              "                IFNULL(sum(aa.num_realamount), 0)    "
                                              "                from    "
                                              "                cey_u_checkout_detail aa,    "
                                              "                cey_u_checkout_master bb,    "
                                              "                cey_bt_paymode cc    "
                                              "                where    "
                                              "                aa.ch_payno = bb.ch_payno    "
                                              "      and bb.ch_state = 'Y'    "
                                              "      and aa.ch_paymodeno = cc.ch_paymodeno    "
                                              "      and cc.ch_incomeflag = 'Y'    "
                                              "      and bb.dt_operdate >= '%1'    "
                                              "      and bb.dt_operdate <= '%2') as 'tabname8',    "
                                              "      case person    "
                                              "      when 0 then 0    "
                                              "      else amount / person    "
                                              "      end as 'tabname9'    "
                                              "      FROM    "
                                              "      (SELECT    "
                                              "       ch_typeno,    "
                                              "       ch_tableno,    "
                                              "       vch_tablename,     "
                                              "       IFNULL((SELECT    "
                                              "               sum(1)    "
                                              "               from    "
                                              "               cey_u_table aa, cey_u_checkout_master bb    "
                                              "               where    "
                                              "               aa.ch_payno = bb.ch_payno    "
                                              "      and aa.ch_tableno = A.ch_tableno    "
                                              "      and bb.ch_state = 'Y'    "
                                              "      and bb.dt_operdate >= '%1'    "
                                              "      and bb.dt_operdate <= '%2'), 0) as 'opentable',    "
                                              "       IFNULL((SELECT    "
                                              "               sum(cc.int_person)    "
                                              "               from    "
                                              "               cey_u_table cc, cey_u_checkout_master dd    "
                                              "               where    "
                                              "               cc.ch_payno = dd.ch_payno    "
                                              "      and cc.ch_tableno = A.ch_tableno    "
                                              "      and dd.ch_state = 'Y'    "
                                              "      and dd.dt_operdate >= '%1'    "
                                              "      and dd.dt_operdate <= '%2'), 0) as 'person',    "
                                              "       IFNULL((Select    "
                                              "               IFNULL(sum(aa.num_realamount), 0)    "
                                              "               from    "
                                              "               cey_u_checkout_detail aa, cey_u_checkout_master bb, cey_bt_paymode cc    "
                                              "               where    "
                                              "               aa.ch_payno = bb.ch_payno    "
                                              "      and bb.ch_state = 'Y'    "
                                              "      and bb.ch_tableno = A.ch_tableno    "
                                              "      and aa.ch_paymodeno = cc.ch_paymodeno    "
                                              "      and cc.ch_incomeflag = 'Y'    "
                                              "      and bb.dt_operdate >= '%1'    "
                                              "      and bb.dt_operdate <= '%2'), 0) as 'amount'    "
                                              "       FROM    "
                                              "       (select    "
                                              "        ch_typeno, ch_areano, ch_tableno, vch_tablename    "
                                              "        from    "
                                              "        cey_bt_table union select 'Q0', 'Q0', 'Q000', 'Q000') A    "
                                              "       where    "
                                              "       A.ch_typeno like '%3%'    "
                                              "       and A.ch_tableno like %4'%'    "
                                              "       group by ch_typeno,  ch_tableno) t    "
                                              )
                                          )
                    );
}

void w_rpt_report_tablebusinessreport::toselect()
{
    //model select
    querymodel->setQuery(tab1info[ui->comboBox_11->currentText()].sql
            .arg(ui->dateTimeEdit->dateTimeStr())
            .arg(ui->dateTimeEdit_2->dateTimeStr())
            .arg(ui->comboBox_12->curusrdata().toString())
            .arg(ui->comboBox_13->curusrdata().toString())
            );
    querymodel->execQuery();
    //更新表头
    querymodel->setHorizontalHeaderLabels(tab1info[ui->comboBox_11->currentText()].headers);
    //更新合计
    if(tab1info[ui->comboBox_11->currentText()].except_c.isEmpty()) {
        ui->tableView->updateSumRange(3, 9);
    }
    ui->tableView->resizeColumnsToContents();
}

void w_rpt_report_tablebusinessreport::toexit()
{
    this->reject();
}

void w_rpt_report_tablebusinessreport::tocom12(int index)
{
    ui->comboBox_12->clear();
    ui->comboBox_12->addItem(QObject::tr("全部"), "");
    switch(index) {
    case 0:
        ui->comboBox_12->addItemsBySql("select * from cey_bt_table_area");
        break;
    case 1:
        ui->comboBox_12->addItemsBySql("select * from cey_bt_table_type");
        break;
    }
}

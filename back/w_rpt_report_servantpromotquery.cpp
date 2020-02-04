#include "w_rpt_report_servantpromotquery.h"
//服务员提成查询
#include "ui_w_rpt_report_servantpromotquery_dialog.h"
#include "n_func.h"
#include "backheader.h"
#include "lds_tableview.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"

w_rpt_report_servantpromotquery::w_rpt_report_servantpromotquery(QWidget *parent) :
    QDialog(parent),ui(new Ui_w_rpt_report_servantpromotquery_Dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);
    //! init
    //开始结束时间
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(23,59,59)));
    ui->comboBox_operator->addItem(QObject::tr("全部"), "%");
    ui->comboBox_operator->addItemsBySql(backheader::SELECT_OPERID_NAME);
    querymodel=new report_querymodel(this);
    querymodel->arg_query =
            "            SELECT "
            "                    b.vch_waiter, "
            "                        a.ch_payno, "
//            "                        d.ch_dishno, "
            "                        d.vch_dishname, "
            "                        d.ch_unitno, "
//            "                        d.ch_dish_typeno, "
//            "                        d.ch_sub_typeno, "
            "                        (num_num - num_back) as num_dish,"
            "                        ((a.num_num - a.num_back) * a.num_price) * int_discount * 0.01 as 'num_total',"
            "                        (case d.ch_deducttype"
            "                            when '2' then d.num_deduct * (num_num - num_back)"
            "                            else 0"
            "                        end) as 'num_fdeduct',"
            "                        (case d.ch_deducttype"
            "                            when '1' then ((a.num_num - a.num_back) * a.num_price) * int_discount * 0.01 * d.int_deduct * 0.01"
            "                            else 0"
            "                        end) as 'num_rdeduct',"
            "                        b.dt_operdate"
            "                FROM"
            "                    cey_u_orderdish a, cey_u_table b, cey_u_checkout_master c, cey_bt_dish d"
            "                WHERE"
            "                    a.ch_tableno = b.ch_tableno"
            "                        and a.ch_payno = b.ch_payno"
            "                        and b.vch_waiter is not null"
            "                        and a.ch_payno = c.ch_payno"
            "                        and c.ch_state = 'Y'"
            "                        and a.ch_suitflag <> 'Y'"
            "                        and a.ch_dishno = d.ch_dishno"
            "                        and (a.ch_presentflag <> 'Y')"
            "                        and (d.ch_deductflag = 'Y')"
            "                        and b.vch_waiter like '%1' "
            "                        and c.dt_operdate >= '%2' "
            "                        and c.dt_operdate <= '%3' "
            "                        and ifnull(b.vch_waiter, '') <> ''"
            "            ORDER BY b.dt_operdate desc;"
            ;
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(querymodel);


    ui->widget->setup(QStringList()<<QObject::tr("导出") <<QObject::tr("打印") << QObject::tr("查询") <<QObject::tr("退出"), Qt::AlignTop);

    connect(ui->widget->index_widget(0),SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(1),SIGNAL(clicked()),this,SLOT(toprint()));
    connect(ui->widget->index_widget(2),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(3),SIGNAL(clicked()),this,SLOT(toexit()));
}

w_rpt_report_servantpromotquery::~w_rpt_report_servantpromotquery()
{

}

void w_rpt_report_servantpromotquery::toselect()
{
    querymodel->setQuery(querymodel->arg_query
                         .arg(ui->comboBox_operator->curusrdata().toString())
                         .arg(ui->dateTimeEdit->dateTimeStr())
                         .arg(ui->dateTimeEdit_2->dateTimeStr()));

    if(ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("ch_unitno")) == 0)
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_unitno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_unit", "ch_unitno", "vch_unitname"));
    if(ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("vch_waiter")) == 0)
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("vch_waiter"), new lds_model_sqltablemodel_delegate_com(this, ui->comboBox_operator));
    ui->tableView->updateSumRange(4, 7);
    ui->tableView->resizeColumnsToContents();
}

void w_rpt_report_servantpromotquery::toexit()
{
    this->reject();
}

void w_rpt_report_servantpromotquery::toprint()
{
    w_rpt_report_printFormat dialog(w_rpt_report_printFormat::show_model_more, this->windowTitle(), lds::gs_operid, n_func::f_get_sysdatetime_str(), this->metaObject()->className(),
                                    ui->dateTimeEdit->dateTimeStr(), ui->dateTimeEdit_2->dateTimeStr(),
                                    ui->tableView,
                                    this);

    dialog.setWindowTitle(MESSAGE_TITLE_VOID);
    transparentCenterDialog(&dialog).exec();
}

void w_rpt_report_servantpromotquery::toexport()
{
    w_rpt_report_dataExport dialog(ui->tableView->tableView, ui->tableView->lineView, this);
    dialog.setWindowTitle(this->windowTitle());
    dialog.defFilepath(this->windowTitle());
    transparentCenterDialog(&dialog).exec();
}

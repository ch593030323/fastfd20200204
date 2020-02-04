#include "w_m_member_report_detail_dialog.h"
#include "ui_w_m_member_report_detail_dialog.h"
#include <QTimer>
#include "lds_query_hddpos.h"
#include "backheader.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_m_member.h"

w_m_member_report_detail_Dialog::w_m_member_report_detail_Dialog(const QString &vch_memberno, const QDateTime &dtFrom, const QDateTime &dtFo, QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui_w_m_member_report_detail_Dialog;

    this->vch_memberno = vch_memberno;
    this->dtFrom = dtFrom;
    this->dtFo = dtFo;
    //
    ui->setupUi(this);
    lds_query_hddpos  query;
    query.execSelect(backheader::SELECT_OPERID_NAME);
    while(query.next()) {
        ui->comboBox_vch_operID->addItem(query.recordValue(1).toString(), query.recordValue(0));
    }
    ui->comboBox_vch_operID->addItem(QObject::tr("所有"), "");
    //
    ui->label_desc->setText(dtFrom.toString(yyyyMMddhhmmss) + ">>"+dtFo.toString(yyyyMMddhhmmss));

    //
    querymodel = new report_querymodel(this);
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(querymodel);

    connect(ui->comboBox_vch_operID, SIGNAL(currentIndexChanged(int)), this, SLOT(torefresh()));
    QTimer::singleShot(10, this, SLOT(torefresh()));
}

w_m_member_report_detail_Dialog::~w_m_member_report_detail_Dialog()
{
    delete ui;
}

void w_m_member_report_detail_Dialog::torefresh()
{
    querymodel->setQuery(QString( " select dt_operdate, "//as `" + QObject::tr("时间")+"`," +
                                  " ch_type , "//as `" + QObject::tr("充值类型")+"`,"+
                                  " ch_pay_mode , "//as `" + QObject::tr("支付方式")+"`,"+
                                  " num_pay , "//as `" + QObject::tr("付款金额")+"`,"+
                                  " num_deposit , "//as `" + QObject::tr("充值金额")+"`,"+
                                  " num_realamount , "//as `" + QObject::tr("实收金额")+"`,"+
                                  " num_deposit-num_realamount as num_free, "//as `" + QObject::tr("优惠")+"`,"+
                                  " vch_operid , "//as `" + QObject::tr("操作员")+"`,"+
                                  " vch_memo "//as `" + QObject::tr("备注")+"`"+
                                  " from ("
                                  " Select"
                                  " a.dt_operdate as dt_operdate,"
                                  " a.ch_deposit_mode as ch_type,"
                                  " a.ch_pay_mode as ch_pay_mode,"
                                  " ifnull( case when a.ch_deposit_mode = '4'"
                                  " then 0 else a.num_deposit end, 0) as num_deposit,"
                                  " a.num_realamount as num_realamount,"
                                  " a.vch_memo as vch_memo,"
                                  " a.vch_operid as vch_operid,"
                                  " 0 as num_pay"
                                  " From t_m_deposit a, t_m_member b"
                                  " Where a.vch_memberno = b.vch_memberno and a.vch_memberno = '%1' and"
                                  " a.dt_operdate >= '%2' and a.dt_operdate <= '%3'  and "
                                  "  (a.vch_operID = '%4' or a.vch_operID like '%4%')"
                                  " union all"
                                  " Select"
                                  " a.dt_operdate as dt_operdate,"
                                  "  '9' as ch_type,"
                                  "  '0' as ch_pay_mode,"
                                  "   0 as num_deposit,"
                                  "   0 as num_realamount,"
                                  "  '' as vch_memo,"
                                  "   a.vch_operid as vch_operid,"
                                  "   a.num_pay as num_pay"
                                  "    From t_m_pay a, t_m_member b"
                                  "   Where a.vch_memberno = b.vch_memberno and a.vch_memberno = '%1' and"
                                  "   a.dt_operdate >= '%2' and a.dt_operdate <= '%3'  and "
                                  "    (a.vch_operID = '%4' or a.vch_operID like '%4%')" //'',  000, 0000
                                  "   ) t")
                         .arg(vch_memberno)
                         .arg(dtFrom.toString(yyyyMMddhhmmss))
                         .arg(dtFo.toString(yyyyMMddhhmmss))
                         .arg(ui->comboBox_vch_operID->itemData(ui->comboBox_vch_operID->currentIndex()).toString())
                         );
    if(0 == ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("ch_type"))) {
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_type"), new lds_model_sqltablemodel_delegate_com(ui->tableView,
                                                                                            &w_m_member::get_RECHARGE_MODEL_CH_DEPOSIT_MODE()));
    }
    if(0 == ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("ch_pay_mode"))) {
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_pay_mode"), new lds_model_sqltablemodel_delegate_com(ui->tableView,
                                                                                            &w_m_member::get_RECHARGE_MODEL_PAYMODE()));
    }
    if(0 == ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("vch_operID"))) {
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("vch_operID"), new lds_model_sqltablemodel_delegate_com(ui->tableView,
                                                                                            ui->comboBox_vch_operID));
    }

    ui->tableView->resizeColumnsToContents();
}

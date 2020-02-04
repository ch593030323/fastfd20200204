#include "w_rpt_report_frame_member.h"
#include "ui_w_rpt_report_frame_member.h"
#include "ui_w_rpt_report_frame.h"
#include "n_func.h"
#include "backheader.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QTableWidget>
#include <QScrollBar>
#include "w_m_member.h"
#include "w_m_member_list_select.h"
#include "ui_w_m_member_report_detail_dialog.h"
#include "lds_pushbuttonlock.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_m_member_report_detail_dialog.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "fexpandmain_delegate_vch_printmemo.h"
//num_dish_total
//num_dish_real_total
//num_price_add
//num_total
w_rpt_report_frame_member::w_rpt_report_frame_member(QWidget *parent) :
    w_rpt_report_frame(
        QStringList()
        << QObject::tr("会员充值付款汇总")
        << QObject::tr("会员充值付款明细")
        << QObject::tr("会员余额积分情况")
        << QObject::tr("会员商品消费汇总")
        << QObject::tr("会员商品消费明细")
        << QObject::tr("会员保本金额")

        << QObject::tr("会员卡量统计")
        << QObject::tr("会员挂账"), parent)
{
    //
    ui_help = new Ui::w_rpt_report_frame_member;
    ui_help->setupUi(ui->frame_other);
    ui->widget->setup(QStringList() << QObject::tr("查询")<< QObject::tr("导出")<< QObject::tr("打印")<< QObject::tr("退出"));
    resize(lds::MAIN_WINDOW_SIZE);
    ///
    int idx = 0;
    //0会员充值付款汇总
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label_2, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit_2, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_state, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_state, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_typeno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_typeno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->lineEdit_vch_memberno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->pushButton_vch_memberno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_dish_type, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_dish_type, 0);
    //1会员充值付款明细
    idx++;
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label_2, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit_2, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_state, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_state, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_typeno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_typeno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->lineEdit_vch_memberno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->pushButton_vch_memberno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_vch_operid, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_vch_operid, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_dish_type, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_dish_type, 0);
    //2会员余额积分情况
    idx++;
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label_2, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit_2, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_state, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_state, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_typeno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_typeno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->lineEdit_vch_memberno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->pushButton_vch_memberno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_dish_type, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_dish_type, 0);
    //3会员商品消费汇总
    idx++;
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label_2, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit_2, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_state, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_state, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_typeno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_typeno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->lineEdit_vch_memberno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->pushButton_vch_memberno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_dish_type, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_dish_type, 1);
    //4会员商品消费明细
    idx++;
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label_2, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit_2, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_state, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_state, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_typeno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_typeno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->lineEdit_vch_memberno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->pushButton_vch_memberno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_dish_type, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_dish_type, 0);
    //5会员保本金额
    idx++;
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label_2, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit_2, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_state, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_state, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_typeno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_typeno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->lineEdit_vch_memberno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->pushButton_vch_memberno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_dish_type, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_dish_type, 0);
    //6会员卡量统计
    idx++;
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label_2, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit_2, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_state, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_state, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_typeno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_typeno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->lineEdit_vch_memberno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->pushButton_vch_memberno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_dish_type, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_dish_type, 0);
    //7会员挂账
    idx++;
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->label_2, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui->dateTimeEdit_2, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_state, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_state, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_typeno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_typeno, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->lineEdit_vch_memberno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->pushButton_vch_memberno, 1);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_vch_operid, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->label_ch_dish_type, 0);
    ui->pushButton_step->insert_map_widget_visiable(idx, ui_help->comboBox_ch_dish_type, 0);

    ui->pushButton_step->update_map_widget_visiable(0);
    ///~
    ui_help->comboBox_ch_state->addItem(QObject::tr("全部"), "%");
    ui_help->comboBox_ch_state->addItemsByMap(w_m_member::get_VIP_MODEL_STATE());
    ui_help->comboBox_ch_typeno->addItem(QObject::tr("全部"), "%");
    ui_help->comboBox_ch_typeno->addItemsBySql("select ch_typeno, vch_typename from t_m_member_type ");
    ui_help->comboBox_ch_dish_type->addItem(QObject::tr("全部"), "%");
    ui_help->comboBox_ch_dish_type->addItemsBySql("SELECT ch_dish_typeno, vch_dish_typename FROM cey_bt_dish_type; ");
    ui_help->comboBox_vch_operid->addItemsBySql(backheader::SELECT_OPERID_NAME);
    //
    connect(ui_help->pushButton_vch_memberno, SIGNAL(clicked()),this,SLOT(toget_m()));
    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()),this,SLOT(toprint()));
}

w_rpt_report_frame_member::~w_rpt_report_frame_member()
{
    delete ui_help;
}

void w_rpt_report_frame_member::toget_m()
{
    w_m_member_list_select dialog(this);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        ui_help->lineEdit_vch_memberno->setText(dialog.ret_m);
    }
}

void w_rpt_report_frame_member::toselect()
{
    lds_pushbuttonlock b(ui->widget->index_widget(QObject::tr("查询")));
    b.toLock();

    lds_query_hddpos  query;
    int index = ui->stackedWidget->currentIndex();
    QTableView *tableview = static_cast<QTableView *>(ui->stackedWidget->widget(index));
    report_querymodel *model = static_cast<report_querymodel *>(tableview->model());
    //
    static int onepagestep = -1;
    if(onepagestep == -1) {
        QTableWidget t;
        t.setColumnCount(1);
        onepagestep = ( tableview->height() - tableview->horizontalScrollBar()->height()- t.horizontalHeader()->height() ) / tableview->verticalHeader()->defaultSectionSize();
    }
    //
    ui->pushButton_step->init(index);
    switch(index) {
    case 0:///会员充值付款汇总报表
    {
        //
        QString sql_where =
                " FROM "
                "     t_m_member, "
                "     t_m_member_type "
                " WHERE "
                "     (t_m_member.ch_typeno = t_m_member_type.ch_typeno) "
                "         and (t_m_member_type.ch_attribute = '2') "
                "         AND (t_m_member.vch_memberno like '%3%' "
                "         or t_m_member.vch_member like '%3%') "
                "         and t_m_member.ch_state like '%4' "
                ;
        query.execSelect(QString("select count(0) "+sql_where)
                         //开始时间、结束时间、会员3、卡状态4
                         .arg(ui_help->lineEdit_vch_memberno->text())
                         .arg(ui_help->comboBox_ch_state->itemData(ui_help->comboBox_ch_state->currentIndex()).toString())
                         );
        query.next();
        int rowcount = query.recordValue(0).toInt();
        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  QString(" SELECT  "
                                          "     t_m_member.vch_memberno,  "
                                          "     t_m_member.vch_member,  "
                                          "     t_m_member_type.vch_typename as m_typename, "
                                          "     t_m_member.ch_state,  "
                                          "     (select  "
                                          "             ifnull(sum(case "
                                          "                             when t_m_deposit.ch_deposit_mode = '4' then 0 "
                                          "                             else t_m_deposit.num_deposit "
                                          "                         end), "
                                          "                         0) "
                                          "         from "
                                          "             t_m_deposit "
                                          "         where "
                                          "             t_m_deposit.vch_memberno = t_m_member.vch_memberno "
                                          "                 and t_m_deposit.dt_operdate between '%1' and '%2') as m_deposit, "
                                          "     (select  "
                                          "             ifnull(sum(t_m_deposit.num_realamount), 0) "
                                          "         from "
                                          "             t_m_deposit "
                                          "         where "
                                          "             t_m_deposit.vch_memberno = t_m_member.vch_memberno "
                                          "                 and t_m_deposit.dt_operdate between '%1' and '%2') as m_deposit_real, "
                                          "     (select  "
                                          "             sum(case  when "
                                          "                         ifnull(case "
                                          "                                     when t_m_deposit.ch_deposit_mode = '4' then 0 "
                                          "                                     else t_m_deposit.num_deposit "
                                          "                                 end, "
                                          "                                 0) - ifnull(t_m_deposit.num_realamount, 0) > 0 "
                                          "                     then "
                                          "                         ifnull(case "
                                          "                                     when t_m_deposit.ch_deposit_mode = '4' then 0 "
                                          "                                     else t_m_deposit.num_deposit "
                                          "                                 end, "
                                          "                                 0) - ifnull(t_m_deposit.num_realamount, 0) "
                                          "                     else 0 "
                                          "                 end) "
                                          "         from "
                                          "             t_m_deposit "
                                          "         where "
                                          "             t_m_deposit.vch_memberno = t_m_member.vch_memberno "
                                          "                 and t_m_deposit.dt_operdate between '%1' and '%2') as m_deposit_discount, "
                                          "     (select  "
                                          "             ifnull(sum(num_pay), 0) "
                                          "         from "
                                          "             t_m_pay "
                                          "         where "
                                          "             t_m_pay.vch_memberno = t_m_member.vch_memberno "
                                          "                 and t_m_pay.dt_operdate between '%1' and '%2') as num_pay "
                                          + sql_where )
                                  //开始时间、结束时间、会员、卡状态
                                  .arg(ui->dateTimeEdit->dateTimeStr())
                                  .arg(ui->dateTimeEdit_2->dateTimeStr())
                                  .arg(ui_help->lineEdit_vch_memberno->text())
                                  .arg(ui_help->comboBox_ch_state->itemData(ui_help->comboBox_ch_state->currentIndex()).toString())
                                  );
        //
        toselect_page();
        if(tableview->itemDelegateForColumn(model->fieldIndex("ch_state")) == 0) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_state"), new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_STATE()));
        }
        //
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "m_deposit");
        ui->pushButton_step->sum_append(index,  "m_deposit_real");
        ui->pushButton_step->sum_append(index,  "m_deposit_discount");
        ui->pushButton_step->sum_append(index,  "num_pay");
        ui->pushButton_step->sum_genrate(index);
    }
        break;///~会员充值付款汇总报表
    case 1:///1会员充值付款明细
    {
        QString sqlarg =
                "   select "
                "   b.vch_memberno,"
                "   b.vch_member,"
                "   a.dt_operdate,"
                "   a.ch_deposit_mode as ch_type,"
                "   a.ch_pay_mode, "
                "   0 as num_pay,"
                "   a.num_deposit, "
                "   a.num_realamount, "
                "   a.num_deposit - a.num_realamount as num_free,"
                "   a.vch_operid, "
                "  a.vch_memo "
                "   From"
                "   t_m_deposit a, t_m_member b"
                "   Where"
                "   a.vch_memberno = b.vch_memberno"
                "   and a.vch_memberno like '%1%'"
                "   and a.dt_operdate >= '%2'"
                "   and a.dt_operdate <= '%3'"
                "   and a.vch_operID = '%4' "
                "   union all "
                "   select "
                "   b.vch_memberno,"
                "   b.vch_member,"
                "   a.dt_operdate as dt_operdate,"
                "  '9' as ch_type,"
                "  '0' as ch_pay_mode,"
                "   a.num_pay as num_pay,"
                "   0 as num_deposit,"
                "   0 as num_realamount,"
                "   0 as num_free,"
                "   a.vch_operid as vch_operid,"
                "  '' as vch_memo"
                "   From"
                "   t_m_pay a, t_m_member b"
                "   Where"
                "   a.vch_memberno = b.vch_memberno"
                "   and a.vch_memberno like '%1%' "
                "   and a.dt_operdate >= '%2'"
                "   and a.dt_operdate <= '%3'"
                "   and a.vch_operID = '%4' "
                ;
        sqlarg = sqlarg
                .arg(ui_help->lineEdit_vch_memberno->text())
                .arg(ui->dateTimeEdit->dateTimeStr())
                .arg(ui->dateTimeEdit_2->dateTimeStr())
                .arg(ui_help->comboBox_vch_operid->itemData(ui_help->comboBox_vch_operid->currentIndex()).toString())
                ;
        model->setQuery(sqlarg);
        if(tableview->itemDelegateForColumn(model->fieldIndex("ch_type")) == 0) {
              tableview->setItemDelegateForColumn(model->fieldIndex("ch_type"), new lds_model_sqltablemodel_delegate_com(  tableview,
                                                                                                &w_m_member::get_RECHARGE_MODEL_CH_DEPOSIT_MODE()));
        }
        if(tableview->itemDelegateForColumn(model->fieldIndex("ch_pay_mode")) == 0) {
              tableview->setItemDelegateForColumn(model->fieldIndex("ch_pay_mode"), new lds_model_sqltablemodel_delegate_com(  tableview,
                                                                                                &w_m_member::get_RECHARGE_MODEL_PAYMODE()));
        }
        if(tableview->itemDelegateForColumn(model->fieldIndex("vch_operID")) == 0) {
              tableview->setItemDelegateForColumn(model->fieldIndex("vch_operID"), new lds_model_sqltablemodel_delegate_com(  tableview,
                                                                                                ui_help->comboBox_vch_operid));
        }
        ui->pushButton_step->init(index, 0, onepagestep, sqlarg);
        //
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "num_pay");
        ui->pushButton_step->sum_append(index,  "num_deposit");
        ui->pushButton_step->sum_append(index,  "num_realamount");
        ui->pushButton_step->sum_append(index,  "num_free");
        ui->pushButton_step->sum_genrate(index);
    }
        break;///~1会员充值付款明细
    case 2:///会员余额积分情况
    {
        //
        QString sql_where =
                " From "
                "     t_m_member a, "
                "     t_m_member_type b, "
                "     t_m_curamount c "
                " Where "
                "     a.ch_typeno = b.ch_typeno "
                "         and a.vch_memberno = c.vch_memberno "
                "         and a.ch_typeno like '%1' "
                "         and a.vch_memberno like '%2%' "
                "         and a.ch_state like '%3' "
                ;
        query.execSelect(QString("select count(0) "+sql_where)
                         //卡类型、卡号、卡状态
                         .arg(ui_help->comboBox_ch_typeno->itemData(ui_help->comboBox_ch_typeno->currentIndex()).toString())
                         .arg(ui_help->lineEdit_vch_memberno->text())
                         .arg(ui_help->comboBox_ch_state->itemData(ui_help->comboBox_ch_state->currentIndex()).toString())
                         );
        query.next();
        int rowcount = query.recordValue(0).toInt();
        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  QString(
                                      " Select  "
                                      "     a.vch_memberno,  "
                                      "     a.vch_member,  "
                                      "     b.vch_typename as m_typename, "
                                      "     a.ch_state,  "
                                      "     ifnull(c.num_amount, 0) as num_remain, "
                                      "     ifnull(c.num_point, 0) as num_point, "
                                      "     c.dt_operdate,  "
                                      "     vch_handtel,  "
                                      "     b.ch_attribute "
                                      + sql_where )
                                  //卡类型、卡号、卡状态
                                  .arg(ui_help->comboBox_ch_typeno->itemData(ui_help->comboBox_ch_typeno->currentIndex()).toString())
                                  .arg(ui_help->lineEdit_vch_memberno->text())
                                  .arg(ui_help->comboBox_ch_state->itemData(ui_help->comboBox_ch_state->currentIndex()).toString())
                                  );
        //
        toselect_page();
        if(tableview->itemDelegateForColumn(model->fieldIndex("ch_state")) == 0) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_state"), new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_STATE()));
        }
        if(tableview->itemDelegateForColumn(model->fieldIndex("ch_attribute")) == 0) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_attribute"), new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_ATTRI()));
        }
        //
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "num_remain");
        ui->pushButton_step->sum_append(index,  "num_point");
        ui->pushButton_step->sum_genrate(index);
    }
        break;///~会员余额积分情况
    case 3:///会员商品消费汇总
    {
        //
        QString sql_where =
                " FROM "
                "     cey_u_orderdish a, "
                "     cey_u_checkout_master b, "
                "     cey_bt_dish c, "
                "     t_m_member_type t, "
                "     t_m_member m "
                " WHERE "
                "     (a.ch_payno = b.ch_payno) "
                "         and (b.ch_state = 'Y') "
                "         and (a.ch_dishno = c.ch_dishno) "
                "         and (a.num_num - a.num_back <> 0) "
                "         and m.vch_memberno = b.vch_memberno "
                "         and m.ch_typeno = t.ch_typeno "
                "         and b.dt_operdate >= '%1' "
                "         and b.dt_operdate <= '%2' "
                "         and (m.vch_memberno like '%3%' "
                "         or m.vch_member like '%3%') "
                "         and c.ch_dish_typeno like '%4' "
                " Group by a.ch_dishno "
                ;

        sql_where = sql_where
                //开始时间、结束时间、卡号
                .arg(ui->dateTimeEdit->dateTimeStr())
                .arg(ui->dateTimeEdit_2->dateTimeStr())
                .arg(ui_help->lineEdit_vch_memberno->text())
                .arg(ui_help->comboBox_ch_dish_type->curusrdata().toString());
        query.execSelect(sql_count_arg1.arg(sql_where));
        query.next();
        qDebug() << query.lastQuery();
        int rowcount = query.recordValue(0).toInt();
        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  QString(
                                      " SELECT  "
                                      "     a.ch_dishno, "
                                      "     c.vch_dishname,  "
                                      "     c.ch_unitno,  "
                                      "     c.ch_dish_typeno,  "
                                      "     sum(a.num_num - a.num_back) as num_dish, "
                                      "     sum( ((a.num_num - a.num_back) * a.num_price) ) as num_total, "
                                      "     sum( if(a.ch_presentflag = 'Y', (a.num_num-a.num_back)*a.num_price, 0)) as num_present, "
                                      "     sum( (a.num_num-a.num_back)*a.num_price * (1 - a.int_discount * 0.01 ) ) as num_discount, "
                                      "     sum( if(a.ch_presentflag = 'Y', 0, (a.num_num-a.num_back)*a.num_price * (a.int_discount * 0.01 ) )) as num_total2, "
                                      "     sum( num_price_add ) as num_price_add, "
                                      "     sum( if(a.ch_presentflag = 'Y', 0, (a.num_num-a.num_back)*a.num_price * (a.int_discount * 0.01 )+num_price_add )) as num_total3 "
                                      + sql_where )
                                  );
        //
        toselect_page();
        if(tableview->itemDelegateForColumn(model->fieldIndex("ch_unitno")) == 0) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_unitno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_unit", "ch_unitno", "vch_unitname"));
        }
        if(tableview->itemDelegateForColumn(model->fieldIndex("ch_dish_typeno")) == 0) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_dish_typeno"), new lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish_type", "ch_dish_typeno", "vch_dish_typename"));
        }
        //
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "num_dish");
        ui->pushButton_step->sum_append(index,  "num_total");
        ui->pushButton_step->sum_append(index,  "num_present");
        ui->pushButton_step->sum_append(index,  "num_discount");
        ui->pushButton_step->sum_append(index,  "num_total2");
        ui->pushButton_step->sum_append(index,  "num_price_add");
        ui->pushButton_step->sum_append(index,  "num_total3");
        ui->pushButton_step->sum_genrate(index);
    }
        break;///~会员商品消费汇总
    case 4:///会员商品消费明细
    {
        //
        QString sql_where =
                " FROM "
                "     cey_u_orderdish a, "
                "     cey_u_checkout_master b, "
                "     cey_bt_dish c, "
                "     t_m_member_type t, "
                "     t_m_member m "
                " WHERE "
                "     (a.ch_payno = b.ch_payno) "
                "         and (b.ch_state = 'Y') "
                "         and (a.ch_dishno = c.ch_dishno) "
                "         and (a.num_num - a.num_back <> 0) "
                "         and m.vch_memberno = b.vch_memberno "
                "         and m.ch_typeno = t.ch_typeno "
                "         and b.dt_operdate >= '%1' "
                "         and b.dt_operdate <= '%2' "
                "         and (m.vch_memberno like '%3%' "
                "         or m.vch_member like '%3%') "
                ;
        sql_where = sql_where
                //开始时间、结束时间、卡号
                .arg(ui->dateTimeEdit->dateTimeStr())
                .arg(ui->dateTimeEdit_2->dateTimeStr())
                .arg(ui_help->lineEdit_vch_memberno->text())
                ;
        query.execSelect(sql_count_arg1.arg(sql_where));
        query.next();
        int rowcount = query.recordValue(0).toInt();
        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  QString(
                                      " Select  "
                                      "     t.vch_typename as m_typename, "
                                      "     b.vch_memberno,  "
                                      "     a.ch_dishno,  "
                                      "     c.vch_dishname,  "
                                      "     (a.num_num - a.num_back) as num_dish, "
                                      "     a.num_price,  "
                                      "     ( ((a.num_num - a.num_back) * a.num_price) ) as num_total, "
                                      "     ( if(a.ch_presentflag = 'Y', (a.num_num-a.num_back)*a.num_price, 0)) as num_present, "
                                      "     ( (a.num_num-a.num_back)*a.num_price * (1 - a.int_discount * 0.01 ) ) as num_discount, "
                                      "     ( if(a.ch_presentflag = 'Y', 0, (a.num_num-a.num_back)*a.num_price * (a.int_discount * 0.01 ) )) as num_total2, "
                                      "      a.vch_print_memo ,  "
                                      "      a.num_price_add, "
                                      "     ( a.num_price_add + if(a.ch_presentflag = 'Y', 0, (a.num_num-a.num_back)*a.num_price * (a.int_discount * 0.01 ) )) num_total3, "
                                      "     a.dt_operdate,  "
                                      "     a.ch_payno "
                                      + sql_where )
                                  );
        //
        toselect_page();
        if(tableview->itemDelegateForColumn(model->fieldIndex("vch_print_memo")) == 0) {
            tableview->setItemDelegateForColumn(model->fieldIndex("vch_print_memo"), new fexpandmain_delegate_vch_printmemo(this));
        }
        //
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "num_dish");
        ui->pushButton_step->sum_append(index,  "num_total");
        ui->pushButton_step->sum_append(index,  "num_discount");
        ui->pushButton_step->sum_append(index,  "num_present");
        ui->pushButton_step->sum_append(index,  "num_total2");

        ui->pushButton_step->sum_append(index,  "num_price_add");
        ui->pushButton_step->sum_append(index,  "num_total3");

        ui->pushButton_step->sum_genrate(index);
    }
        break;///~会员商品消费明细
    case 5:///会员保本金额
    {
        //
        QString sql_where =
                " From "
                "     t_m_member a, "
                "     t_m_member_type b "
                " Where "
                "     a.ch_typeno = b.ch_typeno "
                "         and b.ch_attribute <> '1' "
                "         and a.ch_typeno like '%1' "
                "         and a.ch_state like '%2' "
                ;
        sql_where = sql_where
                //开始时间、结束时间、卡号
                .arg(ui_help->comboBox_ch_typeno->itemData(ui_help->comboBox_ch_typeno->currentIndex()).toString())
                .arg(ui_help->comboBox_ch_state->itemData(ui_help->comboBox_ch_state->currentIndex()).toString())
                ;
        query.execSelect(sql_count_arg1.arg(sql_where));
        query.next();
        int rowcount = query.recordValue(0).toInt();
        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  QString(
                                      " Select  "
                                      "     a.vch_memberno,  "
                                      "     a.vch_member,  "
                                      "     b.vch_typename as m_typename, "
                                      "     b.ch_attribute, "
                                      "     vch_handtel,  "
                                      "     a.ch_state,  "
                                      "     a.ch_cardflag,  "
                                      "     a.int_basedata "
                                      + sql_where )
                                  );
        //
        toselect_page();
        if(tableview->itemDelegateForColumn(model->fieldIndex("ch_state")) == 0) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_state"), new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_STATE()));
        }
        if(tableview->itemDelegateForColumn(model->fieldIndex("ch_attribute")) == 0) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_attribute"), new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_ATTRI()));
        }
        if(tableview->itemDelegateForColumn(model->fieldIndex("ch_cardflag")) == 0) {
            tableview->setItemDelegateForColumn(model->fieldIndex("ch_cardflag"), new lds_model_sqltablemodel_delegate_com(this, &w_m_member::get_VIP_MODEL_FLAG()));
        }
        //
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "int_basedata");
        ui->pushButton_step->sum_genrate(index);
    }
        break;///~会员保本金额

    case 6:///会员卡量统计
    {
        //
        QString sql_where =
                "     FROM "
                "         t_m_member "
                "     where "
                "         ch_typeno like '%1' "
                "     GROUP by ch_typeno ";
        //卡类型
        sql_where = sql_where
                .arg(ui_help->comboBox_ch_typeno->itemData(ui_help->comboBox_ch_typeno->currentIndex()).toString())
                ;
        query.execSelect(sql_count_arg1.arg(sql_where));
        query.next();
        int rowcount = query.recordValue(0).toInt();
        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  QString(
                                      " SELECT  "
                                      "         ch_typeno as m_typename, "
                                      "             count(vch_memberno) as m_cardcount, "
                                      "             sum(case ch_state "
                                      "                 when "
                                      "                     '1' "
                                      "                 then "
                                      "                     case "
                                      "                         when ch_cardflag = 'N' then 1 "
                                      "                         else 0 "
                                      "                     end "
                                      "                 else 0 "
                                      "             end) as m_unsendcardcount, "
                                      "             sum(case ch_state "
                                      "                 when "
                                      "                     '1' "
                                      "                 then "
                                      "                     case "
                                      "                         when ch_cardflag = 'Y' then 1 "
                                      "                         else 0 "
                                      "                     end "
                                      "                 else 0 "
                                      "             end) as m_sendcardcount, "
                                      "             sum(case ch_state "
                                      "                 when '2' then 1 "
                                      "                 else 0 "
                                      "             end) as m_stopcardcount, "
                                      "             sum(case ch_state "
                                      "                 when '3' then 1 "
                                      "                 else 0 "
                                      "             end) as m_hangcardcount, "
                                      "             sum(case ch_state "
                                      "                 when '4' then 1 "
                                      "                 else 0 "
                                      "             end) as m_badcardcount"
                                      + sql_where )
                                  );
        //
        toselect_page();
        if(tableview->itemDelegateForColumn(model->fieldIndex("m_typename")) == 0) {
            tableview->setItemDelegateForColumn(model->fieldIndex("m_typename"), new lds_model_sqltablemodel_delegate_com(this, "t_m_member_type", "ch_typeno", "vch_typename"));
        }
    }
        break;///~会员卡量统计
    case 7:///会员挂账
    {
        //
        QString sql_where =
                " from "
                "     t_m_hang_balance a "
                "         LEFT JOIN "
                "     t_m_member b ON a.vch_memberno = b.vch_memberno "
                " where "
                "     b.vch_memberno like '%1%' ";
        sql_where = sql_where
                //卡号
                .arg(ui_help->lineEdit_vch_memberno->text())
                ;
        query.execSelect(sql_count_arg1.arg(sql_where));
        query.next();
        int rowcount = query.recordValue(0).toInt();
        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  QString(
                                      " select  "
                                      "     a.vch_memberno,  "
                                      "     b.vch_member,  "
                                      "     b.vch_handtel,  "
                                      "     a.num_remain,  "
                                      "     a.num_pay,  "
                                      "     a.vch_operID,  "
                                      "     a.dt_operdate,  "
                                      "     a.vch_memo,  "
                                      "     a.ch_state, "
                                      "     a.ch_balanceno "
                                      + sql_where )
                                  );
        //
        toselect_page();
        //
        ui->pushButton_step->sum_clear(index);
        ui->pushButton_step->sum_append(index,  "num_remain");
        ui->pushButton_step->sum_genrate(index);
    }
        break;///~会员挂账
    }

    sum_filldata(ui->pushButton_step->get_sum_list_total_data(index));
    resize_column_to_content_t();
}

void w_rpt_report_frame_member::todetail()
{
    int index = ui->stackedWidget->currentIndex();
    QTableView *tableview = static_cast<QTableView *>(ui->stackedWidget->widget(index));
    QStandardItemModel *model = static_cast<QStandardItemModel *>(tableview->model());
    //

    int row = tableview->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }
    //
    w_m_member_report_detail_Dialog dialog(model->data(model->index(row, 0)).toString(),
                                           ui->dateTimeEdit->dateTime(),
                                           ui->dateTimeEdit_2->dateTime()
                                           );
    transparentCenterDialog(&dialog).exec();
}

void w_rpt_report_frame_member::toprint()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());

    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));

    w_rpt_report_printFormat *dialog
            = new w_rpt_report_printFormat(w_rpt_report_printFormat::show_model_quick, this->windowTitle(), lds::gs_operid, n_func::f_get_sysdatetime_str(),
                                    QString("%1_%2").arg(this->metaObject()->className()).arg(index),
                                    ui->dateTimeEdit->dateTimeStr(), ui->dateTimeEdit_2->dateTimeStr(),
                                    tableview, ui->pushButton_step->get_limit_sql(index), ui->tableView_sum,
                                    0);
    dialog->pop(b, this);
}

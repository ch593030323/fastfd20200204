#include "w_scr_dish_pay_widget.h"
#include "ui_w_scr_dish_pay_widget.h"
#include "lds_query_hddpos.h"
#include "btnlanguageswicth.h"
#include <QSqlError>
#include "lds.h"
#include "n_func.h"
#include "n_func.h"
#include <QKeyEvent>
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include <QProcess>
#include <QHostInfo>
#include "w_bt_dish.h"
#include "free_view.h"
#include "bank_card.h"
#include "w_m_member_list.h"
#include "w_bt_dish_paymode.h"
#include "lds_menu.h"
#include <QWidgetAction>
#include "w_scr_balance_service.h"
#include "w_scr_dish_m_hang_dialog.h"
#include "w_scr_dish_m_hang_pay_dialog.h"
#include "power_oper.h"
#include "w_scr_dish_member.h"
#include "ui_w_scr_dish_member.h"
#include "w_scr_dish.h"
#include "public_printby_ch_billno.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "ftableview_delegate.h"
#include "ftableview_standmodel_sql_none.h"
#include "public_short_cut_map.h"
#include "w_scr_dish_pay_set.h"
#include "mobile_pay_help.h"
#include "business_data_upload_isoft.h"
#include "public_logindialog.h"
#include "w_scr_dish_pay_module_select.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "lds.h"
#include "w_scr_dish_pay_select_language.h"
#include "w_scr_dish_main.h"
#include "w_bt_dish_kitchenprinter_printerset.h"
#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include "fastfd_update.h"
#include "w_scr_dish_phone_pay.h"
#include "w_scr_dish_pay_set_ogg.h"
#include "w_sys_manage_outer_pay_set_jingquan.h"
#include "lds_scr_table.h"
#include "w_sys_manage_outer_pay_set_fengze.h"
#include "public_pushbuton_rt_check.h"
#include "fupay_help.h"
#include "w_sys_manage_outer_pay_set_locale_wizard.h"
#include "w_i_inventory_provider.h"
#include "business_data_upload_restful.h"
#include "business_interface_malayisa_hourly_pos.h"
#include "w_bt_dish_signal_transfer.h"
#include "w_scr_dish_pay_widget_service_manual.h"
//cey_u_master
//cey_u_checkout_master
//cey_u_service_low
//cey_u_table
//cey_u_orderdish
//cey_u_checkout_detail
//会员卡：t_m_curamount， t_m_point
//cey_u_checkout_master
//会员挂账：t_m_curamount，t_m_hang_balance


bool w_scr_dish_pay_widget::pay_model_data_list_find(const QList<w_scr_dish_pay_widget::payModelData> &pay_model_data_list, const QString &vch_voucherno)
{
    for(int k = 0; k < pay_model_data_list.count(); k++) {
        if(pay_model_data_list[k].vch_voucherno == vch_voucherno) {
            return true;
        }
    }
    return false;
}

double w_scr_dish_pay_widget::valuerountbysys(double value)
{
    fexpandmain_model_sqltablemodel_data m;
    return m.roundBySystem(value);
}

bool w_scr_dish_pay_widget::cey_u_orderdish_empty_pay_transaction(const QString &ch_billno, QString *errstring)
{
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT sum(num_num - num_back) from cey_u_orderdish where ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    if(query.recordValue(0).toInt() > 0) {
        if(errstring) *errstring = QObject::tr("本单数量不为0") + QObject::tr("操作失败");
        return false;
    }

    QString ch_tableno;
    query.execSelect(QString("SELECT ch_tableno from cey_u_table where ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    ch_tableno = query.recordValue(0).toString();

    lds_query_hddpos::tran_saction();
    QString ch_payno = lds_scr_table::get_max_ch_payno_add1();
    //付款
    if(query.execInsert("cey_u_checkout_master",qrtVariantPairList()

                        << qrtVariantPair("ch_billno", ch_billno)
                        << qrtVariantPair("ch_tableno", ch_tableno)
                        << qrtVariantPair("num_cost", 0)
                        << qrtVariantPair("num_discount", 0)
                        << qrtVariantPair("num_present", 0)

                        << qrtVariantPair("num_service", 0)
                        //                        << qrtVariantPair("vch_service_desc", "")
                        << qrtVariantPair("num_lowcost", 0)
                        << qrtVariantPair("num_blotout", 0)
                        << qrtVariantPair("num_run", 0)

                        << qrtVariantPair("vch_run_operID", "")
                        << qrtVariantPair("ch_state", "Y")
                        << qrtVariantPair("vch_operID", lds::gs_operid)
                        << qrtVariantPair("dt_operdate", n_func::f_get_sysdatetime())
                        << qrtVariantPair("vch_operID2", "")

                        << qrtVariantPair("num_rate", 0)
                        << qrtVariantPair("ch_payno", ch_payno)
                        << qrtVariantPair("vch_memberno", "")
                        << qrtVariantPair("ch_member_belong_state", "")
                        << qrtVariantPair("ch_yun_payno", "")

                        << qrtVariantPair("num_cash", 0)

                        )) {
        //餐桌
        if(query.execUpdate("cey_u_table", "ch_payno", ch_payno,qrtEqual("ch_billno",ch_billno))) {
            //商品
            if(query.execUpdate("cey_u_orderdish","ch_payno", ch_payno,qrtEqual("ch_billno",ch_billno))) {

                goto commit;
            }
        }
    }
    if(errstring) *errstring = query.recordError();

    lds_query_hddpos::roll_back();
    return false;
commit:
    lds_query_hddpos::com_mit();
    return true;
}

w_scr_dish_pay_widget::w_scr_dish_pay_widget(QWidget *parent)
    : QFrame(parent)
{
}

w_scr_dish_pay_widget::w_scr_dish_pay_widget(w_scr_dish_pay_widget::operatModes m, QWidget *parent, const fexpandmain_model_sqltablemodel_data *tablemodel)
    : QFrame(parent)
{
    initGeometry(m);
    initdataByM(tablemodel);
}

void w_scr_dish_pay_widget::watchControl()
{
    connect(ui->lineEdit_tax_before, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_tax_cur, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_tax_cur_2, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_tax_cur_3, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_realreceive, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_change, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_moneypay, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_consum, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_present, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_discount, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_int_rate, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_blotout, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_ch_tableno, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_service, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_lowcost, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_vch_memo, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
    connect(ui->lineEdit_int_person, SIGNAL(paintTextChanged()), this,SLOT(toPayMasterWatch()));
}


w_scr_dish_pay_widget::~w_scr_dish_pay_widget()
{
    delete ui;
}

void w_scr_dish_pay_widget::initdataByM(const fexpandmain_model_sqltablemodel_data *tablemodel)
{
    model_orderdish = tablemodel;

    //
    ui->lineEdit_consum->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_present->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());ui->lineEdit_present->enableShowNegativeValue();
    ui->lineEdit_discount->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());ui->lineEdit_discount->enableShowNegativeValue();
    ui->lineEdit_int_rate->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_blotout->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());ui->lineEdit_blotout->enableShowNegativeValue();


    ui->lineEdit_lowcost->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_service->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    //    ui->lineEdit_int_person->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());

    ui->lineEdit_realreceive->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_change->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_moneypay->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());

    ui->lineEdit_tax_before->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_tax_cur->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_tax_cur_2->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    ui->lineEdit_tax_cur_3->setDecimals(tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());

    /////////
    ui->lineEdit_consum->setLabelText(QObject::tr("消费"));// + ":");
    ui->lineEdit_present->setLabelText(QObject::tr("赠送"));// + ":");
    ui->lineEdit_discount->setLabelText(QObject::tr("折扣"));// + ":");
    ui->lineEdit_int_rate->setLabelText(QObject::tr("税收"));// + ":");
    ui->lineEdit_blotout->setLabelText(QObject::tr("抹零"));// + ":");

    ui->lineEdit_ch_tableno->setLabelText(QObject::tr("餐桌"));// + ":");
    ui->lineEdit_service->setLabelText(QObject::tr("服务费"));// + ":");
    ui->lineEdit_lowcost->setLabelText(QObject::tr("低消"));// + ":");
    ui->lineEdit_vch_memo->setLabelText(QObject::tr("备注"));// + ":");
    ui->lineEdit_int_person->setLabelText(QObject::tr("人数"));// + ":");

    ui->lineEdit_change->setLabelText(QObject::tr("找零"));// + ":");
    ui->lineEdit_change->setBgColor(QColor("#ff546b"));
    ui->lineEdit_realreceive->setLabelText(QObject::tr("应收"));// + ":");
    ui->lineEdit_realreceive->setBgColor(QColor("#ff546b"));
    ui->lineEdit_moneypay->setLabelText(QObject::tr("现金"));// + ":");
    ui->lineEdit_moneypay->setBgColor(QColor("green"));
    ui->lineEdit_moneypay->enableSelectAll();

    ui->lineEdit_tax_before->setLabelText(QObject::tr("税前"));// + ":");
    ui->lineEdit_tax_cur->setLabelText(lds_scr_table::int_rate_name(model_orderdish, 1));// + ":");
    ui->lineEdit_tax_cur_2->setLabelText(lds_scr_table::int_rate_name(model_orderdish, 2));// + ":");
    ui->lineEdit_tax_cur_3->setLabelText(lds_scr_table::int_rate_name(model_orderdish, 3));// + ":");

    first_go_in();

    if(pay_operate_mode & OPERAT_QUITBILL) {
        ui->pushButton_pay->setText(QObject::tr("退单"));
        refreshQuitbillControl();
    }
}

void w_scr_dish_pay_widget::initGeometry(w_scr_dish_pay_widget::operatModes m, bool strechLayout)
{
    ui = new Ui::w_scr_dish_pay_widget;
    ui->setupUi(this);
    watchControl();

    pay_operate_mode = m;
    pay_all_discount = -1;
    pay_finished = false;
    //3
    model_paydetail = 0;
    model_orderdish = 0;
    ui->radioButton_payinfo->setChecked1(true);
    ui->radioButton_payinfo->enableLongPress(true);

    if(strechLayout) {
        tostackinfo(ui->radioButton_payinfo);
        resize(lds::MAIN_WINDOW_SIZE.width() - lds::PAY_LEFT_SPACE, lds::MAIN_WINDOW_SIZE.height());

        ui->frame_top->setGeometry(0, 0, this->width(), 30);

        ui->frame_stack0->setGeometry(QRect(0, ui->frame_top->geometry().bottom(), this->width()/2, (this->height() - ui->frame_top->height()) * 2 / 5));
        ui->frame_stack1->setGeometry(QRect(0, ui->frame_top->geometry().bottom(), this->width()/2, (this->height() - ui->frame_top->height()) * 2 / 5));
        ui->frame_stack2->setGeometry(QRect(0, ui->frame_top->geometry().bottom(), this->width()/2, (this->height() - ui->frame_top->height()) * 2 / 5));

        ui->frame_summary->setGeometry(ui->frame_stack0->geometry().right(), ui->frame_stack0->geometry().top(), ui->frame_stack0->width(), ui->frame_stack0->height());
        ui->tableView->setGeometry(ui->frame_summary->geometry().left(), ui->frame_summary->geometry().bottom(), ui->frame_summary->width(), ui->frame_summary->height()/2);

        ui->frame_num_key->setGeometry(0, ui->tableView->geometry().bottom(), ui->frame_summary->width(), ui->frame_summary->height());
        ui->frame_pay_key->setGeometry(ui->frame_num_key->geometry().right(), ui->frame_num_key->geometry().top(), ui->frame_num_key->geometry().width(), ui->frame_num_key->geometry().height());

        connect(ui->lineEdit_moneypay,SIGNAL(valueChanged(double)),this,SLOT(updateChange()));
        connect(ui->lineEdit_realreceive, SIGNAL(valueChanged(double)), this,SLOT(updateDD(double)));
        connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_8,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_10,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_12,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_9,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_14,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_16,SIGNAL(clicked()),this,SLOT(updateAmount()));
        connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(setAmount()));
        connect(ui->pushButton_7,SIGNAL(clicked()),this,SLOT(setAmount()));
        connect(ui->pushButton_clear,SIGNAL(clicked()),this,SLOT(amountClear()));

        connect(ui->radioButton_payinfo,SIGNAL(signalpressed(bool)),this, SLOT(tostackinfo(bool)));
        connect(ui->radioButton_tableinfo,SIGNAL(signalpressed(bool)),this, SLOT(tostackinfo(bool)));
        connect(ui->radioButton_taxinfo,SIGNAL(signalpressed(bool)),this, SLOT(tostackinfo(bool)));

        connect(ui->radioButton_payinfo, SIGNAL(timeout()), this, SLOT(toplayloop()));

        //
        if(pay_operate_mode & OPERAT_PAY) {
            connect(ui->pushButton_pay,SIGNAL(clicked()),this,SLOT(toScrpay()));
        }
        if(pay_operate_mode & OPERAT_QUITBILL) {
            connect(ui->pushButton_pay,SIGNAL(clicked()),this,SLOT(toExitbill()));
        }
        connect(ui->pushButton_pay, SIGNAL(released()),ui->lineEdit_moneypay, SLOT(setFocus()));
        if(pay_operate_mode & TYPE_RESTAURANT) {
            lds::showWidget(ui->radioButton_tableinfo);
        }

        if(pay_operate_mode & TYPE_FAST) {
            lds::hideWidget(ui->radioButton_tableinfo);
        }
        retranslateView();
    }
}


void w_scr_dish_pay_widget::retranslateView()
{
    resetPaySection();
}

void w_scr_dish_pay_widget::initdataPayModel(const QString &ch_payno)
{
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT ch_paymodeno,num_payamount,num_realamount,vch_voucherno,vch_memo, num_card_remain  from cey_u_checkout_detail where ch_payno = '%1' ").arg(ch_payno));
    while(query.next()) {
        payModelAppend(ldsVariantMap()
                       .append("ch_paymodeno", query.recordValue("ch_paymodeno"))
                       .append("num_payamount", query.recordValue("num_payamount"))
                       .append("num_realamount", query.recordValue("num_realamount"))
                       .append("vch_voucherno", query.recordValue("vch_voucherno"))
                       .append("vch_memo", query.recordValue("vch_memo"))
                       );

        updatePaylistCashpay();
    }
}

int w_scr_dish_pay_widget::mread(bool b_always_disable_num_pay, const QString &title)
{
    if(ui->lineEdit_realreceive->value()  <= hasPaid())
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已足额付款无需继续付款"));
        return QDialog::Rejected;
    }

    for(int row = 0; row < payModel()->rowCount(); row ++) {
        if(payModel()->model_data(row, "ch_paymodeno").toString() == "10"
                || payModel()->model_data(row, "ch_paymodeno").toString() == "02") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已有会员卡付款"));
            return QDialog::Rejected;
        }
    }


    //!保存
    m_save();
    //!～保存
    w_scr_dish_member dialog(this);
    dialog.setWindowTitle(title);
    if(b_always_disable_num_pay)
        dialog.always_disable_num_pay();
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        if(dialog.cardinfo.ch_attribute == "2") {//充值卡
            QString ch_paymodeno;
            if(dialog.cardinfo.isyun()) {
                ch_paymodeno = "10";//云会员
            } else {
                ch_paymodeno = "02";
            }

            payModelAppend(ldsVariantMap()
                           .append("ch_paymodeno", ch_paymodeno)
                           .append("num_payamount", dialog.ui->lineEdit_num_pay->value())
                           .append("num_realamount", dialog.ui->lineEdit_num_pay->value())
                           .append("vch_voucherno", dialog.ui->lineEdit_vch_memberno->text())
                           .append("vch_memo", dialog.ui->lineEdit_vch_memo->text())
                           );
            updatePaylistCashpay();
        }
        //充值卡 优惠卡
        return QDialog::Accepted;
    }

    //!还原
    m_restore();
    //!～还原
    refreshControl();
    return QDialog::Rejected;
}

double w_scr_dish_pay_widget::moneyPay()
{
    return ui->lineEdit_moneypay->value();
}

double w_scr_dish_pay_widget::changePay()
{
    return ui->lineEdit_change->value();
}

void w_scr_dish_pay_widget::set_exec_sql_after_paid(const QStringList &sqls)
{
    exec_sql_after_paid = sqls;
}

void w_scr_dish_pay_widget::first_go_in()
{
    pay_data_init();
    //
    if(pay_operate_mode & TYPE_FAST) {
        ui->lineEdit_ch_tableno->setText("Q000");
        ui->lineEdit_vch_memo->setText("");
        ui->lineEdit_int_person->setValue(1);
    }

    if(pay_operate_mode & TYPE_RESTAURANT) {
        ui->lineEdit_ch_tableno->setText(model_orderdish->get_cey_u_table_value(model_orderdish->cur_ch_billno(), "ch_tableno").toString());
        ui->lineEdit_vch_memo->setText(lds_query_hddpos::selectValue(QString(" select vch_memo from cey_u_table where ch_billno = '%1' ").arg(model_orderdish->cur_ch_billno())).toString());
        ui->lineEdit_int_person->setText(model_orderdish->get_cey_u_table_value(model_orderdish->cur_ch_billno(), "int_person").toString());
    }
    //初始化
    refreshControl();
}

void w_scr_dish_pay_widget::pay_data_init()
{
    pay_all_discount = -1;
    pay_keep_data = model_orderdish->getKeepdata();

    vch_membernod = model_orderdish->vch_membernod;

    pay_finished = false;
}

void w_scr_dish_pay_widget::refreshLowServiceControl()
{
    w_scr_dish_pay_widget_service_manual_obj obj;
    obj.readUi(ui, this);
    obj.setService(obj.isServiceEnable(), ui->lineEdit_service->value());
    obj.writeUi(ui);

    //老版本的处理低消的东西

    //    ui->lineEdit_lowcost->setValue(qMax(double(0), pay_keep_data.num_pay_low - pay_without_low_service() - ui->lineEdit_service->value_if_prefix()));
    //    double v = pay_without_low_service() +ui->lineEdit_lowcost->value_if_prefix() + ui->lineEdit_service->value_if_prefix();
    //    ui->lineEdit_realreceive->setValue(valuerountbysys(v));
    //    ui->lineEdit_blotout->setValue(v - ui->lineEdit_realreceive->value());
    //    ui->lineEdit_moneypay->setValue(label_paylist_remain());
}

void w_scr_dish_pay_widget::refreshQuitbillControl()
{
    for(int k = 0; k < pay_section.count(); k++) {
        lds_pushbutton_wrap *p = pay_section[k];
        QString key = p->getData(w_scr_dish_pay_module_select::modeule_key).toString();
        if(canQuitbill(key)) {
            p->setEnabled(true);
        } else {
            p->setEnabled(false);
        }
    }
}

void w_scr_dish_pay_widget::refreshControl()
{
    if( (model_orderdish->vch_membernod.vch_memberno.length() == 0 && vch_membernod.vch_memberno.length() > 0 && vch_membernod.is_m_price())//点单没有读会员卡 && 但是付款时读卡
            || -1 != pay_all_discount
            ) {
        pay_keep_data = fexpandmain_model_sqltablemodel_data::get_keep_data_pay_by_isMPrice_intDiscount(
                    model_orderdish,
                    vch_membernod.is_m_price(),
                    pay_all_discount);
        pay_keep_data.print();
    }
    ui->lineEdit_consum->setValue(pay_keep_data.consum);
    ui->lineEdit_discount->setValue(pay_keep_data.discount);
    ui->lineEdit_present->setValue(pay_keep_data.present);
    ui->lineEdit_int_rate->setValue(pay_keep_data.num_rate());

    ui->lineEdit_tax_before->setValue(pay_keep_data.num_rate_before);
    ui->lineEdit_tax_cur->setValue(pay_keep_data.num_rate1);
    ui->lineEdit_tax_cur_2->setValue(pay_keep_data.num_rate2);
    ui->lineEdit_tax_cur_3->setValue(pay_keep_data.num_rate3);

    pay_keep_data.print();

    //v
    QString ch_billno = model_orderdish->cur_ch_billno();
    int ch_tax_mode = model_orderdish->get_cey_u_taxsale_master_value(ch_billno, "ch_tax_mode").toInt();
    switch(ch_tax_mode) {
    case 1://含税,不含服务费
        ui->lineEdit_int_rate->setMessage(QObject::tr("价格含税"));
        break;
    case 2://不含税,含服务费
        break;
    case 3://含税,含服务费
        ui->lineEdit_int_rate->setMessage(QObject::tr("价格含税"));
        break;
    case 0://不含税,不含服务费
    default:
        break;
    }
    //    ui->lineEdit_lowcost->setPrefix("");ui->lineEdit_lowcost->setValue(0);
    //    ui->lineEdit_service->setPrefix("");ui->lineEdit_service->setValue(0);

    //低消
    ui->lineEdit_service->setValue(pay_keep_data.pay_service(ch_billno));
    ui->lineEdit_service->setMessage(pay_keep_data.pay_service_desc(ch_billno));

    refreshLowServiceControl();

    //判断是否已经设置折扣卡
    lds_pushbutton_wrap *btn_hyk = paySectionValue("hyk");
    if(btn_hyk)
        btn_hyk->show_pixmap(Qt::AlignTop | Qt::AlignRight, public_pushbuton_rt_check::pixmap_checked(vch_membernod));
    frontViewDialog_virtual::static_display_cash(ui->lineEdit_realreceive->value());
    if(model_orderdish->get_cey_sys_parameter_value("qt_def_money", "1").toString() == "0") {
        ui->lineEdit_moneypay->setValue(0);
    }
}

void w_scr_dish_pay_widget::updateMuyBillPay(member_up_yun &muy)
{
    muy.clearallList();
    muy.isrun = false;
    if(vch_membernod.ch_member_belong_state == "Y") {
        muy.isrun = true;
        QString errstring;
        QString dateyyMMdd = n_func::f_get_sysdatetime().toString("yyMMdd");

        QString ch_billno_Y;
        w_sys_manage_cloudsync::get_yun_member_maxorderno("K"+dateyyMMdd, ch_billno_Y, errstring);
        ch_billno_Y = "K"+dateyyMMdd  + QString().sprintf("%05d",ch_billno_Y.right(5).toInt() +1);

        QString ch_payno_Y;
        w_sys_manage_cloudsync::get_yun_member_maxmasterno("F" +dateyyMMdd , ch_payno_Y, errstring);
        ch_payno_Y = "F"+dateyyMMdd  + QString().sprintf("%05d",ch_payno_Y.right(5).toInt() +1);

        muy.ch_billno = ch_billno_Y ;
        muy.ch_payno = ch_payno_Y ;
    }
}

void w_scr_dish_pay_widget::accept()
{
    emit accepted();
}

void w_scr_dish_pay_widget::reject()
{
    emit rejected();
}

//只在还原和保存会员信息是有用
static int pay_all_discount_save;
static bool member_not_read;//点菜时没有读卡
void w_scr_dish_pay_widget::m_save()
{
    pay_all_discount_save = pay_all_discount;
    member_not_read = vch_membernod.vch_memberno.isEmpty();
}

void w_scr_dish_pay_widget::m_restore()
{
    if(member_not_read) {//点菜时没有读卡
        pay_data_init();
        pay_all_discount = pay_all_discount_save;
    }
}

void w_scr_dish_pay_widget::tocancel()
{
    //这个地方。。。
    //    if(model_orderdish) {
    //        model_orderdish = 0;
    //    }

    this->reject();
}

void w_scr_dish_pay_widget::todiscount()
{
    if(payModel()->rowCount() > 0)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已有付款记录,不可进行打折处理"));
        return;
    }

    //! 折扣获取
    lds_dialog_input_double inputdialog(this, true);
    inputdialog.ui->label->setText(QObject::tr("折扣"));
    inputdialog.ui->lineEdit->setRange(0, 100);
    inputdialog.ui->lineEdit->setDecimals(0);
    inputdialog.ui->lineEdit->setSuffix("%");
    inputdialog.ui->lineEdit->setValue(100);
    inputdialog.ui->label_2->setText(QObject::tr("金额"));
    inputdialog.ui->lineEdit_2->setDecimals(model_orderdish->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    inputdialog.ui->lineEdit_2->setRange(0, ui->lineEdit_realreceive->value());
    inputdialog.ui->lineEdit_2->setValue(ui->lineEdit_realreceive->value());
    inputdialog.connect_discount_amount_transfer(ui->lineEdit_realreceive->value());

    if(public_sql::discount_off_mode()) {//折让
        inputdialog.ui->lineEdit->enableValueMaxMinus();
        inputdialog.ui->lineEdit_2->setValue(0);
    }
    if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
        return;
    }
    QString discount_desc;
    //enableValueMaxMinus， 则trueValue返回的就是折让的值
    if(false == public_sql::can_discount_limit(this, inputdialog.ui->lineEdit->trueValue(), QObject::tr("折扣"), discount_desc)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, discount_desc);
    } else {
        //enableValueMaxMinus， 则value返回的折扣的值
        pay_all_discount = inputdialog.ui->lineEdit->value();
    }
    refreshControl();
}

void w_scr_dish_pay_widget::tomember()
{
    mread(false, _TEXT_SLOT(this));
}

void w_scr_dish_pay_widget::tobank()
{
    QString vch_voucherno;
    double kafujine = 0;
    double label_paylist_remain_amt =  label_paylist_remain();
    if(label_paylist_remain_amt == 0)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已足额付款无需继续付款"));
        return;
    }

    QString ch_paymodeno = "03";
    bank_card dialog(ch_paymodeno, label_paylist_remain_amt, ui->lineEdit_consum->value(), this);
    dialog.setWindowTitle(QObject::tr("银行卡"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        kafujine = dialog.getRealPay();
        vch_voucherno = dialog.getVoucher();
    }
    else
    {
        return;
    }

    lds_query_hddpos  model;
    model.execSelect(QString("select ch_paymodeno,ch_incomeflag from cey_bt_paymode where ch_paymodeno = '%1' ")
                     .arg(ch_paymodeno));
    model.next();
    double t_shifuxianjin = 0;
    double t_fukuanjie = 0;
    if(label_paylist_remain_amt <= kafujine)//付清
    {
        t_shifuxianjin = label_paylist_remain_amt;
        t_fukuanjie = kafujine;
    }
    else//未付清
    {
        t_shifuxianjin = kafujine;
        t_fukuanjie = kafujine;
    }

    payModelAppend(ldsVariantMap()
                   .append("ch_paymodeno", ch_paymodeno)
                   .append("num_payamount", t_fukuanjie)
                   .append("num_realamount", t_shifuxianjin)
                   .append("vch_voucherno", vch_voucherno)
                   .append("vch_memo", dialog.getMemo())
                   );

    updatePaylistCashpay();
}

void w_scr_dish_pay_widget::tovouchers()
{
    QString fukuanpingzheng;
    double kafujine = 0;
    double label_paylist_remain_amt =  label_paylist_remain();
    if(label_paylist_remain_amt == 0)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已足额付款无需继续付款"));
        return;
    }
    QString ch_paymodeno = "04";

    bank_card dialog(ch_paymodeno, label_paylist_remain_amt, ui->lineEdit_consum->value(), this);
    dialog.setWindowTitle(QObject::tr("代金券"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        kafujine = dialog.getRealPay();
        fukuanpingzheng = dialog.getVoucher();
    }
    else
    {
        return;
    }

    lds_query_hddpos  model;
    model.execSelect(QString("select ch_paymodeno,ch_incomeflag from cey_bt_paymode where ch_paymodeno = '%1' ")
                     .arg(ch_paymodeno));
    model.next();
    double t_shifuxianjin = 0;
    double t_fukuanjie = 0;
    if(label_paylist_remain_amt <= kafujine)//付清
    {
        t_shifuxianjin = label_paylist_remain_amt;
        t_fukuanjie = kafujine;
    }
    else//未付清
    {
        t_shifuxianjin = kafujine;
        t_fukuanjie = kafujine;
    }


    payModelAppend(ldsVariantMap()
                   .append("ch_paymodeno", ch_paymodeno)
                   .append("num_payamount", t_fukuanjie)
                   .append("num_realamount", t_shifuxianjin)
                   .append("vch_voucherno", fukuanpingzheng)
                   .append("vch_memo", dialog.getMemo())
                   );

    updatePaylistCashpay();
}

void w_scr_dish_pay_widget::torecharge()
{
    w_m_member_list::statictorecharge(this, 0, 0,"");
}

void w_scr_dish_pay_widget::amountClear()
{
    ui->lineEdit_moneypay->setValue(0);
}

void w_scr_dish_pay_widget::updateAmount()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        ui->lineEdit_moneypay->setValue(b->text());
    }
}

void w_scr_dish_pay_widget::setAmount()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        ui->lineEdit_moneypay->clear();
        ui->lineEdit_moneypay->setValue(b->text().toDouble());
    }
}

void w_scr_dish_pay_widget::updateChange()
{
    double change = ui->lineEdit_moneypay->value() - label_paylist_remain();
    ui->lineEdit_change->setValue(change);
}

void w_scr_dish_pay_widget::updateDD(double value)
{
    model_orderdish->emit_num_total_change_label_pay(pay_keep_data.dish_num(), value);
}

void w_scr_dish_pay_widget::toChooseSpecialDispose(const QString &arg1)
{
    if(arg1.length() == 0)
        return;
    if(arg1 == "SETTINGS") {
        toPaysettings();
        return;
    }
    double paylist_remain =  label_paylist_remain();
    if(paylist_remain == 0)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已足额付款无需继续付款"));
        return;
    }

    QString ch_paymodeno = arg1;

    if(arg1 == "ZZ") {//免单
        toBillFree();
    } else if(arg1 == "YY") {//抹零
    } else if(arg1 == "11") {
        toMemberHang();
        //        //挂账/还款
        //        switch(lds_messagebox::question(this, QObject::tr("挂账/还款"), QObject::tr("请选择") +QObject::tr("挂账/还款"), QObject::tr("挂账"), QObject::tr("还款"),QObject::tr("退出"))) {
        //        case 0:
        //            to_member_hang();
        //            break;
        //        case 1:
        //            to_bill_pay();
        //            break;
        //        case 2:
        //            break;
        //        }
    } else if(arg1 == "AA") {
        tomobilepay();
    } else if(arg1 == "DD") {//微支付
        tofupay();
    } else if(arg1 == "05") {
        towxpay();
    } else if(arg1 == "CC") {
        toccbpay();
    } else if(arg1 == "EE") {
        double payamount;
        if(false == w_scr_dish_phone_pay::getInputPayFee(payamount, ch_paymodeno, label_paylist_remain(), this)) {
            return;
        }
        //
        toExecQRCodePay(ch_paymodeno, payamount);
    } else if(arg1 == "03") {
        tobank();

    } else if(arg1 == "04") {
        tovouchers();

    } else if(arg1 == "06") {
        toalipay();

    } else {//其他
        if(otherModePay(arg1))
            return;
        if(towaimaipaymodeno(arg1))
            return;

        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("未找到该支付方式"));
    }
}

void w_scr_dish_pay_widget::toLowcostFree()
{
    if(ui->lineEdit_lowcost->prefix().isEmpty()) {
        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("取消低消"), QObject::tr("确定"),  QObject::tr("取消"))) {
            ui->lineEdit_lowcost->setPrefix(QObject::tr("免"));
        }
    } else {
        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("恢复低消"), QObject::tr("确定"),  QObject::tr("取消"))) {
            ui->lineEdit_lowcost->setPrefix("");
        }
    }
    refreshLowServiceControl();
}

void w_scr_dish_pay_widget::toPaysettings()
{
    w_scr_dish_pay_set dialog(this);
    dialog.setWindowTitle(QObject::tr("结算设置"));
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.move(0,0);
    dialog.exec();
}

void w_scr_dish_pay_widget::toReceiptSwitch()
{
    lds_pushbutton_wrap *b = 0;
    for(int k = 0; k < pay_section.count(); k ++) {
        if("dyxp" == pay_section[k]->getData(w_scr_dish_pay_module_select::modeule_key).toString()) {
            b = pay_section[k];
            break;
        }
    }
    if(b == 0)
        return;
    //
    QString check = lds::sysconf->value("system_setting/maidanzhangdan").toString();
    if(check == "1")
        check = "0";
    else
        check = "1";
    lds::sysconf->setValue("system_setting/maidanzhangdan", check);
    setReceiptCheck(b, check == "1");
}

void w_scr_dish_pay_widget::toServiceFree()
{
    //免服务费，只有中餐有的
    //ue_free_service (none) returns (none)
    //1
    QString ls_ret;
    double ldec_service;
    double ldec_rate;
    double ldec_amount;
    if(payModel()->rowCount() > 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已有付款，操作禁止"));
        return;
    }
    //2
    if(n_func::f_get_sysparm("service_handwork") == "1") {//!手动处理服务费0
        w_scr_balance_service dialog(this);
        dialog.setWindowTitle(QObject::tr("手动处理服务费"));
        if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
            return;
        }
        ls_ret = dialog.get_returnvalue();
        if(ls_ret.isEmpty()) {
            return;
        }

        if(ls_ret.left(1) == "N") {//指定
            ldec_service = ls_ret.mid(2).toDouble();
        } else if(ls_ret.left(1) == "R") {//费率
            ldec_amount = pay_keep_data.num_service;
            ldec_rate = ls_ret.mid(2).toDouble() * 0.01;
            ldec_service = QString::number(ldec_rate * ldec_amount, 'f', 2).toDouble();
        } else {//系统
            ldec_service = pay_keep_data.num_service;
        }

        ui->lineEdit_service->setValue(ldec_service);
    } else {//!手动处理服务费1
        if(ui->lineEdit_service->value() <= 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("服务费等于零，不必免服务费"));
            return;
        }
        if(ui->lineEdit_service->prefix().isEmpty()) {
            if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("取消服务费"), QObject::tr("确定"),  QObject::tr("取消"))) {
                ui->lineEdit_service->setPrefix(QObject::tr("免"));
            }
        } else {
            if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("恢复服务费"), QObject::tr("确定"),  QObject::tr("取消"))) {
                ui->lineEdit_service->setPrefix("");
            }
        }
    }//!手动处理服务费2
    //3
    refreshLowServiceControl();
}

void w_scr_dish_pay_widget::toreSetServiceFree()
{
    w_scr_dish_pay_widget_service_manual dialog(this);
    dialog.readUi(ui, this);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        dialog.writeUi(ui);
    }
}

void w_scr_dish_pay_widget::toreSetServiceFree2()
{
    lds_dialog_input_double inputdialog(this);
    inputdialog.setWindowTitle(QObject::tr("服务费"));
    inputdialog.ui->lineEdit->setDecimals(2);
    inputdialog.ui->lineEdit->setValue(ui->lineEdit_service->value_if_prefix());
    inputdialog.ui->label->setText("");
//    pay_keep_data.ch_service_manual = false;
    if(QDialog::Accepted == transparentCenterDialog(&inputdialog).exec()) {
//        pay_keep_data.ch_service_manual = true;
//        pay_keep_data.num_service_manual = inputdialog.ui->lineEdit->value();
    }
    refreshControl();
}

void w_scr_dish_pay_widget::toMemberHang()
{
    QString ch_paymodeno = "11";
    QString vch_memberno;
    w_scr_dish_m_hang_Dialog dialog(this);
    dialog.setWindowTitle(QObject::tr("选择会员"));
    if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
        return;
    }
    vch_memberno = dialog.m;
    //1
    //2
    free_view freeview(label_paylist_remain_str(), ch_paymodeno, this);
    freeview.setWindowTitle(QObject::tr("挂账") +"-"+vch_memberno);
    freeview.m=vch_memberno;
    transparentCenterDialog(&freeview).exec();
    if(freeview.btn_t_f == true)
    {
        payModelAppend(ldsVariantMap()
                       .append("ch_paymodeno", ch_paymodeno)
                       .append("num_payamount", freeview.line2_num)
                       .append("num_realamount", freeview.line2_num)
                       .append("vch_voucherno", vch_memberno)
                       .append("vch_memo", freeview.beizhu)
                       );

        updatePaylistCashpay();
    }
}

void w_scr_dish_pay_widget::toBillPay()
{
    w_scr_dish_m_hang_pay_Dialog dialog(this);
    dialog.setWindowTitle(QObject::tr("挂账付款"));
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_pay_widget::toBillFree()
{
    toOtherPay("ZZ");
}

void w_scr_dish_pay_widget::toOtherPay(const QString &ch_paymodeno)
{
    otherModePay(ch_paymodeno);
}

void w_scr_dish_pay_widget::towxpay()
{
    lds_query_hddpos  query;
    QString ch_incomeflag;
    QString ch_paymodeno = "05";
    double payamount;
    query.execSelect(QString("select ch_incomeflag, vch_arg6, vch_paymodename from cey_bt_paymode where ch_paymodeno = '%1' ")
                     .arg(ch_paymodeno));
    query.next();
    ch_incomeflag = query.recordValue("ch_incomeflag").toString();
    QString vch_arg6 = query.recordValue("vch_arg6").toString();
    QString vch_paymodename = query.recordValue("vch_paymodename").toString();
    if(vch_arg6 == "Y") {
        //记账
        otherModePay(ch_paymodeno, 0, vch_paymodename+"("+QObject::tr("仅记账") +")");
        return;
    }

    if(payModel()->rowCount() > 0) {
        //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已有其他付款方式"));
        //        return;
    }
    if(false == w_scr_dish_phone_pay::getInputPayFee(payamount, ch_paymodeno, label_paylist_remain(), this)) {
        return;
    }
    toExecQRCodePay(ch_paymodeno, payamount);
}

void w_scr_dish_pay_widget::toccbpay()
{
    lds_query_hddpos  query;
    QString timestamp_not_null= n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss");
    QString out_trade_no;
    QString ch_incomeflag;
    QString ch_paymodeno = "CC";
    double payamount;
    query.execSelect(QString("select ch_incomeflag, vch_arg6, vch_paymodename from cey_bt_paymode where ch_paymodeno = '%1' ")
                     .arg(ch_paymodeno));
    query.next();
    ch_incomeflag = query.recordValue("ch_incomeflag").toString();
    QString vch_arg6 = query.recordValue("vch_arg6").toString();
    QString vch_paymodename = query.recordValue("vch_paymodename").toString();
    if(vch_arg6 == "Y") {
        //记账
        otherModePay(ch_paymodeno, 0, vch_paymodename+"("+QObject::tr("仅记账") +")");
        return;
    }

    if(payModel()->rowCount() > 0) {
        //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已有其他付款方式"));
        //        return;
    }
    if(false == w_scr_dish_phone_pay::getInputPayFee(payamount, ch_paymodeno, label_paylist_remain(), this)) {
        return;
    }
    toExecQRCodePay(ch_paymodeno, payamount);
}

void w_scr_dish_pay_widget::toalipay()
{
    QString ch_paymodeno = "06";
    double payamount;
    //记账
    if(w_scr_dish_phone_pay::isPayRecord(ch_paymodeno)) {
        otherModePay(ch_paymodeno, 0, QObject::tr("支付宝支付") +"("+QObject::tr("仅记账") +")");
        return;
    }
    if(payModel()->rowCount() > 0) {
        //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已有其他付款方式"));
        //        return;
    }
    if(false == w_scr_dish_phone_pay::getInputPayFee(payamount, ch_paymodeno, label_paylist_remain(), this)) {
        return;
    }
    toExecQRCodePay(ch_paymodeno, payamount);
}

void  w_scr_dish_pay_widget::tomobilepay()
{
    mobilepay(false);
}

void w_scr_dish_pay_widget::tofupay()
{
    QString ch_paymodeno = "DD";
    double payamount;
    if(false == w_scr_dish_phone_pay::getInputPayFee(payamount, ch_paymodeno, label_paylist_remain(), this)) {
        return;
    }
    //
    toExecQRCodePay(ch_paymodeno, payamount);
}

int w_scr_dish_pay_widget::toExecQRCodePay(const QString &ch_paymodeno, double payamount)
{
    w_scr_dish_phone_pay dialog(ch_paymodeno, payamount, this);
    if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
        return QDialog::Rejected;
    }

    payModelAppend(ldsVariantMap()
                   .append("ch_paymodeno", ch_paymodeno)
                   .append("num_payamount", payamount)
                   .append("num_realamount", payamount)
                   .append("vch_voucherno", dialog.ret_out_trade_no)
                   .append("vch_memo",  dialog.ret_pay_name)
                   );
    updatePaylistCashpay();

    return QDialog::Accepted;
}

void w_scr_dish_pay_widget::tonuomi()
{
    towaimaipaymodeno("07");
}

void w_scr_dish_pay_widget::todianping()
{
    towaimaipaymodeno("08");
}

void w_scr_dish_pay_widget::tomeituan()
{
    towaimaipaymodeno("09");
}

bool w_scr_dish_pay_widget::towaimaipaymodeno(const QString &ch_paymodeno)
{
    QStandardItemModel model;
    model.setItem(0, 0, new QStandardItem);
    lds_query_hddpos  query;
    query.execSelect(QString("select ch_paymodeno, vch_paymodename,vch_arg2,vch_arg4 from cey_bt_paymode where vch_arg4 = 'T' and ch_paymodeno = '%1' ")
                     .arg(ch_paymodeno));
    if(query.next()) {

        model.setData(model.index(0, 0), query.recordValue("vch_paymodename"), Qt::UserRole);
        model.setData(model.index(0, 0), query.recordValue("ch_paymodeno"), Qt::UserRole + 1);
        model.setData(model.index(0, 0), query.recordValue("vch_arg2"), Qt::UserRole + 2);

        towaimaicheck(model.index(0,0));
        return true;
    }

    return false;
}

void w_scr_dish_pay_widget::towaimaicheck(const QModelIndex &index)
{
    //
    if(index.data(Qt::UserRole + 1).toString() == "-1"
            || index.data(Qt::UserRole + 1).toString() == "-2"
            || index.data(Qt::UserRole + 1).toString().isEmpty()
            ) {
        return;
    }
    //
    if(label_paylist_remain() <= 0)
    {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已足额付款无需继续付款"));
        return;
    }

    QString vch_paymodename = index.data(Qt::UserRole).toString();
    QString ch_paymodeno = index.data(Qt::UserRole + 1).toString();
    QString vch_paymodeaddress = index.data(Qt::UserRole + 2).toString();

    lds_dialog_input inputdialog(this);
    inputdialog.setWindowTitle(vch_paymodename);
    inputdialog.showSecondLineEdt();
    inputdialog.ui->label->setText(QObject::tr("付款金额"));
    inputdialog.ui->label_2->setText(QObject::tr("优惠金额"));
    inputdialog.ui->lineEdit->setText(QString::number(label_paylist_remain()));
    inputdialog.ui->lineEdit_2->setText(QString::number(label_paylist_remain()));
    inputdialog.ui->lineEdit_2->total = label_paylist_remain();
    inputdialog.ui->lineEdit_2->setRemain_sub(inputdialog.ui->lineEdit->text());

    connect(inputdialog.ui->lineEdit,SIGNAL(textChanged(QString)), inputdialog.ui->lineEdit_2,SLOT(setRemain_sub(QString)));
    if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
        return;
    }
    if(inputdialog.ui->lineEdit->text().toDouble() > label_paylist_remain()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("实付金额不能大于应收金额"));
        return;
    }
    if(inputdialog.ui->lineEdit_2->text().toDouble() < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("优惠金额不能小于零"));
        return;
    }
    if((inputdialog.ui->lineEdit_2->text().toDouble() + inputdialog.ui->lineEdit->text().toDouble()) > label_paylist_remain()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("实付金额不能大于应收金额"));
    }
    //所有优惠金额已被优惠处理
    bool  fav_as_manuldis = false;
    if(inputdialog.ui->lineEdit_2->text().toDouble() > 0) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("所有优惠金额已被优惠处理"), QObject::tr("是"),  QObject::tr("否"),  QObject::tr("取消"))) {
        case 2://取消
            return;
        case 0:
            fav_as_manuldis = true;
            break;
        case 1:
            fav_as_manuldis = false;
            break;
        }
    }
    QProcess p;
    p.start(("/usr/desk/ie/QtWeb"" " + vch_paymodeaddress));
#ifndef QT_DEBUG
    if(p.waitForFinished(-1)) {
#endif
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("是否验证成功"), QObject::tr("是"), QObject::tr("否"))) {
            if(fav_as_manuldis) {//1优惠
                double favorite = inputdialog.ui->lineEdit_2->text().toDouble();
                if(favorite > 0 && !otherModePay("ZZ", favorite)) return;
            }
            //2
            QString payamount = inputdialog.ui->lineEdit->text();
            payModelAppend(ldsVariantMap()
                           .append("ch_paymodeno", ch_paymodeno)
                           .append("num_payamount", payamount)
                           .append("num_realamount", payamount)
                           .append("vch_voucherno", "")
                           .append("vch_memo", "")
                           );

            updatePaylistCashpay();
        }
#ifndef QT_DEBUG
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("浏览器打开超时"));
    }
#endif
}

void w_scr_dish_pay_widget::tospecialcheck(const QModelIndex &index)
{
    //
    if(index.data(Qt::UserRole + 1).toString() == "-1"
            || index.data(Qt::UserRole + 1).toString() == "-2"
            || index.data(Qt::UserRole + 1).toString().isEmpty()
            ) {
        return;
    }
    //usr usr_1 usr2 : name, 内部key， 外部key可重复
    toChooseSpecialDispose(index.data(Qt::UserRole + 2).toString());
}

void w_scr_dish_pay_widget::toplayloop()
{
    playloop();
}

void w_scr_dish_pay_widget::playloop()
{
    w_scr_dish_pay_set_ogg dialog(this);
    dialog.setWindowTitle("支付语言轮询播放");
    transparentCenterDialog(&dialog).exec();
}

int w_scr_dish_pay_widget::mobilepay(bool skip_input_amount)
{
    QString ch_paymodeno = "AA";
    double payamount = label_paylist_remain();

    if(payModel()->rowCount() > 0) {
        //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已有其他付款方式"));
        //        return QDialog::Rejected;
    }
    if(false == skip_input_amount
            && false == w_scr_dish_phone_pay::getInputPayFee(payamount, ch_paymodeno, label_paylist_remain() + ui->lineEdit_blotout->value(),  this)) {

        return QDialog::Rejected;
    }

    return  toExecQRCodePay(ch_paymodeno, payamount);
}

void w_scr_dish_pay_widget::setPayMoney(double pay)
{
    ui->lineEdit_moneypay->setValue(pay);
}

void w_scr_dish_pay_widget::toScrpay()
{
    //!等待
    lds_messagebox_loading_show loading(this, "", "WAITING...");
    loading.show();
    //!~等待
    //云会员支付异常
    for(int row = 0; model_paydetail != 0 && row < model_paydetail->rowCount(); row ++) {
        if("10" == model_paydetail->model_data(row, "ch_paymodeno").toString()) {//云会员
            if(false == vch_membernod.isyun()
                    || vch_membernod.vch_memberno.isEmpty()) {//折扣卡异常
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("云会员支付异常") + "\n" +
                                        "vch_membernod.isyun:" + QString::number(vch_membernod.isyun()) + "\n" +
                                        "vch_membernod.vch_memberno:" + vch_membernod.vch_memberno + "\n" +
                                        "vch_voucherno:" +  model_paydetail->model_data(row, "vch_voucherno").toString()
                                        );
                return;
            }
            break;
        }
    }
    //

    //置灰
    pushbuttonAutoEnable b(ui->pushButton_pay);
    //已经付款
    if(pay_finished) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("本单已付款"));
        return;
    }
    //~已经付款
    //超出找零限额
    if(ui->lineEdit_change->value() < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("付款金额不足"));
        return ;
    }

    //    /超出最大找零金额
    double pay_max_change = n_func::f_get_sysparm("pay_max_change", (lds::get_soft_curent_language_first() == _CN_ ? 100 : 100000)).toDouble();
    if(ui->lineEdit_change->value() > pay_max_change) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("超出最大找零金额") + "/" + QString::number(pay_max_change));
        return ;
    }
    QString errstring;
    QString ch_payno;
    QDateTime dt = n_func::f_get_sysdatetime();
    //开始付款
    /**
*/
    ///手牌号
    QString serial_no_cur;
    if(lds::sysconf->value("system_setting/fukuanshishurushoupai").toBool()) {
        lds_dialog_input inputdialog(this);
        inputdialog.ui->label->setText(QObject::tr("手牌号"));
        inputdialog.ui->lineEdit->setText(serial_no_cur);
        inputdialog.setWindowTitle(QObject::tr("请输入手牌号"));
        if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
            errstring = QObject::tr("操作取消");
            return;
        }
        serial_no_cur = inputdialog.ui->lineEdit->text();
        if(serial_no_cur.isEmpty()) {
            if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("手牌号不能为空"), QObject::tr("继续支付"), QObject::tr("取消"))) {
                errstring = QObject::tr("操作取消");
                return;
            }
        }
    }
    ///~手牌号

    //前台买单打印账单
    if(lds::sysconf->value("system_setting/maidanzhangdan").toString() == "1") {
        if(lds::sysconf->value("system_setting/pay_select_language", false).toBool()) {
            w_scr_dish_pay_select_language dialog(this);
            dialog.setWindowTitle(QObject::tr("选择语言"));
            if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
                errstring = QObject::tr("操作取消");
                return;
            }
        }
    }
    QStringList insert_int_flowid_list;
    lds_query_hddpos  query;
    lds_query_hddpos::tran_saction();
    toMPay(dt, errstring, ch_payno, serial_no_cur, insert_int_flowid_list);
    if(pay_finished) {//付款成功
        //经权接口
        if(model_orderdish->get_cey_sys_parameter_value("jingquan_interface").toString() == "1") {
            if(false == w_sys_manage_outer_pay_set_jingquan::postSale(&loading, ch_payno, pay_operate_mode, errstring)) {
                lds_query_hddpos::roll_back();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                return;
            }
        }

        //exec_sql_after_paid
        foreach(const QString &sql, exec_sql_after_paid) {
            if(false == query.execAlter(sql, &errstring)) {
                lds_query_hddpos::roll_back();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                return;
            }
        }

        /**
*/lds_query_hddpos::com_mit();
        //云端上传流水
        toYunPaydataUploadWithTransaction(this, ch_payno, dt);
        //厨打
        public_sql::save_login->to_everytime_to_print_failed_kitchen_bill();
        //客显
        frontViewDialog_virtual::static_display_change(ui->lineEdit_change->value());
        //营业数据上传和打印
        dataupload(ch_payno, dt);
        //预打单状态更新
        if(pay_operate_mode & TYPE_RESTAURANT) {
            public_sql::preprinttablemap.insert(model_orderdish->cur_ch_billno(), false);
        }
        //同步数据
        if(lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false).toBool()) {
            public_sql::save_login->keepdata_time_basedata_polling->ToTryTrigger();
        }
        public_printby_ch_billno::payPrintModes pay_print_mode = public_printby_ch_billno::payNone;
        //开钱箱
        if(ui->lineEdit_moneypay->value() != 0//现金不为0
                && lds::sysconf->value("system_setting/qianxiang", true).toBool()) {
            pay_print_mode |= public_printby_ch_billno::payDrawer;
        }
        //~开钱箱
        //选择打印结算单
        if(lds::sysconf->value("system_setting/maidanzhangdan").toString() == "1") {
            if( pay_operate_mode & OPERAT_QUITBILL) {
                pay_print_mode |= public_printby_ch_billno::payRefund;
            }
            if( pay_operate_mode & OPERAT_PAY) {
                pay_print_mode |= public_printby_ch_billno::payNormal;
            }
        }
        // ~选择打印结算单
        //打印标签
        pay_print_mode |= public_printby_ch_billno::payLabel;
        //~打印标签
        if(w_sys_manage_outer_pay_set_locale_wizard::bp_country_print_img()) {
            QString bp_languageKey = lds::sysconf->value("system_setting/maidan_language").toString();
            public_printby_ch_billno printer(model_orderdish->cur_ch_billno(), lds::sysconf->value("system_setting/maidan_language").toString());
            QImage image = printer.getPayPrintImg(printer.defpath(), pay_print_mode);
            public_sql::save_login->qt_recipt_print_polling->emitsignalTrigger(model_orderdish->cur_ch_billno(),
                                                                               bp_languageKey, insert_int_flowid_list, pay_print_mode,
                                                                               image);
        } else {
            QString bp_languageKey = lds::sysconf->value("system_setting/maidan_language").toString();
            public_sql::save_login->qt_recipt_print_polling->emitsignalTrigger(model_orderdish->cur_ch_billno(),
                                                                               bp_languageKey, insert_int_flowid_list, pay_print_mode);
        }

        this->accept();
        return;
    }
    /**
*/lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_scr_dish_pay_widget::toExitbill()
{
    disconnect(ui->lineEdit_moneypay,SIGNAL(valueChanged(double)),this,SLOT(updateChange()));

    //
    negativeallControl();//正负翻转
    toScrpay();
    if(pay_finished) {
        goto end;
    }
    negativeallControl();//正负翻转
    //
end:
    connect(ui->lineEdit_moneypay,SIGNAL(valueChanged(double)),this,SLOT(updateChange()));
}

void w_scr_dish_pay_widget::toYunPaydataUploadWithTransaction(QWidget *parent, const QString &ch_payno, const QDateTime &pay_dt)
{
    if(false == (lds::sysconf->value("w_sys_manage_cloudsync_with_time/cloud_yun_is_run", false).toBool())) return;

    //!等待
    lds_messagebox_loading_show loading(parent, "", "WAITING...");
    loading.show();
    //!~等待
    //!
    lds_query_hddpos::tran_saction();
    //同步本次会员流水
    QString errstring;
    if(false == w_sys_manage_cloudsync_with_time_running::flow_pay_data_upload(ch_payno, pay_dt, n_func::f_get_sysparm("orgCode"), &errstring)) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("流水上传失败") + ":" + errstring);
        return ;
    }
    //～同步本次会员流水
    lds_query_hddpos::com_mit();
}

void w_scr_dish_pay_widget::toModuleSelect()
{
    if(lds::gs_operid != "0000") {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("非管理员账号"), QObject::tr("管理员授权"), QObject::tr("退出"))) {
            return;
        }
        QString password;
        if(false == public_sql::get_password(this, QObject::tr("管理员密码"), password)) {
            return;
        }
        if(password != n_func::f_get_godpassword()
                && password != n_func::f_get_sysparm("system_pass", "0000")) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
            return;
        }
    }
    lds_pushbutton_wrap *cur_p = 0;
    QStringList select_items;
    foreach(lds_pushbutton_wrap *p, pay_section) {
        QString k = p->getData(w_scr_dish_pay_module_select::modeule_key).toString();
        select_items << k;
        if(p->isDown()) {
            cur_p = p;
        }
    }
    w_scr_dish_pay_module_select dialog(
                select_items,
                pay_operate_mode,
                this);
    dialog.setWindowTitle(QObject::tr("结算模块自定义"));
    int ret = transparentCenterDialog(&dialog).exec();
    if(QDialog::Accepted == ret) {
        QString pay_module1 = dialog.get_select_module_key();
        QString pay_module0 = cur_p->getData(w_scr_dish_pay_module_select::modeule_key).toString();

        //直接替换
        cur_p->setData(w_scr_dish_pay_module_select::modeule_key, pay_module1);
        cur_p->setText(w_scr_dish_pay_module_select::pay_module_list_value(pay_module1, pay_operate_mode));
        updateReceiptCheck(cur_p);

        //若是替换前AB，替换后BB，则第二B应该替换为A
        if(pay_module1.isEmpty()) return;
        foreach(lds_pushbutton_wrap *p, pay_section) {
            if(p != cur_p) {
                if(p->getData(w_scr_dish_pay_module_select::modeule_key).toString() == pay_module1) {
                    p->setData(w_scr_dish_pay_module_select::modeule_key, pay_module0);
                    p->setText(w_scr_dish_pay_module_select::pay_module_list_value(pay_module0, pay_operate_mode));
                    updateReceiptCheck(p);
                }
            }
        }
    }
    if(w_scr_dish_pay_module_select::dialog_remove == ret) {
        cur_p->setData(w_scr_dish_pay_module_select::modeule_key, "");
        cur_p->setText("");
        updateReceiptCheck(cur_p);
    }

    if(w_scr_dish_pay_module_select::dialog_default == ret) {
        lds::sysconf->setValue("w_scr_dish_pay_widget/pay_section", QStringList() << "AA"<< "03"<< "hyk"<< "zk"<< "ZZ"<< "qx"<< ""<< ""<< ""<< ""<< ""<< "");
        pay_section.clear();
        retranslateView();
    }

    cur_p->setDown(false);
    savePayModuleFile();

    if(pay_operate_mode & OPERAT_QUITBILL) {
        refreshQuitbillControl();
    }
}

void w_scr_dish_pay_widget::toPaySectionModule()
{
    lds_pushbutton_wrap *b = qobject_cast<lds_pushbutton_wrap *>(this->sender());
    QString k = b->getData(w_scr_dish_pay_module_select::modeule_key).toString();


    //    if("ydzf" == k) tomobilepay();
    //    if("zfb" == k) toalipay();
    //    if("yhk" == k)tobank();
    if("hyk" == k) {tomember();return;}
    if("cz" == k) {torecharge();return;}

    //    if("djq" == k)tovouchers();
    //    if("mt" == k)tomeituan();
    //    if("dzdp" == k) todianping();
    if("zk" == k) {todiscount();return;}

    if("qx" == k) {tocancel();return;}
    if("jssz" == k) {toPaysettings();return;}
    if("dyxp" == k) {toReceiptSwitch(); return;}
    //    if("yh" == k)to_bill_free();
    //    if("wxzf" == k)towxpay();

    //    if("hygz" == k)to_member_hang();
    if("mdxc" == k) {toLowcostFree();return;}
    if("mfwf" == k) {toServiceFree();return;}
    if("fwf" == k) {toreSetServiceFree();return;}
    toChooseSpecialDispose(k);
    return;
}

void w_scr_dish_pay_widget::toMPay(const QDateTime &dt, QString &errstring, QString &ch_payno, const QString &serial_no_cur, QStringList &insert_int_flowid_list)
{
    ///预付款插入
    lds_query_hddpos  query;
    bool gonext = true;
    if(gonext) {
        if(pay_operate_mode & OPERAT_QUITBILL) {
            //处理快餐退货
            gonext = model_orderdish->try_commit_data(errstring,
                                                      fexpandmain_model_sqltablemodel_data::commitKuaicanQuit, &insert_int_flowid_list);
            if(gonext) {
                gonext = query.execUpdate("cey_u_orderdish",
                                          "vch_plan_k3o3_state", QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_QUIT_KUAICAN)),
                                          qrtEqual("ch_billno", model_orderdish->cur_ch_billno()));
            }
        } else {
            gonext = model_orderdish->try_commit_data(errstring,
                                                      fexpandmain_model_sqltablemodel_data::commitAll, &insert_int_flowid_list);
        }

        if(gonext) {
            errstring = QObject::tr("付款失败");
            //1
            if(model_orderdish->vch_membernod.vch_memberno.isEmpty()//点菜没有读会员卡
                    && !vch_membernod.vch_memberno.isEmpty()) {// 付款时读会员卡
                if(vch_membernod.is_m_price()) {
                    //修正会员特价
                    query.execUpdate("cey_u_orderdish",
                                     QStringList() << "int_flowid",
                                     QString("select a.int_flowid, least(b.num_m_price, a.num_price) as `num_price`, '%1' as `vch_memberno`, '%2' as `ch_member_belong_state` from "
                                             " cey_u_orderdish a,  cey_bt_dish b where "
                                             " a.ch_dishno = b.ch_dishno and a.ch_billno = '%3'  and "
                                             /// 会员价：～赠送、～子套菜,   ~ 特价
                                             " a.ch_presentflag <> 'Y'  and a.ch_suitflag <> 'Y'   and a.ch_specialflag <> 'Y'  ")
                                     .arg(vch_membernod.vch_memberno)
                                     .arg(vch_membernod.ch_member_belong_state)
                                     .arg(model_orderdish->cur_ch_billno()));
                }
            }
            //3 全场折扣
            if(gonext) {
                if(pay_all_discount != -1) {
                    gonext = query.execUpdate("cey_u_orderdish",
                                              QStringList() << "int_flowid",
                                              QString("select a.int_flowid,  %1 as int_discount from cey_u_orderdish a,  cey_bt_dish b where "
                                                      " a.ch_dishno = b.ch_dishno and a.ch_billno = '%2'   and "
                                                      /// 折扣：折扣商品、~赠送，～子套菜,   ~ 特价
                                                      " b.ch_discount = 'Y'  and a.ch_presentflag <> 'Y'   and a.ch_suitflag <> 'Y'  and a.ch_specialflag <> 'Y' ")
                                              .arg(pay_all_discount)
                                              .arg(model_orderdish->cur_ch_billno()));
                    if(false == gonext) {
                    }
                }
            } //4
        }
    }
    if(false == gonext) {
        return;
    }
    ///～预付款插入
    ///
    ///～退款更新u_orderdish_log
    payAndYunCommit(dt, errstring, ch_payno, serial_no_cur);
}

bool w_scr_dish_pay_widget::otherModePay(const QString &ch_paymodeno, double amount, const QString &wtitle)
{
    lds_query_hddpos  query;
    QString vch_paymodename;
    //1
    query.execSelect(QString("select ch_paymodeno, vch_paymodename,vch_arg2,vch_arg4 from cey_bt_paymode where  ch_paymodeno = '%1'  and (vch_arg4 = 'N' or vch_arg4 = 'S') ")
                     .arg(ch_paymodeno));
    if(query.next()) {
        vch_paymodename = query.recordValue("vch_paymodename").toString();
        //2
        free_view freeview(label_paylist_remain_str(), ch_paymodeno, this);
        freeview.setWindowTitle(vch_paymodename);
        if(false == wtitle.isEmpty()) {
            freeview.setWindowTitle(wtitle);
        }
        if(amount > 0) {
            freeview.on_pushButton_ok_clicked();//确定
            freeview.line2_num = amount;
        } else {
            transparentCenterDialog(&freeview).exec();
            if(freeview.btn_t_f == false) return true;
        }

        if(ch_paymodeno == "ZZ" && !checkNumFree(freeview.oper_id, freeview.line2_num)) {
            return true;
        }
        payModelAppend(ldsVariantMap()
                       .append("ch_paymodeno", ch_paymodeno)
                       .append("num_payamount", freeview.line2_num)
                       .append("num_realamount", freeview.line2_num)
                       .append("vch_voucherno", "")
                       .append("vch_memo", freeview.beizhu)
                       );

        updatePaylistCashpay();
        return true;
    }

    return false;// not found
}

void w_scr_dish_pay_widget:: payAndYunCommit(const QDateTime &pay_datetime, QString &errstring, QString &ch_payno, QString serial_no_cur)
{
    lds_query_hddpos  query;
    member_up_yun muy;
    double num_point_add = 0;
    //    QString ch_payno;
    //    QString vch_service_desc;
    pay_finished=false;
    const QString ch_billno = model_orderdish->cur_ch_billno();
    QList<payModelData> pay_model_data_list;
    //ch_payno
    ch_payno = lds_scr_table::get_max_ch_payno_add1(pay_datetime);
    updateMuyBillPay(muy);

    //付款明细
    //////
    query.execSelect(QString("SELECT ch_billno from cey_u_table where ch_billno = '%1' ")
                     .arg(ch_billno));
    if(false == query.next()) {
        if(false == query.execInsert("cey_u_table",qrtVariantPairList()

                                     << qrtVariantPair("ch_billno", ch_billno)
                                     << qrtVariantPair("ch_tableno", "Q000")
                                     << qrtVariantPair("int_person", 1)
                                     << qrtVariantPair("ch_state", 1)
                                     << qrtVariantPair("vch_operID", lds::gs_operid)

                                     << qrtVariantPair("dt_operdate", pay_datetime)
                                     << qrtVariantPair("ch_order_state", 0)
                                     << qrtVariantPair("vch_waiter", "")
                                     << qrtVariantPair("vch_memo", ui->lineEdit_vch_memo->text())
                                     )) {
            errstring = (QObject::tr("数据库错误") + QString::number(__LINE__));
            goto fastpayend;
        }
        if(false == lds_scr_table::cey_u_taxsale_master_insert_table(query, ch_billno, model_orderdish, &errstring)) {
            goto fastpayend;
        }
    }

    //快餐的需要insert
    query.execSelect(QString("SELECT ch_billno from cey_u_master where ch_billno = '%1' ")
                     .arg(ch_billno));
    if(false == query.next()) {
        if(false == lds_scr_table::insert_cey_u_master_Create_serial_no(ch_billno)) {
            errstring = (QObject::tr("数据库错误") + QString::number(__LINE__));
            goto fastpayend;
        }
    }
    ///手牌号
    if(serial_no_cur.count() > 0) {
        if(false == query.execUpdate("cey_u_master", "vch_memo", serial_no_cur, qrtEqual("ch_billno", ch_billno))) {
            errstring = (QObject::tr("数据库错误") + QString::number(__LINE__));
            return;
        }
        errstring = (QObject::tr("数据库错误") + QString::number(__LINE__));
    }
    ///~手牌号
    qDebug() << vch_membernod.vch_memberno;
    if(false == query.execInsert("cey_u_checkout_master",qrtVariantPairList()
                                 << qrtVariantPair("ch_billno", ch_billno)
                                 << qrtVariantPair("ch_tableno", ui->lineEdit_ch_tableno->text())
                                 << qrtVariantPair("num_cost", ui->lineEdit_consum->value())
                                 << qrtVariantPair("num_discount", ui->lineEdit_discount->value())
                                 << qrtVariantPair("num_present", ui->lineEdit_present->value())

                                 << qrtVariantPair("num_service", ui->lineEdit_service->value_if_prefix())
                                 //                         << qrtVariantPair("vch_service_desc", vch_service_desc)
                                 << qrtVariantPair("num_lowcost", ui->lineEdit_lowcost->value_if_prefix())
                                 << qrtVariantPair("num_blotout", ui->lineEdit_blotout->value())
                                 << qrtVariantPair("num_run", 0)

                                 << qrtVariantPair("vch_run_operID", "")
                                 << qrtVariantPair("ch_state", "Y")
                                 << qrtVariantPair("vch_operID", lds::gs_operid)
                                 << qrtVariantPair("dt_operdate", pay_datetime)
                                 << qrtVariantPair("vch_operID2", "")

                                 << qrtVariantPair("num_rate", ui->lineEdit_int_rate->value())
                                 << qrtVariantPair("ch_payno", ch_payno)
                                 << qrtVariantPair("vch_memberno", vch_membernod.vch_memberno)
                                 << qrtVariantPair("ch_member_belong_state", vch_membernod.ch_member_belong_state)
                                 << qrtVariantPair("ch_yun_payno", muy.ch_payno)

                                 << qrtVariantPair("num_cash", ui->lineEdit_realreceive->value()))) {
        errstring = query.errorString();
        goto fastpayend;
    }
    if(false == query.execUpdate("cey_u_taxsale_master", qrtVariantPairList()
                                 << qrtVariantPair("ch_payno", ch_payno)
                                 << qrtVariantPair("num_before", ui->lineEdit_tax_before->value())
                                 << qrtVariantPair("num_cur", ui->lineEdit_tax_cur->value())
                                 << qrtVariantPair("num_cur_2", ui->lineEdit_tax_cur_2->value())
                                 << qrtVariantPair("num_cur_3", ui->lineEdit_tax_cur_3->value())
                                 << qrtVariantPair("dt_operdate", pay_datetime)
                                 , QString("ch_billno = '%1' ").arg(ch_billno)
                                 )) {
        errstring = (QObject::tr("数据库错误") + QString::number(__LINE__));
        goto fastpayend;
    }
    //服务费
    if(ui->lineEdit_service->value() != 0) {
        if(false == query.execInsert("cey_u_service_low",qrtVariantPairList()
                                     << qrtVariantPair("ch_payno", ch_payno)
                                     << qrtVariantPair("ch_type", 1)
                                     << qrtVariantPair("num_amount", ui->lineEdit_service->value())
                                     << qrtVariantPair("vch_operID", lds::gs_operid)
                                     << qrtVariantPair("dt_operdate", pay_datetime)

                                     << qrtVariantPair("ch_state", ui->lineEdit_service->prefix().length() == 0 ? 1 : 0) //为空表示正常
                                     , &errstring)) {

            errstring = (QObject::tr("数据库错误") + QString::number(__LINE__));
            goto fastpayend;
        }
    }
    //低消
    if(ui->lineEdit_lowcost->value() != 0) {
        if(false == query.execInsert("cey_u_service_low",qrtVariantPairList()
                                     << qrtVariantPair("ch_payno", ch_payno)
                                     << qrtVariantPair("ch_type", 2)
                                     << qrtVariantPair("num_amount", ui->lineEdit_lowcost->value())
                                     << qrtVariantPair("vch_operID", lds::gs_operid)

                                     << qrtVariantPair("dt_operdate", pay_datetime)
                                     << qrtVariantPair("ch_state", ui->lineEdit_lowcost->prefix().length() == 0 ? 1 : 0) //为空表示正常
                                     ,  &errstring)) {

            errstring = (QObject::tr("数据库错误") + QString::number(__LINE__));
            goto fastpayend;
        }
    }

    //餐桌
    if(false == query.execUpdate("cey_u_table","ch_payno", ch_payno, "ch_order_state", 4, qrtEqual("ch_billno",ch_billno))) {
        errstring = QObject::tr("数据库错误") + QString::number(__LINE__);
        goto fastpayend;
    }

    //原料
    if(false == query.execUpdate("cey_u_orderdish", qrtVariantPairList()
                                 <<qrtVariantPair("ch_payno", ch_payno)
                                 <<qrtVariantPair("vch_memberno", vch_membernod.vch_memberno)
                                 ,  qrtEqual("ch_billno",ch_billno))) {
        errstring = QObject::tr("数据库错误") + QString::number(__LINE__) + query.recordError();
        goto fastpayend;
    }

    //
    if(false == query.execUpdate("cey_u_orderdish_log",
                                 QStringList() << "int_orderflow",
                                 QString(" select a.int_orderflow,  b.ch_payno from cey_u_orderdish_log a, cey_u_orderdish b where a.int_orderflow = b.int_flowID and b.ch_payno = '%1' ; ").arg(ch_payno)
                                 )) {
        errstring = (QObject::tr("数据库错误") + QString::number(__LINE__));
        goto fastpayend;
    }
    //==============================================================================
    //现金付款
    if(ui->lineEdit_moneypay->value() != 0)
    {
        double aa = ui->lineEdit_moneypay->value() - ui->lineEdit_change->value();
        if(false == query.execInsert("cey_u_checkout_detail",qrtVariantPairList()

                                     << qrtVariantPair("ch_payno", ch_payno)
                                     << qrtVariantPair("ch_paymodeno", "00")
                                     << qrtVariantPair("num_payamount", ui->lineEdit_moneypay->value())
                                     << qrtVariantPair("num_change", ui->lineEdit_change->value())
                                     << qrtVariantPair("num_realamount", aa)

                                     << qrtVariantPair("num_face", 0)
                                     << qrtVariantPair("vch_operID", lds::gs_operid)
                                     << qrtVariantPair("dt_operdate", pay_datetime)
                                     << qrtVariantPair("num_card_remain", 0)
                                     << qrtVariantPair("vch_voucherno", "")

                                     << qrtVariantPair("num_face_Num", 0))) {

            errstring = (QObject::tr("数据库错误") + QString::number(__LINE__));
            goto fastpayend;
        }
    }

    //自动抹零
    if(ui->lineEdit_blotout->value() != 0)//存在自动抹零
    {
        if(false == query.execInsert("cey_u_checkout_detail",qrtVariantPairList()

                                     << qrtVariantPair("ch_payno", ch_payno)
                                     << qrtVariantPair("ch_paymodeno", "YY")
                                     << qrtVariantPair("num_payamount", ui->lineEdit_blotout->value())
                                     << qrtVariantPair("num_change", 0)
                                     << qrtVariantPair("num_realamount", ui->lineEdit_blotout->value())

                                     << qrtVariantPair("num_face", 0)
                                     << qrtVariantPair("vch_operID", lds::gs_operid)
                                     << qrtVariantPair("dt_operdate", pay_datetime)
                                     << qrtVariantPair("num_card_remain", 0)
                                     << qrtVariantPair("vch_voucherno", "")

                                     << qrtVariantPair("num_face_Num", 0))) {

            errstring = (QObject::tr("数据库错误") + QString::number(__LINE__));
            goto fastpayend;
        }
    }


    ///获取积分
    qDebug() << __LINE__ << "vch_memberno"<< vch_membernod.vch_memberno;
    if(false == vch_membernod.vch_memberno.isEmpty()) {
        if(false == get_member_point(ch_payno, num_point_add, errstring)) {
            goto fastpayend;
        }
    }
    ///～
    ///付款方式------------begin
    for(int row = 0; row < payModel()->rowCount(); row++ ) {
        pay_model_data_list.append(payModelData());
        pay_model_data_list.last().ch_paymodeno = payModel()->model_data(row, "ch_paymodeno").toString();
        pay_model_data_list.last().num_payamount = payModel()->model_data(row, "num_payamount").toDouble();
        pay_model_data_list.last().num_realamount = payModel()->model_data(row, "num_realamount").toDouble();
        pay_model_data_list.last().vch_memo = payModel()->model_data(row, "vch_memo ").toString();
        pay_model_data_list.last().vch_voucherno = payModel()->model_data(row, "vch_voucherno").toString();
    }
    if(vch_membernod.vch_memberno.count() > 0) {
        if(false == pay_model_data_list_find(pay_model_data_list, vch_membernod.vch_memberno)) {
            pay_model_data_list.append(payModelData());
            pay_model_data_list.last().ch_paymodeno = (vch_membernod.ch_member_belong_state == "Y" ? "10" : "02");
            pay_model_data_list.last().num_payamount = 0;
            pay_model_data_list.last().num_realamount = 0;
            pay_model_data_list.last().vch_memo = "";
            pay_model_data_list.last().vch_voucherno = vch_membernod.vch_memberno;
        }
    }

    foreach(const payModelData &paym, pay_model_data_list) {
        //会员卡付款不再计算积分
        QString paymodeno = paym.ch_paymodeno;
        bool ok;
        QString ch_member_belong_state;
        QString vch_voucherno = paym.vch_voucherno;
        QString vch_card_name;
        double num_card_point = 0;
        double num_card_remain = 0;
        QString vch_card_adress;
        ///云会员或本地会员付款---------begin
        if(paymodeno == "02"
                || paymodeno == "10") {//云会员付款

            QString num_amount;
            int li_backcash_rate = 0;
            double ldec_payamt;
            double ldec_backcash;
            double num_pay = paym.num_realamount;

            if(paymodeno == "10") {//云会员付款
                ch_member_belong_state = "Y";
            }
            //m_pay
            if(paymodeno == "10") {//云会员付款

                memberPay o;
                o.mpVchMemberNo = vch_voucherno;
                o.mpChPayNo = muy.ch_payno;
                o.mpChPayModeNo = "10";
                o.mpNumPay = num_pay;
                o.mpVchOperId = lds::gs_operid;
                o.mpDtOperDate = pay_datetime;
                muy.appitem(o);
            }
            if(paymodeno == "02") { //本地会员付款{
                if(false == query.execInsert("t_m_pay",qrtVariantPairList()

                                             << qrtVariantPair("vch_memberno", vch_voucherno)
                                             << qrtVariantPair("ch_payno", ch_payno)
                                             << qrtVariantPair("ch_paymodeno", paymodeno)
                                             << qrtVariantPair("num_pay", num_pay)
                                             << qrtVariantPair("vch_operID", lds::gs_operid)

                                             << qrtVariantPair("dt_operdate", pay_datetime)
                                             << qrtVariantPair("int_checkflow", 0)

                                             )) {

                    errstring = (QObject::tr("数据库错误") + QString::number(__LINE__));
                    goto fastpayend;
                }
            }
            //m_deposit
            if(paymodeno == "10") {//云会员付款
                num_amount=w_sys_manage_cloudsync::get_yun_member_readcard(this, vch_voucherno, n_func::f_get_sysparm("cloud_user"),
                                                                           "num_amount", ok);
                if(false == ok) goto fastpayend;
                num_card_point = w_sys_manage_cloudsync::get_yun_member_readcard(this, vch_voucherno, n_func::f_get_sysparm("cloud_user"),
                                                                                 "num_point", ok).toDouble();
                if(false == ok) goto fastpayend;
                li_backcash_rate=w_sys_manage_cloudsync::get_yun_member_readcard(this, vch_voucherno, n_func::f_get_sysparm("cloud_user"),
                                                                                 "int_backcash_rate", ok).toInt();
                if(false == ok) goto fastpayend;

                membernewparam member_info;
                if(!w_sys_manage_cloudsync::get_yun_member_INFO(vch_voucherno, member_info, errstring)) {
                    goto fastpayend;
                }
                vch_card_name = member_info.miVchMember;
                vch_card_adress = member_info.miVchAddress;


                //                vch_card_name = w_sys_manage_cloudsync::get_yun_member_readcard(this, vch_voucherno, n_func::f_get_sysparm("cloud_user"),
                //                                                                                "vch_member", ok);
                //                vch_card_adress = w_sys_manage_cloudsync::get_yun_member_readcard(this, vch_voucherno, n_func::f_get_sysparm("cloud_user"),
                //                                                                                 "vch_address", ok);
                //                if(false == ok) goto fastpayend;
            }
            if(paymodeno == "02") {//本地会员付款

                query.execSelect(QString("select num_amount, num_point  from t_m_curamount where vch_memberno = '%1' ").arg(vch_voucherno));
                query.next();
                num_amount=query.recordValue("num_amount").toString();
                num_card_point=query.recordValue("num_point").toDouble();

                query.execSelect(QString("SELECT b.int_backcash_rate, a.vch_member, a.vch_address from t_m_member a,  t_m_member_type b  where a.ch_typeno =b.ch_typeno and a.vch_memberno = '%1'; ")
                                 .arg(vch_voucherno));
                query.next();
                li_backcash_rate=query.recordValue("int_backcash_rate").toInt();

                vch_card_name=query.recordValue("vch_member").toString();
                vch_card_adress=query.recordValue("vch_address").toString();
            }
            if(vch_membernod.vch_memberno == vch_voucherno) {
                num_card_point += num_point_add;
            }

            ldec_payamt = paym.num_realamount;
            num_card_remain = num_amount.toDouble();
            num_card_remain -= ldec_payamt;
            ldec_backcash = ldec_payamt * (li_backcash_rate * 0.01);
            if(ldec_backcash != 0) {//付款返现 本地，并更新余额和积分
                num_card_remain += ldec_backcash;
                if(paymodeno == "02") {//本地会员付款

                    if(false == query.execInsert("t_m_deposit",qrtVariantPairList()

                                                 << qrtVariantPair("vch_memberno", vch_voucherno)
                                                 << qrtVariantPair("num_deposit", ldec_backcash)
                                                 << qrtVariantPair("num_realamount", 0)
                                                 << qrtVariantPair("ch_deposit_mode", 3)
                                                 << qrtVariantPair("ch_pay_mode", 5)

                                                 << qrtVariantPair("vch_operID", lds::gs_operid)
                                                 << qrtVariantPair("dt_operdate", pay_datetime)
                                                 << qrtVariantPair("vch_memo", "系统付款返现")
                                                 << qrtVariantPair("remain_amt", num_card_remain))) {

                        errstring = (QObject::tr("数据库错误") + QString::number(__LINE__));
                        goto fastpayend;
                    }
                }
                if(paymodeno == "10") { //云会员付款
                    memberDeposit o;

                    o.mdVchMemberNo=vch_voucherno;
                    o.mdNumDeposit=ldec_backcash;
                    o.mdNumRealaMount = 0;
                    o.mdChDepositMode="3";
                    o.mdChPayMode="5";

                    o.mdVchOperId=lds::gs_operid;
                    o.dtOperDate=pay_datetime;
                    o.mdVchMemo="";
                    o.mdRemainAmt=num_card_remain;
                    o.comChPayNo=muy.ch_payno;

                    muy.appitem(o);
                }
            }
            //t_m_curamount
            if(paymodeno == "10") { //云会员付款
                memberCuramount o;
                o.vchMemberno = vch_voucherno;
                o.numAmount = num_card_remain;
                //                o.numPoint;//新增的积分
                o.vchOperId=lds::gs_operid;
                o.dtOperdate=pay_datetime;
                muy.appitem(o);
            }
            if(paymodeno == "02") { //云会员付款
                if(false == query.execInsertDuplicateUpdate("t_m_curamount",
                                                            "vch_memberno",
                                                            qrtVariantPairList()
                                                            << qrtVariantPair("vch_memberno", vch_voucherno)
                                                            << qrtVariantPair("num_amount", num_card_remain)
                                                            << qrtVariantPair("vch_operID", lds::gs_operid)
                                                            << qrtVariantPair("dt_operdate", pay_datetime)
                                                            )) {
                    errstring = ( QObject::tr("数据库错误") + QString::number(__LINE__));
                    goto fastpayend;
                }
            }
        }
        ///云会员或本地会员付款---------end

        //会员优惠
        if(paymodeno == "ZZ") {
            if(false == query.execUpdate("cey_u_checkout_master", qrtVariantPairList()

                                         << qrtVariantPair("num_free", paym.num_realamount),
                                         QString(" ch_payno = '%1' ").arg(ch_payno))) {

                errstring = ( QObject::tr("数据库错误") + QString::number(__LINE__));
                goto fastpayend;
            }
        }
        //会员挂账
        if(paymodeno == "11") {
            double num_hangamount_cur;
            //1
            if(vch_voucherno.isEmpty()) {
                errstring = (QObject::tr("会员挂账") +QObject::tr("会员号不能为空"));
                goto fastpayend;
            }
            //2
            num_hangamount_cur = paym.num_realamount;

            //3 num_hangamount
            if(false == query.execInsertDuplicateUpdate("t_m_curamount",
                                                        "vch_memberno",
                                                        qrtVariantPairList()
                                                        << qrtVariantPair("vch_memberno", vch_voucherno)
                                                        << qrtVariantPair("vch_operID", lds::gs_operid)
                                                        << qrtVariantPair("dt_operdate", pay_datetime)
                                                        << qrtVariantPair("num_hangamount", qrtFunc("ifnull(num_hangamount, 0) + " + QString::number(num_hangamount_cur)))
                                                        )) {
                errstring = ( QObject::tr("数据库错误") + QString::number(__LINE__));
                goto fastpayend;
            }

            //4
            if(false == query.execInsert("t_m_hang_balance",qrtVariantPairList()

                                         << qrtVariantPair("ch_balanceno", ch_payno)
                                         << qrtVariantPair("vch_memberno", vch_voucherno)
                                         << qrtVariantPair("num_remain", paym.num_payamount)
                                         << qrtVariantPair("num_pay", paym.num_payamount)
                                         << qrtVariantPair("num_amount", paym.num_realamount)

                                         << qrtVariantPair("vch_operID", lds::gs_operid)
                                         << qrtVariantPair("dt_operdate", pay_datetime)
                                         << qrtVariantPair("vch_memo", QObject::tr("会员挂账付款"))
                                         << qrtVariantPair("ch_state", "N")
                                         << qrtVariantPair("ch_paymodeno", "02")

                                         << qrtVariantPair("ch_type", ""))) {

                errstring = ( QObject::tr("数据库错误") + QString::number(__LINE__));
                goto fastpayend;
            }
        }

        if(false == query.execInsert("cey_u_checkout_detail",qrtVariantPairList()

                                     << qrtVariantPair("ch_payno", ch_payno)
                                     << qrtVariantPair("ch_paymodeno", paymodeno)
                                     << qrtVariantPair("num_payamount", paym.num_payamount)
                                     << qrtVariantPair("num_change", 0)
                                     << qrtVariantPair("num_realamount", paym.num_realamount)

                                     << qrtVariantPair("num_face", 0)
                                     << qrtVariantPair("vch_operID", lds::gs_operid)
                                     << qrtVariantPair("dt_operdate", pay_datetime)

                                     << qrtVariantPair("num_card_remain", num_card_remain)
                                     << qrtVariantPair("vch_voucherno", vch_voucherno)
                                     << qrtVariantPair("vch_card_name", vch_card_name)
                                     << qrtVariantPair("vch_card_adress", vch_card_adress)
                                     << qrtVariantPair("num_card_point", num_card_point)

                                     << qrtVariantPair("num_face_Num", 0)
                                     << qrtVariantPair("vch_memo", paym.vch_memo))) {

            errstring = QObject::tr("数据库错误") + QString::number(__LINE__);
            goto fastpayend;
        }
    }

    ///若是使用了折扣卡，则会员积分归该折扣卡---------------------begin
    /// 本地不在保留云会员的积分信息，都有云端自己处理
    /// 付款会员的积分默认=0
    if(false == vch_membernod.vch_memberno.isEmpty()) {
        //t_m_curamount
        if(vch_membernod.isyun()) {
            double num_amount;
            bool ok;
            num_amount=w_sys_manage_cloudsync::get_yun_member_readcard(this, vch_membernod.vch_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                       "num_amount", ok).toDouble();if(false == ok) goto fastpayend;
            {memberCuramount o;
                int k;
                o.vchMemberno = vch_membernod.vch_memberno;
                o.numAmount = num_amount;
                o.numPoint = num_point_add;//新增的积分
                o.vchOperId = lds::gs_operid;
                o.dtOperdate = pay_datetime;
                if((k = muy.finditem(o)) >= 0) {
                    muy.memberCuramountList[k].numPoint = o.numPoint;
                } else {
                    muy.appitem(o);
                }
            }
            {
                memberPointList o;
                o.mpChPayNo = muy.ch_payno;
                o.mpChType = "3";
                o.mpDtOperDate =pay_datetime;
                o.mpIntFlowId = 0;
                o.mpNumPoint = num_point_add;
                o.mpVchMemberNo = vch_membernod.vch_memberno;
                o.mpVchMemo = "";
                o.mpVchOperId = lds::gs_operid;
                o.orgCode = n_func::f_get_sysparm("orgCode");
                muy.appitem(o);
            }
        } else {
            if(false == query.execInsertDuplicateUpdate("t_m_curamount",
                                                        "vch_memberno",
                                                        qrtVariantPairList()
                                                        << qrtVariantPair("vch_memberno", vch_membernod.vch_memberno)
                                                        << qrtVariantPair("vch_operID", lds::gs_operid)
                                                        << qrtVariantPair("dt_operdate", pay_datetime)
                                                        << qrtVariantPair("num_point", qrtFunc("ifnull(num_point, 0) + " + QString::number(num_point_add)))
                                                        )) {
                errstring = ( QObject::tr("数据库错误") + QString::number(__LINE__));
                goto fastpayend;
            }

            //t_m_point
            if(num_point_add != 0) {
                if(false == query.execInsert("t_m_point",qrtVariantPairList()

                                             << qrtVariantPair("vch_memberno", vch_membernod.vch_memberno)
                                             << qrtVariantPair("ch_type", 3)
                                             << qrtVariantPair("ch_payno", ch_payno)
                                             << qrtVariantPair("num_point", num_point_add)
                                             << qrtVariantPair("vch_memo", "")

                                             << qrtVariantPair("vch_operID", lds::gs_operid)
                                             << qrtVariantPair("dt_operdate", pay_datetime))) {
                    errstring = ( QObject::tr("数据库错误") + QString::number(__LINE__));
                    goto fastpayend;
                }
            }
        }
    }
    ///若是使用了折扣卡，则会员积分归该折扣卡---------------------end

    ///付款方式------------end
    ///
    if(muy.isrun) {
        yunpayData d = w_sys_manage_cloudsync::upload_yun_pay_info(this, muy, ch_payno);
        if("isOk" == d.message) {
            lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("本地已经支付") +":" + ch_payno);
            goto fastpayend;
        }
        if("ok" != d.message) {
            lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("付款失败"));
            goto fastpayend;
        }
        //云会员的话，默认本次上传成功
        if(false == w_sys_manage_cloudsync_with_time_running::q_u_put_by(ch_payno)) {
            goto fastpayend;
        }
    }

    pay_finished = true;
    goto fastpayend;
fastpayend:
    w_m_member_list::yun_member_close();
    return;
}

void w_scr_dish_pay_widget::dataupload(const QString &ch_payno, const QDateTime &dt)
{
    ///dataupload
    double dish_num = pay_keep_data.num;
    QString ch_billno = model_orderdish->cur_ch_billno();

    //tabwidget选中了科传接口
    //checkbox勾选了启用
    if(n_func::f_get_sysparm("njgj_use") == "1") {///采集数据
        //现金付款
        QList<w_scr_dish::TECH_TRANS_NJGJ_tendercode> tendercodes;
        if(ui->lineEdit_moneypay->value() != 0) {
            tendercodes.append(w_scr_dish::TECH_TRANS_NJGJ_tendercode("CH", ui->lineEdit_moneypay->value()));
        }
        //银行卡
        double bank_pay = 0;
        for(int row=0,rowcount=payModel()->rowCount();row<rowcount;row++) {
            if(payModel()->model_data(row, "ch_paymodeno").toString() == "03") {
                bank_pay += payModel()->model_data(row, "num_payamount").toDouble();
            }
        }
        if(bank_pay != 0) {
            tendercodes.append(w_scr_dish::TECH_TRANS_NJGJ_tendercode("CI", (bank_pay)));
        }

        double other_pay = ui->lineEdit_realreceive->value() - ui->lineEdit_moneypay->value() - bank_pay;

        //其他付款
        if(other_pay != 0) {
            tendercodes.append(w_scr_dish::TECH_TRANS_NJGJ_tendercode("OT",(other_pay)));
        }
        //上传
        w_scr_dish::TECH_TRANS_NJGJ_sql(
                    dt.toString("yyyyMMdd"),
                    dt.toString("hhmmss"),
                    "01",
                    ch_billno,
                    lds::gs_operid,
                    lds::gs_opername,
                    QString::number(dish_num),
                    ui->lineEdit_realreceive->value(),
                    tendercodes
                    );

    }///～采集数据
    //星摩尔
    if(n_func::f_get_sysparm("isoft_use") == "1") {
        business_data_upload_isoft::genrateBill(ch_payno);
        public_sql::save_login->isoft_upload_polling->ToTryTrigger();
    }
    //~星摩尔

    //辽宁顶有
    if(n_func::f_get_sysparm("liaoning_inventory_keep") == "1") {
        w_scr_dish_liaoning_inventory_keep_thread_event::genrateBill(ch_payno);
        public_sql::save_login->liaoning_inventory_keep_polling->ToTryTrigger();
    }
    //~辽宁顶有

    //丰泽
    if(n_func::f_get_sysparm("fengze_interface") == "1") {
        w_sys_manage_outer_pay_set_fengze::genrateBill(ch_payno, pay_keep_data.dish_num(), pay_keep_data.dish_total());
        public_sql::save_login->fengze_upload_polling->ToTryTrigger();
    }
    //~丰泽
    //restful_interface
    if(n_func::f_get_sysparm("restful_interface") == "1") {
        business_data_upload_restful::genrateBill(ch_payno);
        public_sql::save_login->restful_upload_polling->ToTryTrigger();
    }
    //~restful_interface
    //hourly_pos_interface_enable
    //    if(n_func::f_get_sysparm("hourly_pos_interface_enable") == "1") {
    //        if(lds::terminalCode == n_func::f_get_sysparm("api_hourly_pos_machine")) {
    //            if(public_sql::candoPerday(dt,
    //                                       QTime::fromString(n_func::f_get_sysparm("api_hourly_pos_time"), "hh:mm:ss"),
    //                                       60,
    //                                       "api_hourly_pos_")) {
    //                public_sql::save_login->api_malaysia_hourly_polling->ToTryTrigger();
    //            }
    //        }
    //    }
    //~hourly_pos_interface_enable
}

/**/
bool w_scr_dish_pay_widget::get_member_point(const QString &ch_payno, double &num_point, QString &errstring)
{
    QString vch_memberno;
    QString ch_member_belong_state;
    QString ch_typeno;
    QString ch_calc;
    QString ch_calc_type;
    int int_calc_uint;
    QString ch_area_flag;
    double dec_membercost = 0;
    double dec_sub_income = 0;
    double num_cost = 0;
    double num_cash = 0;
    lds_query_hddpos  query;
    num_point = 0;

    query.execSelect(QString(" select vch_memberno, ch_member_belong_state, num_cost, num_cash from cey_u_checkout_master where ch_payno = '%1'  ").arg(ch_payno));
    query.next();
    vch_memberno = query.recordValue("vch_memberno").toString();
    ch_member_belong_state = query.recordValue("ch_member_belong_state").toString();
    num_cost = query.recordValue("num_cost").toDouble();
    num_cash = query.recordValue("num_cash").toDouble();

    if(vch_memberno.length() == 0) {
        return true;
    }

    if("Y" == ch_member_belong_state) {
        if(false == w_sys_manage_cloudsync::get_yun_member_readcard(vch_memberno, n_func::f_get_sysparm("cloud_user"),
                                                                    "ch_typeno", ch_typeno, errstring))
            return false;

        if(false == w_sys_manage_cloudsync::get_yun_member_membertype(ch_typeno,
                                                                      "chCalc", ch_calc, errstring))
            return false;

        if(false == w_sys_manage_cloudsync::get_yun_member_membertype(ch_typeno,
                                                                      "chCalcType", ch_calc_type, errstring))
            return false;

        QString int_calc_uint_str;
        if(false == w_sys_manage_cloudsync::get_yun_member_membertype(ch_typeno,
                                                                      "intCalcUnit", int_calc_uint_str, errstring))
            return false;
        int_calc_uint = int_calc_uint_str.toInt();

        if(false == w_sys_manage_cloudsync::get_yun_member_membertype(ch_typeno,
                                                                      "chAreaFlag", ch_area_flag, errstring))
            return false;
    } else {
        query.execSelect(QString("select ch_typeno from t_m_member where vch_memberno = '%1' ").arg(vch_memberno));
        query.next();
        ch_typeno = query.recordValue(0).toString();//得到类型编号
        query.execSelect(QString("select ch_calc,ch_calc_type,int_calc_unit,ch_area_flag from t_m_member_type where ch_typeno = '%1' ").arg(ch_typeno));
        query.next();
        ch_calc=query.recordValue("ch_calc").toString();
        ch_calc_type=query.recordValue("ch_calc_type").toString();
        int_calc_uint=query.recordValue("int_calc_unit").toInt();
        ch_area_flag=query.recordValue("ch_area_flag").toString();
    }

    if(ch_calc == "Y") {
        switch(ch_area_flag.toInt()) {
        case 0://消费额
            dec_membercost=num_cost;
            break;
        case 2://非营业收入的积分
            query.execSelect(QString(" select b.ch_incomeflag, a.num_realamount  from cey_u_checkout_detail a, cey_bt_paymode b where a.ch_paymodeno = b.ch_paymodeno and a.ch_payno =  '%1' ").arg(ch_payno));
            while(query.next()) {
                if(query.recordValue("ch_incomeflag").toString() != "Y") {
                    dec_sub_income += query.recordValue("num_realamount").toDouble();
                }
            }
        case 1://折后额或卡付款
            dec_membercost = num_cash;
            dec_membercost -= dec_sub_income;
            break;
        }
        if(int_calc_uint == 0) {
            num_point = 0;
        } else {
            if(ch_calc_type == "0") {//小数
                num_point = dec_membercost/int_calc_uint;
            } else if(ch_calc_type == "1") {//取整
                num_point = qRound(dec_membercost / int_calc_uint);
            }
        }
    }
    return true;
}

//void w_scr_dish_pay_widget::printLabel(const QString &ch_billno)
//{
//    //    if(pay_operate_mode & TYPE_FAST)
//    {
//        public_printby_ch_billno::print_label(ch_billno, int_flowid_greater_than);
//    }
//}

void w_scr_dish_pay_widget::negativeallControl()
{
    //付款方式
    for(int row = 0, rowcount = payModel()->rowCount(); row < rowcount; row++) {
        payModel()->model_data_set(row, "num_realamount", -1*payModel()->model_data(row, "num_realamount").toDouble());
        payModel()->model_data_set(row, "num_payamount", -1*payModel()->model_data(row, "num_payamount").toDouble());
    }
    //各种消费详细
    negativeallControl(ui->lineEdit_blotout);
    negativeallControl(ui->lineEdit_change);
    //    ui->lineEdit_ch_billno;
    //    ui->lineEdit_ch_tableno;

    negativeallControl(ui->lineEdit_consum);
    negativeallControl(ui->lineEdit_discount);
    //    ui->lineEdit_int_person;
    negativeallControl(ui->lineEdit_int_rate);
    negativeallControl(ui->lineEdit_lowcost);

    negativeallControl(ui->lineEdit_moneypay);
    negativeallControl(ui->lineEdit_present);
    negativeallControl(ui->lineEdit_realreceive);
    negativeallControl(ui->lineEdit_service);
    //    ui->lineEdit_vch_memo;

}
void w_scr_dish_pay_widget::negativeallControl(lds_label_doublespinbox *l)
{
    l->setValue(-1*l->value());
}

void w_scr_dish_pay_widget::toPayMasterWatch()
{
    lds_label_cell *cell = qobject_cast<lds_label_cell *>(this->sender());
    if(cell == 0)
        return;

    lds::DOUBLE_DATA ddata;
    ddata.udpKey = lds::udp_paying_request;
    ddata.paying_master = cJSON_help::mapTocJSONByteArray(
                ldsVariantMap()
                .append("no", cell->objectName().mid(9)) //lineEdit_tax_before => tax_before
                .append("name", cell->paintLabel())
                .append("value", cell->paintText())
                , true
                );
    public_sql::doublescreensenddataByconf(ddata);

    qDebug() << __FUNCTION__ <<  "paying_desc" << ddata.paying_desc;
    qDebug() << __FUNCTION__ <<  "paying_master" << ddata.paying_master;
    qDebug() << __FUNCTION__ <<  "paying_detail" << ddata.paying_detail;
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_tax_before));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_tax_cur));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_tax_cur_2));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_tax_cur_3));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_realreceive));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_change));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_moneypay));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_consum));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_present));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_discount));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_int_rate));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_blotout));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_ch_tableno));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_service));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_lowcost));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_vch_memo));
    //    ddata.pay_master.append(payMasterWatchPair(ui->lineEdit_int_person));
}

QPair<QString, QString> w_scr_dish_pay_widget::payMasterWatchPair(lds_label_cell *cell)
{
    return qMakePair(cell->paintLabel(), cell->paintText());
}

bool w_scr_dish_pay_widget::get_pay_finished()
{
    return pay_finished;
}

void w_scr_dish_pay_widget::updatePaylistCashpay()
{
    ui->lineEdit_moneypay->setValue(label_paylist_remain());
}

void w_scr_dish_pay_widget::keyPressEvent(QKeyEvent *e)
{
    const int kvalue = e->key();
    if( (kvalue >= Qt::Key_0 && kvalue <= Qt::Key_9)
            || kvalue == Qt::Key_Period) {
        ui->lineEdit_moneypay->setValue(e->text());

    } else  if(kvalue == Qt::Key_Delete
               || kvalue == Qt::Key_Backspace) {
        amountClear();
    } else if(kvalue == Qt::Key_F1) {
        public_short_cut_map dialog(this);
        dialog.showpay();
        dialog.exec();
    } else if(kvalue == Qt::Key_Slash) {
        todiscount();
    } else {
        toShortCutFunction(public_short_cut_map::get_desc_by_keyvalue(kvalue, public_short_cut_map::PAY_BEGIN, public_short_cut_map::PAY_END));
    }

    QWidget::keyPressEvent(e);
}

void w_scr_dish_pay_widget::toShortCutFunction(const QString &k)
{
    if(k == "WX") {
        tomobilepay();
    } else if(k == "ZFB") {
        toalipay();
    } else if(k == "YHK") {
        tobank();
    } else if(k == "HYK") {
        tomember();
    }

    else if(k == "CZ") {
        torecharge();
    } else if(k == "DJQ") {
        tovouchers();
    } else if(k == "MT") {
        tomeituan();
    } else if(k == "DZDP") {
        todianping();
    }

    else if(k == "QCZK") {
        todiscount();
    } else if(k == "YH") {
        toBillFree();
    } else if(k == "HYGZ") {
        toMemberHang();
    }
}

void w_scr_dish_pay_widget::resetPaySection()
{
    /////////////////////////////////////////////////////
    if(pay_section.isEmpty()) {
        pay_section.append(ui->pushButton_mobile_pay);
        pay_section.append(ui->pushButton_alipay);
        pay_section.append(ui->pushButton_bank);
        pay_section.append(ui->pushButton_member);
        pay_section.append(ui->pushButton_memberrecharge);

        pay_section.append(ui->pushButton_vouchers);
        pay_section.append(ui->pushButton_meituan);
        pay_section.append(ui->pushButton_youhui);
        pay_section.append(ui->pushButton_discount);
        pay_section.append(ui->pushButton_cancel);

        pay_section.append(ui->pushButton_waimaiteam);
        pay_section.append(ui->pushButton_other);

        for(int k = 0; k < pay_section.count(); k++) {
            connect(pay_section[k], SIGNAL(timeout()),this,SLOT(toModuleSelect()));
            connect(pay_section[k], SIGNAL(clicked()),this,SLOT(toPaySectionModule()));
            pay_section[k]->enableLongPress(true);
        }
    }
    QStringList pay_section_record = lds::sysconf->value("w_scr_dish_pay_widget/pay_section", QStringList() << "AA"<< "03"<< "hyk"<< "zk"<< "ZZ"<< "qx").toStringList();
    for(int k = 0; k < pay_section.count(); k++) {
        const QString p = pay_section_record.value(k);
        if(w_scr_dish_pay_module_select::pay_section_valid(p, pay_operate_mode)) {
            pay_section[k]->setData(w_scr_dish_pay_module_select::modeule_key, p);
            pay_section[k]->setText(w_scr_dish_pay_module_select::pay_module_list_value(p, pay_operate_mode));
            updateReceiptCheck(pay_section[k]);
        }
    }
}

lds_pushbutton_wrap *w_scr_dish_pay_widget::paySectionValue(const QString &key)
{
    for(int k = 0; k < pay_section.count(); k++) {
        lds_pushbutton_wrap *p = pay_section[k];
        if(p->getData(w_scr_dish_pay_module_select::modeule_key).toString() == key) {
            return p;
        }
    }
    return 0;
}

void w_scr_dish_pay_widget::savePayModuleFile()
{
    QStringList list;
    foreach(lds_pushbutton_wrap *p, pay_section) {
        QString k = p->getData(w_scr_dish_pay_module_select::modeule_key).toString();
        list << k;
    }
    lds::sysconf->setValue("w_scr_dish_pay_widget/pay_section", list);
}

bool w_scr_dish_pay_widget::checkNumFree(const QString &id, double num_free)
{
    QString errstring;
    if(id == "0000")
        return true;
    double num_free_min = lds_query_hddpos::selectValue("select num_free from cey_sys_operator where vch_operID = '%1' ", id).toDouble();
    if(num_free <= num_free_min)
        return true;

    switch(lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("优惠金额") +QString("(%1)").arg(num_free) +("超出了优惠限额") +QString("(%1)").arg(num_free_min), QObject::tr("指定新的操作员"), QObject::tr("取消"))) {
    case 0:
    {
        QString newid;
        if(getNewOperid(errstring, newid)) {
            if(newid == "0000") return true;
            num_free_min = lds_query_hddpos::selectValue("select num_free from cey_sys_operator where vch_operID = '%1' ", newid).toDouble();
            if(num_free <= num_free_min) {
                return true;
            }
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("优惠金额") +QString("(%1)").arg(num_free) +("超出了优惠限额") +QString("(%1)").arg(num_free_min));
        }
        if(false == errstring.isEmpty()) {
            lds_messagebox::warning(this,  MESSAGE_TITLE_VOID, errstring);
        }
    }
        break;
    case 1:
        break;
    }
    return false;
}

bool w_scr_dish_pay_widget::getNewOperid(QString &errstring, QString &id)
{
    //匹配登录账号失败后，需要输入人工密码
    lds_dialog_input inputdialog(this);
    inputdialog.showSecondLineEdt();
    inputdialog.ui->label->setText(QObject::tr("操作员代码"));
    inputdialog.ui->label_2->setText(QObject::tr("操作密码"));
    inputdialog.ui->lineEdit_2->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(0000);
    inputdialog.ui->lineEdit_2->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(QObject::tr("录入新的操作员"));
    if(QDialog::Accepted ==transparentCenterDialog(&inputdialog).exec()) {
        QString  ls_operid_new = inputdialog.ui->lineEdit->text();
        QString  ls_operpassword = inputdialog.ui->lineEdit_2->text();
        errstring = n_func::f_confirm_user_pwd(ls_operid_new, ls_operpassword);
        if(errstring.isEmpty()) {
            id = ls_operid_new;
            errstring = "";
            return true;
        }
    }
    return false;
}

void w_scr_dish_pay_widget::updateReceiptCheck(lds_pushbutton_wrap *b)
{
    setReceiptCheck(b, b->getData(w_scr_dish_pay_module_select::modeule_key).toString());
}

void w_scr_dish_pay_widget::setReceiptCheck(lds_pushbutton_wrap *b, bool flag)
{
    QPixmap p = flag ? QPixmap(":/image/widgets_pic/check_on.png") : QPixmap(":/image/widgets_pic/check_off.png");
    p = p.scaledToHeight(ui->frame_pay_key->height() / 4 / 3);
    b->show_pixmap(Qt::AlignTop | Qt::AlignRight, p);
}

void w_scr_dish_pay_widget::setReceiptCheck(lds_pushbutton_wrap *b, const QString &keystring)
{
    if("dyxp" != keystring) {
        b->show_pixmap(Qt::AlignTop | Qt::AlignRight, QPixmap());
        return;
    }
    QString check = lds::sysconf->value("system_setting/maidanzhangdan").toString();
    setReceiptCheck(b, check == "1");
}

void w_scr_dish_pay_widget::tostackinfo(bool f)
{
    if(f)
        tostackinfo(this->sender());
}

void w_scr_dish_pay_widget::tostackinfo(QObject *sender)
{
    if(ui->radioButton_payinfo == sender) {
        lds::showWidget(ui->frame_stack0);
        lds::hideWidget(ui->frame_stack1);
        lds::hideWidget(ui->frame_stack2);
    }
    if(ui->radioButton_tableinfo == sender) {
        lds::hideWidget(ui->frame_stack0);
        lds::showWidget(ui->frame_stack1);
        lds::hideWidget(ui->frame_stack2);
    }
    if(ui->radioButton_taxinfo == sender) {
        lds::hideWidget(ui->frame_stack0);
        lds::hideWidget(ui->frame_stack1);
        lds::showWidget(ui->frame_stack2);
    }
}

lds_model_sqltablemodel *w_scr_dish_pay_widget::payModel()
{
    if(model_paydetail == 0) {
        QList<int> show_colums;
        model_paydetail = new lds_model_sqltablemodel(this);
        model_paydetail->removeRows(0, model_paydetail->rowCount());
        model_paydetail->setTable("cey_u_checkout_detail");
        model_paydetail->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
        model_paydetail->save_set_header("ch_paymodeno", QObject::tr("付款类型"));show_colums << model_paydetail->fieldIndex("ch_paymodeno");
        //        model_paydetail->save_set_header("num_payamount", QObject::tr("付款金额"));show_colums << model_paydetail->fieldIndex("num_payamount");
        model_paydetail->save_set_header("num_realamount", QObject::tr("实付金额"));show_colums << model_paydetail->fieldIndex("num_realamount");
        model_paydetail->save_set_header("vch_voucherno", QObject::tr("凭证号"));show_colums << model_paydetail->fieldIndex("vch_voucherno");
        //    model_paydetail->save_set_header("vch_memo", QObject::tr("备注"));

        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableView->setModel(model_paydetail);
        ui->tableView->justShowColumns(show_colums);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
        ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
        ui->tableView->setItemDelegateForColumn(model_paydetail->fieldIndex("ch_paymodeno"), new lds_model_sqltablemodel_delegate_com(
                                                    this, model_paydetail, "cey_bt_paymode", "ch_paymodeno", "vch_paymodename"));
    }
    return model_paydetail;
}

void w_scr_dish_pay_widget::payModelAppend(const ldsVariantMap &map)
{
    int row = payModel()->rowCount();
    payModel()->insertRow(row);
    QString ch_paymodeno = map.d.value("ch_paymodeno").toString();
    for(QMap<QString, QVariant>::const_iterator k = map.d.begin(); k != map.d.end(); k ++) {
        payModel()->model_data_set(row, k.key(), k.value());
        if(ch_paymodeno == "10") {
            payModel()->model_data_set(row, k.key(), QColor("green"), Qt::DecorationRole);
        }
    }

    lds::DOUBLE_DATA ddata;
    ddata.udpKey = lds::udp_paying_request;
    ddata.paying_detail = cJSON_help::mapTocJSONByteArray(
                ldsVariantMap()
                .append("no", ch_paymodeno)
                .append("name", model_orderdish->tablename_kvmap_value("cey_bt_paymode", ch_paymodeno, "ch_paymodeno", "vch_paymodename").toString())
                .append("value", lds::septNumber(map.d.value("num_realamount").toDouble()))
                , true
                );
    public_sql::doublescreensenddataByconf(ddata);

    qDebug() << __FUNCTION__ <<  "paying_desc" << ddata.paying_desc;
    qDebug() << __FUNCTION__ <<  "paying_master" << ddata.paying_master;
    qDebug() << __FUNCTION__ <<  "paying_detail" << ddata.paying_detail;
}

double w_scr_dish_pay_widget::pay_without_low_service()
{
    QString ch_billno = model_orderdish->cur_ch_billno();
    int ch_tax_mode = model_orderdish->get_cey_u_taxsale_master_value(ch_billno, "ch_tax_mode").toInt();
    return fexpandmain_model_sqltablemodel::pay_without_low_service(ch_tax_mode,
                                                                    ui->lineEdit_consum->value(),  ui->lineEdit_present->value() ,  ui->lineEdit_discount->value(),  ui->lineEdit_int_rate->value());
}

double w_scr_dish_pay_widget::hasPaid()
{
    double paid = 0;
    for(int row = 0; model_paydetail != 0 && row < payModel()->rowCount(); row ++) {
        paid+=payModel()->model_data(row, "num_realamount").toDouble();
    }
    return paid;
}

QString w_scr_dish_pay_widget::label_paylist_remain_str()
{
    return QString::number(label_paylist_remain());
}

double w_scr_dish_pay_widget::label_paylist_remain()
{
    return ui->lineEdit_realreceive->value() - hasPaid();
}

bool w_scr_dish_pay_widget::canQuitbill(QString key)
{
    lds_query_hddpos  query;
    QStringList paymode_S;
    //自定义的付款方式
    query.execSelect("select ch_paymodeno from cey_bt_paymode where vch_arg4 = 'S' ");
    while(query.next()) {
        paymode_S.append( query.recordValue(0).toString() );
    }

    //
    if(     "hyk" == key//会员卡
            ||"cz" == key//充值
            ||"zk" == key//折扣
            ||"jssz" == key//结算设置
            ||"dyxp" == key//结算设置
            ||"mdxc" == key//免低消差
            ||"mfwf" == key//免服务费
            ||"qx" == key//取消

            ||"00" == key//现金
            ||"02" == key//会员卡
            ||"10" == key//云会员卡
            ||"04" == key//代金券
            ||"03" == key//银行卡
            ||"ZZ" == key//银行卡
            ||"YY" == key//抹零
            ||paymode_S.contains(key)
            ) {
        return true;
    } else {
        return false;
    }
}

#include "w_scr_dish_member.h"
#include "ui_w_scr_dish_member.h"
#include "uo_rw_member.h"
#include "lds_query_hddpos.h"
#include <QSqlError>
#include "n_func.h"
#include "w_m_member_list.h"
#include "fexpandmain_2.h"
#include "w_sys_manage_cloudsync.h"
#include "lds_messagebox.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "public_sql.h"
#include "fast_pay_detail_dialog.h"
#include "ui_w_scr_dish_pay_widget.h"
#include "public_logindialog.h"
#include "ui_fast_pay_detail_dialog.h"

w_scr_dish_member::w_scr_dish_member(w_scr_dish_pay_widget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_member)
{
    ui->setupUi(this);
    _parent = parent;
    connect(ui->pushButton_read, SIGNAL(clicked()),this,SLOT(toread()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_more,  SIGNAL(clicked()),this,SLOT(tomore()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_get_phone_code, SIGNAL(clicked()), this, SLOT(to_get_phone_code()));
    connect(ui->lineEdit_verify_phone_code, SIGNAL(textChanged(QString)), this, SLOT(to_update_verify_state(QString)));
    connect(public_sql::save_login, SIGNAL(signalTimeout_1s()), this, SLOT(verify_phone_code_check()));
    isfirst = true;

    ui->pushButton_get_phone_code->setText(QObject::tr("获取验证码"));
    ui->label_desc->setText("");
    ui->doubleSpinBox_num_point->setEnabled(false);
    ui->lineEdit_num_remain->setEnabled(false);
    ui->lineEdit_num_baseamount->setEnabled(false);

    show_phone_verify(false);
    //
    QTimer::singleShot(50, this,SLOT(toread()));
}

w_scr_dish_member::~w_scr_dish_member()
{
    delete ui;
}

void w_scr_dish_member::always_disable_num_pay()
{
    b_always_disable_num_pay = true;
}

void w_scr_dish_member::took()
{
    if(ui->lineEdit_vch_memberno->text().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有读卡"));
        return;
    }

#ifdef QT_DEBUG
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, "DEBUG MODEL. SKIP PHONE CODE CHECK");
#else
    if(lds::widgetIsShow(ui->lineEdit_verify_phone_code)) {
        if(verify_rand_code.length() == 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请先发送验证码"));
            return;
        }
        if(ui->lineEdit_verify_phone_code->text() != verify_rand_code) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("验证码错误"));
            return;
        }
    }
#endif

    if(!(_parent->pay_operate_mode & w_scr_dish_pay_widget::OPERAT_QUITBILL)) {
        if(cardinfo.ch_attribute == "2") {
            if(ui->lineEdit_num_pay->value() > (ui->lineEdit_num_remain->value() - ui->lineEdit_num_baseamount->value())) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("卡余额(除去保本金额)不足,付款失败"));
                return;
            }
            if(int(ui->lineEdit_num_pay->value() * 100)/100.0   > _parent->ui->lineEdit_realreceive->value())
            {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("付款金额大于结帐金额"));
                return;
            }
        }
    }
    this->accept();
}

void w_scr_dish_member::tocancel()
{
    this->reject();
}

void w_scr_dish_member::toread()
{
    w_m_member_list_yun_auto_close d;

    if(!isfirst) {
        //!还原
        _parent->m_restore();
        //!～还原
        _parent->refreshControl();
        //!保存
        _parent->m_save();
        //!～保存
    }

    uo_rw_member uo_member;
    QString cardno;
    bool skip_discount = false; //跳过优惠
    vch_membernoData mdata = _parent->vch_membernod;

    ///判断已读卡而且已经付款
    if(mdata.vch_memberno.isEmpty())
        goto normalreadcard;
    ui->label_desc->setStyleSheet("color:"+mdata.color());
    ui->label_desc->setText(mdata.desc());

    skip_discount = true;//当前会员卡已做优惠，跳过优惠处理
    if(isfirst) {
        if(mdata.ch_attribute == "2") {//充值卡
            //该卡已经付过款
            for(int row = 0, rowcount = _parent->payModel()->rowCount(); row < rowcount; row++) {
                if(mdata.vch_memberno == _parent->payModel()->model_data(row, "vch_voucherno").toString()) {
                    goto normalreadcard;
                }
            }

            if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("存在已读但是没有付款的会员卡%1,是否使用该会员卡直接付款")
                                             .arg(mdata.vch_memberno), QObject::tr("确定"), QObject::tr("取消"))) {
                if(mdata.isyun()) {
                    d.yunOpen();//即将读卡
                }
                cardno = mdata.vch_memberno;
                goto checkcardstate;
            } else {
                //                isfirst = false;//若是已经
                goto end;
            }
        }
    }

    //判断已读卡而且已经付款
normalreadcard:
    //读卡
    if(!w_m_member_list::static_to_memeber_readcard_pay(this,
                                                        cardno,
                                                        uo_member,
                                                        mdata.mutil_choose())) {
        goto end;
    }
checkcardstate:
    //判断卡是否存在
    if(cardno.trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("无可查询的卡"));
        goto end;
    }
    //获取会员信息
    if(cardinfo.vch_memberno.isEmpty()
            || cardinfo.vch_memberno != cardno) {
        if(!n_func::qtcardinfo_readcard(cardinfo, cardno)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,cardinfo.errstring);
            goto end;
        }
    }
    //已经付过款
    if(mdata.ch_attribute == "1" && mdata.vch_memberno == cardno) {//优惠卡1，充值卡2
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("该会员卡已做过优惠"));
        goto end;
    }
    if(mdata.ch_attribute == "1" && cardinfo.ch_attribute == "1") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已有优惠卡优惠"));
        goto end;
    }
    if(mdata.ch_attribute == "2" && cardinfo.ch_attribute == "1") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已读会员卡"));
        goto end;
    }
    for(int row = 0,rowcount = _parent->payModel()->rowCount(); row < rowcount; row++) {
        if(_parent->payModel()->model_data(row, "vch_voucherno").toString() == cardno) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已经付过款"));
            goto end;
        }
    }
    //判断优惠卡
    if(cardinfo.ch_attribute == "1") {
        ui->lineEdit_num_remain->setVisible(false);
        ui->lineEdit_num_baseamount->setVisible(false);
        ui->lineEdit_num_pay->setEnabled(false);
        ui->label_4->setVisible(false);
        ui->label_7->setVisible(false);
    } else {
        ui->lineEdit_num_remain->setVisible(true);
        ui->lineEdit_num_baseamount->setVisible(true);
        ui->lineEdit_num_pay->setEnabled(true);
        ui->label_4->setVisible(true);
        ui->label_7->setVisible(true);
    }
    //判断密码
    if(!cardinfo.vch_password.isEmpty()) {
        lds_dialog_input inputdialog(this);
        inputdialog.ui->label->setText(QObject::tr("密码"));
        inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
        inputdialog.ui->pushButton_link->setText(QObject::tr("忘记密码"));
        inputdialog.ui->pushButton_link->setFocusPolicy(Qt::NoFocus);
        inputdialog.setWindowTitle(QObject::tr("请输入密钥"));
        inputdialog.showView(
                    lds_dialog_input::viewShow1 |
                    lds_dialog_input::viewShowLink |
                    lds_dialog_input::viewShowKey);
        connect(inputdialog.ui->pushButton_link, SIGNAL(clicked()), this, SLOT(topwdreset()));
        connect(inputdialog.ui->pushButton_link, SIGNAL(clicked()), inputdialog.ui->lineEdit, SLOT(clear()));
        forever{
            if(QDialog::Rejected == transparentCenterDialog(&inputdialog).exec()) {
                goto end;
            }
            if(cardinfo.vch_password == inputdialog.ui->lineEdit->text()) {
                break;
            }
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
            inputdialog.ui->lineEdit->clear();
        }
    }
    //卡信息审核
    if(cardinfo.vch_memberno.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此卡未发卡或不存在"));
        goto end;
    }
    if(cardinfo.ch_state == "4") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此卡已经作废"));
        goto end;
    }
    if(cardinfo.ch_state == "3") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此卡已经挂失"));
        goto end;
    }
    if(cardinfo.ch_state == "2") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此卡已经止付"));
        goto end;
    }
    if(cardinfo.ch_state.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("非本公司卡或无效卡"));
        goto end;
    }
    if(cardinfo.dt_limit <= n_func::f_get_sysdatetime()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此卡已经超过有效期"));
        goto end;
    }
    if(cardinfo.ch_cardflag != "Y") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员卡") + cardinfo.vch_memberno+QObject::tr("还未发卡"));
        goto end;
    }
    //核实折扣信息
    //if(!skip_discount)
    switch(cardinfo.ch_favourable.toInt()) {
    case 0://无优惠
        break;
    case 1://会员价
        if(cardinfo.ch_pricekind.isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有设置会员价")+","+QObject::tr("请到会员类型里修改"));
            goto end;
        }

        if(w_m_member_list::is_yun_member)
            ui->label_desc->setStyleSheet("color:green");
        else
            ui->label_desc->setStyleSheet("color:#ff546b");
        ui->label_desc->setText(cardinfo.desc());
        break;
    case 2://固定折扣
        if(cardinfo.int_discount.isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有设置会员折扣")+","+QObject::tr("请到会员类型里修改"));
            goto end;
        }
        _parent->pay_all_discount = cardinfo.int_discount.toInt();

        ui->label_desc->setStyleSheet("color:"+cardinfo.color());
        ui->label_desc->setText(cardinfo.desc());
        break;
    default://没有找到该会员优惠信息
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有找到该会员优惠信息"));
        goto end;
        break;
    }
    _parent->vch_membernod.init0(cardinfo.vch_memberno,
                                 cardinfo.vch_member,
                                 cardinfo.ch_member_belong_state,
                                 cardinfo.ch_favourable,
                                 cardinfo.int_discount.toInt(),
                                 cardinfo.ch_pricekind,
                                 cardinfo.ch_attribute,
                                 cardinfo.vch_typename
                                 );
    _parent->pay_keep_data =
            fexpandmain_model_sqltablemodel_data::get_keep_data_pay_by_isMPrice_intDiscount(
                _parent->model_orderdish,
                _parent->vch_membernod.is_m_price(),
                _parent->pay_all_discount);

    ui->lineEdit_num_baseamount->setValue(cardinfo.int_card_baseamount.toDouble());
    ui->doubleSpinBox_num_point->setValue(cardinfo.num_point);
    ui->lineEdit_num_remain->setValue(cardinfo.num_amount);
    ui->lineEdit_vch_handtel->setText(cardinfo.vch_handtel);
    ui->lineEdit_vch_typename->setText(cardinfo.vch_typename);
    ui->lineEdit_vch_member->setText(cardinfo.vch_member);
    ui->lineEdit_vch_memberno->setText(cardinfo.vch_memberno);

    ui->lineEdit_vch_handtel->setHidden(cardinfo.vch_handtel.isEmpty());
    ui->label_handtel->setHidden(cardinfo.vch_handtel.isEmpty());
    //获取付款金额
    _parent->refreshControl();
    ui->lineEdit_num_pay->setValue(_parent->label_paylist_remain());
    ui->lineEdit_num_pay->setEnabled(n_func::f_m_get_parameter(this,"deposit_pay_all","0") == "0");
    //
    show_phone_verify("1" == n_func::f_m_get_parameter(this, "vip_phone_verify", "0"));
    //
    ui->lineEdit_num_pay->setFocus();

    if(b_always_disable_num_pay) {
        ui->lineEdit_num_pay->setEnabled(false);
    }
    isfirst = false;
    return;
end:
    if(isfirst) {
        this->reject();
    }
    return;
}

void w_scr_dish_member::tomore()
{
    fast_pay_detail_Dialog dialog(this);
    dialog.setVisible(_parent->model_orderdish->cur_ch_tableno());
    dialog.ui->label_amount->setText(_parent->ui->lineEdit_consum->falseText());
    dialog.ui->label_blotout->setText(_parent->ui->lineEdit_blotout->falseText());
    dialog.ui->label_tax->setText(_parent->ui->lineEdit_int_rate->falseText());
    dialog.ui->label_discount->setText(_parent->ui->lineEdit_discount->falseText());
    dialog.ui->label_present->setText(_parent->ui->lineEdit_present->falseText());
    dialog.ui->label_rsv->setText(_parent->ui->lineEdit_realreceive->falseText());
    dialog.ui->label_servicecost->setText(_parent->ui->lineEdit_service->falseText());
    dialog.ui->label_lowcost->setText(_parent->ui->lineEdit_lowcost->falseText());
    if(!ui->lineEdit_num_pay->isEnabled()) dialog.ui->label_desc->setText(QObject::tr("充值卡付款必须全单付满"));
    if(b_always_disable_num_pay) dialog.ui->label_desc->setText(ui->label_desc->text() + "      "+  QObject::tr("会员付款") );
    dialog.setWindowTitle(QObject::tr("付款详情"));
    transparentCenterDialog(&dialog).exec();
}

void w_scr_dish_member::to_get_phone_code()
{
    lds_messagebox_loading_show loading(this, "", "LOADING");
    loading.show_delay();

    QString errstring;
    verify_rand_code = QString::number(rand()).rightJustified(4, '0', true);
    verify_phone_code_60s = 60;
#ifdef QT_DEBUG
    verify_phone_code_60s = 10;
#endif

    if(false == w_sys_manage_cloudsync::member_mobileNo_verify(ui->lineEdit_vch_handtel->text(), verify_rand_code, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("发送成功"));
    ui->pushButton_get_phone_code->setEnabled(false);
    ui->pushButton_read->setEnabled(false);
    ui->lineEdit_verify_phone_code->setFocus();
}

void w_scr_dish_member::to_update_verify_state(const QString &code)
{
    if(code == verify_rand_code) {
        ui->label_verify_state->setPixmap(QPixmap(":/image/btn_icon/ok.png"));
    } else {
        ui->label_verify_state->setPixmap(QPixmap(":/image/btn_icon/cancel.png"));
    }
}

void w_scr_dish_member::verify_phone_code_check()
{
    if(ui->pushButton_get_phone_code->isEnabled()) {
    } else {
        if(verify_phone_code_60s == 0) {
            ui->pushButton_get_phone_code->setEnabled(true);
            ui->pushButton_read->setEnabled(true);
            ui->pushButton_get_phone_code->setText(QObject::tr("获取验证码"));
        } else {
            ui->pushButton_get_phone_code->setText(QObject::tr("获取验证码") + "(" + QString::number(verify_phone_code_60s) +")");
            verify_phone_code_60s --;
        }
    }
}

void w_scr_dish_member::topwdreset()
{
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(QObject::tr("请输入密钥") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(QObject::tr("联系客服获取密钥"));
    if(QDialog::Rejected == transparentCenterDialog(&inputdialog).exec()) {
        return;
    }
    if(inputdialog.ui->lineEdit->text() != n_func::f_get_godpassword()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
        return;
    }
    w_m_member_pwdchange dialog(cardinfo.vch_memberno, this);
    dialog.enableForget();
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        cardinfo.vch_password = lds_query_hddpos::selectValue(QString("select ifnull(vch_password, '') from t_m_member where vch_memberno='%1'").arg(cardinfo.vch_memberno)).toString();
    }
}

void w_scr_dish_member::show_phone_verify(bool isshow)
{
    lds::showWidget(ui->label_verify, isshow);
    lds::showWidget(ui->label_verify_state, isshow);
    lds::showWidget(ui->lineEdit_verify_phone_code, isshow);
    lds::showWidget(ui->pushButton_get_phone_code, isshow);
    ui->lineEdit_vch_handtel->setEnabled(isshow);
}


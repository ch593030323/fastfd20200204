#include "w_m_member_list_yun_card_info.h"
#include "ui_w_m_member_sendcard_dialog.h"
#include "backheader.h"
#include "lds.h"
#include "w_m_member.h"
#include "n_func.h"
#include "reader_rf.h"
#ifdef Q_OS_WIN
#include "readcard_mwrf32.h"
#endif
#include "w_m_member_list.h"

w_m_member_list_yun_card_info::w_m_member_list_yun_card_info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_m_member_sendcard_Dialog)
{
    ui->setupUi(this);
    ui->tabWidget->removeTab(1);
    ui->tabWidget->removeTab(1);
    ui->pushButton_ch_typeno->setEnabled(false);

    ui->widget->setup(QStringList()<<QObject::tr("发卡")<<QObject::tr("取消"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("发卡")), SIGNAL(clicked()), this, SLOT(tosendCard()));
    connect(ui->widget->index_widget(QObject::tr("取消")), SIGNAL(clicked()), this, SLOT(tocancel()));
}

w_m_member_list_yun_card_info::~w_m_member_list_yun_card_info()
{
    delete ui;
}

bool w_m_member_list_yun_card_info::initData(const QString &vch_memberno, QString &errstring)
{
    sqldt = n_func::f_get_sysdatetime();
    if(vch_memberno.length() == 0) {
        errstring = QObject::tr("云会员编号为空");
        return false;
    }

    if(false == w_m_member::get_vip_card_type(is_cardtype, errstring)) {
        return false;
    }
    if(is_cardtype == "1"//磁卡
            || is_cardtype == "2"//IC卡
            ) {
    } else {
        errstring = QObject::tr("发卡类型为无会员卡");
        return false;
    }
    if(false == n_func::f_m_get_parameter("vip_ic_password", vip_ic_password, "", errstring)) {
        return false;
    }

    if(w_m_member_list::is_yun_member) {
        return initDataY(vch_memberno, errstring);
    }
    return initDataL(vch_memberno, errstring);
}

void w_m_member_list_yun_card_info::tosendCard()
{
    if(ui->lineEdit_vch_member->text().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员名称不能为空"));
        return;
    }
    if(ui->lineEdit_vch_handtel->text().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("手机号不能为空"));
        return ;
    }
    if(ui->lineEdit_vch_cardno->text().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("卡面卡号不能为空"));
        return ;
    }
    if(ui->dateTimeEdit_dt_limit_show->dateTime() < sqldt) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("有效期限不能小于当前日期"));
        return ;
    }

    if(w_m_member_list::is_yun_member) {
        if(sendcardY()) {
            this->accept();
        }
        return;
    }
    if(sendcardL()) {
        this->accept();
    }
}

void w_m_member_list_yun_card_info::tocancel()
{
    this->done(false);;
}

bool w_m_member_list_yun_card_info::initDataY(const QString &vch_memberno, QString &errstring)
{
    membernewparam param;

    QMap<QString, QVariant> comlist;
    if(false == w_sys_manage_cloudsync::get_yun_member_membertypelist(comlist, errstring)) {
        return false;
    }

    ui->label_record->setText("");
    ui->comboBox_ch_typeno->addItemsByMap(comlist);
    ui->comboBox_ch_state->addItemsByMap(w_m_member::get_VIP_MODEL_STATE());
    ui->comboBox_ch_cardflag->addItemsByModel(&w_m_member::get_VIP_MODEL_FLAG());
    ui->lineEdit_vch_cardno->setEnabled(false);
    ui->lineEdit_vch_memberno->setEnabled(false);
    ui->comboBox_ch_state->setEnabled(false);
    ui->comboBox_ch_cardflag->setEnabled(false);

    //
    if(w_sys_manage_cloudsync::get_yun_member_INFO(vch_memberno, param, errstring)) {

        ui->comboBox_ch_typeno->setEnabled(false);//若是已存在的话，会员类型不能修改

        if(param.miChState == "4") {
            errstring = QObject::tr("此会员已是作废状态");
            return false;
        }
        if(param.miChCardFlag == "Y") {
            errstring = QObject::tr("此会员已发卡");
            return false;
        }
        ui->lineEdit_vch_memberno->setText(param.miVchMemberNo);
        ui->lineEdit_vch_member->setText(param.miVchMember);
        ui->lineEdit_vch_cardno->setText(param.miVchCardNo);
        ui->comboBox_ch_typeno->setcomusrdata(param.miChTypeNo);
        ui->comboBox_ch_cardflag->setcomusrdata(param.miChCardFlag);
        ui->comboBox_ch_state->setcomusrdata(param.miChState);
        ui->lineEdit_vch_address->setText(param.miVchAddress);
        ui->lineEdit_vch_handtel->setText(param.miVchHandTel);
        ui->lineEdit_vch_id->setText(param.miVchCardNo);
        ui->lineEdit_vch_tel->setText(param.miVchTel);
        ui->dateTimeEdit_dt_birthday_show->setDateTime(QVariant(param.miDtBirthday).toDateTime());
        ui->dateTimeEdit_dt_limit_show->setDateTime(QVariant(param.miDtLimit).toDateTime());
        ui->dateTimeEdit_dt_operdate_show->setDateTime(QVariant(param.miDtOperDate).toDateTime());

        setWindowTitle(QObject::tr("云会员发卡"));
    } else {
        ui->lineEdit_vch_memberno->setText(vch_memberno);
        ui->lineEdit_vch_cardno->setText(vch_memberno);
        ui->dateTimeEdit_dt_birthday_show->setDateTime(sqldt);
        ui->dateTimeEdit_dt_limit_show->setDateTime(sqldt.addYears(1));
        ui->dateTimeEdit_dt_operdate_show->setDateTime(sqldt);

        setWindowTitle(QObject::tr("新增云会员"));
    }
    return true;
}

bool w_m_member_list_yun_card_info::initDataL(const QString &vch_memberno, QString &errstring)
{
    ui->comboBox_ch_state->addItemsByMap(w_m_member::get_VIP_MODEL_STATE());
    ui->comboBox_ch_cardflag->addItemsByModel(&w_m_member::get_VIP_MODEL_FLAG());
    ui->comboBox_ch_typeno->addItemsBySql(" select ch_typeno, vch_typename from t_m_member_type ");

    //    ui->lineEdit_vch_cardno->setEnabled(false);
    ui->lineEdit_vch_memberno->setEnabled(false);
    ui->comboBox_ch_state->setEnabled(false);
    ui->comboBox_ch_cardflag->setEnabled(false);

    lds_query_hddpos  query;
    query.execSelect(QString(" select * from t_m_member where vch_memberno = '%1' ")
                     .arg(vch_memberno));
    if(query.next()) {
        ui->comboBox_ch_typeno->setEnabled(false);//若是已发卡的话，会员类型不能修改

        if(query.recordValue("ch_state").toString() == "4") {
            errstring = QObject::tr("此会员已是作废状态");
            return false;
        }
        if(query.recordValue("ch_cardflag").toString() == "Y") {
            errstring = QObject::tr("此会员已发卡");
            return false;
        }

        ui->comboBox_ch_cardflag->setcomusrdata(query.recordValue("ch_cardflag"));
        ui->comboBox_ch_state->setcomusrdata(query.recordValue("ch_state"));
        ui->lineEdit_vch_address->setText(query.recordValue("vch_address").toString());
        ui->lineEdit_vch_cardno->setText(query.recordValue("vch_cardno").toString());
        ui->lineEdit_vch_handtel->setText(query.recordValue("vch_handtel").toString());;
        ui->lineEdit_vch_id->setText(query.recordValue("vch_id").toString());;
        ui->lineEdit_vch_member->setText(query.recordValue("vch_member").toString());;
        ui->lineEdit_vch_memberno->setText(query.recordValue("vch_memberno").toString());;
        ui->lineEdit_vch_operid->setText(query.recordValue("vch_operid").toString());;
        ui->lineEdit_vch_tel->setText(query.recordValue("vch_tel").toString());;
        ui->dateTimeEdit_dt_birthday_show->setDateTime(query.recordValue("dt_birthday").toDateTime());
        ui->dateTimeEdit_dt_limit_show->setDateTime(query.recordValue("dt_limit").toDateTime());
        ui->dateTimeEdit_dt_operdate_show->setDateTime(query.recordValue("dt_operdate").toDateTime());
        ui->comboBox_ch_typeno->setcomusrdata(query.recordValue("ch_typeno"));

        setWindowTitle(QObject::tr("会员发卡"));
    } else {
        ui->lineEdit_vch_memberno->setText(vch_memberno);
        ui->lineEdit_vch_cardno->setText(vch_memberno);
        ui->dateTimeEdit_dt_birthday_show->setDateTime(sqldt);
        ui->dateTimeEdit_dt_limit_show->setDateTime(sqldt.addYears(1));
        ui->dateTimeEdit_dt_operdate_show->setDateTime(sqldt);

        setWindowTitle(QObject::tr("新增会员"));
    }
    return true;
}

bool w_m_member_list_yun_card_info::sendcardY()
{
    QString errstring;
    membernewparam param;
    //新增
    param.miVchMemberNo = ui->lineEdit_vch_memberno->text();
    param.miVchMember = ui->lineEdit_vch_member->text();
    param.miChTypeNo = ui->comboBox_ch_typeno->comusrdata().toString();
    param.miVchId = ui->lineEdit_vch_id->text();

    param.miDtBirthday = ui->dateTimeEdit_dt_birthday_show->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    param.miVchTel = ui->lineEdit_vch_tel->text();
    param.miVchHandTel = ui->lineEdit_vch_handtel->text();
    param.miVchAddress = ui->lineEdit_vch_address->text();
    param.miDtLimit = ui->dateTimeEdit_dt_limit_show->dateTime().toString("yyyy-MM-dd hh:mm:ss");

    param.miChState = "1";
    param.miChStateStr = QObject::tr("正常");
    //                            param.numLimit;
    param.miChCardFlag = ui->comboBox_ch_cardflag->comusrdata().toString();
    param.miVchCardNo = ui->lineEdit_vch_cardno->text();

    //                            param.miVchPassword;
    param.miVchOperId = lds::gs_operid;
    param.miDtOperDate = sqldt.toString("yyyy-MM-dd hh:mm:ss");
    //                            param.miIntBasedata;
    //                            param.miIntBasedataStr;

    param.miDtSendTime = sqldt.toString("yyyy-MM-dd hh:mm:ss");
    param.orgCode = n_func::f_get_sysparm("orgCode");
    //                            param.remark;
    //                            param.idStr;

    param.operate = sqldt.toString("yyyy-MM-dd hh:mm:ss");

    //新增或修改资料
    if(false == w_sys_manage_cloudsync::get_yun_member_save(param, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return false;
    }

    if(param.miChCardFlag == "Y") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员已发卡"));
        return false;
    }


    if(is_cardtype == "1")
    {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("将IC卡放置读卡器上"));

        if(false == writeIC(param.miVchMemberNo, errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return false;
        }
    }
    if(false == w_sys_manage_cloudsync::get_yun_member_send(param.miVchMemberNo, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return false;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("发卡成功"));
    return true;
}

bool w_m_member_list_yun_card_info::sendcardL()
{
    QString errstring;
    lds_query_hddpos  query;
    int li_basedata;
    query.execSelect(QString("select int_card_baseamount  from t_m_member_type where ch_typeno ='%1'  ")
                     .arg(ui->comboBox_ch_typeno->curusrdata().toString()));
    query.next();
    li_basedata = query.recordValue("int_card_baseamount").toInt();
    //
    lds_query_hddpos::tran_saction();
    if(false == query.execInsertDuplicateUpdate("t_m_member",
                                                "vch_memberno",
                                                qrtVariantPairList()
                                                << qrtVariantPair("vch_memberno", ui->lineEdit_vch_memberno->text())
                                                << qrtVariantPair("vch_member", ui->lineEdit_vch_member->text())
                                                << qrtVariantPair("ch_typeno", ui->comboBox_ch_typeno->curusrdata())
                                                << qrtVariantPair("dt_limit", ui->dateTimeEdit_dt_limit_show->dateTime())
                                                << qrtVariantPair("num_limit", 0)

                                                << qrtVariantPair("vch_operID", lds::gs_operid)
                                                << qrtVariantPair("dt_operdate", ui->dateTimeEdit_dt_operdate_show->dateTime())
                                                << qrtVariantPair("vch_cardno", ui->lineEdit_vch_cardno->text())
                                                << qrtVariantPair("dt_birthday", ui->dateTimeEdit_dt_birthday_show->dateTime())
                                                << qrtVariantPair("vch_tel", ui->lineEdit_vch_tel->text())

                                                << qrtVariantPair("vch_handtel", ui->lineEdit_vch_handtel->text())
                                                << qrtVariantPair("vch_id", ui->lineEdit_vch_id->text())
                                                << qrtVariantPair("vch_address", ui->lineEdit_vch_address->text()),
                                                &errstring)) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return false;
    }

    query.execSelect(QString(" select ch_cardflag  from t_m_member where vch_memberno = '%1' ")
                     .arg(ui->lineEdit_vch_memberno->text()));
    query.next();
    if(query.recordValue("ch_cardflag").toString() == "Y") {
        lds_query_hddpos::com_mit();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员已发卡"));
        return false;
    }

    if(is_cardtype == "1")
    {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("将IC卡放置读卡器上"));

        if(false == writeIC(ui->lineEdit_vch_memberno->text(), errstring)) {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return false;
        }
    }

    if(false == query.execUpdate("t_m_member",
                                 qrtVariantPairList()
                                 << qrtVariantPair("ch_cardflag", "Y")
                                 << qrtVariantPair("dt_operdate", sqldt)
                                 << qrtVariantPair("int_basedata", li_basedata)
                                 << qrtVariantPair("dt_sendtime", sqldt),
                                 QString(" vch_memberno = '%1' ").arg(ui->lineEdit_vch_memberno->text()),
                                 &errstring)) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return false;
    }
    lds_query_hddpos::com_mit();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("发卡成功"));
    return true;
}

bool w_m_member_list_yun_card_info::writeIC(const QString &vch_memberno, QString &errstring)
{
    //IC卡需要写密码，并且写卡号
    QByteArray transferpasswd = n_func::f_m_get_cardpwdtransfer(vip_ic_password);
    if(vip_ic_password.isEmpty()) {
        errstring = QObject::tr("IC卡密码不能为空");
        return false;;
    }
#ifdef Q_OS_WIN
    ReadCard_mwrf32 reader;reader.setDefPwd(n_func::f_m_defcardpwd);//需要设置一次默认密码，应为changepwd会不完全清卡
#else
    Reader_RF reader;
#endif
    //修改密码成功
    if(false == reader.pwdChange(n_func::f_m_defcardpwd, transferpasswd)) {//0
        errstring = QObject::tr("修改密码失败");
        return false;;
    }
    reader.setDefPwd(transferpasswd);
    //写数据
    if(false == reader.writecard(vch_memberno, "", "")) {//1
        errstring = QObject::tr("写入卡号失败");
        return false;;
    }
    return true;
}

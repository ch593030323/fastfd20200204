#include "w_m_member_cardic.h"
//卡芯片清除
#include "ui_w_m_member_cardic_dialog.h"
#include "public_cika_read_dialog.h"
#include "reader_rf.h"
#include "backheader.h"
#include "lds_messagebox.h"
#include "w_m_member.h"
#include "backheader.h"
#include <QSqlError>
#include "w_m_member_list.h"
#ifdef Q_OS_WIN
#include "readcard_mwrf32.h"
#endif

w_m_member_cardic::w_m_member_cardic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_m_member_cardic_Dialog)
{
    ui->setupUi(this);
    ui->widget->setup(QStringList() /*<<QObject::tr("直接清除") */<< QObject::tr("读卡") << QObject::tr("清除") << QObject::tr("退出"),Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("读卡")), SIGNAL(clicked()),this,SLOT(to_read_card()));
    connect(ui->widget->index_widget(QObject::tr("清除")), SIGNAL(clicked()),this,SLOT(to_clear()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(to_exit()));
    //    connect(ui->widget->index_widget(QObject::tr("直接清除")), SIGNAL(clicked()),this,SLOT(to_dynamic_clear()));

    ui->lineEdit_ch_state->setEnabled(false);
    ui->lineEdit_memberno->setEnabled(false);
    ui->lineEdit_num_amount->setEnabled(false);
    ui->lineEdit_point->setEnabled(false);
}

void w_m_member_cardic::to_read_card()
{
    QString ls_value;
    QString cardno;
    QString cardno_tmp1;
    QString cardno_tmp2;
    QString as_memberno;

    if(!w_m_member_list::static_to_memeber_readcard(this, cardno, uo_member)) {
        ui->lineEdit_ch_state->clear();
        ui->lineEdit_memberno->clear();
        ui->lineEdit_num_amount->clear();
        ui->lineEdit_point->clear();
        return;
    }

    as_memberno = cardno;
    //    lds_query_hddpos  query;
    //    query.execSelect(QString("select vch_memberno from t_m_member where vch_cardno = '%1' ")
    //               .arg(cardno)
    //               );
    //    query.next();
    //    as_memberno=query.recordValue("vch_memberno").toString();
    //    if(as_memberno.isEmpty()) {
    //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("对应会员号为空"));
    //        return ;
    //    }

    lst_card=uo_member.of_read_card(this, as_memberno);
    if(lst_card.flag == false) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,lst_card.errtext);
        return;
    }
    lst_member=n_func::f_m_get_limit(this, lst_card.cardid);

    ui->lineEdit_ch_state->setText(w_m_member::get_VIP_MODEL_STATE().value(lst_member.as_m_state).toString());
    ui->lineEdit_memberno->setText(lst_card.cardid);
    ui->lineEdit_num_amount->setValue(lst_card.amount);
    ui->lineEdit_point->setValue(lst_card.point);
}

void w_m_member_cardic::to_clear()
{
    QString errtext;

    if(ui->lineEdit_memberno->text().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("卡号为空请先读卡"));
        return;
    }
    if(lst_member.as_m_state == "1" &&  lst_card.amount > 0) {//4
        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("此会员状态正常且尚有余额,你确定要初始化这张卡么?"), QObject::tr("确定"), QObject::tr("取消"))) {//3
        } else {
            return;
        }
    }
    //! <--transaction begin-->
    lds_query_hddpos::tran_saction();
    lds_query_hddpos  query;
    errtext = QObject::tr("操作失败");
    query.execSelect(QString("select ch_cardflag from t_m_member where vch_memberno = '%1' ").arg(lst_card.cardid));
    query.next();
    errtext = QObject::tr("会员卡未发卡");
    if(query.recordValue(0).toString() == "Y") {//3
        if(query.execUpdate("t_m_member",qrtVariantPairList()

                            << qrtVariantPair("ch_cardflag", "N")
                            << qrtVariantPair("vch_cardno", "")
                            << qrtVariantPair("int_basedata", 0),
                            QString(" vch_memberno = '%1' ").arg(lst_card.cardid))) {
            errtext = QObject::tr("操作失败");
            if((errtext=n_func::f_m_insert_log(lst_card.cardid, QObject::tr("清除芯片"), lst_card.cardid, 0)).isEmpty()) {//1

                //清除ic卡
                errtext = QObject::tr("卡类型即不是磁卡也不是IC卡");
                if(uo_member.is_cardtype == "1") {//0
                    QString icpasswd=n_func::f_m_get_parameter(this, "vip_ic_password");
                    QByteArray transferpasswd = n_func::f_m_get_cardpwdtransfer(icpasswd);
#ifdef Q_OS_WIN
    ReadCard_mwrf32 reader;reader.setDefPwd(n_func::f_m_defcardpwd);//需要设置一次默认密码，因为changepwd后不会完全清卡
#else
    Reader_RF reader;
#endif
                    if(reader.setDefPwd(transferpasswd)) {
                        if(reader.writecard("","","")) {
                            if(reader.pwdChange(transferpasswd, n_func::f_m_defcardpwd)) {
                                //操作成功
                                goto success;
                            }
                        }
                    }
                    errtext=reader.lastError();
                }//0

                //磁卡
                if(uo_member.is_cardtype == "2") {
                    goto success;
                }
            }//1
        }//2
    }//3
    //! <--transaction rollback-->
//failure:
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+errtext);
    goto end;
success:
    lds_query_hddpos::com_mit();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
end:
    ui->lineEdit_ch_state->clear();
    ui->lineEdit_memberno->clear();
    ui->lineEdit_num_amount->clear();
    ui->lineEdit_point->clear();
}

void w_m_member_cardic::to_exit()
{
    this->reject();
}

void w_m_member_cardic::to_dynamic_clear()
{
    QString icpasswd=n_func::f_m_get_parameter(this,"vip_ic_password");
    QByteArray transferpasswd = n_func::f_m_get_cardpwdtransfer(icpasswd);
#ifdef Q_OS_WIN
    ReadCard_mwrf32 reader;reader.setDefPwd(n_func::f_m_defcardpwd);//需要设置一次默认密码，因为changepwd后不会完全清卡
#else
    Reader_RF reader;
#endif
    if(reader.setDefPwd(transferpasswd)) {
        if(reader.writecard("","","")) {
            if(reader.pwdChange(transferpasswd, n_func::f_m_defcardpwd)) {
                //操作成功
                return;
            }
        }
    }
}

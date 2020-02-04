#include "w_sys_manage_operatorright_pwdchange.h"
#include "ui_w_sys_manage_operatorright_pwdchange.h"
#include "n_func.h"

w_sys_manage_operatorright_pwdchange::w_sys_manage_operatorright_pwdchange(QWidget *parent):
    QDialog(parent),
    ui(new Ui_w_sys_manage_operatorright_pwdchange)
{
    ui->setupUi(this);
    //! toolbar
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(toexit()));

    //! after
    ui->lineEdit_cur_operator->setEnabled(false);
    ui->lineEdit_newpwd->setEchoMode(QLineEdit::Password);
    ui->lineEdit_newpwd2->setEchoMode(QLineEdit::Password);
    ui->lineEdit_oldpwd->setEchoMode(QLineEdit::Password);
}

void w_sys_manage_operatorright_pwdchange::took()
{
    //clicked (none) returns long [pbm_bnclicked]
    //long ancestorreturnvalue
    QString ls_operid;
    QString ls_old;
    QString ls_new;
    QString ls_confirm;
    QString ls_err;
    lds_query_hddpos  query;

    ls_operid=ui->lineEdit_cur_operator->text();
    ls_old=ui->lineEdit_oldpwd->text();
    ls_new=ui->lineEdit_newpwd->text();
    ls_confirm=ui->lineEdit_newpwd2->text();

    //如果是自己修改自己的密码， 则必须输入旧密码
    if(lds::gs_operid == ls_operid) {
        if(ls_old.trimmed().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请输入旧密码"));
            return;
        }
    }
    //如果是 修改 admin的话，需要匹配旧密码
    if(ls_operid == "0000") {//0
        if(!n_func::f_cmp_syspwd(ls_old)) {// != n_func::f_get_sysparm("system_pass", "0000")) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("旧密码错误"));
            return;
        }
    }  else {
        //如果是admin 来修改 非admin密码的话
        if(lds::gs_operid == "0000") {
        } else {
            //如果是非admin 来修改 非admin密码的话
            query.execSelect(QString("Select count(0)  from cey_sys_operator  where vch_operID ='%1' and ifnull(vch_password,  '') = '%2'")
                             .arg(ls_operid)
                             .arg(ls_old));
            query.next();
            if(query.recordValue(0).toLongLong()==0) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("旧密码错误"));
                return;
            }
        }
    }//0


    //新密码不为空
    if(ls_new.trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("新密码不能为空"));
        return;
    }
    //新旧密码一致
    if(ls_new != ls_confirm) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("新密码与确认新密码不一致"));
        return;
    }
    //! <--transaction begin-->
    lds_query_hddpos::tran_saction();
    if(ls_operid == "0000") {
        if((ls_err=n_func::f_set_sysparm("system_pass", ls_new, QObject::tr("系统管理员密码"))).isEmpty()) {

        }
    } else {
        if(query.execUpdate("cey_sys_operator",qrtVariantPairList()

                            << qrtVariantPair("vch_password", ls_new),
                            QString(" vch_operID = '%1' ").arg(ls_operid))) {

        } else {
            ls_err=query.recordError();
        }
    }
    if(ls_err.isEmpty()) {
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("密码修改成功，请记住新密码"));
        this->accept();
    } else {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码修改失败"));
    }
    //! <--transaction end-->

}

void w_sys_manage_operatorright_pwdchange::toexit()
{
    this->reject();
}

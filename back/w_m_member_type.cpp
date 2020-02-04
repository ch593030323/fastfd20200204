#include "w_m_member_type.h"
//会员类型变更
#include "ui_w_m_member_type_dialog.h"
#include "w_m_member_list.h"
#include "lds_query_hddpos.h"
#include <QSqlError>
#include "lds_messagebox.h"
#include "n_func.h"
#include <QTableView>
#include "lds_tableview.h"
#include "lds.h"
#include "w_sys_manage_cloudsync_with_time.h"


w_m_member_type::w_m_member_type(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_m_member_type_dialog)
{
    ui->setupUi(this);
    QSqlQueryModel *commodel=new QSqlQueryModel;
    commodel->setQuery(QString("select vch_typename as '%1', ch_typeno as '%2', ch_attribute as '%3' from t_m_member_type;")
                       .arg(QObject::tr("会员类型"))
                       .arg(QObject::tr("类型编号"))
                       .arg(QObject::tr("充值编号"))
                       );
    ui->comboBox_vch_typename->setModel(commodel);
    ui->comboBox_vch_typename->setModelColumn(0);
    ui->comboBox_vch_typename->setCurrentIndex(-1);

    lds_tableView *view = new lds_tableView;
    ui->comboBox_typenames->setModel(commodel);
    ui->comboBox_typenames->setMaxVisibleItems(5);
    ui->comboBox_typenames->setView(view);
    ui->comboBox_typenames->setCurrentIndex(-1);
    view->resizeColumnsToContents();


    ui->frame->setup(QStringList()<<QObject::tr("读卡") << QObject::tr("确定") << QObject::tr("取消"),Qt::AlignRight);
    connect(ui->frame->index_widget(QObject::tr("读卡")), SIGNAL(clicked()),this,SLOT(toreadcard()));
    connect(ui->frame->index_widget(QObject::tr("确定")), SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->frame->index_widget(QObject::tr("取消")), SIGNAL(clicked()),this,SLOT(toexit()));


    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
//    ui->widget->index_widget(QObject::tr("退出"))->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun
}

void w_m_member_type::toreadcard()
{
    n_func::st_member_type lst_member;
    QString cardno;
    QString as_memberno;
    uo_rw_member uo_member;

    //读卡
    if(!w_m_member_list::static_to_memeber_readcard(this,cardno, uo_member)) {
        ui->lineEdit_vch_memberno->clear();
        ui->lineEdit_vch_member->clear();
        ui->comboBox_vch_typename->setCurrentIndex(-1);
        ui->lineEdit_num_amount->clear();
        ui->lineEdit_num_point->clear();
        return;
    }

    as_memberno = cardno;//会员编号和卡号一致
    lds_query_hddpos  query;
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
    if(lst_member.as_m_state.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("非本公司卡或无效卡"));
        return;
    }
    if(lst_member.as_m_state!="1") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此卡状态非正常"));
        return;
    }

    query.execSelect(QString("select a.vch_member, b.ch_typeno from t_m_member a, t_m_member_type b where a.ch_typeno=b.ch_typeno and a.vch_memberno='%1' ")
               .arg(lst_card.cardid));
    query.next();

    ui->lineEdit_num_amount->setValue(lst_card.amount);
    ui->lineEdit_num_point->setValue(lst_card.point);
    ui->lineEdit_vch_member->setText(query.recordValue("vch_member").toString());
    ui->lineEdit_vch_memberno->setText(lst_card.cardid);
    for(int row=0,rowcount=ui->comboBox_vch_typename->count();row<rowcount;row++) {
        if(ui->comboBox_vch_typename->model()->index(row, 1).data().toString()==
                query.recordValue("ch_typeno").toString()) {ui->comboBox_vch_typename->setCurrentIndex(row);break;}
    }
}

void w_m_member_type::took()
{
    //clicked (none) returns long [pbm_bnclicked]
    //long ancestorreturnvalue
    QString ls_attribute;
    QString ls_typeno;
    QString ls_memberno;
    QString ls_attribute_new;
    QString ls_typeno_new;
    QString ls_err;
    double ldec_amount;


    ls_memberno = ui->lineEdit_vch_memberno->text();
    ls_attribute = ui->comboBox_vch_typename->model()->index(ui->comboBox_vch_typename->currentIndex(), 2).data().toString();
    ls_typeno = ui->comboBox_vch_typename->model()->index(ui->comboBox_vch_typename->currentIndex(), 1).data().toString();
    ls_attribute_new = ui->comboBox_typenames->model()->index(ui->comboBox_typenames->currentIndex(), 2).data().toString();
    ls_typeno_new = ui->comboBox_typenames->model()->index(ui->comboBox_typenames->currentIndex(), 1).data().toString();
    ldec_amount = ui->lineEdit_num_amount->value();
    if(ls_memberno.trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请先读卡"));
        return;
    }
    if(ls_typeno_new.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请先选择新类型"));
        return;
    }
    if(ls_typeno==ls_typeno_new) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("会员类型相同"));
        return;
    }
    switch(ls_attribute.toInt()) {
    case 1:
    case 2:
        if(ls_attribute_new!="2") {
            if(ldec_amount!=0) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("卡余额不为零，充值卡不能转其它类型卡"));
                return;
            }
        }
        break;
    case 3:
        if(ls_attribute_new!="3") {
            if(ldec_amount!=0) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("未结算金额不为零，借记卡不能转其它类型卡"));
                return;
            }
        }
        break;
    }
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确定当前会员转指定类型?"),QObject::tr("确定"),QObject::tr("取消"))) {
        //!----------------transaction
        lds_query_hddpos::tran_saction();
        ls_err.clear();
        lds_query_hddpos  query;
        if(query.execUpdate("t_m_member","ch_typeno", ls_typeno_new, qrtEqual("vch_memberno", ls_memberno), &ls_err)) {
            if((ls_err=n_func::f_m_insert_log(ls_memberno,QObject::tr("转类型"),ui->comboBox_vch_typename->currentText()+ "转为" + ui->comboBox_typenames->currentText(),0)).isEmpty()) {
                ui->lineEdit_vch_memberno->clear();
                ui->lineEdit_vch_member->clear();
                ui->comboBox_vch_typename->setCurrentIndex(-1);
                ui->lineEdit_num_amount->clear();
                ui->lineEdit_num_point->clear();
            }
        }
        //!----------------transaction result
        if(ls_err.isEmpty()) {
            lds_query_hddpos::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("会员转类型成功"));
        } else {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+ls_err);
        }
    }
}

void w_m_member_type::toexit()
{
    w_sys_manage_cloudsync_cloud2::exec_keepdata_byconf(this);

    this->reject();

}

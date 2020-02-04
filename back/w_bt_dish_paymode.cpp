//收银方式
#include "w_bt_dish_paymode.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_tableview_delegate_check.h"
#include "backheader.h"
#include "lds_messagebox.h"
#include <QSqlRecord>
#include "lds_query_hddpos.h"
#include <qsqlerror.h>
#include <QSqlRelationalDelegate>
#include <QDataWidgetMapper>
#include "ui_w_bt_dish_paymode_dialog.h"
#include <QFileDialog>
#include "alipay_help.h"
#include "w_scr_dish_phone_pay.h"
#include <QScrollArea>
#include <QHostInfo>
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_menu.h"
#include "lds.h"
#include "n_func.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "mobile_pay_help.h"
#include "getfilepath.h"
#include <QClipboard>
#include "w_sys_manage_sqlcopy.h"
#include "ui_w_bt_dish_paymode_set.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "fupay_help.h"
#include "w_bt_dish_paymode_ccb.h"
#include "w_bt_dish_paymode_liantuofu.h"

/**
1.arg4用于记录色值, arg6用于记账
2.
*/
void w_bt_dish_paymode::update_vch_english()
{
    lds_query_hddpos query;
    QList<PayModeData> paylist;
    fille_paylist(paylist);
    foreach(const PayModeData &d, paylist) {
        query.execSelect(QString("select 1 from cey_bt_paymode where ch_paymodeno = '%1' ").arg(d.ch_paymodeno));
        if(query.next()) {
            query.execUpdate("cey_bt_paymode", qrtVariantPairList()
                             << qrtVariantPair("vch_paymodename",d.vch_paymodename)
                             << qrtVariantPair("vch_english",d.vch_english),
                             QString(" ch_paymodeno = '%1' ").arg(d.ch_paymodeno));
        } else {
            query.execInsert("cey_bt_paymode", qrtVariantPairList()
                             << qrtVariantPair("ch_paymodeno",d.ch_paymodeno)
                             << qrtVariantPair("ch_IDflag",  d.ch_IDflag)
                             << qrtVariantPair("ch_faceflag",  d.ch_faceflag)
                             << qrtVariantPair("ch_changeflag",  d.ch_changeflag)
                             << qrtVariantPair("ch_incomeflag",  d.ch_incomeflag)
                             << qrtVariantPair("vch_arg4",  d.vch_arg4)
                             << qrtVariantPair("vch_paymodename",  d.vch_paymodename)
                             << qrtVariantPair("vch_english",  d.vch_english)
                             );
        }
    }
}


void w_bt_dish_paymode::update_tanslate()
{
    lds_query_hddpos query;
    QList<PayModeData> paylist;
    fille_paylist(paylist);

    foreach(const PayModeData &d, paylist) {
        query.execUpdate("cey_bt_paymode", qrtVariantPairList()
                         << qrtVariantPair("ch_IDflag",  d.ch_IDflag)
                         << qrtVariantPair("ch_faceflag",  d.ch_faceflag)
                         << qrtVariantPair("ch_changeflag",  d.ch_changeflag)
                         << qrtVariantPair("ch_incomeflag",  d.ch_incomeflag)
                         << qrtVariantPair("vch_arg4",  d.vch_arg4)
                         << qrtVariantPair("vch_paymodename",  d.vch_paymodename)
                         << qrtVariantPair("vch_english",  d.vch_english),
                         QString("ch_paymodeno=%1").arg(d.ch_paymodeno)
                         );
    }
}

void w_bt_dish_paymode::fille_paylist(QList<w_bt_dish_paymode::PayModeData> &paylist)
{
    paylist << (PayModeData){"00", "N", "N", "Y", "Y", "N", QObject::tr("现金"), "Cash"};
    paylist << (PayModeData){"02", "Y", "N", "N", "Y", "N", QObject::tr("充值卡", "Card")};
    paylist << (PayModeData){"03", "Y", "N", "N", "Y", "N", QObject::tr("银行卡"), "Bank"};
    paylist << (PayModeData){"04", "Y", "N", "N", "Y", "N", QObject::tr("代金券"), "Voucher"};
    paylist << (PayModeData){"05", "N", "N", "N", "Y", "N", QObject::tr("微信支付"), "Wechat"};

    paylist << (PayModeData){"06", "N", "N", "N", "Y", "N", QObject::tr("支付宝支付"), "Alipay"};
//    paylist << (PayModeData){"07", "N", "N", "N", "Y", "T", QObject::tr("百度糯米"), "NULL"};
    paylist << (PayModeData){"08", "N", "N", "N", "Y", "T", QObject::tr("大众点评"), "NULL"};
    paylist << (PayModeData){"09", "N", "N", "N", "Y", "T", QObject::tr("美团"), "NULL"};
    paylist << (PayModeData){"10", "N", "N", "N", "Y", "N", QObject::tr("云会员卡"), "NULL"};

    paylist << (PayModeData){"AA", "N", "N", "N", "Y", "N", QObject::tr("摇钱树支付"), "YQS pay"};
    paylist << (PayModeData){"BB", "N", "N", "N", "Y", "N", QObject::tr("快捷支付"), "NULL"};
    paylist << (PayModeData){"CC", "N", "N", "N", "Y", "N", QObject::tr("龙支付"), "NULL"};
    paylist << (PayModeData){"DD", "N", "N", "N", "Y", "N", QObject::tr("微支付"), "NULL"};
    paylist << (PayModeData){"AB", "N", "N", "N", "Y", "N", QObject::tr("美国运通"), "Amex"};

    paylist << (PayModeData){"AC", "N", "N", "N", "Y", "N", QObject::tr("万事达卡"), "MasterCard"};
    paylist << (PayModeData){"AD", "N", "N", "N", "Y", "N", QObject::tr("Visa信用卡"), "Visa"};
    paylist << (PayModeData){"AE", "N", "N", "N", "Y", "N", QObject::tr("TNG电子钱包"), "TNG"};


    paylist << (PayModeData){"CL", "N", "N", "N", "N", "N", QObject::tr("客户"), "NULL"};
    paylist << (PayModeData){"11", "N", "N", "N", "N", "N", QObject::tr("会员挂账"), "NULL"};
    paylist << (PayModeData){"XX", "N", "N", "N", "N", "N", QObject::tr("手工抹零"), "NULL"};
    paylist << (PayModeData){"YY", "N", "N", "N", "N", "N", QObject::tr("自动抹零"), "NULL"};
    paylist << (PayModeData){"ZZ", "N", "N", "N", "N", "N", QObject::tr("优惠"), "NULL"};
}

w_bt_dish_paymode::w_bt_dish_paymode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_bt_dish_paymode_dialog)
{
    ui->setupUi(this);
    //! tableview
    ui->label_vch_arg4->kcmap = w_bt_dish::get_kcmap();
    tablemodel=new paymodetablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("cey_bt_paymode");
    tablemodel->save_set_header("ch_paymodeno", QObject::tr("收银代码"));
    tablemodel->save_set_header("vch_paymodename", QObject::tr("收银名称"));

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->addMapping(ui->lineEdit_ch_key,tablemodel->fieldIndex("ch_key"));
    mapper->addMapping(ui->lineEdit_ch_paymodeno,tablemodel->fieldIndex("ch_paymodeno"));
    mapper->addMapping(ui->lineEdit_vch_paymodename,tablemodel->fieldIndex("vch_paymodename"));
    mapper->addMapping(ui->checkBox_ch_changeflag,tablemodel->fieldIndex("ch_changeflag"));
    mapper->addMapping(ui->checkBox_ch_faceflag,tablemodel->fieldIndex("ch_faceflag"));
    mapper->addMapping(ui->checkBox_ch_idflag,tablemodel->fieldIndex("ch_IDflag"));
    mapper->addMapping(ui->checkBox_ch_incomeflag,tablemodel->fieldIndex("ch_incomeflag"));
    mapper->addMapping(ui->label_vch_arg4, tablemodel->fieldIndex("vch_arg4"), "color");
    //! toolbar
    ui->widget->setup(QStringList() << QObject::tr("团购新增") << QObject::tr("普通新增") << QObject::tr("删除") <<  QObject::tr("刷新") << QObject::tr("保存") << QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("团购新增")), SIGNAL(clicked()),this,SLOT(tonew2()));
    connect(ui->widget->index_widget(QObject::tr("普通新增")), SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(QObject::tr("删除")), SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(QObject::tr("刷新")), SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(QObject::tr("保存")), SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView,SIGNAL(selectChanged(int)),this,SLOT(mapper_setcurindex(int)));
    connect(ui->pushButton_arg,SIGNAL(clicked()),this,SLOT(toarg()));

    //5
    QStandardItemModel *standmodel = new QStandardItemModel;
    for(QMap<QString, QString>::iterator itr = w_bt_dish::get_kcmapdesc().begin();
        itr != w_bt_dish::get_kcmapdesc().end(); itr++) {
        QStandardItem *item = new QStandardItem;
        item->setData(QColor(itr.key()), Qt::DecorationRole);
        item->setData(itr.value(), Qt::DisplayRole);
        standmodel->appendRow(item);
    }
    standmodel->setHorizontalHeaderLabels(QStringList() << QObject::tr("说明"));
    ui->tableView_2->setModel(standmodel);
    ui->tableView_2->verticalHeader()->hide();
    ui->tableView_2->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_2->horizontalHeader()->setStretchLastSection(true);
    ui->tableView_2->verticalHeader()->setDefaultSectionSize(30);


    ui->lineEdit_unitprefix->setText(n_func::f_get_sysparm("pay_unit_prefix"));
    connect(ui->lineEdit_unitprefix,SIGNAL(textEdited(QString)),this,SLOT(tosave_unitprefix(QString)));

    QTimer::singleShot(10, this, SLOT(torefresh()));
}

void w_bt_dish_paymode::tonew2()
{
    w_bt_dish_paymode_teampay dialog(this);
    dialog.setWindowTitle(qobject_cast<QPushButton *>(this->sender())->text());
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void w_bt_dish_paymode::tonew()
{
    int rowcount=tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_paymodeno"), tablemodel->max_column_add_1__max_sql_add_1_dec("ch_paymodeno","%02d"));
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("vch_arg4"), QColor(w_bt_dish::get_kcmap().value("S")), Qt::DecorationRole);
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("vch_arg4"), "S");


    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_IDflag"), "N");
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_faceflag"), "N");
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_changeflag"), "N");
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_incomeflag"), "N");

    ui->tableView->selectRow(tablemodel->rowCount() - 1);
    ui->lineEdit_vch_paymodename->setFocus();

}

void w_bt_dish_paymode::todel()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    lds_query_hddpos  query;
    query.execSelect(QString(" select ch_paymodeno from cey_u_checkout_detail where ch_paymodeno = '%1' ").arg(tablemodel->model_data(row, "ch_paymodeno").toString()));
    if(query.next()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已存在消费记录"));
        return;
    }
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"),QObject::tr("确认"),QObject::tr("取消"))) {
        tablemodel->removeRow(ui->tableView->currentIndex().row());
        tablemodel->trySubmitAll();
        torefresh();
    }
}

void w_bt_dish_paymode::torefresh()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->widget->index_widget(QObject::tr("退出"))->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun

    QString filter = " 1= 1 ";
    if(lds::get_soft_curent_language_first() == _EN_) {
        ui->widget->index_widget(QObject::tr("团购新增"))->hide();
        filter += " and ch_paymodeno not in ('07', '08', '09') ";
    }

    tablemodel->setFilter(filter +" order by vch_arg4 asc ");
    ui->tableView->restore(0);
}

bool w_bt_dish_paymode::tosave()
{
    for(int r=0,r_count=tablemodel->rowCount();r<r_count;r++) {
        if(tablemodel->model_data(r, "vch_paymodename").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("收银方式名称不能为空"));
            return false;
        }
        if(tablemodel->model_data(r, "ch_changeflag").toString() == "Y"
                &&tablemodel->model_data(r, "ch_faceflag").toString() == "Y") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("是否找零与是否面额只能选其一"));
            return false;
        }
    }

    if(tablemodel->trySubmitAll()) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误")+QString("%1").arg(__LINE__));
        return false;
    }
}

void w_bt_dish_paymode::toexit()
{
    if(tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!tosave()) {
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }

    w_sys_manage_cloudsync_cloud2::exec_keepdata_byconf(this);
    this->reject();
}

void w_bt_dish_paymode::toarg()
{
    QString ch_paymodeno = tablemodel->model_data(ui->tableView->currentIndex().row(), "ch_paymodeno").toString();
    QString vch_arg4 = tablemodel->model_data(ui->tableView->currentIndex().row(), "vch_arg4").toString();
    if(ch_paymodeno == "06"
            || ch_paymodeno == "05"
            || ch_paymodeno == "DD"
            ||ch_paymodeno == "AA") {
        w_bt_dish_paymode_set dialog(ch_paymodeno, this);
        if(ch_paymodeno == "06")dialog.setWindowTitle(QObject::tr("支付宝"));
        if(ch_paymodeno == "05")dialog.setWindowTitle(QObject::tr("微信"));
        if(ch_paymodeno == "AA")dialog.setWindowTitle(QObject::tr("摇钱树支付"));
        if(ch_paymodeno == "DD")dialog.setWindowTitle(QObject::tr("微支付"));
        transparentCenterDialog(&dialog).exec();
    }

    if(vch_arg4 == "T") {
        w_bt_dish_paymode_teampay dialog(this);
        dialog.setWindowTitle(qobject_cast<QPushButton *>(this->sender())->text());
        dialog.set_ch_paymodeno(ch_paymodeno);
        transparentCenterDialog(&dialog).exec();
        torefresh();
    }
    if(ch_paymodeno == "CC") {
        w_bt_dish_paymode_CCB dialog(this);
        transparentCenterDialog(&dialog).exec();
    }
    if(ch_paymodeno == "EE") {
        w_bt_dish_paymode_liantuofu dialog(this);
        transparentCenterDialog(&dialog).exec();
    }
}

void w_bt_dish_paymode::mapper_setcurindex(int index)
{
    QString ls_paymodeno = tablemodel->model_data(index, "ch_paymodeno").toString();
    QString ls_paytype = tablemodel->model_data(index, "vch_arg4").toString();
    bool isenable=
            !(ls_paymodeno == "00" ||
              ls_paymodeno == "02"||
              ls_paymodeno == "03"||
              ls_paymodeno == "04"||

              ls_paymodeno == "05"||
              ls_paymodeno == "06"||

              ls_paymodeno == "07"||
              ls_paymodeno == "08"||
              ls_paymodeno == "09"||

              ls_paymodeno == "10"||
              ls_paymodeno == "11"||

              ls_paymodeno == "AA"||
              ls_paymodeno == "XX"||
              ls_paymodeno == "YY"||
              ls_paymodeno == "DD"||
              ls_paymodeno == "CL"||
              ls_paymodeno == "CC"||
              ls_paymodeno == "EE"||
              ls_paymodeno == "ZZ"
              );
    isenable = (ls_paytype == "S");
    //    ui->lineEdit_ch_key->setEnabled(isenable);
    ui->lineEdit_ch_paymodeno->setEnabled(isenable);
    ui->lineEdit_vch_paymodename->setEnabled(isenable);
    ui->checkBox_ch_changeflag->setEnabled(isenable);
    ui->checkBox_ch_faceflag->setEnabled(isenable);
    ui->checkBox_ch_idflag->setEnabled(isenable);
    ui->checkBox_ch_incomeflag->setEnabled(isenable);
    ui->widget->index_widget(QObject::tr("删除"))->setEnabled(isenable);

    ui->pushButton_arg->setEnabled(ls_paymodeno == "05"||
                                   ls_paymodeno == "06"||
                                   ls_paymodeno == "07"||
                                   ls_paymodeno == "08"||
                                   ls_paymodeno == "09"||
                                   ls_paymodeno == "AA"||
                                   ls_paymodeno == "DD"||
                                   ls_paymodeno == "CC"||
                                   ls_paymodeno == "EE"||
                                   false
                                   );

    mapper->setCurrentIndex(index);
}

void w_bt_dish_paymode::todef()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        lds_query_hddpos  query;
        if(query.execUpdate("cey_bt_paymode",qrtVariantPairList()
                            <<qrtVariantPair("vch_arg1", "wxd9ccc742b1c39110")
                            <<qrtVariantPair("vch_arg2", "10036785")
                            <<qrtVariantPair("vch_arg3", "wwwscposcomhdd802600000000000000"),
                            qrtEqual("ch_paymodeno", "05"))) {
            if(query.execUpdate("cey_bt_paymode",qrtVariantPairList()
                                <<qrtVariantPair("vch_arg1", "2015121000954514")
                                <<qrtVariantPair("vch_arg2", "")
                                <<qrtVariantPair("vch_arg3", "121.41.82.112:9100"),
                                qrtEqual("ch_paymodeno", "06"))) {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID,  b->text() +"\n"+ QObject::tr("设置成功"));
                return;
            }
        }
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, b->text() + "\n" +  QObject::tr("操作失败"));
}

void w_bt_dish_paymode::tosave_unitprefix(const QString &text)
{
    n_func::f_set_sysparm("pay_unit_prefix", text, "付款金额是的标志如￥、$");
}

//=============
//05
//06
static QStandardItem * creatItemByQuery(lds_query_hddpos  &query, const QString &field_name) {
    QStandardItem *item = new QStandardItem;
    item->setData(query.recordValue(field_name), Qt::EditRole);
    item->setData(field_name, Qt::UserRole);
    return item;

}

//arg3存文本，例如公钥
//arg6是否记账
w_bt_dish_paymode_set::w_bt_dish_paymode_set(const QString &ch_paymodeno, QWidget *parent)
    : QDialog(parent),
      ui(new Ui_w_bt_dish_paymode_set)
{
    ui->setupUi(this);

    ui->widget_com->hide();

    _ch_paymodeno = ch_paymodeno;
    QStandardItem *item;
    //! tableview
    /*
     * 05 微信
*/
    standmodel = new paymode_model(this);
    standmodel->setHorizontalHeaderLabels(QStringList() << QObject::tr("参数说明") << QObject::tr("参数详情"));
    ui->tableView->setModel(standmodel);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);


    //! data
    lds_query_hddpos  query;
    query.execSelect(QString("select * FROM cey_bt_paymode where ch_paymodeno = '%1' ").arg(ch_paymodeno));
    query.next();
    if(_ch_paymodeno == "05") {
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("APP_ID") + "(*)") << creatItemByQuery(query, "vch_arg1"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("商户号") + "(*)") << creatItemByQuery(query, "vch_arg2"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("支付密钥") + "(*)") << creatItemByQuery(query, "vch_arg3"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("仅记账")) << (item = creatItemByQuery(query, "vch_arg6")));

        ui->tableView->setItemDelegate(new lds_tableview_delegate_check(item->row(), item->column(), ui->tableView, true));
    }
    if(_ch_paymodeno == "06") {
        ///支付宝
        //vch_arg1 ：appid
        //vch_arg2 ：商户门店编号
        //vch_arg3 ：私钥路径
        //vch_arg4 ：付款方式类别
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem("app_id(*)") << creatItemByQuery(query, "vch_arg1"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("商户门店编号")) << creatItemByQuery(query, "vch_arg2"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("私钥") + "(*)") << creatItemByQuery(query, "vch_arg3"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("仅记账")) << (item = creatItemByQuery(query, "vch_arg6")));
        ui->tableView->setItemDelegate(new lds_tableview_delegate_check(item->row(), item->column(), ui->tableView, true));
    }

    if(_ch_paymodeno == "AA") {
        ui->label->setText(QObject::tr("支付宝签名方式"));

        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("微信子商户号") + "(*)") << creatItemByQuery(query, "vch_arg1"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("支付宝") +"appid"+"(*)") << creatItemByQuery(query, "vch_arg2"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("支付宝") +QObject::tr("公钥") + "(*)") << creatItemByQuery(query, "vch_arg3"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("口碑门店")) << creatItemByQuery(query, "vch_arg5"));
        //
        ui->widget_com->show();
        ui->comboBox->addItems(mobile_pay_help::mobile_pay_alipay_keys());
        ui->comboBox->setCurrentIndex(ui->comboBox->findText(n_func::f_get_sysparm("mobile_alipay_sign_type", "RSA2")));
        toShowRSA2(ui->comboBox->currentIndex());
    }
    if(_ch_paymodeno == "DD") {
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("商户号") + "(*)") << creatItemByQuery(query, "vch_arg1"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("终端号") + "(*)") << creatItemByQuery(query, "vch_arg2"));
        standmodel->appendRow(QList<QStandardItem *>()
                              << new QStandardItem(QObject::tr("令牌") + "(*)") << creatItemByQuery(query, "vch_arg3"));
    }

    //! end
    ui->tableView->setColumnWidth(0, 150);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);


    if(_ch_paymodeno == "AA") {//摇钱树支付

        ui->widget->setup(QStringList()  << QObject::tr("演示用例") << QObject::tr("获取支付宝公钥") << QObject::tr("读取文本") <<QObject::tr("测试") << QObject::tr("保存") << QObject::tr("退出"), Qt::AlignRight);
        connect(ui->widget->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
        connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
        connect(ui->widget->index_widget(QObject::tr("演示用例")),SIGNAL(clicked()),this,SLOT(todef()));
        connect(ui->widget->index_widget(QObject::tr("测试")),SIGNAL(clicked()),this,SLOT(totest()));
        connect(ui->widget->index_widget(QObject::tr("获取支付宝公钥")),SIGNAL(clicked()),this,SLOT(towxzfb_paste_public_key()));

        connect(ui->widget->index_widget(QObject::tr("读取文本")),SIGNAL(clicked()),this,SLOT(togetpathData()));
        connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(toShowRSA2(int)));
    }

    if(_ch_paymodeno == "05"
            || _ch_paymodeno == "06") {//微信、支付宝
        ui->widget->setup(QStringList()  << QObject::tr("演示用例")<< QObject::tr("获取路径") << QObject::tr("测试") << QObject::tr("保存") << QObject::tr("退出"), Qt::AlignRight);
        connect(ui->widget->index_widget(QObject::tr("获取路径")),SIGNAL(clicked()),this,SLOT(togetpath()));
        connect(ui->widget->index_widget(QObject::tr("测试")),SIGNAL(clicked()),this,SLOT(totest()));
        connect(ui->widget->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
        connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
        connect(ui->widget->index_widget(QObject::tr("演示用例")),SIGNAL(clicked()),this,SLOT(todef()));
    }

    if(_ch_paymodeno == "DD") {
        ui->widget->setup(QStringList()  << /*QObject::tr("演示用例") << */QObject::tr("测试") << QObject::tr("保存") << QObject::tr("退出"), Qt::AlignRight);
        connect(ui->widget->index_widget(QObject::tr("测试")),SIGNAL(clicked()),this,SLOT(totest()));
        connect(ui->widget->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
        connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
        //        connect(ui->widget->index_widget(QObject::tr("演示用例")),SIGNAL(clicked()),this,SLOT(todef()));

    }
}

w_bt_dish_paymode_set::~w_bt_dish_paymode_set()
{
    delete ui;
}

void w_bt_dish_paymode_set::togetpath()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    //读取文本
    QString filepath = getFilePath::getOpenFileName(this, QObject::tr("导入文件"),standmodel->item(row, 1)->text());
    if(filepath.isEmpty()) {
        return;
    }
    standmodel->item(row, 1)->setText(filepath);
}

void w_bt_dish_paymode_set::togetpathData()
{
    bool ok;
    QString errstring;
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    //读取文本
    QString filepath = getFilePath::getOpenFileName(this, QObject::tr("导入文件"),standmodel->item(row, 1)->text());
    if(filepath.isEmpty()) {
        return;
    }
    QByteArray readall = lds::fileOpenReadAll(filepath, &ok, &errstring);
    if(false == ok) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    //
    standmodel->item(row, 1)->setText(readall);
}

void w_bt_dish_paymode_set::totest()
{
    QString ls_error;
    //微信测试
    if(_ch_paymodeno == "05") {
        QByteArray qrcodecontent;
        QDateTime sqldt = n_func::f_get_sysdatetime();
        QString appid = modelFieldString("vch_arg1");
        QString mch_id = modelFieldString("vch_arg2");
        QString paykey = modelFieldString("vch_arg3");
        QString record = modelFieldString("vch_arg6");

        QString out_trade_no = lds::LOCAL_TERMINAL_INDEX_FF + sqldt.toString("yyyyMMddhhmmsszzz");
        ls_error = QObject::tr("测试不正常");//2
        if(testwx_help::AmountQRcodeRequest(appid
                                            , this->windowTitle()+sqldt.toString("yyMMdd hh:mm:ss")
                                            , int(0.01*100)
                                            , paykey
                                            , mch_id
                                            , out_trade_no
                                            , qrcodecontent)) {
            ls_error = QObject::tr("测试正常:二维码不正常");//1
            if(!qrcodecontent.isEmpty()) {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("测试正常:二维码正常"));
                return;
            }//1
        }//2
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, ls_error);
    }

    if(_ch_paymodeno == "06") {//支付宝
        alipay_help::alipay_public_request_argument arg0;
        alipay_help::alipay_trade_precreate_argument arg1;
        alipay_help::alipay_trade_precreate_return_argument arg2;

        QString appid = modelFieldString("vch_arg1");
        QString store_id = modelFieldString("vch_arg2");
        QString rsa_private_key_path = modelFieldString("vch_arg3");
        QString record = modelFieldString("vch_arg6");


        arg0.app_id_not_null = appid;//APPID
        arg0.charset_not_null="utf-8";
        arg0.sign_type= lds::sysconf->value("alipay_help/" + arg0.app_id_not_null).toString();//alipay_help::sign_type_SHA256;
        arg0.timestamp_not_null="2015-12-17 03:07:50";
        arg0.version_not_null="1.0";
        arg0.rsa_private_key_data_not_null = alipay_help::alipay_private_key_add_start_line(rsa_private_key_path, 0);
        arg1.out_trade_no_not_null = "201503022001";
        arg1.subject_not_null = "apple";
        arg1.total_amount_not_null="21.3";
        arg1.store_id = "njgdn_001";//商店编号，可以不填，填写后享受支付宝的折扣
        arg1.extend_params.sys_service_provider_id = "njgdn_001";//系统商编号 该参数作为系统商返佣数据提取的依据，请填写系统商签约协议的PID

        if(alipay_help::alipay_trade_precreate(arg0, arg1, arg2)) {
            lds::sysconf->setValue("alipay_help/" + arg0.app_id_not_null, arg2.sign_type_not_null);
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,  arg2.sign_type_not_null + QObject::tr("测试成功") + ":"
                                        +"\nout_trade_no"+(arg2.out_trade_no_not_null.isEmpty()?QObject::tr("不正常"):QObject::tr("正常"))
                                        +"\nqr_code"+(arg2.qr_code_not_null.isEmpty()?QObject::tr("不正常"):QObject::tr("正常")));
        } else {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("操作失败") + alipay_help::errstring);
        }
    }

    if(_ch_paymodeno == "DD") {//微支付
        lds_dialog_input inputdialog(this);
        inputdialog.ui->label->setText(QObject::tr("授权码"));
        inputdialog.setWindowTitle(QObject::tr("支付") + ":0.01");
        if(QDialog::Accepted == transparentCenterDialog(&inputdialog).exec()) {
            QString auth_code = inputdialog.ui->lineEdit->text();
            QString errstring;
            QString out_trade_no  = lds::LOCAL_TERMINAL_INDEX_FF + QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
            QString merchant_no = modelFieldString("vch_arg1");
            QString terminal_id = modelFieldString("vch_arg2");
            QString access_token = modelFieldString("vch_arg3");

            fupay_help::payType pay_type;
            lds_messagebox_loading_show loading(this, QObject::tr("支付验证") + "...", "");
            fupay_help payObj;
            connect(&payObj, SIGNAL(signal_code_query(qint64,qint64)), &loading, SLOT(updateProgress(qint64,qint64)));
            loading.show_delay();
            if(payObj.codepay(pay_type, errstring,
                              auth_code,
                              "1",
                              out_trade_no,

                              merchant_no,
                              terminal_id,
                              access_token
                              )) {
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功") + ":"+payObj.payTypeName(pay_type));
            } else {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            }
        }
    }
    if(_ch_paymodeno == "AA") {//微支付
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("测试"), QObject::tr("支付宝测试"), QObject::tr("微信测试"), QObject::tr("取消"))) {
        case 0:
            wxzfb_test(OnlinePayTypeAliPay);
            break;
        case 1:
            wxzfb_test(OnlinePayTypeWeChat);
            break;
        default:
            break;
        }
    }
}

void w_bt_dish_paymode_set::tosave()
{
    ///支付宝
    //vch_arg1 ：appid
    //vch_arg2 ：商户门店编号
    //vch_arg3 ：私钥路径
    //vch_arg4 ：付款方式类别

    qrtVariantPairList vplist;
    //usr = key
    //edit = value
    for(int r = 0; r < standmodel->rowCount(); r++) {
        QString u = standmodel->item(r, 1)->data(Qt::UserRole).toString();
        if(!u.isEmpty()) {
            vplist << qrtVariantPair(u, standmodel->item(r, 1)->data(Qt::EditRole).toString());
        }
    }
    lds_query_hddpos  query;
    if(query.execUpdate("cey_bt_paymode",
                        vplist,
                        qrtEqual("ch_paymodeno", _ch_paymodeno))) {
        if(_ch_paymodeno == "AA") {//摇钱树支付
            n_func::f_set_sysparm("mobile_alipay_sign_type", ui->comboBox->currentText());
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return;
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+QString("%1").arg(__LINE__));
}

void w_bt_dish_paymode_set::toexit()
{
    this->reject();
}

void w_bt_dish_paymode_set::todef()
{
    if(_ch_paymodeno == "05") {
        setModelFieldString("vch_arg1","wxd9ccc742b1c39110" );
        setModelFieldString("vch_arg2","10036785" );
        setModelFieldString("vch_arg3","wwwscposcomhdd802600000000000000" );
    }
    if(_ch_paymodeno == "06") {
        setModelFieldString("vch_arg1","2015121000954514" );
        setModelFieldString("vch_arg2","" );
        //        setModelFieldString("vch_arg3","wwwscposcomhdd802600000000000000" );
    }
    if(_ch_paymodeno == "AA") {
        setModelFieldString("vch_arg1","1431014702" );
        setModelFieldString("vch_arg2","2015121000954514" );
        setModelFieldString("vch_arg3","MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvprAybgqWuLiL1IYG/nT13oRtR782HXzapBfORVNEo+iGVBLpvH9XSxI1JzQ3/TMjiN4HNB5DNLoepssEO3WAlD2MUa9KWbhWpfRR/ps3aEPPIvSfENXQeJF2re/CbX4jNvsFStAgEgd23/ch6bkH/yilKdMTr8UFtlL4RQ/ECO8gmpo7j+LDwsP83qdOm/NHcdQNOmqPom/ByGVWl9C4ZDEfkjbMpM9MZuMaB6zjQ18KbLrrN4+9DxsA7/HCq33s7BP6gptfMfhq3UoM67GDUAgX4D9PsjMx4cWAu6t9lF64ZeWj51/8YKd2j8+OVSx+W1JMnye3rJnjpBt5PEBRwIDAQAB" );
        setModelFieldString("vch_arg5","" );

        ui->comboBox->setCurrentIndex(ui->comboBox->findText(mobile_pay_help_sign_type::RSA));
    }
    if(_ch_paymodeno == "DD") {
        setModelFieldString("vch_arg1","852100302000008" );
        setModelFieldString("vch_arg2","30052759" );
        setModelFieldString("vch_arg3","d7393417dc0d4a9dabc54ff5bd1657f6" );
    }
}

void w_bt_dish_paymode_set::towxzfb_paste_public_key()
{
    QByteArray ret_rsa_public_key;
    QString errstring;
    bool f = false;
    {
        //!等待
        lds_messagebox_loading_show loading(this, "", "WAITING...");
        loading.show();
        //!~等待
        f = mobile_pay_help::getzfbpublic_key(ret_rsa_public_key, errstring, ui->comboBox->currentText());
    }
    if(f) {
        QApplication::clipboard()->setText(ret_rsa_public_key);
        if(0 == lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("已复制到剪切板") +"\n" + QObject::tr("请打开记事本，新建，粘贴，保存到U盘，卸载U盘"), QObject::tr("卸载U盘"), QObject::tr("退出"))) {
            w_sys_manage_sqlcopy::unmount_u_pan(this);
        }
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败") + "\n" + errstring);
}

void w_bt_dish_paymode_set::toShowRSA2(int index)
{
    if(mobile_pay_help_sign_type::RSA == ui->comboBox->itemText(index)) {
        ui->tableView->hideRow(2);
    }
    if(mobile_pay_help_sign_type::RSA2 == ui->comboBox->itemText(index)) {
        ui->tableView->showRow(2);
    }
}

void w_bt_dish_paymode_set::wxzfb_test(OnlinePayType type)
{
    if(_ch_paymodeno == "AA") {//支付宝
        //金额为整数，单位分，如12.34元，实际传入1234
        //auth_code 自动替换清除空格
        QString sub_mch_id = modelFieldString("vch_arg1");
        QString appid = modelFieldString("vch_arg2");
        QString new_alipay_public_key = modelFieldString("vch_arg3");
        QString kbstore_id = modelFieldString("vch_arg5");

        QString auth_code;
        QString total_amount = "1";

        QString out_trade_no;
        QString body = QObject::tr("摇钱树支付");
        QString sub_appid = "";
        QString device_info = "";
        out_trade_no = lds::LOCAL_TERMINAL_INDEX_FF + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
        auth_code = "1300 6767 7289 989988";

        QByteArray code_url;
        QString errstring;
        bool flag = false; {
            //!等待
            lds_messagebox_loading_show loading(this, "", "WAITING...");
            loading.show();
            //!~等待
            if(type == OnlinePayTypeWeChat) {
                flag = mobile_pay_help::getwxpayurl(code_url,
                                                    errstring,
                                                    sub_mch_id,
                                                    total_amount,
                                                    out_trade_no,
                                                    body);
            }
            if(type == OnlinePayTypeAliPay) {
                flag = mobile_pay_help::getzfbpayurl(code_url,
                                                     errstring,
                                                     ui->comboBox->currentText(),
                                                     appid,
                                                     total_amount,
                                                     out_trade_no,
                                                     body,
                                                     kbstore_id,
                                                     new_alipay_public_key.toLocal8Bit());
            }
        }
        if(flag) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID,  QObject::tr("测试成功"));
            return;
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("测试失败") + "\n"+errstring);
    }
}

QString w_bt_dish_paymode_set::modelFieldString(const QString &field_name)
{
    for(int row = 0; row < standmodel->rowCount(); row ++) {
        if(standmodel->index(row, 1).data(Qt::UserRole).toString() == field_name) {
            return standmodel->index(row, 1).data(Qt::EditRole).toString();
        }
    }
    return "";
}

void w_bt_dish_paymode_set::setModelFieldString(const QString &field_name, const QString &value)
{
    for(int row = 0; row < standmodel->rowCount(); row ++) {
        if(standmodel->index(row, 1).data(Qt::UserRole).toString() == field_name) {
            standmodel->setData(standmodel->index(row, 1), value, Qt::EditRole);
        }
    }
}


w_bt_dish_paymode::paymodetablemodel::paymodetablemodel(QObject *parent)
    :lds_model_sqltablemodel(parent)
{
}

w_bt_dish_paymode::paymodetablemodel::~paymodetablemodel()
{

}

QVariant w_bt_dish_paymode::paymodetablemodel::data(const QModelIndex &item, int role) const
{
    if(role == Qt::TextColorRole) {
        QString d = record(item.row()).value("vch_arg4").toString();
        QColor c = QColor(w_bt_dish::get_kcmap().value(d));
        if(c.isValid()) return c;
    }
    return lds_model_sqltablemodel::data(item, role);
}

w_bt_dish_paymode_teampay::w_bt_dish_paymode_teampay(QWidget *parent) : QDialog(parent), ui(new Ui_w_bt_dish_unitset_Dialog)
{
    ui->setupUi(this);
    //1
    //2
    tablemodel = new w_bt_dish_paymode::paymodetablemodel ;
    tablemodel->setTable("cey_bt_paymode");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);

    tablemodel->save_set_header("ch_paymodeno", QObject::tr("收银方式"));
    tablemodel->save_set_header("vch_paymodename", QObject::tr("方式名称"));
    tablemodel->save_set_header("vch_arg1", QObject::tr("网址简称"));
    tablemodel->save_set_header("vch_arg2", QObject::tr("网址"), true);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);

    //3
    ui->widget->setup(QStringList() << QObject::tr("新增") << QObject::tr("删除") << QObject::tr("测试") << QObject::tr("保存") << QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("删除")), SIGNAL(clicked()),this, SLOT(todel()));
    connect(ui->widget->index_widget(QObject::tr("测试")), SIGNAL(clicked()),this, SLOT(tonettest()));
    connect(ui->widget->index_widget(QObject::tr("保存")), SIGNAL(clicked()),this, SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this, SLOT(toexit()));

    //4
    {
        //
        QStandardItemModel *standmodel = new QStandardItemModel;
        standmodel->setHorizontalHeaderLabels(QStringList()<<QObject::tr("新增团购"));
        QStandardItem *item;
        for(int index = 0, indexcount = w_bt_dish::get_kvaddresslist().count(); index < indexcount; index++) {
            standmodel->appendRow(item = new QStandardItem(w_bt_dish::get_kvaddresslist()[index].first));item->setData(QColor("moccasin"), Qt::BackgroundRole);
        }
        standmodel->appendRow(item = new QStandardItem(QObject::tr("其他团购")));item->setData(QColor("mintcream"), Qt::BackgroundRole);

        lds_tableView *t = creatpopView(standmodel->rowCount());
        t->setModel(standmodel);
        t->resizeColumnToContents(0);
        t->setFixedWidth(t->columnWidth(0));
        //2
        menu=new lds_menu(this);
        QWidgetAction *widgetaction=new QWidgetAction(ui->widget->index_widget(QObject::tr("新增")));
        widgetaction->setDefaultWidget(t);
        menu->addAction(widgetaction);
        ui->widget->index_widget(QObject::tr("新增"))->setMenu(menu);
        connect(t,SIGNAL(clicked(QModelIndex)),this,SLOT(newpaymode_team(QModelIndex)));
        connect(menu,SIGNAL(aboutToShow()),t,SLOT(clearSelection()));
    }

    //5
    ui->label_desc->setMaximumHeight(100);
    ui->label_desc->setText("");

    tablemodel->setFilter("vch_arg4='T' ");
}

lds_tableView *w_bt_dish_paymode_teampay::creatpopView(int rowcount)
{
    lds_tableView *t = new lds_tableView;
    //    t->horizontalHeader()->setStretchLastSection(true);
    t->verticalHeader()->hide();
    t->horizontalHeader()->hide();
    t->setEditTriggers(QTableView::NoEditTriggers);
    t->setSelectionMode(QTableView::SingleSelection);
    t->verticalHeader()->setDefaultSectionSize(30);
    t->horizontalHeader()->setFixedHeight(30);
    t->setFixedHeight(qMin(500, 30 + rowcount * t->verticalHeader()->defaultSectionSize()));
    return t;

}

void w_bt_dish_paymode_teampay::set_ch_paymodeno(const QString &ch_paymodeno)
{
    tablemodel->setFilter(QString("ch_paymodeno = '%1' ").arg(ch_paymodeno));
}

void w_bt_dish_paymode_teampay::tonew()
{
}

void w_bt_dish_paymode_teampay::todel()
{
    tablemodel->removeRow(ui->tableView->currentIndex().row());
}

void w_bt_dish_paymode_teampay::torefresh()
{
    tablemodel->select();
}

void w_bt_dish_paymode_teampay::tonettest()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0 ) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    QString vch_paymodeaddress = tablemodel->model_data(row, "vch_arg2").toString();
    QProcess p;
    p.start(("/usr/desk/ie/QtWeb"" " + vch_paymodeaddress));
#ifndef QT_DEBUG
    if(p.waitForFinished(-1)) {
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("浏览器打开超时"));
    }
#endif
}

bool w_bt_dish_paymode_teampay::tosave()
{
    if(tablemodel->model_is_dirty()) {
        if(tablemodel->trySubmitAll()) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
            return true;
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        return false;
    }
    return true;
}

void w_bt_dish_paymode_teampay::toexit()
{
    if(tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!tosave()) {
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }
    this->reject();
}

void w_bt_dish_paymode_teampay::newpaymode_team(const QModelIndex &index)
{
    QPair<QString, QString> def(QObject::tr("外卖团购"), QObject::tr("外卖团购"));
    int rowcount = tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount,  tablemodel->fieldIndex("ch_paymodeno"), tablemodel->max_column_add_1__max_sql_add_1_dec("ch_paymodeno", "%02d"));
    tablemodel->model_data_set(rowcount,  tablemodel->fieldIndex("vch_paymodename"), w_bt_dish::get_kvaddresslist().value(index.row(),def).first);
    tablemodel->model_data_set(rowcount,  tablemodel->fieldIndex("vch_arg1"), w_bt_dish::get_kvaddresslist().value(index.row(), def).first);
    tablemodel->model_data_set(rowcount,  tablemodel->fieldIndex("vch_arg2"), w_bt_dish::get_kvaddresslist().value(index.row()).second);
    tablemodel->model_data_set(rowcount,  tablemodel->fieldIndex("vch_arg4"), "T");

    menu->close();
}


Qt::ItemFlags w_bt_dish_paymode_set::paymode_model::flags(const QModelIndex &index) const
{
    if(index.column() == 0) {
        return Qt::NoItemFlags;
    }
    return QStandardItemModel::flags(index);
}

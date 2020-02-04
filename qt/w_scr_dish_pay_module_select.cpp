#include "w_scr_dish_pay_module_select.h"
#include "ui_w_scr_dish_pay_module_select.h"
#include "ftableview_standmodel_sql_none.h"
#include "ftableview_delegate.h"
#include "lds.h"
#include "lds_roundeddialog_rect_align.h"
#include <QFile>
#include "lds_messagebox.h"
#include "lds_query_hddpos.h"
#include "w_scr_dish_pay_widget.h"
#include "saas_pay_detail.h"
#include "fastfd_update.h"
#include <QtDebug>

QList<QPair<QString, QString> >   w_scr_dish_pay_module_select::pay_module_list;
int w_scr_dish_pay_module_select::paymodecount = 0;

w_scr_dish_pay_module_select::w_scr_dish_pay_module_select(const QStringList &cur_select_items, int operatMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_pay_module_select)
{
    ui->setupUi(this);

    QList<ftableview_standmodel_sql_none::noneData> list_data;
    for(int k = 0; k <get_pay_module_list(operatMode).count(); k++) {
        list_data << ftableview_standmodel_sql_none::noneData(
                         get_pay_module_list(operatMode)[k].second,
                         get_pay_module_list(operatMode)[k].first);
    }

    ftableview_standmodel_sql_none *model = new ftableview_standmodel_sql_none;
    model->setPrepareList(list_data);
    ftableview_delegate *d = new ftableview_delegate(ui->tableView);
    d->keepConnect(ui->tableView);
    //    d->setMargin(1);
    d->indexisEmptyDrawGrad = true;
    for(int k = 0; k < cur_select_items.count(); k++) {
        model->set_list_data(cur_select_items[k], 1, Qt::UserRole + 6);
    }

    ui->tableView->setModel(model);
    ui->tableView->setProperty("outer_stylesheet", "tableView_type");
    ui->tableView->setItemDelegate(d);
    ui->tableView->updateFixed_fillData_addAnimation(3, 3, width() / 3, 60);
    //
//    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
//    ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::Stretch);

    model->toinit();
    //

    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_default, SIGNAL(clicked()),this,SLOT(todefault()));
    connect(ui->tableView, SIGNAL(signalclick(QModelIndex)), this, SLOT(tocheckremove(QModelIndex)));
}

w_scr_dish_pay_module_select::~w_scr_dish_pay_module_select()
{
    delete ui;
}

//"hyk" "会员卡"
//"_03" "银行卡"
//"_04" "代金券"
//"_05" "微信支付"
//"_06" "支付宝支付"
//"hygz" "会员挂账"
//"_AA" "摇钱树支付"
//"_ZZ" "优惠"
//"_07" "百度糯米"
//"_08" "大众点评"
//"_09" "美团"
//"_12" "支付宝付款"
//"_13" "微信付款"
//"cz" "充值"
//"zk" "折扣"
//"qx" "取消"
//"jssz" "结算设置"
//"mdxc" "免低消差"
//"mfwf" "免服务费"

QList<QPair<QString, QString> > &w_scr_dish_pay_module_select::get_pay_module_list(int operatMode)
{
    int cey_bt_paymode_count = lds_query_hddpos::selectValue("select count(0) from cey_bt_paymode").toInt();
    if(pay_module_list.count() == 0 || paymodecount != cey_bt_paymode_count) {
        paymodecount = cey_bt_paymode_count;
        pay_module_list.clear();

        pay_module_list.append(QPair<QString, QString>("sc", QObject::tr("删除")));
        lds_query_hddpos  query;
        query.execSelect("select ch_paymodeno, vch_paymodename from cey_bt_paymode where vch_arg4 = 'N'; ");//默认的
        while(query.next()) {
            QString ch_paymodeno = query.recordValue("ch_paymodeno").toString();
            QString vch_paymodename = query.recordValue("vch_paymodename").toString();
            //现金
            if("00" == ch_paymodeno) {
                continue;
            }
            //自动抹零
            if("YY" == ch_paymodeno) {
                continue;
            }
            //充值卡云
            if("10" == ch_paymodeno) {//hyk
                continue;
            }

            //充值卡
            if("02" == ch_paymodeno) {//hyk
                continue;
            }
            pay_module_list.append(QPair<QString, QString>(ch_paymodeno, vch_paymodename));
        }

        query.execSelect("select ch_paymodeno, vch_paymodename from cey_bt_paymode where vch_arg4 = 'T' ; ");//所有的团购
        while(query.next()) {
            QString ch_paymodeno = query.recordValue("ch_paymodeno").toString();
            QString vch_paymodename = query.recordValue("vch_paymodename").toString();
            pay_module_list.append(QPair<QString, QString>(ch_paymodeno, vch_paymodename));
        }

        query.execSelect("select ch_paymodeno, vch_paymodename from cey_bt_paymode where vch_arg4 = 'S' ; ");//所有的新增
        while(query.next()) {
            QString ch_paymodeno = query.recordValue("ch_paymodeno").toString();
            QString vch_paymodename = query.recordValue("vch_paymodename").toString();
            pay_module_list.append(QPair<QString, QString>(ch_paymodeno, vch_paymodename));
        }

        //        pay_module_list.append(QPair<QString, QString>("ydzf", QObject::tr("摇钱树支付")));
        //        pay_module_list.append(QPair<QString, QString>("zfb", QObject::tr("支付宝")));
        //        pay_module_list.append(QPair<QString, QString>("yhk", QObject::tr("银行卡")));
        pay_module_list.append(QPair<QString, QString>("hyk", QObject::tr("会员卡")));
        pay_module_list.append(QPair<QString, QString>("cz", QObject::tr("充值")));

        //        pay_module_list.append(QPair<QString, QString>("djq", QObject::tr("代金券")));
        //        pay_module_list.append(QPair<QString, QString>("mt", QObject::tr("美团")));
        //        pay_module_list.append(QPair<QString, QString>("dzdp", QObject::tr("大众点评")));
        pay_module_list.append(QPair<QString, QString>("zk", QObject::tr("折扣")));

        pay_module_list.append(QPair<QString, QString>("qx", QObject::tr("取消")));
        pay_module_list.append(QPair<QString, QString>("jssz", QObject::tr("结算设置")));
        pay_module_list.append(QPair<QString, QString>("dyxp", QObject::tr("打印小票")));
        //        pay_module_list.append(QPair<QString, QString>("yh", QObject::tr("优惠")));
        //        pay_module_list.append(QPair<QString, QString>("wxzf", QObject::tr("微信支付")));

        //        pay_module_list.append(QPair<QString, QString>("hygz", QObject::tr("会员挂账")));
    }

    pay_module_list_show("mdxc", QObject::tr("免低消差"), operatMode);
    pay_module_list_show("mfwf", QObject::tr("免服务费"), operatMode);
    pay_module_list_show("fwf", QObject::tr("服务费"), operatMode);

    return pay_module_list;
}

void w_scr_dish_pay_module_select::pay_module_list_clear()
{
    pay_module_list.clear();
}

QString w_scr_dish_pay_module_select::pay_module_list_value(const QString &key, int operatMode)
{
    if(0 == pay_module_list.count()) {
        get_pay_module_list(operatMode);
    }

    for(int k = 0; k < pay_module_list.count(); k++) {
        if(pay_module_list[k].first == key) {
            return pay_module_list[k].second;
        }
    }
    return "";
}

int w_scr_dish_pay_module_select::pay_module_list_index(const QString &key, int operatMode)
{
    if(0 == pay_module_list.count()) {
        get_pay_module_list(operatMode);
    }

    for(int k = 0; k < pay_module_list.count(); k++) {
        if(pay_module_list[k].first == key) {
            return k;
        }
    }
    return -1;
}

QString w_scr_dish_pay_module_select::get_select_module_key()
{
    return select_module_key;
}

bool w_scr_dish_pay_module_select::pay_section_valid(const QString &key, int operatMode)
{
    if(key == "mdxc" //免低消差
            || key == "mfwf" //免服务费
            ) {
        if(w_scr_dish_pay_widget::TYPE_FAST & operatMode) {
            return false;
        }
    }
    return true;
}

void w_scr_dish_pay_module_select::pay_module_list_insert(const QString &key, const QString &text)
{
    for(int k = 0; k < pay_module_list.count(); k++) {
        if(pay_module_list[k].first == key) {
            pay_module_list[k].second = text;
            return;
        }
    }
    pay_module_list << qMakePair(key, text);
}

void w_scr_dish_pay_module_select::pay_module_list_remove(const QString &key)
{
    for(int k = 0; k < pay_module_list.count(); k++) {
        if(pay_module_list[k].first == key) {
            pay_module_list.removeAt(k);
            return;
        }
    }
}

void w_scr_dish_pay_module_select::pay_module_list_show(const QString &key, const QString &text, int operatMode)
{
    if(pay_section_valid(key, operatMode)) {
        pay_module_list_insert(key, text);
    } else {
        pay_module_list_remove(key);
    }
}

void w_scr_dish_pay_module_select::clearCache()
{
    pay_module_list.clear();
}

void w_scr_dish_pay_module_select::took()
{
    if(!ui->tableView->currentIndex().isValid()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    select_module_key  = ui->tableView->currentIndex().data(Qt::UserRole + 1).toString();
    if(select_module_key == "-1"
            || select_module_key == "-2"
            ) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
    }

    if(select_module_key == "sc") {
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"), QObject::tr("是"), QObject::tr("否"))) {
            this->done(dialog_remove);
        }
    } else {
        this->accept();
    }
}

void w_scr_dish_pay_module_select::tocancel()
{

    this->reject();
}

void w_scr_dish_pay_module_select::tocheckremove(const QModelIndex &index)
{
    if("sc" == index.data(Qt::UserRole + 1).toString()) {
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"), QObject::tr("确认"), QObject::tr("取消"))) {
            this->done(dialog_remove);
        }
    }
}

void w_scr_dish_pay_module_select::todefault()
{
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("恢复默认"), QObject::tr("确认"), QObject::tr("取消"))) {
        this->done(dialog_default);
    }
}


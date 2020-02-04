#include "w_sys_manage_outer_pay_set.h"
#include "ui_w_sys_manage_outer_pay_set.h"
#include "ftableview_delegate.h"
#include "ftableview_standmodel_sql_none.h"
#include "n_func.h"
#include "lds.h"
#include "public_printby_ch_billno.h"
#include "lds_menu.h"
#include "lds.h"
#include "lds_messagebox.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "w_sys_manage_sqlcopy.h"
#include "public_sql.h"
#include "n_func.h"
#include <QSqlError>
#include "w_sys_manage_businessdataclear.h"
#include "w_sys_manage_outer_pay_set_sql.h"
#include "printer_pos.h"
#include "w_scr_dish.h"
#include <QFile>
#include <QTextEdit>
#include "back_view_dialog.h"
#include "lds_tableview_delegate_radio.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include <QDir>
#include "business_data_upload_isoft.h"
#include "public_logindialog.h"
#include "ui_w_sys_manage_outer_pay_set_isoft.h"
#include "ui_w_sys_manage_outer_pay_set_xiexin.h"
#include "w_scr_dish_data_collection.h"
#include "w_sys_manage_outer_pay_set_jingquan.h"
#include "w_sys_manage_outer_pay_set_fengze.h"
#include "w_sys_manage_outer_pay_set_tax_type.h"
#include "w_sys_manage_outer_pay_set_mt_smart_cate.h"
#include "w_scr_dish_language_switch_set.h"
#include "w_sys_manage_outer_pay_set_locale_wizard.h"
#include "w_sys_manage_outer_settings.h"
#include "w_sys_manage_cloudsync_transfer.h"
#include "ui_w_sys_manage_outer_pay_set_ftp.h"
#include "ui_w_sys_manage_outer_pay_set_kechuan.h"
#include "business_data_upload_restful.h"
#include "business_interface_malayisa_hourly_pos.h"

QList<QPair<QString, QString> > w_sys_manage_outer_pay_set::languagelist;

w_sys_manage_outer_pay_set::w_sys_manage_outer_pay_set(QWidget *parent) :
    public_Dialog(parent),
    ui(new Ui::w_sys_manage_outer_pay_set)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    frame_upload_dialog = 0;
    //1
    backlist_model = new ftableview_standmodel_sql_none;
    backlist_model->set_row_column(6, 1);

    backlist_model->model_data_set(0, 0, "anyvalue", Qt::UserRole + 1);
    backlist_model->model_data_set(1, 0, "anyvalue", Qt::UserRole + 1);
    backlist_model->model_data_set(2, 0, "anyvalue", Qt::UserRole + 1);
    backlist_model->model_data_set(3, 0, "anyvalue", Qt::UserRole + 1);
    backlist_model->model_data_set(4, 0, "anyvalue", Qt::UserRole + 1);
    backlist_model->model_data_set(5, 0, "anyvalue", Qt::UserRole + 1);
    backlist_model->model_data_set(6, 0, "anyvalue", Qt::UserRole + 1);

    ftableview_delegate *d = new ftableview_delegate(ui->tableView_list);
    d->keepConnect(ui->tableView_list);
    d->indexisEmptyDrawGrad = true;
    ui->tableView_list->setProperty("outer_stylesheet", "tableView_type");

    ui->tableView_list->setModel(backlist_model);
    ui->tableView_list->setItemDelegate(d);
    ui->tableView_list->setHeaderResizeMode(Qt::Horizontal, QHeaderView::Stretch);
    ui->tableView_list->verticalHeader()->setDefaultSectionSize(50);
    ui->tableView_list->setCurrentIndex(backlist_model->index(0, 0));
    //
    ui->frame_upload->hide();
    ui->frame_liaoning_inventory->hide();

    ui->stackedWidget_2->setCurrentIndex(0);
    retranslateView();


    toupdateSection(ui->tableView_list->currentIndex());
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->tableView_list,SIGNAL(clicked(QModelIndex)),this,SLOT(toupdateSection(QModelIndex)));
    connect(ui->comboBox_blotout_round,SIGNAL(activated(int)),this,SLOT(toshowround(int)));

    //    connect(ui->pushButton_skin_app,SIGNAL(clicked()),this,SLOT(toskinapp()));
    connect(ui->pushButton_function, SIGNAL(clicked()),this,SLOT(tofunction()));
    //    connect(ui->pushButton_function_2, SIGNAL(clicked()),this,SLOT(tofunction_2()));
    connect(ui->pushButton_function_3, SIGNAL(clicked()),this,SLOT(tofunction_3()));
    //    connect(ui->pushButton_function_4, SIGNAL(clicked()),this,SLOT(tofunction_4()));
    connect(ui->pushButton_function_5, SIGNAL(clicked()),this,SLOT(tofunction_5()));
    connect(ui->pushButton_function_7, SIGNAL(clicked()),this,SLOT(tofunction_7()));
    connect(ui->pushButton_function_8, SIGNAL(clicked()),this,SLOT(tofunction_8()));
    connect(ui->pushButton_function_9, SIGNAL(clicked()),this,SLOT(tofunction_9()));
    connect(ui->pushButton_function_10, SIGNAL(clicked()),this,SLOT(tofunction_10()));
    connect(ui->pushButton_function_11, SIGNAL(clicked()),this,SLOT(tofunction_11()));

    connect(ui->radioButton_upload1, SIGNAL(clicked(bool)),this, SLOT(toupload1(bool)));
    connect(ui->radioButton_upload2, SIGNAL(clicked(bool)),this, SLOT(toupload2(bool)));
    connect(ui->radioButton_upload3, SIGNAL(clicked(bool)),this, SLOT(toupload3(bool)));
    connect(ui->radioButton_upload4, SIGNAL(clicked(bool)),this, SLOT(toupload4(bool)));
    connect(ui->radioButton_upload_restful_interface, SIGNAL(clicked(bool)),this, SLOT(torestful_interface(bool)));
    connect(ui->radioButton_upload_hourly_pos_interface, SIGNAL(clicked(bool)),this, SLOT(tohourly_pos_interface(bool)));
    connect(ui->radioButton_liaoning_inventory_keep, SIGNAL(clicked(bool)),this, SLOT(toliaoning_inventory_keep(bool)));
    connect(ui->radioButton_upload_jingquan, SIGNAL(clicked(bool)),this, SLOT(toupload_jingquan(bool)));
    connect(ui->radioButton_upload_fengze, SIGNAL(clicked(bool)),this, SLOT(toupload_upload_fengze(bool)));
    connect(ui->pushButton_dishNo2barcode, SIGNAL(clicked(bool)),this, SLOT(toDishNo2barcode()));
    connect(ui->pushButton_all_invent, SIGNAL(clicked(bool)),this, SLOT(toAlldish2invent()));
    connect(ui->pushButton_all_weight, SIGNAL(clicked(bool)),this, SLOT(toAlldish2weight()));

    connect(ui->lineEdit_upload_awake, SIGNAL(textChanged(QString)),this,SLOT(touploadaware(QString)));

    connect(ui->checkBox_4, SIGNAL(clicked(bool)), this,SLOT(toSaaSCheck(bool)));

    //暂时不启用
    ui->radioButton_liaoning_inventory_keep->hide();
    lds::hideWidget(ui->comboBox_font_select);
    lds::hideWidget(ui->label_7);
    lds::hideWidget(ui->pushButton_function_11);


#ifdef QT_SCPOS_SUPPORT
    lds::hideWidget(ui->label_yingye_model);
    lds::hideWidget(ui->comboBox_yingye_model);
    lds::hideWidget(ui->checkBox_15);
    lds::hideWidget(ui->checkBox_samedishNotadd_qtprint);
    lds::hideWidget(ui->checkBox_qr_code_order_mode);
    lds::hideWidget(ui->checkBox_13);//中餐下退出录入操作员
#endif

    //    lds::hideWidget(ui->pushButton_tax_type);
}

w_sys_manage_outer_pay_set::~w_sys_manage_outer_pay_set()
{
    delete ui;
}

void w_sys_manage_outer_pay_set::dataCheck(QWidget *parent, bool ispop)
{
    lds_query_hddpos::tran_saction();
    qlonglong errcount = 0;
    lds_query_hddpos  query;
    QStringList ch_payno_list;
    QStringList ch_billno_list;
    //1
    query.execSelect("select count(0) from cey_u_checkout_master where ch_billno = '' or ch_tableno = '' ");
    if(query.next()) {
        errcount += query.recordValue(0).toLongLong();
        if(!query.execDelete("cey_u_checkout_master",  "ch_billno = '' or ch_tableno = '' ")) {
            goto failure;
        }
    }
    //2
    query.execSelect("select count(0) from cey_u_orderdish where ch_billno = '' or ch_tableno = '' ");
    if(query.next()) {
        errcount += query.recordValue(0).toLongLong();
        if(!query.execDelete("cey_u_orderdish",  " ch_billno = '' or ch_tableno = '' ")) {
            goto failure;
        }
    }
    //3
    query.execSelect("select count(0) from cey_u_master where ch_billno = '' ");
    if(query.next()) {
        errcount += query.recordValue(0).toLongLong();
        if(!query.execDelete("cey_u_master",  "ch_billno = '' ")) {
            goto failure;
        }
    }
    //4
    query.execSelect("select count(0) from cey_u_checkout_detail where ch_paymodeno = '' ");
    if(query.next()) {
        errcount += query.recordValue(0).toLongLong();
        if(!query.execDelete("cey_u_checkout_detail", "ch_paymodeno = '' ")) {
            goto failure;
        }
    }
    //6
    query.execSelect("select count(0) from t_m_hang_balance where isnull(num_remain)");
    if(query.next()) {
        errcount += query.recordValue(0).toLongLong();
        if(!query.execUpdate("t_m_hang_balance", "num_remain", qrtFunc("num_pay"), "isnull(num_remain)")) {
            goto failure;
        }
    }
    //    7.
    query.execSelect("select count(0) from cey_u_master where  ch_billno like '%,%' ");
    if(query.next()) {
        errcount += query.recordValue(0).toLongLong();
        if(!query.execDelete("cey_u_master", "ch_billno like '%,%' ")) {
            goto failure;
        }
    }

    ch_payno_list.clear();
    query.execSelect(
                " select (t2.p), (t3.p), t3.ch_payno from "
                " (Select a.num_payamount as p, a.ch_payno from cey_u_checkout_detail a, cey_u_checkout_master b, cey_bt_paymode c WHERE a.ch_payno = b.ch_payno and b.ch_state = 'Y' and a.ch_paymodeno = c.ch_paymodeno and b.dt_operdate > '2016-10-19 00:00:00' and b.dt_operdate <='2016-10-19 23:59:59') t3"

                " left join ( select sum(amount - discount - present) as p, ch_payno from (SELECT b.ch_payno, a.ch_dishno, c.vch_dishname,   c.ch_unitno,    (a.num_num - a.num_back) as 'num',  ((a.num_num - a.num_back) * a.num_price + a.num_price_add) as 'amount',  ((a.num_num - a.num_back) * a.num_price + a.num_price_add) * (1 - a.int_discount * 0.01) as 'discount',  case a.ch_presentflag when 'Y' then (((a.num_num - a.num_back) * a.num_price + a.num_price_add) * a.int_discount * 0.01) else 0 end as 'present',  b.vch_operID as 'vch_operID', c.ch_dish_typeno as 'typeno', b.dt_operdate as 'dt_operdate', a.vch_operID as 'vch_order', a.dt_operdate as 'dt_order' FROM cey_u_orderdish a,cey_u_checkout_master b,cey_bt_dish c WHERE(a.ch_payno = b.ch_payno)and(b.ch_state = 'Y')and(a.ch_dishno = c.ch_dishno)and(a.num_num - a.num_back <> 0)and(b.dt_operdate > '2016-10-19 00:00::00' and b.dt_operdate <='2016-10-19 23:59::59' and ''=''))t group by ch_payno) t2"

                " on t2.ch_payno = t3.ch_payno  where isnull(t2.p)");
    while(query.next()) {
        ch_payno_list.append(query.recordValue("ch_payno").toString());
    }
    foreach(const QString &p, ch_payno_list) {
        query.execDelete("cey_u_checkout_detail", QString("ch_payno = '%1'  ").arg(p));
        query.execDelete("cey_u_checkout_detail", QString("ch_payno = '%1'  ").arg(p));
    }
    errcount += ch_payno_list.count();


    //清除业务数据后，正常打印厨显退货单的情况
    ch_billno_list.clear();
    query.execSelect("select ch_billno from (select count(0) as count, ch_billno, group_concat(ch_payno) as pays from cey_u_checkout_master where ch_state = 'Y' group by ch_billno ) t where t.count > 1;");
    while(query.next()) {
        errcount ++;
        QString ch_billno = query.recordValue("ch_billno").toString();
        ch_billno_list.append(ch_billno);
    }
    ch_payno_list.clear();
    foreach(const QString &ch_billno, ch_billno_list) {
        query.execSelect(QString("select ch_payno from cey_u_checkout_master where ch_billno = '%1' and ch_state = 'Y'  order by ch_payno asc").arg(ch_billno));
        while(query.next())
            ch_payno_list.append(query.recordValue("ch_payno").toString());
        if(!ch_payno_list.isEmpty())
            ch_payno_list.removeLast();
    }
    foreach(const QString &ch_payno, ch_payno_list) {
        query.execDelete("cey_u_checkout_master", QString("ch_payno = '%1' ").arg(ch_payno));
        query.execDelete("cey_u_checkout_detail", QString("ch_payno = '%1' ").arg(ch_payno));
    }

    query.execSelect("select count(0) from cey_u_checkout_master where ifnull(ch_payno,'') ='' ");
    query.next();
    if(query.recordValue(0).toInt()) {
        errcount += query.recordValue(0).toInt();
        query.execDelete("cey_u_checkout_master", "ifnull(ch_payno, '') = '' ");
    }
    query.execSelect("select count(0) from cey_u_checkout_detail where ifnull(ch_payno,'') ='' ");
    query.next();
    if(query.recordValue(0).toInt()) {
        errcount += query.recordValue(0).toInt();
        query.execDelete("cey_u_checkout_detail", "ifnull(ch_payno, '') = '' ");
    }

    query.execSelect("select count(0) from t_m_curamount where vch_memberno = '' ");
    query.next();
    errcount += query.recordValue(0).toInt();
    query.execDelete("t_m_curamount", "vch_memberno = '' ");

    if(errcount > 0) {
        lds_query_hddpos::com_mit();
        if(ispop)
            lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("成功清除了一些脏数据") + QString::number(errcount));
        return;
    }
failure:
    lds_query_hddpos::roll_back();
    if(errcount > 0) {
        if(ispop)lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    } else {
        if(ispop)lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("没有需要校对的数据"));
    }
}

void w_sys_manage_outer_pay_set::addLanguagesWithDefault(QComboBox *com)
{
    const QList<QPair<QString, QString> >& list = w_sys_manage_outer_pay_set::getLanguagelist();
    com->addItem(QObject::tr("默认"), "");
    for(int k = 0; k < list.count();k ++) {
        com->addItem(list[k].second, list[k].first);
    }
}

const QList<QPair<QString, QString> > &w_sys_manage_outer_pay_set::getLanguagelist()
{
    if(languagelist.isEmpty()) {

        QMap<QString, QString> map;
        map.insert("sq", QObject::tr("阿尔巴尼亚语"));
        map.insert("ar", QObject::tr("阿拉伯语"));
        map.insert("am", QObject::tr("阿姆哈拉语"));
        map.insert("az", QObject::tr("阿塞拜疆语"));
        map.insert("ga", QObject::tr("爱尔兰语"));
        map.insert("et", QObject::tr("爱沙尼亚语"));
        map.insert("eu", QObject::tr("巴斯克语"));
        map.insert("be", QObject::tr("白俄罗斯语"));
        map.insert("bg", QObject::tr("保加利亚语"));
        map.insert("is", QObject::tr("冰岛语"));
        map.insert("pl", QObject::tr("波兰语"));
        map.insert("bs", QObject::tr("波斯尼亚语"));
        map.insert("fa", QObject::tr("波斯语"));
        map.insert("af", QObject::tr("布尔语(南非荷兰语)"));
        map.insert("da", QObject::tr("丹麦语"));
        map.insert("de", QObject::tr("德语"));
        map.insert("ru", QObject::tr("俄语"));
        map.insert("fr", QObject::tr("法语"));
        map.insert("tl", QObject::tr("菲律宾语"));
        map.insert("fi", QObject::tr("芬兰语"));
        map.insert("fy", QObject::tr("弗里西语"));
        map.insert("km", QObject::tr("高棉语"));
        map.insert("ka", QObject::tr("格鲁吉亚语"));
        map.insert("gu", QObject::tr("古吉拉特语"));
        map.insert("kk", QObject::tr("哈萨克语"));
        map.insert("ht", QObject::tr("海地克里奥尔语"));
        map.insert("ko", QObject::tr("韩语"));
        map.insert("ha", QObject::tr("豪萨语"));
        map.insert("nl", QObject::tr("荷兰语"));
        map.insert("ky", QObject::tr("吉尔吉斯语"));
        map.insert("gl", QObject::tr("加利西亚语"));
        map.insert("ca", QObject::tr("加泰罗尼亚语"));
        map.insert("cs", QObject::tr("捷克语"));
        map.insert("kn", QObject::tr("卡纳达语"));
        map.insert("co", QObject::tr("科西嘉语"));
        map.insert("hr", QObject::tr("克罗地亚语"));
        map.insert("ku", QObject::tr("库尔德语"));
        map.insert("la", QObject::tr("拉丁语"));
        map.insert("lv", QObject::tr("拉脱维亚语"));
        map.insert("lo", QObject::tr("老挝语"));
        map.insert("lt", QObject::tr("立陶宛语"));
        map.insert("lb", QObject::tr("卢森堡语"));
        map.insert("ro", QObject::tr("罗马尼亚语"));
        map.insert("mg", QObject::tr("马尔加什语"));
        map.insert("mt", QObject::tr("马耳他语"));
        map.insert("mr", QObject::tr("马拉地语"));
        map.insert("ml", QObject::tr("马拉雅拉姆语"));
        map.insert("ms", QObject::tr("马来语"));
        map.insert("mk", QObject::tr("马其顿语"));
        map.insert("mi", QObject::tr("毛利语"));
        map.insert("mn", QObject::tr("蒙古语"));
        map.insert("bn", QObject::tr("孟加拉语"));
        map.insert("my", QObject::tr("缅甸语"));
        map.insert("hmn", QObject::tr("苗语"));
        map.insert("xh", QObject::tr("南非科萨语"));
        map.insert("zu", QObject::tr("南非祖鲁语"));
        map.insert("ne", QObject::tr("尼泊尔语"));
        map.insert("no", QObject::tr("挪威语"));
        map.insert("pa", QObject::tr("旁遮普语"));
        map.insert("pt", QObject::tr("葡萄牙语"));
        map.insert("ps", QObject::tr("普什图语"));
        map.insert("ny", QObject::tr("齐切瓦语"));
        map.insert("ja", QObject::tr("日语"));
        map.insert("sv", QObject::tr("瑞典语"));
        map.insert("sm", QObject::tr("萨摩亚语"));
        map.insert("sr", QObject::tr("塞尔维亚语"));
        map.insert("st", QObject::tr("塞索托语"));
        map.insert("si", QObject::tr("僧伽罗语"));
        map.insert("eo", QObject::tr("世界语"));
        map.insert("sk", QObject::tr("斯洛伐克语"));
        map.insert("sl", QObject::tr("斯洛文尼亚语"));
        map.insert("sw", QObject::tr("斯瓦希里语"));
        map.insert("gd", QObject::tr("苏格兰盖尔语"));
        map.insert("ceb", QObject::tr("宿务语"));
        map.insert("so", QObject::tr("索马里语"));
        map.insert("tg", QObject::tr("塔吉克语"));
        map.insert("te", QObject::tr("泰卢固语"));
        map.insert("ta", QObject::tr("泰米尔语"));
        map.insert("th", QObject::tr("泰语"));
        map.insert("tr", QObject::tr("土耳其语"));
        map.insert("cy", QObject::tr("威尔士语"));
        map.insert("ur", QObject::tr("乌尔都语"));
        map.insert("uk", QObject::tr("乌克兰语"));
        map.insert("uz", QObject::tr("乌兹别克语"));
        map.insert("es", QObject::tr("西班牙语"));
        map.insert("iw", QObject::tr("希伯来语"));
        map.insert("el", QObject::tr("希腊语"));
        map.insert("haw", QObject::tr("夏威夷语"));
        map.insert("sd", QObject::tr("信德语"));
        map.insert("hu", QObject::tr("匈牙利语"));
        map.insert("sn", QObject::tr("修纳语"));
        map.insert("hy", QObject::tr("亚美尼亚语"));
        map.insert("ig", QObject::tr("伊博语"));
        map.insert("it", QObject::tr("意大利语"));
        map.insert("yi", QObject::tr("意第绪语"));
        map.insert("hi", QObject::tr("印地语"));
        map.insert("su", QObject::tr("印尼巽他语"));
        map.insert("id", QObject::tr("印尼语"));
        map.insert("jw", QObject::tr("印尼爪哇语"));
        map.insert("en", QObject::tr("英语"));
        map.insert("yo", QObject::tr("约鲁巴语"));
        map.insert("vi", QObject::tr("越南语"));
        map.insert("zh-TW", QObject::tr("中文(繁体)"));
        map.insert("zh", QObject::tr("中文(简体)"));
        map.insert("uy", QObject::tr("维吾尔族语"));

        languagelist.append(QPair<QString, QString>("en", map.take("en")));
        languagelist.append(QPair<QString, QString>("zh", map.take("zh")));
        for(QMap<QString, QString>::iterator k = map.begin(); k != map.end(); k ++) {
            languagelist.append(QPair<QString, QString>(k.key(), k.value()));
        }
    }
    return languagelist;
}

//随系统 is empty
QString w_sys_manage_outer_pay_set::getLanguageValue(const QString &key)
{
    const QList<QPair<QString, QString> >& list = w_sys_manage_outer_pay_set::getLanguagelist();
    for(int k = 0; k < list.count();k ++) {
        if(list[k].first == key) {
            return list[k].second;
        }
    }
    return "";
}

QString w_sys_manage_outer_pay_set::getLanguageKeyLike(const QString &key_or_value)
{
    const QList<QPair<QString, QString> >& list = w_sys_manage_outer_pay_set::getLanguagelist();
    for(int k = 0; k < list.count();k ++) {
        if(list[k].first.toLower().startsWith(key_or_value.toLower())) return list[k].first;
        if(list[k].second.toLower().startsWith(key_or_value.toLower())) return list[k].first;
    }
    return "";
}

void w_sys_manage_outer_pay_set::clearCache()
{
    languagelist.clear();
}

void w_sys_manage_outer_pay_set::retranslateView()
{
    ui->retranslateUi(this);
    //
    backlist_model->model_data_set(0, 0,QObject::tr("基础设置"), Qt::UserRole);
    backlist_model->model_data_set(1, 0,QObject::tr("数据库设置"), Qt::UserRole);
    backlist_model->model_data_set(2, 0,QObject::tr("税率设置"), Qt::UserRole);
    backlist_model->model_data_set(3, 0,QObject::tr("功能设置") + "1", Qt::UserRole);
    backlist_model->model_data_set(4, 0,QObject::tr("功能设置") + "2", Qt::UserRole);
    backlist_model->model_data_set(5, 0,QObject::tr("功能设置") + "3", Qt::UserRole);


    comIndexSaveClear(ui->comboBox_blotout_round);
    ui->comboBox_blotout_round->addItem(QObject::tr("选用抹零"),1);
    ui->comboBox_blotout_round->addItem(QObject::tr("选用四舍五入"),2);

    comIndexSaveClear(ui->comboBox_blotout_mode);
    ui->comboBox_blotout_mode->addItem(QObject::tr("不抹零"), 1);
    ui->comboBox_blotout_mode->addItem(QObject::tr("抹零到元"), 2);
    ui->comboBox_blotout_mode->addItem(QObject::tr("抹零到十"), 3);
    ui->comboBox_blotout_mode->addItem(QObject::tr("抹零到百"), 4);

    comIndexSaveClear(ui->comboBox_round_mode);
    ui->comboBox_round_mode->addItem(QObject::tr("不四舍五入"), 1);
    ui->comboBox_round_mode->addItem(QObject::tr("到角"), 2);
    ui->comboBox_round_mode->addItem(QObject::tr("到元"), 3);
    ui->comboBox_round_mode->addItem(QObject::tr("到十元"), 4);
    ui->comboBox_round_mode->addItem(QObject::tr("到五厘"), 5);

    comIndexSaveClear(ui->comboBox_yingye_model);
    ui->comboBox_yingye_model->addItem(QObject::tr("快餐模式"));
    ui->comboBox_yingye_model->addItem(QObject::tr("酒楼模式"));
    ui->comboBox_yingye_model->addItem(QObject::tr("酒吧模式"));
    comIndexSaveClear(ui->comboBox_skin);
    ui->comboBox_skin->addItemsByMap(lds::skin_name_map);

    comIndexRestoreAll();
}


void w_sys_manage_outer_pay_set::took()
{
    foreach(int index, index_set) {
        if(!saveData(index, true))
            return;
    }

    this->accept();
}

void w_sys_manage_outer_pay_set::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_pay_set::toupdateSection(const QModelIndex &index)
{
    saveData(index.row());
}

void w_sys_manage_outer_pay_set::toskinapp()
{
    QString key = ui->comboBox_skin->curusrdata().toString();
    qApp->setStyleSheet(public_sql::stylesheetData(key));
    lds::sysconf->setValue("system_setting/skin_name", key);
}

void w_sys_manage_outer_pay_set::toshowround(int index)
{
    switch(index) {
    case 0:
        ui->comboBox_blotout_mode->show();
        ui->comboBox_round_mode->hide();
        break;
    case 1:
        ui->comboBox_blotout_mode->hide();
        ui->comboBox_round_mode->show();
        break;
    }
}

void w_sys_manage_outer_pay_set::tofunction()
{
    lds_query_hddpos  query;
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("该操作将会将会员特价同步为零售价,是否继续"), QObject::tr("继续"), QObject::tr("取消"))) {
        return;
    }
    if(query.execUpdate("cey_bt_dish", "num_m_price", qrtFunc("num_price"),  "")) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("同步成功"));
        return;
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("同步失败"));
    return;
}

void w_sys_manage_outer_pay_set::tofunction_3()
{
    lds_query_hddpos  query;
    //!等待
    lds_messagebox_loading_show loading(this, "", "WAITING...");
    loading.show_delay();
    //!~等待

    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(QObject::tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(QObject::tr("请输入密钥"));
    if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
        return;
    }
    if(inputdialog.ui->lineEdit->text() != n_func::f_get_godpassword()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
        return;
    }

    if(false == query.execDelete("cey_sys_sqlversion  ")) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
        return;
    }
    this->done(public_sql::Reboot);
    return;
}

void w_sys_manage_outer_pay_set::tofunction_5()
{
    switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("切换"), QObject::tr("中文"), QObject::tr("英文"), QObject::tr("取消"))) {
    case 0:
        if(w_sys_manage_sqlcopy::cnkeep()) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        break;
    case 1:
        if(w_sys_manage_sqlcopy::enkeep()) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        break;
    case 2:
        return;
        break;
    }
}

void w_sys_manage_outer_pay_set::tofunction_7()
{
    int msg_check = lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("业务数据清除"),
                                             QObject::tr("开业清库"), QObject::tr("营业数据"), QObject::tr("库存数据"), QObject::tr("取消"));
    if(3 == msg_check) {
        return;
    }
    QString table_name;
    lds_query_hddpos  query;
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(QObject::tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(QObject::tr("请输入密钥"));
    if(QDialog::Accepted==transparentCenterDialog(&inputdialog).exec()) {
        if(inputdialog.ui->lineEdit->text() != n_func::f_get_godpassword()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
            return;
        }
        lds_messagebox_loading_show loading(this, "");
        loading.show_delay();
        loading.setTile(QObject::tr("业务数据清除"));
        loading.setText("0/0");

        QStringList exec_sql_list;
        switch(msg_check) {
        case 0://开业清库
            query.execSelect(" show tables;");
            while(query.next()) {
                table_name = query.recordValue(0).toString();
                if(table_name == "cey_bt_kitchen_print"
                        || table_name == "cey_bt_kitchen_plan"
                        || table_name == "cey_sys_parameter"
                        || table_name == "t_m_parameter")
                    continue;
                exec_sql_list << "delete from " + table_name;
            }
            break;
        case 1://营业数据
            exec_sql_list << "delete from cey_u_checkout_detail";
            exec_sql_list << "delete from cey_u_handover_total";
            exec_sql_list << "delete from cey_u_handover_master";
            exec_sql_list << "delete from cey_u_orderdish";
            exec_sql_list << "delete from material_u_orderdish";

            exec_sql_list << "delete from cey_u_orderdish_print_tr";
            exec_sql_list << "delete from cey_u_orderdish_log";
            exec_sql_list << "delete from cey_u_service_low";
            exec_sql_list << "delete from cey_u_table";
            exec_sql_list << "delete from cey_u_togo";

            exec_sql_list << "delete from cey_u_master";
            exec_sql_list << "delete from cey_u_master_help";
            exec_sql_list << "delete from cey_u_checkout_master";
            exec_sql_list << "delete from cey_u_taxsale_master";
            exec_sql_list << "update cey_bt_table set ch_billno = '' ";
            break;
        case 2://库存数据
            exec_sql_list << "delete from cey_st_check_master";
            exec_sql_list << "delete from cey_st_check_master";
            exec_sql_list << "delete from cey_st_material_flow";
            exec_sql_list << "delete from cey_st_material_num";
            exec_sql_list << "delete from cey_st_sheet_detail";
            exec_sql_list << "delete from cey_st_sheet_master";

            exec_sql_list << "delete from cey_st_provider_pay";
            exec_sql_list << "delete from cey_st_provider_pay_flow";
            break;
        }
        lds_query_hddpos  query;
        //
        lds_query_hddpos::tran_saction();
        for(int k = 0; k < exec_sql_list.count(); k++) {
            if(false == query.execAlter(exec_sql_list[k])) {
                lds_query_hddpos::roll_back();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.errorString());
                return;
            }
            loading.setText(QString("%1/%2").arg(k + 1).arg(exec_sql_list.count()));
        }
        lds_query_hddpos::com_mit();
        //
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, loading.text() + "\n" + QObject::tr("操作成功"));
        this->done(public_sql::Reboot);
    }
}

void w_sys_manage_outer_pay_set::tofunction_8()
{
    dataCheck(this);
}

void w_sys_manage_outer_pay_set::tofunction_9()
{
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(QObject::tr("数量"));
    inputdialog.ui->lineEdit->setText("0");
    inputdialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == transparentCenterDialog(&inputdialog).exec()) {
        //1商品设置服务费、设置服务费率模式、设置服务费率值
        lds_query_hddpos  query;
        if(query.execUpdate("cey_bt_dish", "num_item_warn", inputdialog.ui->lineEdit->text(),  "")) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    }
}

void w_sys_manage_outer_pay_set::tofunction_10()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, _TEXT_SLOT(this), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }
    lds_query_hddpos::tran_saction();
    lds_query_hddpos  query;
    if(query.execUpdate("cey_bt_dish", "ch_groupno", qrtFunc("ch_dishno"),"")) {
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        return;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    return;
}

void w_sys_manage_outer_pay_set::tofunction_11()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("只会保留：厨打方案、打印机设置、会员设置、软件设置等设置?"), QObject::tr("确认"), QObject::tr("取消"))) {
        return;
    }
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(QObject::tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(QObject::tr("请输入密钥"));
    if(QDialog::Accepted==transparentCenterDialog(&inputdialog).exec()) {
        if(inputdialog.ui->lineEdit->text() != n_func::f_get_godpassword()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
            return;
        }
        lds_messagebox_loading_show loading(this, "");
        loading.show_delay();
        loading.setTile(QObject::tr("开业清库"));
        loading.setText("0/0");

        QStringList keep_table_list;
        keep_table_list << "cey_bt_kitchen_print";
        keep_table_list << "cey_sys_parameter";
        keep_table_list << "t_m_parameter";
        QStringList clear_table_list;
        lds_query_hddpos  query;
        //
        lds_query_hddpos::tran_saction();
        query.execSelect(" show tables;");
        while(query.next()) {
            if(keep_table_list.contains(query.recordValue(0).toString())) {
                qDebug()<< query.recordValue(0).toString();
                continue;
            }

            clear_table_list << query.recordValue(0).toString();
        }
        for(int k = 0; k < clear_table_list.count(); k++) {
            query.execDelete(clear_table_list[k]);
            loading.setText(QString("%1/%2").arg(k).arg(clear_table_list.count()));
        }
        lds_query_hddpos::com_mit();
        //
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        this->done(public_sql::Reboot);
    }
}

void w_sys_manage_outer_pay_set::toDishNo2barcode()
{
    QString sql_where;
    switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, _TEXT_SLOT(this), QObject::tr("称重商品"), QObject::tr("所有商品"), QObject::tr("取消"))) {
    case 0:
        sql_where = "ch_weight = 'Y' ";
        break;
    case 1:
        sql_where = "";
        break;
    case 2:return;
    }
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(QObject::tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(QObject::tr("请输入密钥"));
    if(QDialog::Accepted==transparentCenterDialog(&inputdialog).exec()) {
        if(inputdialog.ui->lineEdit->text() != n_func::f_get_godpassword()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
            return;
        }
        lds_query_hddpos  query;
        if(query.execUpdate("cey_bt_dish",  qrtVariantPairList()
                            << qrtVariantPair("vch_barcode", qrtFunc("ch_dishno")), sql_where)) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, query.recordError());
    }
}

void w_sys_manage_outer_pay_set::toAlldish2invent()
{
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(QObject::tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(QObject::tr("请输入密钥"));
    if(QDialog::Accepted==transparentCenterDialog(&inputdialog).exec()) {
        if(inputdialog.ui->lineEdit->text() != n_func::f_get_godpassword()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
            return;
        }
        lds_query_hddpos  query;
        if(query.execUpdate("cey_bt_dish",  "item_flag", "Y", "")) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, query.recordError());
    }
}

void w_sys_manage_outer_pay_set::toAlldish2weight()
{
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(QObject::tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(QObject::tr("请输入密钥"));
    if(QDialog::Accepted==transparentCenterDialog(&inputdialog).exec()) {
        if(inputdialog.ui->lineEdit->text() != n_func::f_get_godpassword()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
            return;
        }
        lds_query_hddpos  query;
        if(query.execUpdate("cey_bt_dish",  "ch_weight", "Y", "")) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, query.recordError());
    }
}

void w_sys_manage_outer_pay_set::totax_settax()
{
    //    tax_settax(ui->pushButton_tax_set_tax, "int_rate", "pay_fixed_tax_value");
}

void w_sys_manage_outer_pay_set::totax_settax_2()
{
    //    tax_settax(ui->pushButton_tax_set_tax_2, "int_rate_2", "pay_fixed_tax_2_value");
}

void w_sys_manage_outer_pay_set::totax_settax_3()
{
    //    tax_settax(ui->pushButton_tax_set_tax_3, "int_rate_3", "pay_fixed_tax_3_value");
}

void w_sys_manage_outer_pay_set::tax_settax(public_pushbuton_number *b, const QString &field_name, const QString &pay_fixed_tax_name)
{
}

void w_sys_manage_outer_pay_set::totax_setservice()
{

}

void w_sys_manage_outer_pay_set::totax_type()
{
}

void w_sys_manage_outer_pay_set::toupload1(bool f)
{
    if(false == f) return;

    w_sys_manage_outer_pay_set_ftp dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {

    }
}

void w_sys_manage_outer_pay_set::toupload2(bool f)
{
    if(false == f) return;

    w_sys_manage_outer_pay_set_kechuan dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {

    }
}

void w_sys_manage_outer_pay_set::toupload3(bool f)
{
    if(false == f) return;

    w_sys_manage_outer_pay_set_xiexin dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {

    }
}

void w_sys_manage_outer_pay_set::toupload4(bool f)
{
    if(false == f) return;

    w_sys_manage_outer_pay_set_isoft dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_outer_pay_set::torestful_interface(bool f)
{
    if(false == f) return;

    business_data_upload_restful dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_outer_pay_set::tohourly_pos_interface(bool f)
{
    if(false == f) return;

    business_interface_malayisa_hourly_pos dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_outer_pay_set::toliaoning_inventory_keep(bool f)
{
    if(false == f) return;

    ui->comboBox_liaoning_inventory_cache->clear();
    QString path = lds::localdataPath +"/liaoning_inventory_keep_cache";
    QDir().mkpath(path);
    QFileInfoList fileinfos=QDir(path).entryInfoList(QStringList() << "*.txt");
    foreach(const QFileInfo &info, fileinfos) {
        ui->comboBox_liaoning_inventory_cache->addItem(info.fileName(), info.filePath());
    }

    ui->frame_liaoning_inventory->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    ui->frame_liaoning_inventory->move(lds::getPopPoint(ui->lineEdit_upload_awake, ui->frame_upload));
    ui->frame_liaoning_inventory->raise();
    ui->frame_liaoning_inventory->show();
}

void w_sys_manage_outer_pay_set::toupload_jingquan(bool f)
{
    if(false == f) return;

    w_sys_manage_outer_pay_set_jingquan dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {

    }
}

void w_sys_manage_outer_pay_set::toupload_upload_fengze(bool f)
{
    if(false == f)
        return;
    w_sys_manage_outer_pay_set_fengze dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {

    }
}

void w_sys_manage_outer_pay_set::touploadaware(const QString &arg)
{
    if(arg.trimmed().toLower() == "sc") {//上传
        if(frame_upload_dialog == 0) {
            frame_upload_dialog = new QDialog(this);
            frame_upload_dialog->setWindowFlags(frame_upload_dialog->windowFlags() | Qt::FramelessWindowHint);
            frame_upload_dialog->resize(ui->frame_upload->size() + QSize(20, 20));
            connect(ui->pushButton_upload_exit, SIGNAL(clicked()), frame_upload_dialog, SLOT(reject()));

            ui->frame_upload->setParent(frame_upload_dialog);
            ui->frame_upload->move(10, 10);
            ui->frame_upload->show();
        }
        frame_upload_dialog->exec();

    } else if(arg.trimmed().toLower() == "sql") {
        w_sys_manage_outer_pay_set_sql dialog(this);
        dialog.setWindowTitle("SQL_RUN");
        dialog.exec();
    } else if(arg.trimmed().toLower() == "meituan") {
        w_sys_manage_outer_pay_set_mt_smart_cate dialog(this);
        transparentCenterDialog(&dialog).exec();
    }
}

bool  w_sys_manage_outer_pay_set::saveData(int index, bool issave)
{
    ui->stackedWidget_2->setCurrentIndex(index);
    lds_query_hddpos  query;
    switch(index) {
    case 0://基础设置
        if(issave) {
            n_func::f_set_sysparm_q(query,"blotout_mode", ui->comboBox_blotout_mode->curusrdata());
            n_func::f_set_sysparm_q(query,"round_mode", ui->comboBox_round_mode->curusrdata());
            n_func::f_set_sysparm_q(query,"blotout_round", ui->comboBox_blotout_round->curusrdata());

            lds::sysconf->setValue("system_setting/yingyemoshi", ui->comboBox_yingye_model->currentIndex());

            n_func::f_set_sysparm_q(query,"pay_auto_discount", ui->checkBox_auto_dis->isChecked()?"1":"0");
            n_func::f_set_sysparm_q(query,"pay_auto_discount_value", ui->spinBox_auto_dis_value->value());
            n_func::f_set_sysparm_q(query,"pay_max_change", ui->doubleSpinBox_pay_max_change->value());

            n_func::f_set_sysparm_q(query,"serial_no_begin", ui->lineEdit_serial_no_begin->text());
            n_func::f_set_sysparm_q(query,"serial_no_end", ui->lineEdit_serial_no_end->text());

            lds::sysconf->setValue("system_setting/qt_type_column_count", ui->lineEdit_type_columns->text());

            lds::sysconf->setValue("system_setting/qt_iconsize", ui->lineEdit_dish_columns->text()+"x"+ui->lineEdit_dish_rows->text());

            lds::sysconf->setValue("system_setting/getweigthtimeout", ui->lineEdit_auto_weight_timeout->text());
            //字体选择
            lds::sysconf->setValue("system_setting/sys_font", ui->comboBox_font_select->curusrdata().toString());

            n_func::f_set_sysparm("bp_fmt_num_dec", ui->bp_fmt_num_dec->value());
            n_func::f_set_sysparm("bp_fmt_price_dec", ui->bp_fmt_price_dec->value());
            n_func::f_set_sysparm("bp_fmt_amount_dec", ui->bp_fmt_amount_dec->value());

            n_func::f_set_sysparm("ftptp_use", ui->radioButton_upload1->isChecked()? "1" : "0");
            n_func::f_set_sysparm("njgj_use", ui->radioButton_upload2->isChecked()? "1" : "0");
            n_func::f_set_sysparm("xiexin_use", ui->radioButton_upload3->isChecked()? "1" : "0");
            n_func::f_set_sysparm("isoft_use", ui->radioButton_upload4->isChecked()? "1" : "0");
            n_func::f_set_sysparm("liaoning_inventory_keep", ui->radioButton_liaoning_inventory_keep->isChecked()? "1" : "0");
            n_func::f_set_sysparm("jingquan_interface", ui->radioButton_upload_jingquan->isChecked()? "1" : "0");
            n_func::f_set_sysparm("fengze_interface", ui->radioButton_upload_fengze->isChecked()? "1" : "0");
            n_func::f_set_sysparm("restful_interface", ui->radioButton_upload_restful_interface->isChecked()? "1" : "0");
            n_func::f_set_sysparm("hourly_pos_interface_enable", ui->radioButton_upload_hourly_pos_interface->isChecked()? "1" : "0");

            if(lds::sysconf->value("system_setting/skin_name", "obsidian").toString() != ui->comboBox_skin->curusrdata().toString()) {
                lds::sysconf->setValue("system_setting/skin_name", ui->comboBox_skin->curusrdata().toString());
                qApp->setStyleSheet(public_sql::stylesheetData(ui->comboBox_skin->curusrdata().toString()));
                toskinapp();
            }
            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            ui->comboBox_blotout_round->setcomusrdata(n_func::f_get_sysparm_q(query,"blotout_round"));
            ui->comboBox_blotout_mode->setcomusrdata(n_func::f_get_sysparm_q(query,"blotout_mode"));
            ui->comboBox_round_mode->setcomusrdata(n_func::f_get_sysparm_q(query,"round_mode"));
            toshowround(ui->comboBox_blotout_round->currentIndex());
            //

            //字体选择
            QDir dir("userdata/settings/font");
            if(!dir.exists()) dir.mkpath(dir.path());
            ui->comboBox_font_select->addItem("", "");
            QFileInfoList infolist = dir.entryInfoList(QStringList() << "*.ttf" << "*.TTF");
            foreach(const QFileInfo &info, infolist) {
                if(info.baseName() == "." || info.baseName() == "..") continue;
                ui->comboBox_font_select->addItem(info.baseName(), info.fileName());
            }
            ui->comboBox_font_select->setCurrentIndex(ui->comboBox_font_select->findData(lds::sysconf->value("system_setting/sys_font").toString()));

            //
            ui->comboBox_yingye_model->setCurrentIndex(lds::sysconf->value("system_setting/yingyemoshi").toInt());

            ui->checkBox_auto_dis->setChecked(n_func::f_get_sysparm_q(query,"pay_auto_discount", "0") == "1");
            ui->spinBox_auto_dis_value->setValue(n_func::f_get_sysparm_q(query,"pay_auto_discount_value", 0).toInt());
            ui->doubleSpinBox_pay_max_change->setValue(n_func::f_get_sysparm_q(query,"pay_max_change", (lds::get_soft_curent_language_first() == _CN_ ? 100 : 100000)).toDouble());

            ui->lineEdit_serial_no_begin->setText(n_func::f_get_sysparm_q(query,"serial_no_begin"));
            ui->lineEdit_serial_no_end->setText(n_func::f_get_sysparm_q(query,"serial_no_end"));

            ui->lineEdit_type_columns->setText(lds::sysconf->value("system_setting/qt_type_column_count", "5").toString());

            QStringList xy = lds::sysconf->value("system_setting/qt_iconsize", "5x4").toString().split("x");
            ui->lineEdit_dish_columns->setText(xy.value(0));
            ui->lineEdit_dish_rows->setText(xy.value(1));

            ui->lineEdit_auto_weight_timeout->setText(lds::sysconf->value("system_setting/getweigthtimeout", "3").toString());

            ui->comboBox_skin->setCurrentIndex(ui->comboBox_skin->findData(lds::sysconf->value("system_setting/skin_name", "obsidian").toString()));

            ui->bp_fmt_num_dec->setValue(n_func::f_get_sysparm("bp_fmt_num_dec", 3).toInt());
            ui->bp_fmt_price_dec->setValue(n_func::f_get_sysparm("bp_fmt_price_dec", 2).toInt());
            ui->bp_fmt_amount_dec->setValue(n_func::f_get_sysparm("bp_fmt_amount_dec", 2).toInt());
            //科传接口
            ui->radioButton_upload_disable->setChecked(true);
            ui->radioButton_upload1->setChecked(n_func::f_get_sysparm("ftptp_use") == "1");
            ui->radioButton_upload2->setChecked(n_func::f_get_sysparm("njgj_use") == "1");
            ui->radioButton_upload3->setChecked(n_func::f_get_sysparm("xiexin_use") == "1");
            ui->radioButton_upload4->setChecked(n_func::f_get_sysparm("isoft_use") == "1");
            ui->radioButton_liaoning_inventory_keep->setChecked(n_func::f_get_sysparm("liaoning_inventory_keep") == "1");
            ui->radioButton_upload_jingquan->setChecked(n_func::f_get_sysparm("jingquan_interface") == "1");
            ui->radioButton_upload_fengze->setChecked(n_func::f_get_sysparm("fengze_interface") == "1");
            ui->radioButton_upload_restful_interface->setChecked(n_func::f_get_sysparm("restful_interface") == "1");
            ui->radioButton_upload_hourly_pos_interface->setChecked(n_func::f_get_sysparm("hourly_pos_interface_enable") == "1");
            //            connect(ui->pushButton_upload_exit, SIGNAL(clicked()), ui->frame_upload, SLOT(hide()));
            connect(ui->pushButton_liaoning_inventory_exit, SIGNAL(clicked()), ui->frame_liaoning_inventory, SLOT(hide()));
            connect(ui->pushButton_liaoning_inventory_upload, SIGNAL(clicked()), this, SLOT(to_liaoning_inventory_upload()));
            connect(ui->pushButton_liaoning_inventory_look, SIGNAL(clicked()),this, SLOT(to_liaoning_inventory_look()));
        }
        break;
        //////////////////////////////
    case 1://数据库设置
        break;

    case 2://税率设置
    {
        if(issave) {
            return true;
        }
        {
            lds::hideWidget(ui->pushButton_locale);
            w_sys_manage_outer_pay_set_locale_wizard d(this);
            transparentCenterDialog(&d).exec();
            ui->tableView_list->setCurrentIndex(ui->tableView_list->model()->index(0, 0));
            ui->stackedWidget_2->setCurrentIndex(0);
        }
        //        if(index_set.find(index) == index_set.end()) {
        //            ui->pushButton_locale->setText(n_func::f_get_sysparm("locale_country_name"));
        //            connect(ui->pushButton_locale, SIGNAL(clicked()), this, SLOT(to_app_locale_setttings()));
        //        }
    }
        break;

    case 3://功能设置
        if(issave) {
            //交班必须接班人
            n_func::f_set_sysparm_q(query,"handover_oper", ui->checkBox_1->isChecked()?"1":"0");
            //交班前显示交班信息
            n_func::f_set_sysparm_q(query,"show_info_before_shift", ui->checkBox_show_info_before_shift->isChecked()?"1":"0");
            //锁屏解锁必须为系统登录员工
            lds::sysconf->setValue("system_setting/jiesuoyuangong", ui->checkBox_7->isChecked()?"1":"0");
            //中餐下退出输入当前操作员信息
            lds::sysconf->setValue("system_setting/restaurant_inputoperinfo", ui->checkBox_13->isChecked());
            //是否输入手牌号
            lds::sysconf->setValue("system_setting/fukuanshishurushoupai", ui->checkBox_4->isChecked()?"1":"0");
            //退货必须输入退货原因
            n_func::f_set_sysparm_q(query,"orderdish_back", ui->checkBox_2->isChecked()?"1":"0");
            //手工处理服务费
            n_func::f_set_sysparm_q(query,"service_handwork", ui->checkBox_10->isChecked()?"1":"0");
            //点菜时相同商品不叠加
            lds::sysconf->setValue("system_setting/samedishNotadd_order", ui->checkBox_15->isChecked());
            //打印时相同商品不叠加
            lds::sysconf->setValue("system_setting/samedishNotadd_qtprint", ui->checkBox_samedishNotadd_qtprint->isChecked());
            //是否启用会员价
            n_func::f_set_sysparm_q(query,"bp_discountproject", ui->checkBox_8->isChecked()?"1":"0");
            //前台付款默认现金
            n_func::f_set_sysparm_q(query,"qt_def_money", ui->checkBox_qt_def_money->isChecked()?"1":"0");
            //退菜标识
            n_func::f_set_sysparm_q(query,"kt_kitchen_quit_flag", ui->checkBox_kitchen_quit_flag->isChecked()?"1":"0");
            //自动锁屏
            n_func::f_set_sysparm_q(query,"restrant_auto_lock_flag", ui->checkBox_restrant_auto_lock_flag->isChecked()?"1":"0");

            //            前台允许会员发卡
            //            lds::sysconf->setValue("system_setting/qt_can_member_sendcard", ui->checkBox_18->isChecked());
        }
        if(index_set.find(index) == index_set.end()) {

            //交班必须接班人
            ui->checkBox_1->setChecked(n_func::f_get_sysparm_q(query,"handover_oper") == "1");
            //交班前显示交班信息
            ui->checkBox_show_info_before_shift->setChecked(n_func::f_get_sysparm_q(query,"show_info_before_shift", "0") == "1");
            //锁屏解锁必须为系统登录员工
            ui->checkBox_7->setChecked(lds::sysconf->value("system_setting/jiesuoyuangong").toBool());
            //中餐下退出输入当前操作员信息
            ui->checkBox_13->setChecked(lds::sysconf->value("system_setting/restaurant_inputoperinfo",false).toBool());
            //是否输入手牌号
            ui->checkBox_4->setChecked(lds::sysconf->value("system_setting/fukuanshishurushoupai").toBool());
            //退货必须输入退货原因
            ui->checkBox_2->setChecked(n_func::f_get_sysparm_q(query,"orderdish_back") == "1");
            //手工处理服务费
            ui->checkBox_10->setChecked(n_func::f_get_sysparm_q(query,"service_handwork") == "1");
            //点菜时相同商品不叠加
            ui->checkBox_15->setChecked(lds::sysconf->value("system_setting/samedishNotadd_order", false).toBool());
            //打印时相同商品不叠加
            ui->checkBox_samedishNotadd_qtprint->setChecked(lds::sysconf->value("system_setting/samedishNotadd_qtprint", false).toBool());
            //是否启用会员价
            ui->checkBox_8->setChecked(n_func::f_get_sysparm_q(query,"bp_discountproject") == "1");
            //前台付款默认现金
            ui->checkBox_qt_def_money->setChecked(n_func::f_get_sysparm_q(query,"qt_def_money","1") == "1");
            //退菜标识
            ui->checkBox_kitchen_quit_flag->setChecked(n_func::f_get_sysparm_q(query,"kt_kitchen_quit_flag","1") == "1");
            //自动锁屏
            ui->checkBox_restrant_auto_lock_flag->setChecked(n_func::f_get_sysparm_q(query,"restrant_auto_lock_flag","1") == "1");
            //前台允许会员发卡
            //            ui->checkBox_18->setChecked(lds::sysconf->value("system_setting/qt_can_member_sendcard", false).toBool());
        }



        break;

    case 4://功能设置2
        if(issave) {
            //点菜时弹出商品详细
            n_func::f_set_sysparm_q(query,"ordertonum", ui->checkBox_6->isChecked()?"1":"0");
            //前台点菜时子套菜折叠
            n_func::f_set_sysparm_q(query,"qt_Y_dish_fold", ui->checkBox_19->isChecked()?"1":"0");
            //前台显示库存信息
            lds::sysconf->setValue("system_setting/inventory_qt_show_info", ui->checkBox_16->isChecked());
            //前台显示原料
            n_func::f_set_sysparm_q(query,"qt_show_ingredient", ui->checkBox_ingredient->isChecked()?"1":"0");
            //负库存可以消费
            n_func::f_set_sysparm_q(query,"inventory_negative_cannot_pay", ui->checkBox_17->isChecked()?"1":"0");
            //快餐开启会员付款
            lds::sysconf->setValue("system_setting/kuaican_mpay_enabled", ui->checkBox_kuaican_mpay->isChecked());
            //快餐开启快捷退货
            lds::sysconf->setValue("system_setting/kuaican_tuihuo_enabled", ui->checkBox_kuaican_tuihuo->isChecked());
            //快餐挂单打印
            lds::sysconf->setValue("system_setting/kuaicanprint_on_hang", ui->checkBox_kuaicanprint_on_hang->isChecked());
            //前台禁用长按功能
            n_func::f_set_sysparm_q(query,"qt_forbide_change_new", ui->checkBox_qt_forbide_change_new->isChecked()?"1":"0");
            //开启二维码点餐
            n_func::f_set_sysparm_q(query,"qr_code_order_mode", ui->checkBox_qr_code_order_mode->isChecked()?"1":"0");
            //开启快捷支付
            lds::sysconf->setValue("w_sys_manage_outer_pay_set/quickpay", ui->checkBox_quickpay->isChecked());
            //开启称重模式
            lds::sysconf->setValue("w_sys_manage_outer_pay_set/scale_led_mode", ui->checkBox_scale_led_mode->isChecked());
            //外卖送餐员默认0000
            lds::sysconf->setValue("system_setting/waimaidef0000", ui->checkBox->isChecked());
        }
        if(index_set.find(index) == index_set.end()) {
            //点菜时弹出商品详细
            ui->checkBox_6->setChecked(n_func::f_get_sysparm_q(query,"ordertonum") == "1");
            //前台显示库存信息
            ui->checkBox_16->setChecked(lds::sysconf->value("system_setting/inventory_qt_show_info", false).toBool());
            //负库存可以消费
            ui->checkBox_17->setChecked(n_func::f_get_sysparm_q(query,"inventory_negative_cannot_pay","0") == "1");
            //前台点菜时子套菜折叠
            ui->checkBox_19->setChecked(n_func::f_get_sysparm_q(query,"qt_Y_dish_fold","0") == "1");
            //前台显示原料
            ui->checkBox_ingredient->setChecked(n_func::f_get_sysparm_q(query,"qt_show_ingredient","0") == "1");
            //商品启用大类
            //            ui->checkBox_20->setChecked(lds::sysconf->value("system_setting/dish_type_enabled", false).toBool());
            //快餐开启会员付款
            ui->checkBox_kuaican_mpay->setChecked(lds::sysconf->value("system_setting/kuaican_mpay_enabled", false).toBool());
            //快餐开启快捷退货
            ui->checkBox_kuaican_tuihuo->setChecked(lds::sysconf->value("system_setting/kuaican_tuihuo_enabled", false).toBool());
            //快餐挂单打印
            ui->checkBox_kuaicanprint_on_hang->setChecked(lds::sysconf->value("system_setting/kuaicanprint_on_hang", false).toBool());
            //前台禁用长按功能
            ui->checkBox_qt_forbide_change_new->setChecked(n_func::f_get_sysparm_q(query,"qt_forbide_change_new","0") == "1");
            //开启二维码点餐
            ui->checkBox_qr_code_order_mode->setChecked(n_func::f_get_sysparm_q(query,"qr_code_order_mode","0") == "1");
            //开启快捷支付
            ui->checkBox_quickpay->setChecked(lds::sysconf->value("w_sys_manage_outer_pay_set/quickpay", false).toBool());
            //开启称重模式
            ui->checkBox_scale_led_mode->setChecked(lds::sysconf->value("w_sys_manage_outer_pay_set/scale_led_mode", false).toBool());

            //外卖送餐员默认0000
            ui->checkBox->setChecked(lds::sysconf->value("system_setting/waimaidef0000", false).toBool());
        }
        break;
    case 5://功能设置3
        if(issave) {
            //开启称重模式
            lds::sysconf->setValue("system_setting/login_with_pwd", ui->checkBox_login_with_pwd->isChecked()?"1":"0");
            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            ui->checkBox_login_with_pwd->setChecked(lds::sysconf->value("system_setting/login_with_pwd","0").toString() == "1");
        }
        break;
    }
    index_set.insert(index);
    return true;
}


QString w_sys_manage_outer_pay_set_ftp::FtpAddress = "FTP://ftp.lncrland.com";

w_sys_manage_outer_pay_set_ftp::w_sys_manage_outer_pay_set_ftp(QWidget *parent) :
    lds_model_mapwidget_vertical(parent), ui(new Ui_w_sys_manage_outer_pay_set_ftp)
{
    ui->setupUi(this);
    //
    scan_all_init_value("cey_sys_parameter", "vch_parameter", "vch_value", "ftptp_");
    //
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_test, SIGNAL(clicked()),this,SLOT(totest()));
    /*
    connect(ui->pushButton_detail, SIGNAL(clicked()),this,SLOT(todetail()));
    connect(ui->pushButton_example, SIGNAL(clicked()),this,SLOT(toexample()));*/
    //    ui->pushButton_detail->hide();

    //
    QString path = public_sql::ftp_businessdataupload_dir;
    QDir  dir(path);
    if(!dir.exists()) dir.mkpath(path);
    QFileInfoList fileinfos=dir.entryInfoList();
    ui->comboBox_records->clear();
    foreach(QFileInfo info, fileinfos) {
        if(info.fileName() == "."
                ||info.fileName() == "..")continue;
        ui->comboBox_records->addItem(info.fileName());
    }

    ui->ftptp_address->setText(w_sys_manage_outer_pay_set_ftp::FtpAddress);
}

w_sys_manage_outer_pay_set_ftp::~w_sys_manage_outer_pay_set_ftp()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_ftp::took()
{
    lds_query_hddpos::tran_saction();
    commitData();
    lds_query_hddpos::com_mit();
    this->accept();
}

void w_sys_manage_outer_pay_set_ftp::todetail()
{

}

void w_sys_manage_outer_pay_set_ftp::toexample()
{
    /*
商铺号 B41B0231N01

货号 B41B0231N01CY0001

FTP地址  ftp.lncrland.com

账号 B41B41B0231N01

密码 J9w8400Q*/
    ui->ftptp_storecode->setText("B41B0231N01");
    ui->ftptp_plu->setText("B41B0231N01CY0001");
    //    ui->ftptp_address->setText("FTP://ftp.lncrland.com");
    ui->ftptp_usr->setText("B41B0231N01");
    //    ui->ftptp_pwd->setText("J9w8400Q");
    //
    mappedSubmit();
}

void w_sys_manage_outer_pay_set_ftp::totest()
{
    tablemodel->trySubmitAll();

    QDate last_update_date = n_func::f_get_sysdatetime().date();
    QString ls_err;
    QList<w_scr_dish::HUARUN_BUSINESS_DATA> datalist;
    w_scr_dish::GET_HUARUN_BUSINESS_DATA(last_update_date, datalist, ls_err);
    int ret = w_scr_dish::HUARUN_BUSINESS_DATA_UPLOAD(w_sys_manage_outer_pay_set_ftp::FtpAddress
                                                      , n_func::f_get_sysparm("ftptp_usr")
                                                      , n_func::f_get_sysparm("ftptp_pwd")
                                                      , datalist
                                                      );
    if(w_scr_dish::huarun_ftp_server_success == ret) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        return;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, w_scr_dish::huarunInfo_error(ret));
}

void w_sys_manage_outer_pay_set_ftp::toaddress_def()
{
    //    ui->ftptp_address->setText("FTP://ftp.lncrland.com");
}

void w_sys_manage_outer_pay_set_ftp::saveData()
{

}

w_sys_manage_outer_pay_set_kechuan::w_sys_manage_outer_pay_set_kechuan(QWidget *parent)
    : lds_model_mapwidget_vertical(parent), ui(new Ui_w_sys_manage_outer_pay_set_kechuan)
{
    ui->setupUi(this);
    //
    scan_all_init_value("cey_sys_parameter", "vch_parameter", "vch_value", "njgj_");
    //
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_detail, SIGNAL(clicked()),this,SLOT(todetail()));
    connect(ui->pushButton_example, SIGNAL(clicked()),this,SLOT(toexample()));
}

w_sys_manage_outer_pay_set_kechuan::~w_sys_manage_outer_pay_set_kechuan()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_kechuan::took()
{
    commitData();
    this->accept();
}

void w_sys_manage_outer_pay_set_kechuan::todetail()
{
    w_scr_dish_data_collection dialog(this);
    dialog.setWindowTitle(QObject::tr("科传"));
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_outer_pay_set_kechuan::toexample()
{
    ui->njgj_ip->setText("211.94.93.147");
    ui->njgj_itemcode->setText("33201L15000131");
    ui->njgj_license_agreement->setText("");
    ui->njgj_mallid->setText("33201");
    ui->njgj_port->setText("");
    //    ui->njgj_port->setText("7088");
    ui->njgj_storecode->setText("33201L1500013");
    ui->njgj_username->setText("");
    //    ui->njgj_username->setText("jinmao");
    ui->njgj_web->setText("/salestrans.asmx");
    //
    mappedSubmit();
}

bool w_sys_manage_outer_pay_set_kechuan::insert_sql_objectname(const QString &objectname)
{
    lds_query_hddpos  query;
    return query.execInsert("cey_sys_parameter", qrtVariantPairList()
                            << qrtVariantPair("vch_parameter", objectname)
                            << qrtVariantPair("vch_value", ""));
}

//////////////////
w_sys_manage_outer_pay_set_xiexin::w_sys_manage_outer_pay_set_xiexin(QWidget *parent)
    : lds_model_mapwidget_vertical(parent)
{
    ui = new Ui_w_sys_manage_outer_pay_set_xiexin;
    ui->setupUi(this);

    //
    scan_all_init_value("cey_sys_parameter", "vch_parameter", "vch_value", "xiexin");
    //
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_test_upload_today, SIGNAL(clicked()),this,SLOT(touploadyesterday()));


    //
    QString path = lds::localdataPath +"/XIEXIN_BUSINESS_DATA_UPLOAD";
    QDir  dir(path);
    if(!dir.exists()) dir.mkpath(path);
    QFileInfoList fileinfos=dir.entryInfoList();
    ui->comboBox_records->clear();
    foreach(QFileInfo info, fileinfos) {
        if(info.fileName() == "."
                ||info.fileName() == "..")continue;
        ui->comboBox_records->addItem(info.fileName());
    }
    ui->pushButton_test_upload_today->show();
}
w_sys_manage_outer_pay_set_xiexin::~w_sys_manage_outer_pay_set_xiexin()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_xiexin::took()
{
    saveData();
    this->accept();
}

bool w_sys_manage_outer_pay_set_xiexin::insert_sql_objectname(const QString &objectname)
{
    lds_query_hddpos  query;
    return query.execInsert("cey_sys_parameter", qrtVariantPairList()
                            << qrtVariantPair("vch_parameter", objectname)
                            << qrtVariantPair("vch_value", ""));
}

void w_sys_manage_outer_pay_set_xiexin::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_pay_set_xiexin::touploadyesterday()
{
    saveData();

    QString errstring;
    if(1 == w_scr_dish::XIEXIN_BUSINESS_DATA_UPLOAD(n_func::f_get_sysdatetime().date(), &errstring)) {
        //
        QString path = lds::localdataPath +"/XIEXIN_BUSINESS_DATA_UPLOAD";
        QDir  dir(path);
        if(!dir.exists()) dir.mkpath(path);
        QFileInfoList fileinfos=dir.entryInfoList();
        ui->comboBox_records->clear();
        foreach(QFileInfo info, fileinfos) {
            if(info.fileName() == "."
                    ||info.fileName() == "..")continue;
            ui->comboBox_records->addItem(info.fileName());
        }
        //


        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("上传成功"));
        this->accept();
        return;
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+errstring);
    }
}

bool w_sys_manage_outer_pay_set_xiexin::saveData()
{
    if(n_func::f_get_sysparm("xiexin_lastdate").isEmpty())
        n_func::f_set_sysparm("xiexin_lastdate", n_func::f_get_sysdatetime().toString("yyyy-MM-dd"), "XIEXIN LAST UPLOAD DATE");
    commitData();

    return true;
}

void w_sys_manage_outer_pay_set::toSaaSCheck(bool check)
{
    if(!check) return;

    if(ui->checkBox_4 == this->sender()) {
        if(!saas_pay_detail::SaaSFuncCheck(this, "0101"/*手牌号*/)) {
            ui->checkBox_4->setChecked(false);
        }
    }
}

void w_sys_manage_outer_pay_set::to_liaoning_inventory_upload()
{
    w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad dialog(this);
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_outer_pay_set::to_liaoning_inventory_look()
{
    QDialog *dialog = new QDialog;
    dialog->setStyleSheet("border:2px solid #ff546b");
    dialog->setWindowFlags(Qt::Popup);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    QTextEdit *edit = new QTextEdit(dialog);
    dialog->resize(500, 500);
    edit->setGeometry(0, 0, 500, 500);
    dialog->move(lds::getGlobalCenterPopup(this, dialog));

    int index = ui->comboBox_liaoning_inventory_cache->currentIndex();
    QFile file;
    file.setFileName(ui->comboBox_liaoning_inventory_cache->itemData(index).toString());
    file.open(QFile::ReadOnly);
    edit->setText(file.readAll());
    file.close();

    dialog->exec();
}

void w_sys_manage_outer_pay_set::to_language_settings()
{
    //    w_scr_dish_language_switch_set dialog(this);
    //    dialog.setWindowTitle(QObject::tr("切换语言"));
    //    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
    //        public_Dialog::retranslateSystem(key);
    //    }
}

void w_sys_manage_outer_pay_set::to_app_locale_setttings()
{
    w_sys_manage_outer_pay_set_locale_wizard d(this);
    transparentCenterDialog(&d).exec();
    ui->pushButton_locale->setText(n_func::f_get_sysparm("locale_country_name"));
}

/////////////////////////////
w_sys_manage_outer_pay_set_isoft::w_sys_manage_outer_pay_set_isoft(QWidget *parent)
    : lds_model_mapwidget_vertical(parent)
{
    ui = new Ui_w_sys_manage_outer_pay_set_isoft;
    ui->setupUi(this);
    //
    scan_all_init_value("cey_sys_parameter", "vch_parameter", "vch_value", "isoft");

    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_show,SIGNAL(clicked()),this,SLOT(toshowcontent()));
    connect(ui->pushButton_reset,SIGNAL(clicked()),this,SLOT(to_reset()));

    //
    makeCache();
}

w_sys_manage_outer_pay_set_isoft::~w_sys_manage_outer_pay_set_isoft()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_isoft::makeCache()
{
    ui->comboBox_upload_caches->clear();

    QString path = lds::localdataPath +"/business_data_upload_isoft";
    QDir().mkpath(path);
    QFileInfoList fileinfos=QDir(path).entryInfoList(QStringList() << "*.txt");
    foreach(const QFileInfo &info, fileinfos) {
        ui->comboBox_upload_caches->addItem(info.fileName(), info.filePath());
    }
}

void w_sys_manage_outer_pay_set_isoft::took()
{
    commitData();
    this->accept();
}

void w_sys_manage_outer_pay_set_isoft::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_pay_set_isoft::toshowcontent()
{
    int index = ui->comboBox_upload_caches->currentIndex();
    if(index >= 0) {

        QDialog *dialog = new QDialog;
        dialog->setStyleSheet("border:2px solid #ff546b");
        dialog->setWindowFlags(Qt::Popup);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        QTextEdit *edit = new QTextEdit(dialog);
        dialog->resize(500, 500);
        edit->setGeometry(0, 0, 500, 500);
        dialog->move(lds::getGlobalCenterPopup(this, dialog));

        QFile file;
        file.setFileName(ui->comboBox_upload_caches->itemData(index).toString());
        file.open(QFile::ReadOnly);
        edit->setText(file.readAll());
        file.close();

        dialog->exec();
    }
}

void w_sys_manage_outer_pay_set_isoft::to_reset()
{
    //remove rmFilePathList
    QStringList rmFilePathList;
    QString path = lds::localdataPath +"/business_data_upload_isoft";
    foreach(const QFileInfo &info, QDir(path).entryInfoList()) {
        if(info.baseName() == ""
                || info.baseName() == "."
                || info.baseName() == "..") {
            continue;
        }

        if(info.baseName().endsWith("_error")) {
            rmFilePathList << info.filePath();
        }
    }
    foreach(const QString &filepath, rmFilePathList) {//删除错误新建_error;
        QFile::remove(filepath);
    }
    //重新生成上传文件
    foreach(const QFileInfo &info, QDir(path).entryInfoList()) {
        if(info.baseName() == ""
                || info.baseName() == "."
                || info.baseName() == "..") {
            continue;
        }
        business_data_upload_isoft::genrateBill(info.baseName());
    }
    //
    if(1 == lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"), QObject::tr("开始上传"), QObject::tr("取消"))) {
        return;
    }

    //
    lds_messagebox_loading_show loading(this, "");
    loading.show_delay();
    QEventLoop loop;
    connect(public_sql::save_login, SIGNAL(signal_isoft_upload_polling_upload(QString)),
            &loading, SLOT(setText(QString)));
    connect(public_sql::save_login, SIGNAL(signal_isoft_upload_polling_finished(QString)),
            &loading, SLOT(setText(QString)));
    connect(public_sql::save_login, SIGNAL(signal_isoft_upload_polling_finished(QString)),
            &loop, SLOT(quit()));
    public_sql::save_login->isoft_upload_polling->ToTryTrigger();
    loop.exec();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, loading.text());
    makeCache();
}

bool w_sys_manage_outer_pay_set_isoft::insert_sql_objectname(const QString &objectname)
{
    lds_query_hddpos  query;
    return query.execInsert("cey_sys_parameter", qrtVariantPairList()
                            << qrtVariantPair("vch_parameter", objectname)
                            << qrtVariantPair("vch_value", ""));
}


QByteArray w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad::car_name()
{
    return "顶有鸣餐车1";
}

w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad::w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad(QWidget *parent)
    : QDialog(parent)
{
    lds_tableView *tableview = new lds_tableView(this);
    standmodel = new lds_model_sqlstandardmodel(this);
    standmodel->setHorizontalHeaderLabels(QStringList() << QObject::tr("进度") << QObject::tr("商品") << QObject::tr("库存"));
    tableview->setModel(standmodel);
    tableview->setEditTriggers(QTableView::NoEditTriggers);
    tableview->setColumnWidth(0, 200);
    tableview->setColumnWidth(1, 250);
    tableview->setColumnWidth(3, 100);
    btn_exit =new QPushButton(this);
    btn_exit->setText(QObject::tr("退出"));
    label_desc = new QLabel(this);
    resize(800, 400);
    tableview->setGeometry(10, 10, width() - 20, height() - 20 - 50);
    btn_exit->setGeometry(width() -60, height() - 40, 50, 30);
    label_desc->setGeometry(10, height() - 40, width() - 80, 30);
    connect(btn_exit, SIGNAL(clicked()),this, SLOT(close()));
    btn_exit->hide();

    this->setWindowTitle(car_name());
    QTimer::singleShot(1000, this, SLOT(toUpLoad()));
}

void w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad::toUpLoad()
{
    QNetworkAccessManager manager;
    lds_query_hddpos  query;
    QString url;
    QNetworkReply *reply = 0;
    int httpStatus;
    QByteArray readall;
    QString err_code;
    double num_num;
    QString item_name;
    int item_count;
    int item_frame;
    int item_frame_success;

    query.execSelect(" select count(0) from cey_st_material_num a, cey_bt_dish b where a.ch_materialno = b.ch_dishno; ");
    query.next();
    item_count = query.recordValue(0).toInt();

    item_frame = 0;
    item_frame_success = 0;
    query.execSelect(" select b.vch_dishname, a.num_num from cey_st_material_num a, cey_bt_dish b where a.ch_materialno = b.ch_dishno; ");
    while(query.next()) {
        item_frame ++;
        item_name = query.recordValue("vch_dishname").toString();
        num_num = query.recordValue("num_num").toDouble();
        url = QString("http://www.ddyouming.com/interface/sysdata/Item_AddMenu?") +
                "item_name=" + item_name.toLocal8Bit().toPercentEncoding() +
                "&car_name=" + car_name().toPercentEncoding() +
                "&item_num=" + QString("%1").arg(num_num).toLocal8Bit().toPercentEncoding();
        reply = w_sys_manage_cloudsync_transfer::directGet(&manager, url);
        httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        readall = reply->readAll();
        if(httpStatus == 200) {
            err_code = cJSON_help::getcJSONvalue(readall, "err_code").toString();
            if(err_code == "0" || err_code == "1") {
                item_frame_success++;
                standmodel->appendRow(QList<QStandardItem*>()
                                      << standmodel->newItem(QPixmap(":/image/widgets_pic/radio_on.png"), QString("同步成功(%1/%2)").arg(item_frame).arg(item_count))
                                      << standmodel->newItem(item_name)
                                      << standmodel->newItem(num_num));
                continue;
            }
        }
        standmodel->appendRow(QList<QStandardItem*>()
                              << standmodel->newItem(QPixmap(":/image/widgets_pic/radio_off_warn.png"), QString("同步失败(%1/%2)").arg(item_frame).arg(item_count))
                              << standmodel->newItem(item_name)
                              << standmodel->newItem(num_num));
    }
    label_desc->setText(QObject::tr("上传成功") + QString(":%1/%2").arg(item_frame_success).arg(item_count));
    btn_exit->show();
}

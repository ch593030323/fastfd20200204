#include "saas_pay_detail.h"
#include "ui_saas_pay_detail.h"
#include <QStandardItemModel>
#include <QtDebug>
#include <QTimer>
#include "lds_messagebox.h"
#include "lds_query_hddpos.h"
#include "lds.h"
#include "saas_guide.h"
#include "cjson_help.h"
#include "saas_guide_activateterminal.h"
#include <QtDebug>
#include <QHostInfo>
#include "saas_guide2.h"
#include "saas_guide_direct.h"
#include <QPainter>
#include "n_func.h"
#include "public_sql.h"
#include <QDir>
#include "w_sys_manage_cloudsync_transfer.h"

int saas_pay_detail::signWidgetLight = 0;

saas_pay_detail::saas_pay_detail(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::saas_pay_detail)
{
    ui->setupUi(this);
    ui->tableView->setModel(saasModel());

    resize(700, 400);

    QTimer::singleShot(0, ui->tableView, SLOT(resizeColumnsToContents()));

    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this,SLOT(took()));
}

saas_pay_detail::~saas_pay_detail()
{
    delete ui;
}

bool saas_pay_detail::isSaaSAA()
{
    return lds::terminalCode.startsWith("AA")
            && "" == saas_type();
}

bool saas_pay_detail::isSaaSAA21()
{
    return lds::terminalCode.startsWith("AA")
            && "21" == saas_type();
}

bool saas_pay_detail::isSaaSFA20()
{
    return lds::terminalCode.startsWith("FA")
            && ("" == saas_type()/*未激活*/ || "20" == saas_type()/*已激活*/);
}

bool saas_pay_detail::isSaaSFA21()
{
    return lds::terminalCode.startsWith("FA")
            && "21" == saas_type();
}

bool saas_pay_detail::isSaaS()
{
    //    return saas_type() != ""
    //            || lds::terminalCode.startsWith("FA");
    return isSaaSAA21()
            || isSaaSFA20()
            || isSaaSFA21();
}

bool saas_pay_detail::btnlanguageswicthCan()
{
    if(isSaaSFA20())//标准版
        return false;
    return true;//专业版或普通版
}

void saas_pay_detail::mobile_connect_max_value_set_zero()//开机强制
{
    if(isSaaSFA20()) {
        lds_query_hddpos  query;
        query.execSelect("select  vch_value from cey_sys_parameter  where vch_parameter = 'mobile_connect_max_value' ");
        if(!query.next()) {
            n_func::f_set_sysparm("mobile_connect_max_value", 0);
        }
        qDebug() << "若是标准版,则强制连接数为0";
    }
}

bool saas_pay_detail::SaaSFuncCheck(QWidget *parent, const QString & code)
{
    int saas_ret = 0;
    QString SaaS_title;
    //普通版
    if(isSaaSAA()) {
        return true;
    }
    //专业版
    if(isSaaSAA21()) {
        return true;
    }
    //标准版
    if(isSaaSFA20()) {
        QAbstractItemModel *model = saasModel();
        saas_guide::getTerminal();

        for(int row = 0; row < model->rowCount(); row ++) {
            if(model->data(model->index(row, 0)).toString() == code) {
                if("0201" == code) {//餐桌数量
                    lds_query_hddpos  query;
                    query.execSelect(QString("SELECT count(0) from cey_bt_table "));
                    query.next();
                    if(query.recordValue(0).toInt() >= 10) {
                        SaaS_title = model->data(model->index(row, 2)).toString()+":"+model->data(model->index(row, 3)).toString();
                        goto saasUpdate;
                    }
                }  else if("0508" == code) {
                    lds_query_hddpos  query;
                    query.execSelect(QString("SELECT count(0) FROM cey_bt_kitchen_plan; "));
                    query.next();
                    if(query.recordValue(0).toInt() > 0) {
                        SaaS_title = model->data(model->index(row, 2)).toString()+":"+model->data(model->index(row, 3)).toString();
                        goto saasUpdate;
                    }
                } else {//其他
                    SaaS_title = model->data(model->index(row, 2)).toString()+":"+model->data(model->index(row, 3)).toString();
                    goto saasUpdate;
                }
            }
            //
        }//for
    }
    //saas专业版
    if(isSaaSFA21()) {
        return true;
    }

    return true;
saasUpdate:
    while(1 == (saas_ret = lds_messagebox::information(parent
                                                       ,  SaaS_title
                                                       ,  QObject::tr("终端号") + ":" +lds::terminalCode + "\n" + QObject::tr("您目前使用的是标准版本，无法使用更高级的功能，85%的用户已经升级了专业版；享受更强大的专业版软件，支持多语言系统自如切换，支持区域厨打及手机、平板点菜、结算、支付，各类促销导流功能及其它更激动人心的功能，请抓紧升级专业版。")
                                                       ,  QObject::tr("升级"), QObject::tr("了解专业版"), QObject::tr("取消")))) {
        //saas_guide_direct
        saas_guide2 d(new saas_guide_direct(3, 0), parent);
        d.only_show_exit();
        transparentCenterDialog(&d, false).exec();
    }
    if(0 == saas_ret) {
        //saas_guide_pay
        saas_guide2 d(new saas_guide_pay_update, parent);
        if(QDialog::Rejected == transparentCenterDialog(&d, false).exec()) {
            return false;
        }
        ///从云端读取终端权限、确认升级成功
        httpDataAutoDel httpd = saas_guide::getTerminal();
        if(200 == httpd.httpStatus
                && "21" == saas_type())
        {
            lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return true;
        }
        lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    }

    return false;
}

QString saas_pay_detail::terminalTypeGet()
{
    if(lds::terminalCode.startsWith("FA")) {
        return "FA";//saas专业版
    } else {
        return "AA";//专业版
    }
    return "";
}

QString saas_pay_detail::terminalSytemGet()
{
    //00餐饮linux   01餐饮PC   02餐饮android
#ifdef Q_OS_UNIX
    return "00";
#endif

#ifdef Q_OS_WIN
    return "01";
#endif

#ifdef Q_OS_ANDROID
    return "02";
#endif

    return "";
}

const QMap<QString, QString> &saas_pay_detail::codeSaaSMapGet()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map.insert("20", QObject::tr("标准版"));
        map.insert("21", QObject::tr("专业版"));
    }
    return map;
}

QString saas_pay_detail::saasDesc()
{
    if(isSaaSAA()) return QObject::tr("普通版");
    if(isSaaSAA21()) return QObject::tr("经典") + QObject::tr("专业版");
    if(isSaaSFA20()) return "SaaS"+QObject::tr("标准版");
    if(isSaaSFA21()) return "SaaS"+QObject::tr("专业版");

    return "";
}

bool saas_pay_detail::saaslimitcheck(QWidget *parent)
{
    //判断有效期
    httpDataAutoDel httpd = saas_guide::getTerminal(1000);

    if(200 == httpd.httpStatus)
    {
        QDateTime curDate = QDateTime::currentDateTime();
        if(saas_pay_detail::saas_dueDate().isValid() == false) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,
                                    QObject::tr("终端号") + ":" + lds::terminalCode + "\n" +
                                    QObject::tr("已连接云端")+"\n"+
                                    QObject::tr("截止日期格式错误") + "\n" +
                                    QObject::tr("请登录云端核实"));
            return false;
        }
        int remainDay = curDate.daysTo(saas_pay_detail::saas_dueDate()) ;
        if(remainDay >= 0 && remainDay < 30) {
            if(0 == lds_messagebox::question(parent, MESSAGE_TITLE_VOID,
                                             QObject::tr("终端号") + ":" + lds::terminalCode + "\n" +
                                             QObject::tr("已连接云端")+"\n"+
                                             QObject::tr("使用时间不足30天")+"\n"+
                                             QObject::tr("当前日期") + ":" + curDate.toString("yyyy-MM-dd hh:mm:ss")+"\n"+
                                             QObject::tr("截止日期") + ":" + saas_pay_detail::saas_dueDate_str(),
                                             QObject::tr("立刻购买"), QObject::tr("继续使用"))) {
                saas_guide2 d(new saas_guide_pay, 0);
                if(QDialog::Rejected == transparentCenterDialog(&d, false).exec()) {
                    return false;
                }
            }
        }
        if(remainDay < 0) {
            if(0 == lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,
                                            QObject::tr("终端号") + ":" + lds::terminalCode + "\n" +
                                            QObject::tr("已连接云端")+"\n"+
                                            QObject::tr("使用时间不足")+"\n"+
                                            QObject::tr("当前日期") + ":" + curDate.toString("yyyy-MM-dd hh:mm:ss")+"\n"+
                                            QObject::tr("截止日期") + ":" + saas_pay_detail::saas_dueDate_str(),
                                            QObject::tr("立刻购买"), QObject::tr("退出软件"))) {
                saas_guide2 d(new saas_guide_pay, 0);
                if(QDialog::Rejected == transparentCenterDialog(&d, false).exec()) {
                    return false;
                }
            } else {
                return false;
            }
        }
    } else if(550 == httpd.httpStatus) {
        if("-103" == httpd.code) {//终端已过有效期
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,
                                    QObject::tr("终端号") + ":" + lds::terminalCode + "\n" +
                                    httpd.message);
            saas_guide2 d(new saas_guide_pay, 0);
            if(QDialog::Rejected == transparentCenterDialog(&d, false).exec()) {
                return false;
            }
        }
        if("-105" == httpd.code) {//终端已挂起
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,
                                    QObject::tr("终端号") + ":" + lds::terminalCode + "\n" +
                                    httpd.message + "\n" +
                                    QObject::tr("请登录云端核实"));
            return false;
        }
    } else {//离线
        QDateTime curDate = QDateTime::currentDateTime();
        if(saas_pay_detail::saas_dueDate().isNull()) {
            lds_messagebox::information(parent, MESSAGE_TITLE_VOID,
                                        QObject::tr("终端号") + ":" + lds::terminalCode + "\n" +
                                        QObject::tr("未连接云端")
                                        );
            return false;
        }
        int remainDay = curDate.daysTo(saas_pay_detail::saas_dueDate()) ;
        if(remainDay >= 0 && remainDay < 30) {
            lds_messagebox::information(parent, MESSAGE_TITLE_VOID,
                                        QObject::tr("终端号") + ":" + lds::terminalCode + "\n" +
                                        QObject::tr("未连接云端")+"\n"+
                                        QObject::tr("使用时间不足30天")+"\n"+
                                        QObject::tr("当前日期") + ":" + curDate.toString("yyyy-MM-dd hh:mm:ss")+"\n"+
                                        QObject::tr("截止日期") + ":" + saas_pay_detail::saas_dueDate_str() +"\n"+
                                        QObject::tr("请及时续期"));
        }
        if(remainDay < 0) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,
                                    QObject::tr("终端号") + ":" + lds::terminalCode + "\n" +
                                    QObject::tr("未连接云端")+"\n"+
                                    QObject::tr("使用时间不足")+"\n"+
                                    QObject::tr("当前日期") + ":" + curDate.toString("yyyy-MM-dd hh:mm:ss")+"\n"+
                                    QObject::tr("截止日期") + ":" + saas_pay_detail::saas_dueDate_str() +"\n"+
                                    QObject::tr("请及时续期"));
            return false;
        }
    }
    return true;
}

QStandardItemModel *saas_pay_detail::saasModel()
{
    static QStandardItemModel m;
    if(m.rowCount() == 0) {
        QStandardItemModel *model = &m;
        model->setHorizontalHeaderLabels(QStringList() <<  QObject::tr("编码") << QObject::tr("分类") << QObject::tr("项目") << QObject::tr("标准版") << QObject::tr("专业版"));

        QList<QStandardItem*> items;
        /*OK*/  items.clear(); items<< new QStandardItem("0101") << new QStandardItem("前台点单")<< new QStandardItem(QObject::tr("手牌号"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0102") << new QStandardItem("前台点单")<< new QStandardItem(QObject::tr("点单叫号"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0201") << new QStandardItem("中餐模式")<< new QStandardItem(QObject::tr("餐桌数量"))<< new QStandardItem(QObject::tr("10台"))<< new QStandardItem(QObject::tr("不限制"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0202") << new QStandardItem("中餐模式")<< new QStandardItem(QObject::tr("餐桌地图模式"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0203") << new QStandardItem("中餐模式")<< new QStandardItem(QObject::tr("本地餐桌预订"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0301") << new QStandardItem("会员设置")<< new QStandardItem(QObject::tr("云会员、本地会员积分"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0302") << new QStandardItem("会员设置")<< new QStandardItem(QObject::tr("云会员、本地会员积分兑换"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0303") << new QStandardItem("会员设置")<< new QStandardItem(QObject::tr("云会员、本地会员充值"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0304") << new QStandardItem("会员设置")<< new QStandardItem(QObject::tr("微会员")) << new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0401") << new QStandardItem("基础资料")<< new QStandardItem(QObject::tr("组合商品"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0402") << new QStandardItem("基础资料")<< new QStandardItem(QObject::tr("库存管理"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0403") << new QStandardItem("基础资料")<< new QStandardItem(QObject::tr("商品原料"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0404") << new QStandardItem("基础资料")<< new QStandardItem(QObject::tr("时价商品"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0405") << new QStandardItem("基础资料")<< new QStandardItem(QObject::tr("临时商品"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0406") << new QStandardItem("基础资料")<< new QStandardItem(QObject::tr("商品沽清"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0407") << new QStandardItem("基础资料")<< new QStandardItem(QObject::tr("商品服务费"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0408") << new QStandardItem("基础资料")<< new QStandardItem(QObject::tr("商品营业员提成"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0501") << new QStandardItem("系统设置")<< new QStandardItem(QObject::tr("优惠券"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0502") << new QStandardItem("系统设置")<< new QStandardItem(QObject::tr("点菜宝"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0503") << new QStandardItem("系统设置")<< new QStandardItem(QObject::tr("手机最大连接数"))<< new QStandardItem(QObject::tr("固定0"))<< new QStandardItem(QObject::tr("可以修改"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0504") << new QStandardItem("系统设置")<< new QStandardItem(QObject::tr("双屏"))<< new QStandardItem(QObject::tr("广告只能读取云端"))<< new QStandardItem(QObject::tr("广告可以自定义"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0505") << new QStandardItem("系统设置")<< new QStandardItem(QObject::tr("促销"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0506") << new QStandardItem("系统设置")<< new QStandardItem(QObject::tr("结账打印格式"))<< new QStandardItem(QObject::tr("没法修改"))<< new QStandardItem(QObject::tr("可以修改"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0507") << new QStandardItem("系统设置")<< new QStandardItem(QObject::tr("标签格式"))<< new QStandardItem(QObject::tr("不能修改"))<< new QStandardItem(QObject::tr("可以修改"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0508") << new QStandardItem("系统设置")<< new QStandardItem(QObject::tr("厨房打印机数量"))<< new QStandardItem(QObject::tr("只能1台厨打"))<< new QStandardItem(QObject::tr("不限制"));
        model->appendRow(items);
        /*OK*/  items.clear(); items<< new QStandardItem("0509") << new QStandardItem("系统设置")<< new QStandardItem(QObject::tr("区域厨打"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0510") << new QStandardItem("系统设置")<< new QStandardItem(QObject::tr("厨显"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0511") << new QStandardItem("系统设置")<< new QStandardItem(QObject::tr("来电弹屏"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);
        /**/  items.clear(); items<< new QStandardItem("0512") << new QStandardItem("系统设置")<< new QStandardItem(QObject::tr("多语言"))<< new QStandardItem(QObject::tr("不支持"))<< new QStandardItem(QObject::tr("支持"));
        model->appendRow(items);


        //        标准版：移动最大连接数为0，
        //        多语言： 厨打语言、结算单语言、点单语言
        //        沽清：商品修改，和沽清报表的批量修改

        //避免重复
#ifdef QT_DEBUG
        QSet<QString> code_set;
        for(int row = 0; row < model->rowCount(); row ++) {
            code_set.insert(model->item(row, 0)->text());
        }
        if(code_set.size() != model->rowCount()) {
            qDebug("model rowcount must equal code_set count");
            QObject *o=0;o->children();
        }
#endif
    }

    //    //debug
    //    QString line;
    //    for(int c = 0; c < m.columnCount(); c++) {
    //        line +=  m.horizontalHeaderItem(c)->text() + "\t";
    //    }
    //    qDebug() << line;
    //    for(int r = 0; r < m.rowCount(); r++) {
    //        QString line;
    //        for(int c = 0; c < m.columnCount(); c++) {
    //            if(m.item(r,c ))
    //                line +=  m.item(r, c)->text()+"\t";
    //        }
    //        qDebug() << line;
    //    }
    //    //~debug
    return &m;
}

QVariant saas_pay_detail::saasValue(const QString &key, const QVariant &defaultValue)
{
#if defined(QT_DEBUG)
    QSettings settings("saas_type",QSettings::IniFormat);
    return settings.value(key, defaultValue);
#elif defined(Q_OS_UNIX)
    QSettings settings("/usr/desk/pos/saas_type", QSettings::IniFormat);
    return settings.value(key, defaultValue);
#elif defined(Q_OS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\fastfd", QSettings::NativeFormat);
    return settings.value(key, defaultValue);
#endif
    return QVariant();
}

void saas_pay_detail::saasSetValue(const QString &key, const QVariant &value)
{
#if defined(QT_DEBUG)
    QSettings settings("saas_type",QSettings::IniFormat);
    settings.setValue(key, value);
#elif defined(Q_OS_UNIX)
    QSettings settings("/usr/desk/pos/saas_type", QSettings::IniFormat);
    settings.setValue(key, value);
#elif defined(Q_OS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\fastfd", QSettings::NativeFormat);
    settings.setValue(key, value);
#endif
}
//AA78937493                       FA12321423
//"", "AA"                               "20", "21",
//"", "20", "21", "AA"
int saas_pay_detail::saas_lan_num()
{
    return saas_pay_detail::saasValue("saas/saas_lan_num").toInt();
}

QString saas_pay_detail::saas_type()
{
    return saas_pay_detail::saasValue("saas/saas_type").toString();
}

QDateTime saas_pay_detail::saas_dueDate()
{
    return saas_pay_detail::saasValue("saas/saas_dueDate").toDateTime();
}

QString saas_pay_detail::saas_dueDate_str()
{
    return saas_dueDate().toString("yyyy-MM-dd hh:mm:ss");
}

QDateTime saas_pay_detail::saas_online_last_run()
{
    return saas_pay_detail::saasValue("saas/saas_online_last_run").toDateTime();
}

QString saas_pay_detail::appName()
{
    return lds::appName();
}

bool saas_pay_detail::saas_update_adv(const QString &dir, lds_messagebox_loading_show *loading)
{
    if(false == QDir().exists(dir)) {
        QDir().mkpath(dir);
    }
    QString errstring;
    QSettings conf(dir+"/conf.ini", QSettings::IniFormat);
    QString version = QString::number(conf.value("advertiisement/version", "0").toInt() + 1);//最大值,版本号为1

    if(QDir(dir).entryList().count() <= 3/*.\..*/) {//若是空的文件夹,则版本号重置
        version = "1";
    }
    QString url = http_www_yqspos_com +"/arm/store/advertiisement-rest";
    QMap<QString, QString> map;
    map.insert("version", version);
    w_sys_manage_cloudsync_transfer cloud_transfer;
    int mesc = 30*1000;
    QString retparam;
    httpDataAutoDel httpd = cloud_transfer.sendGet(url, map, retparam, mesc, &errstring);
    if(200 == httpd.httpStatus) {
        cJSON *array = cJSON_help::getcJSON(httpd.json, "advertiisement");
        version = cJSON_help::getcJSONvalue(httpd.json, "version").toString();
        for(int k = 0, count = cJSON_GetArraySize(array); k < count; k++) {
            cJSON *item = cJSON_GetArrayItem(array, k);
            QString imag_url = cJSON_help::getcJSONvalue(item, "img").toString();
            QByteArray strResponse;
            QList<networkHttpRequest::httpHeader> httpd_list;
            if(200 == networkHttpRequest::get(imag_url, httpd_list, strResponse, mesc, &errstring) ) {
                QFile file(dir + "/" + QFileInfo(imag_url).fileName());
                if(errorStringExec(file, open(QFile::WriteOnly), errstring)) {
                    file.write(strResponse);
                    file.close();
                    continue;
                }
            }
            loading->setText(errstring, 3000);
        }
        conf.setValue("advertiisement/version", version);
        return true;
    }
    return false;
}

//saas 专业版和标准版的区分
void saas_pay_detail::paintLeftTop(const QRect &rect0, QPainter *painter)
{
    QPolygon poly;
    int off =  15;
    int gap = 5;
    poly.append(QPoint(rect0.x() + off + gap, rect0.y()));//C
    poly.append(QPoint(rect0.x(),  rect0.y() + off + gap));//F
    poly.append(QPoint(rect0.x(),  rect0.y() + off + gap + off));//E
    poly.append(QPoint(rect0.x() + off + gap + off, rect0.y()));//D
    QPainterPath path;
    path.addPolygon(poly);
    if(2 == lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {//酒吧模式

        QPolygon py;
        py.append(QPoint(rect0.x() + off, rect0.y()));//B
        py.append(QPoint(rect0.x(),  rect0.y() + off));
        py.append(QPoint(rect0.x(),  rect0.y() ));
        QPainterPath ph;
        ph.addPolygon(py);

        painter->fillPath(ph, QColor("#ffff00"));
    }
    if(saas_pay_detail::isSaaSFA20()) {
        painter->fillPath(path, QColor("#ff546b"));
        return;
    }
    if(saas_pay_detail::isSaaSFA21()) {
        painter->fillPath(path, QColor("gold"));
        return;
    }
    if(saas_pay_detail::isSaaSAA21()) {
        painter->fillPath(path, QColor("cyan"));
        return;
    }
    if(saas_pay_detail::isSaaSAA()) {
        return;
    }
    painter->fillPath(path, QColor("red"));
    painter->setPen(QColor("red"));
    painter->drawText(rect0, (Qt::AlignTop | Qt::AlignRight), "ERROR:""MACHINE-ID:" + lds::terminalCode + "  SYS-TYPE:" + saas_type() + "  ");
}

void saas_pay_detail::paintLeftTopAA(const QRect &rect0, QPainter *painter)
{
    QPolygon poly;
    int rect0h_2 =  qMin(30, rect0.height()/2);
    poly.append(QPoint(rect0.x() + rect0h_2  - 5, rect0.y()));
    poly.append(QPoint(rect0.x() + rect0h_2 + 5, rect0.y()));
    poly.append(QPoint(rect0.x(),  rect0.y() + rect0h_2 + 5));
    poly.append(QPoint(rect0.x(),  rect0.y() + rect0h_2 - 5));
    QPainterPath path;
    path.addPolygon(poly);

    painter->fillPath(path, QColor("blue"));
}

void saas_pay_detail::took()
{
    this->accept();
}

void saas_pay_detail::toexit()
{
    this->reject();
}

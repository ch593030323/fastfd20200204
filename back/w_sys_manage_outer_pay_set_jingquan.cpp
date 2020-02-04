#include "w_sys_manage_outer_pay_set_jingquan.h"
#include "ui_w_sys_manage_outer_pay_set_jingquan.h"
#include "lds_query_hddpos.h"
#include "cjson_help.h"
#include "cJSON.h"
#include "lds.h"
#include <QNetworkRequest>


bool w_sys_manage_outer_pay_set_jingquan::postSale(lds_messagebox_loading_show *loading, const QString &ch_payno, w_scr_dish_pay_widget::operatModes flag, QString &errstring)
{
    QString baseUrl = n_func::f_get_sysparm("jingquan_baseURL");
    QString companyCode = n_func::f_get_sysparm("jingquan_companyCode");
    QString trade_code = n_func::f_get_sysparm("jingquan_trade_code");

#ifdef QT_DEBUG
    baseUrl = "http://jscms.letskd.com:40350/test";
    companyCode = "jycy";
    trade_code = "310001";
#endif

    QString ch_billno;
    lds_query_hddpos  query;
    query.execSelect(QString(" select * from cey_u_checkout_master where ch_payno = '%1' ").arg(ch_payno));
    query.next();
    ch_billno = query.recordValue("ch_billno").toString();

    ///销售头表参数
    loading->setText(QObject::tr("销售头表参数"));
    cJSON_help  obj(cJSON_Object);
    cJSON_help::_cJSON_AddValueToObject(obj, "companys", companyCode);
    //    cJSON_help::_cJSON_AddValueToObject(obj, "Shiftid", "");//      是 string 24 班次号
    cJSON_help::_cJSON_AddValueToObject(obj, "pid", ch_payno);//      是 string 50 销售 ID 主键，不可重复
    cJSON_help::_cJSON_AddValueToObject(obj, "trans_no", ch_payno);//    是 string 50 POS 交易号
    //    cJSON_help::_cJSON_AddValueToObject(obj, "ctr_code", "");//    否 Int 系统序号 默认 1
    //        cJSON_help::_cJSON_AddValueToObject(obj, "till_code", till_code);//   是 string 24 收银机代码 外键关联 pos
    //    cJSON_help::_cJSON_AddValueToObject(obj, "sale_id", "");//      是 Int 交易序号
    cJSON_help::_cJSON_AddValueToObject(obj, "trans_type", "H");//  是 string 1 交易类别 H:交易 A：预收
    //    cJSON_help::_cJSON_AddValueToObject(obj, "trans_reason", "");//    否 string 256 交易理由
    cJSON_help::_cJSON_AddValueToObject(obj, "sale_type", (flag & w_scr_dish_pay_widget::OPERAT_QUITBILL) ? "RETURN" : "SALE");//  是 string 10 销售类型 SALE：销售 RETURN：退货
    cJSON_help::_cJSON_AddValueToObject(obj, "trade_code", trade_code);// 是 string 24 门店代码 外键关联门店
    cJSON_help::_cJSON_AddValueToObject(obj, "oper_code", query.recordValue("vch_operid"));//  是 string 24 操作员代码 外键关联收银员
    //    cJSON_help::_cJSON_AddValueToObject(obj, "member_code", "");// 否 string 20 会员编号
    cJSON_help::_cJSON_AddValueToObject(obj, "sale_dates", query.recordValue("dt_operdate").toDateTime().date());//  是 date 销售日期
    cJSON_help::_cJSON_AddValueToObject(obj, "sale_hour", query.recordValue("dt_operdate").toDateTime().time().hour());//   是 int 销售小时
    cJSON_help::_cJSON_AddValueToObject(obj, "sale_minute", query.recordValue("dt_operdate").toDateTime().time().minute());// 是 int 销售分钟
    cJSON_help::_cJSON_AddValueToObject(obj, "gross_amt",  query.recordValue("num_cost"));//   是 decimal (12, 4) 销售毛额
    cJSON_help::_cJSON_AddValueToObject(obj, "net_amt", query.recordValue("num_cash"));//    是 decimal (12, 4) 销售净额
    cJSON_help::_cJSON_AddValueToObject(obj, "discount_amt", query.recordValue("num_cost").toDouble() - query.recordValue("num_cash").toDouble());//   是 decimal (12, 4) 折扣金额
    cJSON_help::_cJSON_AddValueToObject(obj, "cur_qty", lds_query_hddpos::selectValue(QString(" select sum(num_num-num_back) from cey_u_orderdish where ch_payno = '%1' ").arg(ch_payno)));//    是 decimal (10, 4) 销售数量
    cJSON_help::_cJSON_AddValueToObject(obj, "ctm_count", lds_query_hddpos::selectValue(QString(" select int_person from cey_u_table where ch_payno = '%1' ").arg(ch_payno)));//  是 Int 来客数
    cJSON_help::_cJSON_AddValueToObject(obj, "table_no", lds_query_hddpos::selectValue(QString(" select ch_tableno from cey_u_table where ch_payno = '%1' ").arg(ch_payno)));//   否 string 20 桌号
    //    cJSON_help::_cJSON_AddValueToObject(obj, "order_code", "");//  否 string 50 订单编号
//    cJSON_help::_cJSON_AddValueToObject(obj, "trade_date", query.recordValue("dt_operdate"));//  是 datetime 交易日期
    //    cJSON_help::_cJSON_AddValueToObject(obj, "field1", "");//  否 varchar 64 交易模式 堂食：TS\美团外卖:MWM\饿了么外面：EWM\百度外面：BWM
    //    cJSON_help::_cJSON_AddValueToObject(obj, "Field2", "");//  否 varchar 256 交易描述 用于记录免单的情
    cJSON_help::_cJSON_AddValueToObject(obj, "invoice_amt", 0);// 否 decimal (12,4) 开票金额 默认 0
    cJSON_help::_cJSON_AddValueToObject(obj, "discount_code", "");//  否 varchar 64 折扣编号 没有则为空
    cJSON_help::_cJSON_AddValueToObject(obj, "oth_dis_amt", 0);// 是 decimal (12,4) 第三方折扣金额 默认 0

    ///销售明细参数
    loading->setText(QObject::tr("销售明细参数"));
    cJSON *saleDetail = cJSON_CreateArray();
    cJSON_help::_cJSON_AddItemToObject(obj, "saleDetail", saleDetail);
    fexpandmain_model_sqltablemodel_data tablemodel;
    tablemodel.setTable("cey_u_orderdish");
    tablemodel.select(ch_billno);
    for(int row = 0; row < tablemodel.rowCount(); row ++) {
        cJSON *saleDetail_d = cJSON_CreateObject();
        cJSON_AddItemToArray(saleDetail, saleDetail_d);

        fexpandmain_keepData kdata = tablemodel.getKeepdata(row);

        cJSON_help::_cJSON_AddValueToObject(saleDetail_d, "pid"      ,  tablemodel.model_data(row, "int_flowid"));//    是 string 50 销售明细 ID 主键，不可重复
        cJSON_help::_cJSON_AddValueToObject(saleDetail_d, "header_id"      ,  ch_payno);//    是 string 50 销售 ID 外键关联销售表
        //    cJSON_help::_cJSON_AddValueToObject(saleDetail_d, "ctr_code"      ,  "");//   否 int 64 系统序号 默认 1
        cJSON_help::_cJSON_AddValueToObject(saleDetail_d, "posplu_no"      ,  tablemodel.model_data(row, "ch_dishno"));//    是 string 20 pos 商品编号
        cJSON_help::_cJSON_AddValueToObject(saleDetail_d, "plu_no"      ,  tablemodel.model_data(row, "ch_dishno"));//   是 string 20 商品编号 外键关联商品表
        cJSON_help::_cJSON_AddValueToObject(saleDetail_d, "plu_price"      ,  tablemodel.model_data(row, "num_price"));//是 decimal (12, 4) 商品价格
        cJSON_help::_cJSON_AddValueToObject(saleDetail_d, "gross_amt"      ,  kdata.consum);//是 decimal (12, 4) 销售毛额
        cJSON_help::_cJSON_AddValueToObject(saleDetail_d, "net_amt"      ,  kdata.dish_total());//  是 decimal (12, 4) 销售净额
        cJSON_help::_cJSON_AddValueToObject(saleDetail_d, "discount_amt"      ,  kdata.discount + kdata.present);// 是 decimal (12, 4) 折扣金额
        cJSON_help::_cJSON_AddValueToObject(saleDetail_d, "cur_qty"      ,  kdata.dish_num());//    是 decimal (10, 4) 销售数量
        //        cJSON_help::_cJSON_AddValueToObject(saleDetail_d, "gift_num"      ,  "");//   否 string 20 礼券编号 没有为空
    }

    ///支付明细参数
    loading->setText(QObject::tr("支付明细参数"));
    cJSON *payDetail = cJSON_CreateArray();
    cJSON_help::_cJSON_AddItemToObject(obj, "payDetail", payDetail);
    query.execSelect(QString(" select * from cey_u_checkout_detail where ch_payno = '%1' ").arg(ch_payno));
    while(query.next()) {
        cJSON *payDetail_d = cJSON_CreateObject();
        cJSON_AddItemToArray(payDetail, payDetail_d);

        cJSON_help::_cJSON_AddValueToObject(payDetail_d, "pid", query.recordValue("int_flowid"));//      是 string 50 支付 ID 主键，不可重复
        cJSON_help::_cJSON_AddValueToObject(payDetail_d, "header_id", ch_payno);//  是 string 50 销售 ID 外键关联销售表
        //        cJSON_help::_cJSON_AddValueToObject(payDetail_d, "ctr_code", "");//   是 int 系统序号 默认 1
        cJSON_help::_cJSON_AddValueToObject(payDetail_d, "tender_type", "H");//    是 char 1 支付类型 H:交易\A：预收
        cJSON_help::_cJSON_AddValueToObject(payDetail_d, "tender_code", paymodenoFilter(query.recordValue("ch_paymodeno").toString()));//    是 string 10 支付代码 外键关联支付方式
        //        cJSON_help::_cJSON_AddValueToObject(payDetail_d, "ptender_code", "");//   否 string 10 父支付代码 默认tender_code
        cJSON_help::_cJSON_AddValueToObject(payDetail_d, "tender_amt", query.recordValue("num_realamount"));// 是 decimal (12, 4) 应收金额
        cJSON_help::_cJSON_AddValueToObject(payDetail_d, "tender_ramt", query.recordValue("num_realamount"));//    是 decimal (12, 4) 实收金额
        cJSON_help::_cJSON_AddValueToObject(payDetail_d, "oth_dis_amt", 0);//    是 decimal (12, 4) 第三方折扣金额
    }


    QByteArray readall;
    QNetworkAccessManager m;
    QEventLoop loop;
    QNetworkRequest request;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QByteArray param;
    int httpStatus;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"textml;charset=UTF-8;charset=UTF-8");

    //销售单据
    request.setUrl(QUrl(baseUrl + "/jinterface/isalebill"));
    param = cJSON_help::getcJSONprint(obj);
    QNetworkReply *reply = m.post(request, param);
    loop.exec();
    httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    readall = reply->readAll();


    if(200 == httpStatus) {
        if("0" == cJSON_help::getcJSONvalue(readall, "code").toString()) {
            return true;
        }
    }
    errstring = loading->text() + "\n" + cJSON_help::getcJSONvalue(readall, "msg").toString();
    return false;
}

bool w_sys_manage_outer_pay_set_jingquan::postIGood(lds_messagebox_loading_show *loading, QString &errstring)
{
    QString baseUrl = n_func::f_get_sysparm("jingquan_baseURL");
    QString companyCode = n_func::f_get_sysparm("jingquan_companyCode");
    QString trade_code = n_func::f_get_sysparm("jingquan_trade_code");

#ifdef QT_DEBUG
    baseUrl = "http://jscms.letskd.com:40350/test";
    companyCode = "jycy";
    trade_code = "3100011";
#endif

    int progressCount = 6;
    int progressValue = 0;
    QNetworkAccessManager m;
    QEventLoop loop;
    QNetworkRequest request;
    QObject::connect(&m, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    lds_query_hddpos  query;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"textml;charset=UTF-8;charset=UTF-8");

    ///
    loading->setText(QObject::tr("商品大类") +  QString("(%1/%2)").arg(++progressValue).arg(progressCount));
    request.setUrl(QUrl(baseUrl + "/jinterface/igroup"));
    query.execSelect(" SELECT * FROM cey_bt_dish_type;  ");
    while(query.next()) {
        if(httpPost(loading->text(),
                    ldsVariantMap()
                    .append("companyCode", companyCode)
                    .append("groupcode", query.recordValue("ch_dish_typeno"))
                    .append("groupname", query.recordValue("vch_dish_typename"))
                   , 
                    m,
                    request,
                    loop,
                    errstring)) {
            continue;
        }
        return false;
    }

    ///
    loading->setText(QObject::tr("商品中类") + QString("(%1/%2)").arg(++progressValue).arg(progressCount));
    request.setUrl(QUrl(baseUrl + "/jinterface/iclass"));
    query.execSelect(" SELECT * FROM cey_bt_dish_subtype  ");
    while(query.next()) {
        if(httpPost(loading->text(),
                    ldsVariantMap()
                    .append("companyCode", companyCode)
                    .append("classcode", query.recordValue("ch_sub_typeno"))
                    .append("classname", query.recordValue("vch_sub_typename"))
                    .append("groupcode", query.recordValue("ch_dish_typeno"))
                   , 
                    m,
                    request,
                    loop,
                    errstring)) {
            continue;
        }
        return false;
    }

    ///
    loading->setText(QObject::tr("计量单位") + QString("(%1/%2)").arg(++progressValue).arg(progressCount));
    request.setUrl(QUrl(baseUrl + "/jinterface/iuom"));
    query.execSelect(" SELECT * FROM cey_bt_unit;  ");
    while(query.next()) {
        if(httpPost(loading->text(),
                    ldsVariantMap()
                    .append("companyCode", companyCode)
                    .append("ucode", query.recordValue("ch_unitno"))
                    .append("uname", query.recordValue("vch_unitname"))
                   , 
                    m,
                    request,
                    loop,
                    errstring)) {
            continue;
        }
        return false;
    }

    ///
    loading->setText(QObject::tr("商品基础信息") + QString("(%1/%2)").arg(++progressValue).arg(progressCount));
    request.setUrl(QUrl(baseUrl + "/jinterface/igoods"));
    query.execSelect(" SELECT * FROM cey_bt_dish;  ");
    while(query.next()) {
        if(httpPost(loading->text(),
                    ldsVariantMap()
                    .append("companyCode", companyCode)
                    .append("code", query.recordValue("ch_dishno"))
                    .append("name", query.recordValue("vch_dishname"))
                    //                    .append("spec", "")//规格
                    .append("uomcode", query.recordValue("ch_unitno"))//计量单位编号
                    .append("groupcode", query.recordValue("ch_dish_typeno"))//大类编号

                    .append("classcode", query.recordValue("ch_sub_typeno"))//中类编号
                    //                    .append("sccode", "")//子类编号
                   , 
                    m,
                    request,
                    loop,
                    errstring)) {
            continue;
        }
        return false;
    }

    ///
    loading->setText(QObject::tr("支付方式") + QString("(%1/%2)").arg(++progressValue).arg(progressCount));
    request.setUrl(QUrl(baseUrl + "/jinterface/ipaywan"));
    query.execSelect(" SELECT *  FROM cey_bt_paymode;  ");
    while(query.next()) {
        if(httpPost(loading->text(),
                    ldsVariantMap()
                    .append("companyCode", companyCode)
                    .append("code", paymodenoFilter(query.recordValue("ch_paymodeno").toString()))
                    .append("name", query.recordValue("vch_paymodename"))
                   , 
                    m,
                    request,
                    loop,
                    errstring)) {
            continue;
        }
        return false;
    }
    ///
    loading->setText(QObject::tr("收银员") + QString("(%1/%2)").arg(++progressValue).arg(progressCount));
    request.setUrl(QUrl(baseUrl + "/jinterface/icashier"));
    query.execSelect(" select '0000' as vch_operID, 'system' as vch_operator union SELECT vch_operID, vch_operator FROM cey_sys_operator ");
    while(query.next()) {
        if(httpPost(loading->text(),
                    ldsVariantMap()
                    .append("companyCode", companyCode)
                    .append("code", query.recordValue("vch_operID"))
                    .append("name", query.recordValue("vch_operator"))
                   , 
                    m,
                    request,
                    loop,
                    errstring)) {
            continue;
        }
        return false;
    }
    return true;
}

QString w_sys_manage_outer_pay_set_jingquan::paymodenoFilter(const QString &ch_paymodeno)
{
    if(ch_paymodeno == "00") {
        return "1";
    }
    return ch_paymodeno;
}

w_sys_manage_outer_pay_set_jingquan::w_sys_manage_outer_pay_set_jingquan( QWidget *parent) :
    lds_model_mapwidget_vertical(parent),
    ui(new Ui::w_sys_manage_outer_pay_set_jingquan)
{
    ui->setupUi(this);
    scan_all_init_value("cey_sys_parameter", "vch_parameter", "vch_value","jingquan_");

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_upload, SIGNAL(clicked()), this, SLOT(todataupload()));
}

w_sys_manage_outer_pay_set_jingquan::~w_sys_manage_outer_pay_set_jingquan()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_jingquan::took()
{
    this->commitData();
    this->accept();
}

void w_sys_manage_outer_pay_set_jingquan::tocancel()
{
    this->reject();
}
void w_sys_manage_outer_pay_set_jingquan::todataupload()
{
    //!等待
    lds_messagebox_loading_show loading(this, "", "WAITING...");
    loading.show();
    //!~等待
    QString errstring;
    if(false ==w_sys_manage_outer_pay_set_jingquan::postIGood(&loading, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
}

bool w_sys_manage_outer_pay_set_jingquan::mapHasEmptyValue(const QString &title, const ldsVariantMap &map, QString &errstring)
{
    QMapIterator<QString, QVariant> mapitr(map.d);
    while(mapitr.hasNext()) {
        mapitr.next();

        if(mapitr.value().toString().isEmpty()) {
            errstring = title +  "\n"
                    + mapitr.key() + " is empty" + "\n"
                    + cJSON_help::mapTocJSONByteArray(map);
            return false;
        }
    }
    return true;
}

bool w_sys_manage_outer_pay_set_jingquan::httpPost(const QString &title, const ldsVariantMap &map, QNetworkAccessManager &m, QNetworkRequest &request, QEventLoop &loop, QString &errstring)
{
    if(!mapHasEmptyValue(title, map, errstring)) {
        return false;
    }

    QByteArray readall;
    QByteArray param = cJSON_help::mapTocJSONByteArray(map);
    QNetworkReply *reply = m.post(request, param);
    loop.exec();
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    readall = reply->readAll();

    if(200 == httpStatus) {
        if("0" == cJSON_help::getcJSONvalue(readall, "code").toString()) {
            return true;
        }
    }

    errstring = title + "\n" + cJSON_help::getcJSONvalue(readall, "msg").toString();
    return false;
}

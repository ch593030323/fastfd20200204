#include "w_scr_dish.h"
#include "n_func.h"
#include "backheader.h"
#include "printer_0.h"
#include "qtsoap.h"
#include <QSslConfiguration>
#include <QSqlError>
#include "frontviewdialog_virtual.h"
#include "take_cashier.h"
#include "lds_messagebox.h"
#include "lds.h"
#include "public_logindialog.h"
#include "public_sql.h"
#include "w_sys_manage_outer_pay_set.h"
#include "cJSON.h"
#include "cjson_help.h"
#include "w_sys_manage_cloudsync_transfer.h"

w_scr_dish::w_scr_dish(QObject *parent) :
    QObject(parent)
{
}

QString w_scr_dish::TECH_TRANS_NJGJ_sql(const QString &txdate_yyyymmdd, const QString &txtime_hhmmss, const QString &tillid, const QString &txdocno, const QString &cashier, const QString &salesman, const QString &qty, double netamount, const QList<w_scr_dish::TECH_TRANS_NJGJ_tendercode> &tendercodes)
{
    lds_query_hddpos  query;
    QString sqlerr;
    qlonglong id;


    bool sqlexec = false;
    query.execSelect("select max(cast(id as decimal)) from cey_web_data_collection");//数据是varchar的，max会出现9 > 10的情况
    query.next();
    id=query.recordValue(0).toLongLong() + 1;
    sqlexec = query.execInsert("cey_web_data_collection",qrtVariantPairList()

                               << qrtVariantPair("id", id) << qrtVariantPair("service_ip", n_func::f_get_sysparm("njgj_ip")) << qrtVariantPair("service_port", n_func::f_get_sysparm("njgj_port")) << qrtVariantPair("service_action", "http://tempurl.org/postsalescreate") << qrtVariantPair("service_method_name", "postsalescreate")
                               << qrtVariantPair("service_method_uri", "http://tempurl.org") << qrtVariantPair("service_submit_path", n_func::f_get_sysparm("njgj_web")) << qrtVariantPair("username", n_func::f_get_sysparm("njgj_username")) << qrtVariantPair("password", n_func::f_get_sysparm("njgj_password")) << qrtVariantPair("messagetype", "SALESDATA")
                               << qrtVariantPair("messageid", "332") << qrtVariantPair("version", "V332M") << qrtVariantPair("txdate_yyyymmdd", txdate_yyyymmdd) << qrtVariantPair("txtime_hhmmss", txtime_hhmmss) << qrtVariantPair("mallid", n_func::f_get_sysparm("njgj_mallid"))
                               << qrtVariantPair("storecode", n_func::f_get_sysparm("njgj_storecode")) << qrtVariantPair("tillid", tillid) << qrtVariantPair("txdocno", txdocno) << qrtVariantPair("cashier", cashier) << qrtVariantPair("salesman", salesman)
                               << qrtVariantPair("itemcode", n_func::f_get_sysparm("njgj_itemcode")) << qrtVariantPair("qty", qty) << qrtVariantPair("netamount", netamount) << qrtVariantPair("ch_uploadflag", "N") << qrtVariantPair("ch_uploadmemo", "ready")
                               ) ;

    if(sqlexec) {
        foreach(const TECH_TRANS_NJGJ_tendercode &t, tendercodes) {
            sqlexec = query.execInsert("cey_web_data_collection_tendercode",qrtVariantPairList()

                                       << qrtVariantPair("id", id) << qrtVariantPair("tendercode", t.tendercode) << qrtVariantPair("baseamount", t.baseamount));
            if(!sqlexec) {
                break;
            }
        }
    }
    sqlerr=query.recordError().trimmed();

    //!科传开始上传
    static w_scr_dish_TECH_TRANS_thread threadtechtrans;
    if(sqlerr.isEmpty()) {
        public_sql::save_login->ttpolling->emitsignalupload(0);
    }
    return sqlerr;
}

void w_scr_dish::TECH_TRANS_NJGJ_postsalescreate(w_scr_dish_TECH_TRANS_threadevent *parent)
{
    Q_UNUSED(parent)

    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    lds_query_hddpos  query3;
    QEventLoop eloop;
    QtSoapHttpTransport http;
    QTimer timer;
    QtSoapStruct *soapstruct=0;
    QtSoapMessage request;
    qlonglong id;
    connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    connect(&http, SIGNAL(responseReady()), &eloop,SLOT(quit()));
    timer.setSingleShot(true);

    query.execSelect("select * from cey_web_data_collection where ch_uploadflag='N' ");
    while(query.next()) {
        http.setHost(query.recordValue("service_ip").toString(),query.recordValue("service_port").toInt());
        http.setAction(query.recordValue("service_action").toString());
        soapstruct=request.setMethod(query.recordValue("service_method_name").toString(),
                                     query.recordValue("service_method_uri").toString());
        id=query.recordValue("id").toLongLong();
        int loopcount=3;

        while(loopcount >= 1 && loopcount--) {//! while(loopcount--)
            {//!request
                QtSoapStruct *astr_request=0;
                soapstruct->insert(astr_request=new QtSoapStruct(QtSoapQName("astr_request", "")));
                {//!astr_request
                    QtSoapStruct *header=0;
                    astr_request->insert(header=new QtSoapStruct(QtSoapQName("header", "")));
                    header->insert(new QtSoapSimpleType(QtSoapQName("username", ""), query.recordValue("username").toString()));
                    header->insert(new QtSoapSimpleType(QtSoapQName("password", ""), query.recordValue("password").toString()));
                    header->insert(new QtSoapSimpleType(QtSoapQName("messagetype", ""), query.recordValue("messagetype").toString()));
                    header->insert(new QtSoapSimpleType(QtSoapQName("messageid", ""), query.recordValue("messageid").toString()));
                    header->insert(new QtSoapSimpleType(QtSoapQName("version", ""), query.recordValue("version").toString()));

                    QtSoapStruct *salestotal=0;
                    astr_request->insert(salestotal=new QtSoapStruct(QtSoapQName("salestotal", "")));
                    salestotal->insert(new QtSoapSimpleType(QtSoapQName("txdate_yyyymmdd", ""), query.recordValue("txdate_yyyymmdd").toDateTime().toString("yyyyMMdd")));
                    salestotal->insert(new QtSoapSimpleType(QtSoapQName("txtime_hhmmss", ""), query.recordValue("txtime_hhmmss").toTime().toString("hhmmss")));
                    //                    salestotal->insert(new QtSoapSimpleType(QtSoapQName("mallid", ""), query.recordValue("mallid").toString()));
                    salestotal->insert(new QtSoapSimpleType(QtSoapQName("storecode", ""), query.recordValue("storecode").toString()));
                    salestotal->insert(new QtSoapSimpleType(QtSoapQName("tillid", ""), query.recordValue("tillid").toString()));
                    salestotal->insert(new QtSoapSimpleType(QtSoapQName("txdocno", ""), query.recordValue("txdocno").toString()));
                    salestotal->insert(new QtSoapSimpleType(QtSoapQName("cashier", ""), query.recordValue("cashier").toString()));
                    salestotal->insert(new QtSoapSimpleType(QtSoapQName("salesman", ""), query.recordValue("salesman").toString()));

                    QtSoapStruct *salesitems=0;
                    astr_request->insert(salesitems=new QtSoapStruct(QtSoapQName("salesitems", "")));
                    {//!salesitems:也支持多条item上传,但是目前只是支持商品汇总后上传,itemcode是唯一的
                        QtSoapStruct *salesitem=0;
                        salesitems->insert(salesitem=new QtSoapStruct(QtSoapQName("salesitem", "")));
                        salesitem->insert(new QtSoapSimpleType(QtSoapQName("itemcode", ""), query.recordValue("itemcode").toString()));
                        salesitem->insert(new QtSoapSimpleType(QtSoapQName("qty", ""), query.recordValue("qty").toDouble()));
                        salesitem->insert(new QtSoapSimpleType(QtSoapQName("netamount", ""), query.recordValue("netamount").toDouble()));
                    }//!salesitems

                    QtSoapStruct *salestenders=0;
                    astr_request->insert(salestenders=new QtSoapStruct(QtSoapQName("salestenders", "")));
                    {//!salestenders:可以有多条如现金付款,国内银行卡付款
                        query3.execSelect(QString("select * from cey_web_data_collection_tendercode where id='%1' ").arg(id));
                        while(query3.next()) {
                            QtSoapStruct *salestender=0;
                            salestenders->insert(salestender=new QtSoapStruct(QtSoapQName("salestender", "")));
                            salestender->insert(new QtSoapSimpleType(QtSoapQName("tendercode", ""), query3.recordValue("tendercode").toString()));
                            salestender->insert(new QtSoapSimpleType(QtSoapQName("baseamount", ""), query3.recordValue("baseamount").toDouble()));
                        }
                    }//!salestenders
                }//!astr_request
            }//!request


            http.submitRequest(request, query.recordValue("service_submit_path").toString());
            timer.start(30*1000);//30s的超时eply*)),&eloop,SLOT(quit()));缩短时间,方便及时退出
            eloop.exec(QEventLoop::ExcludeUserInputEvents);
            if(timer.isActive()) {
                const QtSoapMessage &message = http.getResponse();
                if (message.isFault()) {
                    qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
                } else {
                    if(query2.execUpdate("cey_web_data_collection",qrtVariantPairList()
                                         << qrtVariantPair("ch_uploadflag", "Y")<< qrtVariantPair("ch_uploadmemo", "success"),
                                         QString("id = '%1' ").arg(id))) {
                        break;
                    } else {
                    }
                }
            }

            if(loopcount==0) {
                query2.execUpdate("cey_web_data_collection",qrtVariantPairList()
                                  << qrtVariantPair("ch_uploadmemo", http.networkReply()->errorString()),
                                  QString("id = '%1' ").arg(id));
            }
        }//!while(loopcount--)
    }//!while(query.next())
}

QString w_scr_dish::huarunInfo_error(int info)
{
    QString errstring;
    switch(info) {
    case w_scr_dish::huarun_local_business_is_empty:
        errstring = "huarun_local_business_is_empty";
        break;
    case w_scr_dish::huarun_local_up_file_is_empty:
        errstring = "huarun_local_up_file_is_empty";
        break;
    case w_scr_dish::huarun_local_up_file_open_error:
        errstring = "huarun_local_up_file_open_error";
        break;
    case w_scr_dish::huarun_local_dir_mk_error:
        errstring = "huarun_local_dir_mk_error";
        break;
    case w_scr_dish::huarun_ftp_network_not_access:
        errstring = QObject::tr("网络不可访问");
        break;

    case w_scr_dish::huarun_ftp_file_exists:
        errstring =QObject::tr("服务器文件已存在");
        break;
    case w_scr_dish::huarun_ftp_server_error:
        errstring =QObject::tr("上传失败");
        break;
    case w_scr_dish::huarun_ftp_server_timeout:
        errstring = "huarun_ftp_server_timeout";
        break;
    case w_scr_dish::huarun_ftp_server_success:
        break;
    }

    return errstring;
}

bool w_scr_dish::GET_HUARUN_BUSINESS_DATA(const QDate &uploaddate, QList<HUARUN_BUSINESS_DATA> &dataList, QString &errstring)
{
    //1 00:00:00 <= date <= 23:59:59
    //-> 昨天晚上23：59：:9的数据上传
    //-<销售店铺号
    //storecode
    //-<收银机号
    //01  固定
    //-<日期 yyyy/MM/dd
    //select  dt_operdate from cey_u_checkout_master where  ch_payno = 'P15110400065'
    //-<时间 hhmm
    //select  dt_operdate from cey_u_checkout_master where  ch_payno = 'P15110400065'
    //-<单据号S000000001开始
    //年初清零
    //-<货号
    //提供
    //-<会员卡号
    //select  vch_voucherno  from cey_u_checkout_detail where ch_payno = 'P15110400065' and ch_paymodeno = '02';
    //-<现金
    //select  sum(num_payamount - num_change) from cey_u_checkout_detail where ch_payno = 'P15110400065' and ch_paymodeno = '00';
    //-<国内银行卡
    //select  sum(num_payamount - num_change) from cey_u_checkout_detail where ch_payno = 'P15110400065' and ch_paymodeno = '03';
    //-<其他付款
    //select  sum(num_payamount - num_change)  from cey_u_checkout_detail where ch_payno = 'P15110400065' and ch_paymodeno <> '03' and ch_paymodeno <> '00';
    //-<折扣
    //select sum(num_discount) from cey_u_checkout_master where ch_payno = 'P15110400065' ;
    //-<消费
    //select  sum(num_cost) from cey_u_checkout_master where ch_payno = 'P15110400065' ;
    QStringList ch_paynolist;
    lds_query_hddpos  query;
    HUARUN_BUSINESS_DATA st_uploaddata;
    
    st_uploaddata.ls_docno=n_func::f_get_sysparm("ftptp_docno", "S000000000");
    //销售店铺好
    //机号 01
    st_uploaddata.ls_storecode = n_func::f_get_sysparm("ftptp_storecode");
    st_uploaddata.ls_tillid = "01";
    //销售开始时间 150925
    query.execSelect(QString("select ch_payno  from cey_u_checkout_master where left(ch_payno, 7) = '%1' and ch_state = 'Y' ").arg("P"+uploaddate.toString("yyMMdd")));
    while(query.next()) {
        ch_paynolist.append(query.recordValue("ch_payno").toString());
    }
    //生成上次需要上传的数据
    foreach(const QString &ch_payno, ch_paynolist) {
        query.execSelect(QString("select  dt_operdate from cey_u_checkout_master where  ch_payno = '%1'").arg(ch_payno));
        query.next();
        st_uploaddata.ls_txdate = query.recordValue("dt_operdate").toDate().toString("yyyy/MM/dd");
        st_uploaddata.ls_txtime = query.recordValue("dt_operdate").toTime().toString("hhmm");
        //加1
        st_uploaddata.ls_docno = "S"+QString::number(st_uploaddata.ls_docno.right(9).toInt()+1).rightJustified(9, '0', true);
        n_func::f_set_sysparm("ftptp_docno", st_uploaddata.ls_docno);

        st_uploaddata.ls_plu = n_func::f_get_sysparm("ftptp_plu");

        query.execSelect(QString("select  vch_voucherno  from cey_u_checkout_detail where ch_payno = '%1' and ch_paymodeno = '02'").arg(ch_payno));
        query.next();
        st_uploaddata.ls_vipcode = query.recordValue("vch_voucherno").toString();

        query.execSelect(QString("select  sum(num_payamount - num_change) from cey_u_checkout_detail where ch_payno = '%1' and ch_paymodeno = '00'").arg(ch_payno));
        query.next();
        st_uploaddata.ls_ch = QString::number(query.recordValue(0).toFloat());

        query.execSelect(QString("select  sum(num_payamount - num_change) from cey_u_checkout_detail where ch_payno = '%1' and ch_paymodeno = '03'").arg(ch_payno));
        query.next();
        st_uploaddata.ls_ci = QString::number(query.recordValue(0).toFloat());

        st_uploaddata.ls_co = "0";//金额的没有也是0

        ///跳过：优惠、抹零、银行卡、现金
        query.execSelect(QString("select  sum(num_payamount - num_change)  from cey_u_checkout_detail where ch_payno = '%1' and ch_paymodeno <> 'ZZ' and ch_paymodeno <> 'YY'  and ch_paymodeno <> '03' and ch_paymodeno <> '00';").arg(ch_payno));
        query.next();
        st_uploaddata.ls_ot = QString::number(query.recordValue(0).toFloat());

        query.execSelect(QString("select sum(num_discount) from cey_u_checkout_master where ch_payno = '%1' ;").arg(ch_payno));
        query.next();
        st_uploaddata.ls_ttldiscount = QString::number(query.recordValue(0).toFloat());//折扣强行改成0

        query.execSelect(QString("select  sum(num_cost - ifnull(num_discount,0) - ifnull(num_present,0) + ifnull(num_service,0) + ifnull(num_lowcost,0) - ifnull(num_blotout, 0) - ifnull(num_free, 0)) from cey_u_checkout_master where ch_payno = '%1';").arg(ch_payno));
        query.next();
        st_uploaddata.ls_netamt = QString::number(query.recordValue(0).toFloat());
        if(st_uploaddata.ls_netamt.toFloat() != 0) {
            dataList.append(st_uploaddata);
        }
    }

    errstring = "";
    return true;
}

int w_scr_dish::HUARUN_BUSINESS_DATA_UPLOAD(const QString &ftp, const QString &usr, const QString &pwd, const QList<HUARUN_BUSINESS_DATA> &datalist)
{
    QTimer timer;
    QString uploaddata;
    QString filename;
    QString ls_docno;
    timer.setInterval(60*1000);
    timer.setSingleShot(true);
    if(datalist.isEmpty()) {
        qDebug() << "营业数据为空，华润上传";
        return huarun_local_business_is_empty;
    }

    foreach(const HUARUN_BUSINESS_DATA &data, datalist) {
        if(filename.isEmpty()) {
            QString date = data.ls_txdate;//"yyyy/MM/dd"
            filename = data.ls_storecode + date.replace("/","") + ".txt";
        }
        uploaddata += data.ls_storecode+"\t"+
                data.ls_tillid+"\t"+
                data.ls_txdate+"\t"+
                data.ls_txtime+"\t"+
                data.ls_docno+"\t"+

                data.ls_plu+"\t"+
                data.ls_vipcode+"\t"+
                data.ls_ch+"\t"+
                data.ls_ci+"\t"+
                data.ls_co+"\t"+

                data.ls_ot+"\t"+
                data.ls_ttldiscount+"\t"+
                data.ls_netamt+"\t"+
                "\r\n"
                ;
        if(data.ls_docno > ls_docno) ls_docno =  data.ls_docno;
    }
    if(filename.isEmpty()) {
        return huarun_local_up_file_is_empty;
    }

    if(!QDir(public_sql::ftp_businessdataupload_dir).exists()) {
        if(!QDir().mkpath(public_sql::ftp_businessdataupload_dir)) {
            return huarun_local_dir_mk_error;
        }
    }

    QNetworkAccessManager manager;
    QNetworkRequest req;
    QEventLoop eloop;

    QUrl u(ftp+"/"+filename);
    u.setUserName(usr);
    u.setPassword(pwd);
    req.setUrl(u);

    timer.setSingleShot(true);
    connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    connect(&manager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply=manager.put(req, uploaddata.toLocal8Bit());

    timer.start();//10s的超时eply*)),&eloop,SLOT(quit()));
    eloop.exec(QEventLoop::ExcludeUserInputEvents);
    if(timer.isActive()) {
        if(reply->error()==QNetworkReply::NoError) {
            n_func::f_set_sysparm("ftptp_docno", ls_docno);

            //保存记录
            QFile txt1(public_sql::ftp_businessdataupload_dir+"/"+filename);
            if(!txt1.open(QFile::WriteOnly)) {
                return huarun_local_up_file_open_error;
            }
            txt1.write(uploaddata.toLocal8Bit());
            txt1.close();

            return huarun_ftp_server_success;
            //上传成功
        } else {
            if(ftp_download(u)) {
                //                return QObject::tr("文件已存在");
                return huarun_ftp_file_exists;
            }
            return huarun_ftp_server_error;
        }
    } else {
        return huarun_ftp_server_timeout;
    }
}

bool w_scr_dish::ftp_download(const QUrl &url)
{
    QTimer timer;
    QEventLoop loop;
    QNetworkAccessManager networkAccessManager;
    QNetworkRequest request;
    request.setUrl(url);

    connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()),&loop, SLOT(quit()));
    QNetworkReply *reply = networkAccessManager.get(request);

    timer.setSingleShot(true);
    timer.start(60 * 1000);
    loop.exec(QEventLoop::ExcludeUserInputEvents);
    if(timer.isActive()) {
        if(reply->error()==QNetworkReply::NoError) {
            return true;
        } else {
            qDebug() << reply->errorString();
        }
    } else {
        qDebug() << "time out";
    }
    return false;
}

void w_scr_dish::static_waimaipay(QWidget *parent)
{
    take_cashier cashierdialog(parent);
    cashierdialog.setWindowTitle(QObject::tr("外卖收银结算"));
    transparentCenterDialog(&cashierdialog).exec();
}

int w_scr_dish::XIEXIN_BUSINESS_DATA_UPLOAD(const QDate &date, QString *errstring)
{
    lds_query_hddpos  query;
    QString CUBE_ID = n_func::f_get_sysparm("xiexin_CUBE_ID");//收款机号
    QString xiexin_service_ip = n_func::f_get_sysparm("xiexin_service_ip");//收款机号
    QString SALES_DATE = date.toString("yyyy/MM/dd");
    int TRANS_NO_max_999999 = n_func::f_get_sysparm("xiexin_TRANS_NO", 0).toInt();//获取最大值然后+1
    TRANS_NO_max_999999++;
    TRANS_NO_max_999999 = (TRANS_NO_max_999999) % 1000000;//0-999999循环
    QString RECEIVER_ID = "0000001";
    QString TRANS_TYPE = "01";//退货为02,固定为进货

    query.execSelect(QString("select min(dt_operdate), max(dt_operdate) from cey_u_checkout_master where ch_state = 'Y' and  date(dt_operdate) = '%1' ").arg(SALES_DATE));
    query.next();
    QString RECORD_BEGIN = query.recordValue(0).toDateTime().toString("hhmm");
    QString RECORD_END = query.recordValue(1).toDateTime().toString("hhmm");
    if(RECORD_BEGIN.isEmpty() || RECORD_END.isEmpty()) {
        if(errstring) *errstring = "no business data";
        return 2;
    }
    query.execSelect(QString("select  sum(num_cost) as num_cost, "
                             "sum(num_present) as sum_present, "
                             "sum(num_discount) as sum_discount, "
                             "count(ch_payno) as SALE_COUNT "
                             "from cey_u_checkout_master  where ch_state = 'Y' and  date(dt_operdate) = '%1' ;").arg(SALES_DATE));
    query.next();
    
    double GROSSPLUS = query.recordValue("num_cost").toDouble();
    double DISC_AMT = query.recordValue("sum_present").toDouble();
    DISC_AMT += query.recordValue("sum_discount").toDouble();
    double SALE_COUNT = query.recordValue("SALE_COUNT").toDouble();
    double NET = GROSSPLUS - DISC_AMT;

    query.execSelect(QString("select sum(a.num_num - a.num_back) as TOT_QTY from cey_u_orderdish a,cey_u_checkout_master b  where a.ch_payno = b.ch_payno and b.ch_state = 'Y' and  date(b.dt_operdate) = '%1' ; ").arg(SALES_DATE));
    query.next();
    
    double TOT_QTY = query.recordValue("TOT_QTY").toDouble();

    cJSON *root = cJSON_CreateObject();
    cJSON_help::_cJSON_AddValueToObject( root, "CUBE_ID", CUBE_ID);//收款机号
    cJSON_help::_cJSON_AddValueToObject( root, "SALES_DATE", SALES_DATE);//交易日期
    cJSON_help::_cJSON_AddValueToObject( root, "TRANS_NO", TRANS_NO_max_999999);//交易序号
    cJSON_help::_cJSON_AddValueToObject( root, "RECORD_BEGIN", RECORD_BEGIN);//交易开始时间
    cJSON_help::_cJSON_AddValueToObject( root, "RECORD_END", RECORD_END);//交易结束时间
    cJSON_help::_cJSON_AddValueToObject( root, "RECEIVER_ID", RECEIVER_ID);//收银员编号
    cJSON_help::_cJSON_AddValueToObject( root, "TRANS_TYPE", TRANS_TYPE);//交易类别码
    cJSON_help::_cJSON_AddValueToObject( root, "GROSSPLUS", GROSSPLUS);//交易金额
    cJSON_help::_cJSON_AddValueToObject( root, "DISC_AMT", DISC_AMT);//折扣金额
    cJSON_help::_cJSON_AddValueToObject( root, "NET", NET);//交易净额
    cJSON_help::_cJSON_AddValueToObject( root, "SALE_COUNT", SALE_COUNT);//交易笔数
    cJSON_help::_cJSON_AddValueToObject( root, "TOT_QTY", TOT_QTY);//交易商品件数
    cJSON_help::_cJSON_AddValueToObject( root, "VIP_NO", "");//会员编号
    cJSON_help::_cJSON_AddValueToObject( root, "ORG_GUI_DATE", "");//原交易日期
    cJSON_help::_cJSON_AddValueToObject( root, "ORG_POS_ID", "");//原交易机号
    cJSON_help::_cJSON_AddValueToObject( root, "ORG_TRANS_NO", "");//原交易序号


    cJSON *root_array = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "SALES_D", root_array);
    query.execSelect(QString("select "
                             " sum((a.num_num - a.num_back) *a.num_price+a.num_price_add) as num_cost,"
                             " sum(a.num_price) as num_price, "
                             " sum(a.num_num - a.num_back) as num_dish, "
                             " sum( if(a.ch_presentflag = 'Y', "
                             "((a.num_num - a.num_back) *a.num_price), 0)) as num_present, "
                             " sum( ((a.num_num - a.num_back) *a.num_price) * (100-a.int_discount)*0.01) as num_discount,  "
                             " a.ch_dishno"
                             " from cey_u_orderdish a, cey_u_checkout_master b "

                             "   where a.ch_payno = b.ch_payno and b.ch_state = 'Y' and date(b.dt_operdate) = '%1'  "
                             "   group by a.ch_dishno;"
                             ).arg(SALES_DATE));
    
    while(query.next()) {
        double QTY =  query.recordValue("num_dish").toFloat();
        double SELL_AMT =  query.recordValue("num_cost").toDouble();
        double DISC_AMT =  query.recordValue("num_discount").toDouble() + query.recordValue("num_present").toDouble();;
        double SALES_AMT =  SELL_AMT - DISC_AMT;
        QString GOODS_ID =  query.recordValue("ch_dishno").toString();
        double SELL_PRICE = 0;
        if(QTY != 0) SELL_PRICE = SELL_AMT / QTY;

        cJSON *item = cJSON_CreateObject();
        cJSON_AddItemToArray(root_array, item);

        cJSON_help::_cJSON_AddValueToObject( item, "CUBE_ID", CUBE_ID);//  收款机号
        cJSON_help::_cJSON_AddValueToObject( item, "SALES_DATE", SALES_DATE);
        cJSON_help::_cJSON_AddValueToObject( item, "TRANS_NO", TRANS_NO_max_999999);
        cJSON_help::_cJSON_AddValueToObject( item, "SELL_PRICE", SELL_PRICE);
        cJSON_help::_cJSON_AddValueToObject( item, "QTY", QTY);
        cJSON_help::_cJSON_AddValueToObject( item, "SELL_AMT", SELL_AMT);
        cJSON_help::_cJSON_AddValueToObject( item, "DISC_AMT", DISC_AMT);
        //        cJSON_help::_cJSON_AddValueToObject( item, "DISC_RATE", "0");
        cJSON_help::_cJSON_AddValueToObject( item, "SALES_AMT", SALES_AMT);
        cJSON_help::_cJSON_AddValueToObject( item, "GROUP_ID", "");
        cJSON_help::_cJSON_AddValueToObject( item, "GOODS_ID", "");
        cJSON_help::_cJSON_AddValueToObject( item, "ND", "000");//默认
        cJSON_help::_cJSON_AddValueToObject( item, "ND_TYPE", "0");//默认

    }

    root_array = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "PAY_TYPE", root_array);
    query.execSelect(QString("select  sum(num_cost) as sum_cost, "
                             "sum(num_present) as sum_present, "
                             "sum(num_discount) as sum_discount "
                             "from cey_u_checkout_master  where ch_state = 'Y' and  date(dt_operdate) = '%1' group by ch_payno").arg(SALES_DATE));
    
    while(query.next()) {
        float num_discount = query.recordValue("sum_discount").toFloat();
        float num_present = query.recordValue("sum_present").toFloat();
        float num_cost = query.recordValue("sum_cost").toFloat();

        cJSON *item = cJSON_CreateObject();
        cJSON_AddItemToArray(root_array, item);

        cJSON_help::_cJSON_AddValueToObject( item, "CUBE_ID", CUBE_ID);//  收款机号
        cJSON_help::_cJSON_AddValueToObject( item, "SALES_DATE", SALES_DATE);
        cJSON_help::_cJSON_AddValueToObject( item, "TRANS_NO", TRANS_NO_max_999999);
        cJSON_help::_cJSON_AddValueToObject( item, "TENDER",  "10");//现金支付方式
        cJSON_help::_cJSON_AddValueToObject( item, "AMOUNT", (num_cost - num_discount - num_present));
    }

    QString xiexin_upload_data = cJSON_help::getcJSONprint(root);
    cJSON_Delete(root);
    xiexin_upload_data  = "="+xiexin_upload_data;
    //
    QEventLoop eloop;
    QNetworkRequest request;
    request.setUrl(xiexin_service_ip);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
    QNetworkAccessManager networkAccessManager;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    connect(&networkAccessManager,SIGNAL(finished(QNetworkReply*)),&eloop,SLOT(quit()));
    QNetworkReply *reply = networkAccessManager.post(request,xiexin_upload_data.toLocal8Bit());
    timer.start(60*1000);//10s的超时eply*)),&eloop,SLOT(quit()));
    eloop.exec(QEventLoop::ExcludeUserInputEvents);
    if(timer.isActive()
            &&reply->error()==QNetworkReply::NoError) {
        QString p = reply->readAll();
        cJSON *item = cJSON_Parse(p.toLocal8Bit());
        if("0" == cJSON_help::getcJSONvalue(item, "RTN")) {

            QString path = lds::localdataPath + "/XIEXIN_BUSINESS_DATA_UPLOAD";
            QDir().mkpath(path);
            QFile file(path+"/"+date.toString("yyyy-MM-dd")+".txt");
            if(!file.open(QFile::WriteOnly)) {
                if(errstring) *errstring = "file write error" + file.errorString();
                return 0;
            }
            file.write(xiexin_upload_data.toLocal8Bit());
            file.close();

            n_func::f_set_sysparm("xiexin_TRANS_NO", TRANS_NO_max_999999);//获取最大值然后+1

            //"上传成功";
            return 1;
        }
        if(errstring) *errstring = "upload data error" + cJSON_help::getcJSONvalue(item, "MSG").toString();
        return 0;
    }
    reply->abort();
    if(errstring) *errstring = "time out" + reply->readAll();
    return 0;
}

//! 线程对数据库操作需要,克隆数据库
//! 线程锁保证,两次付款不会冲突(a成功,b失败的情况)
w_scr_dish_TECH_TRANS_thread::w_scr_dish_TECH_TRANS_thread(QObject *parent)
    : lds_thread("科传接口", parent)
{
}

w_scr_dish_TECH_TRANS_thread::~w_scr_dish_TECH_TRANS_thread()
{
}

void w_scr_dish_TECH_TRANS_thread::run()
{

    w_scr_dish_TECH_TRANS_threadevent printevent(this);
    connect(this,SIGNAL(signalupload(QObject*)),&printevent, SLOT(upload(QObject*)));
    this->exec();

}

//===================
w_scr_dish_TECH_TRANS_threadevent::w_scr_dish_TECH_TRANS_threadevent(QObject */*parent*/)
    :QObject(0)
{

}

w_scr_dish_TECH_TRANS_threadevent::~w_scr_dish_TECH_TRANS_threadevent()
{

}

void w_scr_dish_TECH_TRANS_threadevent::upload(QObject */*parent*/)
{
    w_scr_dish::TECH_TRANS_NJGJ_postsalescreate(this);
}



void w_scr_dish_liaoning_inventory_keep_thread::run()
{

    w_scr_dish_liaoning_inventory_keep_thread_event event;
    execEvent(&event);
}


void w_scr_dish_liaoning_inventory_keep_thread_event::genrateBill(const QString &ch_payno)
{
    QString content;
    lds_query_hddpos  query;
    query.execSelect(QString(" select (select vch_dishname from cey_bt_dish where ch_dishno = cey_u_orderdish.ch_dishno) as dishname, -1 * (num_num - num_back) as dishnum_sub from cey_u_orderdish where ch_payno = '%1' ")
                     .arg(ch_payno));
    while(query.next()) {
        QString url =
                QString("http://www.ddyouming.com/interface/sysdata/Item_UpdateMenu?") +
                "item_name=" + query.recordValue("dishname").toByteArray().toPercentEncoding() +
                "&car_name=" + w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad::car_name().toPercentEncoding() +
                "&item_num=" + query.recordValue("dishnum_sub").toByteArray().toPercentEncoding();
        content += url + "\n";
    }
    QString path = lds::localdataPath +"/liaoning_inventory_keep_cache";
    QFile file(path + "/" + ch_payno + ".txt");
    if(false == file.open(QFile::WriteOnly)) {
        qDebug() << file.errorString();
    }
    qDebug() << __FUNCTION__ << content;
    file.write(content.toLocal8Bit());
    file.close();
}

void w_scr_dish_liaoning_inventory_keep_thread_event::uploadBill()
{
    QString path = lds::localdataPath +"/liaoning_inventory_keep_cache";
    QFileInfoList fileinfos =QDir(path).entryInfoList(QStringList() << "*.txt");
    QNetworkAccessManager manager;
    QUrl url;
    QNetworkReply *reply = 0;
    QByteArray readall;
    QString err_code ;
    QString errstringcontent;
    QFile file;
    QString upload_filepath;
    QString upload_filepath_error;
    QByteArray conent_remain;

    foreach(const QFileInfo &info, fileinfos) {
        if(info.baseName().endsWith("_error")) continue;

        upload_filepath = info.filePath();
        upload_filepath_error = info.path() + "/" +info.baseName()+"_error.txt";
        errstringcontent = "";
        file.close();
        file.setFileName(upload_filepath);
        if(false == file.open(QFile::ReadOnly)) {
            errstringcontent += file.errorString() + "\n";
            goto error;
        }
        conent_remain = "";
        while(false == file.atEnd()) {
            url = QString().append(file.readLine()).trimmed();
            reply = w_sys_manage_cloudsync_transfer::directGet(&manager, url);
            if(200 != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()) {
                conent_remain += url.toString() + "\n";
                errstringcontent += "通信失败\n";
                continue;
            }
            readall = reply->readAll();
            err_code = cJSON_help::getcJSONvalue(readall, "err_code").toString();
            if(err_code == "0" || err_code == "1") {
                qDebug() << __FUNCTION__ << url;
                continue;
            }
            conent_remain += url.toString() + "\n";
            errstringcontent += readall +"\n";
        }

        //完全上传成功，则删除file
        file.close();
        if(conent_remain.length() == 0) {
            QFile::remove(upload_filepath);
            QFile::remove(upload_filepath_error);
        } else {//上传失败的保留
            file.setFileName(upload_filepath);
            if(!file.open(QFile::WriteOnly)) {
                qDebug() << file.errorString();
            }
            file.write(conent_remain);
        }
error:
        if(errstringcontent.length() == 0) continue;

        file.close();
        file.setFileName(upload_filepath_error);
        if(!file.open(QFile::WriteOnly)) {
            qDebug() << file.errorString();
        }
        file.write(errstringcontent.toLocal8Bit());
        file.close();
    }
}

void w_scr_dish_liaoning_inventory_keep_thread_event::toExec()
{
    uploadBill();
}

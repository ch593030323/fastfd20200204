#include "w_sys_manage_outer_pay_set_fengze.h"
#include "ui_w_sys_manage_outer_pay_set_fengze.h"
#include "lds.h"
#include "n_func.h"
#include "lds_messagebox.h"
#include "qtsoap.h"
#include <QSslConfiguration>
#include "fexpandmain_model_sqltablemodel_data.h"
#include "lds_roundeddialog_rect_pop.h"


void w_sys_manage_outer_pay_set_fengze::uploadBill()
{
    QString website = n_func::f_get_sysparm("fengze_website");
    QString mallid = n_func::f_get_sysparm("fengze_mallid");
    QString license = n_func::f_get_sysparm("fengze_license");
    QString storeCode = n_func::f_get_sysparm("fengze_storeCode");
    QString itemCode = n_func::f_get_sysparm("fengze_itemCode");
    QString username = n_func::f_get_sysparm("fengze_username");
    QString password = n_func::f_get_sysparm("fengze_password");

    //#ifdef QT_DEBUG
    //    website = "http://shfengze.f3322.net:8008/sales.asmx";
    //    mallid = "02101";
    //    license = "";
    //    storeCode = "02101F116-2201";
    //    itemCode = "02101F116-220101";
    //    username = "010201";
    //    password = "010201";
    //#endif

    QUrl url(website);
    QTimer      timer;
    timer.setSingleShot(true);
    QtSoapHttpTransport http;
    QString path = lds::localdataPath + "/business_data_upload_fengze";
    QFile file;
    QByteArray readall;
    QEventLoop eloop;
    QObject::connect(&http, SIGNAL(responseReady()), &eloop, SLOT(quit()));
    QObject::connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    QString errstring;

    http.setHost(url.host(), url.port());
    http.setAction("http://tempurl.org/postsalescreate");

    QFileInfoList fileinfos =QDir(path).entryInfoList(QStringList() << "*.txt");
    for(int k = 0; k < fileinfos.count(); k++) {
        QFileInfo info = fileinfos[k];
        if(info.baseName().endsWith("_error"))
            continue;
        file.setFileName(info.filePath());
        if(!file.open(QFile::ReadOnly)) {
            qDebug() << __FILE__ << __LINE__ << file.errorString();
        }
        readall = file.readAll();
        file.close();

        http.submitRequest(readall, url.path());
        timer.start(10*1000);
        eloop.exec(QEventLoop::ExcludeUserInputEvents);
        if(!timer.isActive()) {
            errstring = QObject::tr("超时");
        } else {
            const QtSoapMessage &message = http.getResponse();
            QString strXML = message.toXmlString();
            errstring = strXML;

            if (message.isFault()) {
                QDomDocument doc;
                doc.setContent(strXML);
                QDomNodeList Faultcode = doc.elementsByTagName("Faultcode");
                QDomNodeList Faultstring = doc.elementsByTagName("Faultstring");
                errstring = QObject::tr("上传失败");
                if(Faultcode.length() > 0)
                    errstring += "\n Faultcode:\t" + Faultcode.at(0).toElement().text();
                if(Faultstring.length() > 0)
                    errstring += "\n Faultstring:\t" + Faultstring.at(0).toElement().text();
            } else {
                QDomDocument doc;
                doc.setContent(strXML);
                QDomNodeList responsecode = doc.elementsByTagName("responsecode");
                QDomNodeList responsemessage = doc.elementsByTagName("responsemessage");
                QString responsecode_value;
                if(responsecode.length() > 0)
                    responsecode_value = responsecode.at(0).toElement().text();
                QString responsemessage_value;
                if(responsemessage.length() > 0)
                    responsemessage_value = responsemessage.at(0).toElement().text();

                if(responsecode_value == "0" //成功
                        || responsecode_value == "-100"//重复上传
                        ) {
                    QFile::remove(info.filePath());
                    QFile::remove(path+ "/" + info.baseName() + "_error.txt");
                    continue;
                } else {
                    errstring = QObject::tr("上传失败") + "\n"
                            + "responsecode:\t" + responsecode_value + "\n"
                            + "responsemessage:\t" + responsemessage_value
                            ;
                }
            }
        }

        file.setFileName(path+ "/" + info.baseName() + "_error.txt");
        if(!file.open(QFile::WriteOnly)) {
            qDebug() << __FILE__ << __LINE__ << file.errorString();
        }
        file.write(errstring.toLocal8Bit());
        file.close();
    }
}

void w_sys_manage_outer_pay_set_fengze::genrateBill(const QString &ch_payno, double dish_num, double dish_total)
{
    QString website = n_func::f_get_sysparm("fengze_website");
    QString mallid = n_func::f_get_sysparm("fengze_mallid");
    QString license = n_func::f_get_sysparm("fengze_license");
    QString storeCode = n_func::f_get_sysparm("fengze_storeCode");
    QString itemCode = n_func::f_get_sysparm("fengze_itemCode");
    QString username = n_func::f_get_sysparm("fengze_username");
    QString password = n_func::f_get_sysparm("fengze_password");

    //#ifdef QT_DEBUG
    //    website = "http://shfengze.f3322.net:8008/sales.asmx";
    //    mallid = "02101";
    //    license = "";
    //    storeCode = "02101F116-2201";
    //    itemCode = "02101F116-220101";
    //    username = "010201";
    //    password = "010201";
    //#endif
    //    "http://shfengze.f3322.net:8008/sales.asmx"
    //    商场编号（mallid）：02101

    //    交易店铺号（StoreCode）：
    //    02101F116-2201
    //    交易货号（ItemCode）：
    //    02101F116-220101
    //    测试许可协议帐号:（空）

    //    测试用户帐号: 010201
    //    测试用户密码: 010201

    lds_query_hddpos  query;
    QtSoapStruct *soapstruct=0;
    QtSoapMessage request;
    soapstruct=request.setMethod("postsalescreate", "http://tempurl.org");
    {///request
        QtSoapStruct *astr_request=0;
        soapstruct->insert(astr_request=new QtSoapStruct(QtSoapQName("astr_request", "")));
        {///astr_request
            QtSoapStruct *header=0;
            astr_request->insert(header=new QtSoapStruct(QtSoapQName("header", "")));
            header->insert(new QtSoapSimpleType(QtSoapQName("username", ""), username));
            header->insert(new QtSoapSimpleType(QtSoapQName("password", ""), password));
            header->insert(new QtSoapSimpleType(QtSoapQName("messagetype", ""), "SALESDATA"));
            header->insert(new QtSoapSimpleType(QtSoapQName("messageid", ""), "332"));
            header->insert(new QtSoapSimpleType(QtSoapQName("version", ""), "V332M"));

            QtSoapStruct *salestotal=0;
            astr_request->insert(salestotal=new QtSoapStruct(QtSoapQName("salestotal", "")));
            salestotal->insert(new QtSoapSimpleType(QtSoapQName("txdate_yyyymmdd", ""), lds_query_hddpos::selectValue(query, QString(" select dt_operdate from cey_u_checkout_master where ch_payno = '%1' ").arg(ch_payno)).toDateTime().toString("yyyyMMdd")));
            salestotal->insert(new QtSoapSimpleType(QtSoapQName("txtime_hhmmss", ""), lds_query_hddpos::selectValue(query, QString(" select dt_operdate from cey_u_checkout_master where ch_payno = '%1' ").arg(ch_payno)).toDateTime().toString("hhmmss")));
            //            salestotal->insert(new QtSoapSimpleType(QtSoapQName("mallid", ""), mallid));
            salestotal->insert(new QtSoapSimpleType(QtSoapQName("storecode", ""), storeCode));
            salestotal->insert(new QtSoapSimpleType(QtSoapQName("tillid", ""), "01"));//暂时写死
            salestotal->insert(new QtSoapSimpleType(QtSoapQName("txdocno", ""), ch_payno));
            salestotal->insert(new QtSoapSimpleType(QtSoapQName("cashier", ""), lds_query_hddpos::selectValue(query, QString(" select vch_operid from cey_u_checkout_master where ch_payno = '%1' ").arg(ch_payno)).toString()));
            salestotal->insert(new QtSoapSimpleType(QtSoapQName("salesman", ""), lds_query_hddpos::selectValue(query, QString(" select vch_operid from cey_u_checkout_master where ch_payno = '%1' ").arg(ch_payno)).toString()));

            QtSoapStruct *salesitems=0;
            astr_request->insert(salesitems=new QtSoapStruct(QtSoapQName("salesitems", "")));
            {///salesitems:也支持多条item上传,但是目前只是支持商品汇总后上传,itemcode是唯一的
                QtSoapStruct *salesitem=0;
                salesitems->insert(salesitem=new QtSoapStruct(QtSoapQName("salesitem", "")));
                salesitem->insert(new QtSoapSimpleType(QtSoapQName("itemcode", ""), itemCode));
                salesitem->insert(new QtSoapSimpleType(QtSoapQName("qty", ""),  dish_num));
                salesitem->insert(new QtSoapSimpleType(QtSoapQName("netamount", ""), dish_total));
            }//!salesitems

            QtSoapStruct *salestenders=0;
            astr_request->insert(salestenders=new QtSoapStruct(QtSoapQName("salestenders", "")));
            {///salestenders:可以有多条如现金付款,国内银行卡付款
                QtSoapStruct *salestender=0;
                salestenders->insert(salestender=new QtSoapStruct(QtSoapQName("salestender", "")));
                salestender->insert(new QtSoapSimpleType(QtSoapQName("tendercode", ""), "OT"));
                salestender->insert(new QtSoapSimpleType(QtSoapQName("payamount", ""), dish_total));
                salestender->insert(new QtSoapSimpleType(QtSoapQName("baseamount", ""), dish_total));
            }//!salestenders
        }//!astr_request
    }//!request


    QString path = lds::localdataPath + "/business_data_upload_fengze";
    if(!QDir(path).exists()) {
        QDir().mkpath(path);
    }
    QFile file(path+"/"+ch_payno+".txt");
    if(!file.open(QFile::WriteOnly)) {
        qDebug() << __FILE__ << __LINE__ <<  file.errorString();
    }
    file.write(request.toXmlString().toUtf8());
    file.close();

    //    http.submitRequest(request, web_service);
    //    timer.start(30*1000);//30s的超时eply*)),&eloop,SLOT(quit()));缩短时间,方便及时退出
    //    eloop.exec(QEventLoop::ExcludeUserInputEvents);
    //    if(timer.isActive()) {
    //        const QtSoapMessage &message = http.getResponse();
    //        if (message.isFault()) {
    //            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
    //        } else {
    //        }
    //        }
}

w_sys_manage_outer_pay_set_fengze::w_sys_manage_outer_pay_set_fengze(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_outer_pay_set_fengze)
{
    ui->setupUi(this);


    ui->lineEdit_website->setText(n_func::f_get_sysparm("fengze_website"));
    ui->lineEdit_mallid->setText(n_func::f_get_sysparm("fengze_mallid"));
    ui->lineEdit_license->setText(n_func::f_get_sysparm("fengze_license"));
    ui->lineEdit_storeCode->setText(n_func::f_get_sysparm("fengze_storeCode"));
    ui->lineEdit_itemCode->setText(n_func::f_get_sysparm("fengze_itemCode"));
    ui->lineEdit_username->setText(n_func::f_get_sysparm("fengze_username"));
    ui->lineEdit_password->setText(n_func::f_get_sysparm("fengze_password"));


    QString path = lds::localdataPath + "/business_data_upload_fengze";
    QFileInfoList fileinfos =QDir(path).entryInfoList(QStringList() << "*_error.txt");
    for(int k = 0; k < fileinfos.count(); k++) {
        ui->comboBox->addItem(fileinfos[k].baseName(), fileinfos[k].filePath());
    }


    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this,SLOT(tocancel()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(lookError()));
}

w_sys_manage_outer_pay_set_fengze::~w_sys_manage_outer_pay_set_fengze()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_fengze::took()
{
    n_func::f_set_sysparm("fengze_website", ui->lineEdit_website->text());
    n_func::f_set_sysparm("fengze_mallid", ui->lineEdit_mallid->text());
    n_func::f_set_sysparm("fengze_license", ui->lineEdit_license->text());
    n_func::f_set_sysparm("fengze_storeCode", ui->lineEdit_storeCode->text());
    n_func::f_set_sysparm("fengze_itemCode", ui->lineEdit_itemCode->text());
    n_func::f_set_sysparm("fengze_username", ui->lineEdit_username->text());
    n_func::f_set_sysparm("fengze_password", ui->lineEdit_password->text());

    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    this->accept();
}

void w_sys_manage_outer_pay_set_fengze::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_pay_set_fengze::lookError()
{
    QFile file(ui->comboBox->itemData(ui->comboBox->currentIndex()).toString());
    file.open(QFile::ReadOnly);

    QDialog *dialog = new QDialog;
    QLabel *label = new QLabel(dialog);
    label->setText(ui->comboBox->currentText() + "\n" +
                  file.readAll());
    label->resize(label->sizeHint());
    label->move(10, 10);
    dialog->resize(label->size() + QSize(20, 10));
    lds_roundeddialog_rect_pop d(dialog, this, Qt::Dialog | Qt::FramelessWindowHint);
    d.updatePos(lds::getParentCenterPopup(&d, dialog));
    d.exec();
}

void w_sys_manage_outer_pay_set_fengze::test()
{
    genrateBill("P1804180100001", 23, 11);
    uploadBill();
}


void w_sys_manage_outer_pay_set_fengze_thread::run()
{
    w_sys_manage_outer_pay_set_fengze_thread_event event;
    execEvent(&event);
}


void w_sys_manage_outer_pay_set_fengze_thread_event::toExec()
{
    w_sys_manage_outer_pay_set_fengze::uploadBill();
}

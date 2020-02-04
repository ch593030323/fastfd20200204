#include "business_data_upload_restful.h"
#include "ui_business_data_upload_restful.h"
#include "n_func.h"
#include "cjson_help.h"
#include "lds.h"
#include <QDir>
#include <QTimer>
#include "lds_network.h"

/*
PUT https://tms.1utama.com.my/POS/POSService.svc/SendReceipts HTTP/1.0
Content-type: text/json
Authorization: Basic TEdLNTowQ1mFEWmpobGtDcz0=
Host: tms.1utama.com.my
Content-Length: 620

[{
    "ReceiptNo": "10008",
    "ReceiptDateAndTime2": "2017-06-11 21:14:14",
    "SubTotal": 100.0,
    "DiscountPercent": 0.0,
    "DiscountAmount": 0.0,
    "GstPercent": 0.0,
    "GstAmount": 0.0,
    "ServiceChargePercent": 0.0,
    "ServiceChargeAmount": 0.0,
    "GrandTotal": 100.0,
    "IsTest": false,
    "IsVoid": true
}, {
    "ReceiptNo": "10009",
    "ReceiptDateAndTime2": "2017-06-11 21:14:14",
    "SubTotal": 56.5,
    "DiscountPercent": 0.0,
    "DiscountAmount": 0.0,
    "GstPercent": 0.0,
    "GstAmount": 0.0,
    "ServiceChargePercent": 0.0,
    "ServiceChargeAmount": 0.0,
    "GrandTotal": 56.5,
    "IsTest": false,
    "IsVoid": true
}]
*/

void business_data_upload_restful::uploadBill()
{
    QString path = lds::localdataPath + "/business_data_upload_restful";
    QFileInfoList fileinfos =QDir(path).entryInfoList(QStringList() << "*.txt");
    QString restful_authcode = n_func::f_get_sysparm("restful_authcode");//TEdLNTowQ1mFEWmpobGtDcz0=
    QNetworkRequest request;
    request.setUrl(QUrl("https://tms.1utama.com.my/POS/POSService.svc/SendReceipts"));
    request.setRawHeader("Content-type", "text/json");
    request.setRawHeader("Authorization", "Basic " + restful_authcode.toLocal8Bit());
    request.setRawHeader("Host", "tms.1utama.com.my");
    QByteArray body;
    QByteArray readall;
    QString errstring;
    int httpcode;

    QString upload_filepath;
    QString upload_filepath_error;
    for(int k = 0; k < fileinfos.count(); k++) {
        QFileInfo info = fileinfos[k];
        if(info.baseName().endsWith("_error"))
            continue;
        //
        upload_filepath = info.filePath();
        upload_filepath_error = info.path() + "/" +info.baseName()+"_error.txt";
        body = lds::fileOpenReadAll(upload_filepath);
        qDebug() << "upload_filepath:" << upload_filepath;
        if(lds_network::managerPut(request, body, readall, errstring, httpcode)) {
            if(200 == httpcode) {
                if(readall == "true") {
                    QFile::remove(upload_filepath);
                    QFile::remove(upload_filepath_error);
                    continue;
                }
            }
        }
        lds::fileWriteByte(upload_filepath_error, (errstring + "\n" + readall).toLocal8Bit());
    }
}

void business_data_upload_restful::genrateBill(const QString &ch_payno)
{
    lds_query_hddpos query;
    query.execSelect(QString(
                         " SELECT  "
                         " a.ch_payno as ReceiptNo,  "
                         " a.dt_operdate as ReceiptDateAndTime2,  "
                         " if(b.ch_tax_mode = 1 or b.ch_tax_mode = 3, num_cost - num_rate, num_cost) as SubTotal, "
                         " a.num_discount as DiscountAmount,  "
                         " a.num_rate as GstAmount,  "
                         " a.num_service as ServiceChargeAmount "
                         " FROM cey_u_checkout_master a, cey_u_taxsale_master b where a.ch_payno = b.ch_payno and a.ch_payno = '%1' ;").arg(ch_payno));
    query.next();
    QString ReceiptNo = query.recordValue("ReceiptNo").toString();
    QString ReceiptDateAndTime2 = query.recordValue("ReceiptDateAndTime2").toDateTime().toString(yyyyMMddhhmmss);
    double SubTotal = query.recordValue("SubTotal").toDouble();
    double DiscountAmount = query.recordValue("DiscountAmount").toDouble();
    double DiscountPercent = (SubTotal == 0 ? 0 : (DiscountAmount / SubTotal));
    double GstAmount = query.recordValue("GstAmount").toDouble();
    double GstPercent = (SubTotal == 0 ? 0 : (GstAmount / SubTotal));
    double ServiceChargeAmount = query.recordValue("ServiceChargeAmount").toDouble();
    double ServiceChargePercent = (SubTotal == 0 ? 0 : (ServiceChargeAmount / SubTotal));
    double GrandTotal = SubTotal - DiscountAmount + GstAmount + ServiceChargeAmount;
    QByteArray byte =
            cJSON_help::mapTocJSONByteArray(ldsVariantMap()
                                            .append("ReceiptNo", ReceiptNo)
                                            .append("ReceiptDateAndTime2", ReceiptDateAndTime2)
                                            .append("SubTotal", lds::doubleRound(SubTotal, 2))
                                            .append("DiscountPercent", lds::doubleRound(DiscountPercent, 2))
                                            .append("DiscountAmount", lds::doubleRound(DiscountAmount, 2))
                                            .append("GstPercent", lds::doubleRound(GstPercent, 2))
                                            .append("GstAmount", lds::doubleRound(GstAmount, 2))
                                            .append("ServiceChargePercent", lds::doubleRound(ServiceChargePercent, 2))
                                            .append("ServiceChargeAmount", lds::doubleRound(ServiceChargeAmount, 2))
                                            .append("GrandTotal", lds::doubleRound(GrandTotal, 2))
                                            .append("IsTest", false)
                                            .append("IsVoid", false)
                                            );
    byte = "[" + byte + "]";
    QDir().mkpath(lds::localdataPath + "/business_data_upload_restful" );
    lds::fileWriteByte(lds::localdataPath + "/business_data_upload_restful/" + ch_payno + ".txt", byte);
}

business_data_upload_restful::business_data_upload_restful(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::business_data_upload_restful)
{
    ui->setupUi(this);

    ui->lineEdit_auth->setText(n_func::f_get_sysparm("restful_authcode"));

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
}

business_data_upload_restful::~business_data_upload_restful()
{
    delete ui;
}

void business_data_upload_restful::took()
{
    n_func::f_set_sysparm("restful_authcode", ui->lineEdit_auth->text());
    this->accept();
}

void business_data_upload_restful::tocancel()
{
    this->reject();
}


void business_data_upload_restful_thread::run()
{
    business_data_upload_restful_thread_event event;
    execEvent(&event);
}


void business_data_upload_restful_thread_event::toExec()
{
    business_data_upload_restful::uploadBill();
}

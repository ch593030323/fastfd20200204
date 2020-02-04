#include "business_interface_malayisa_hourly_pos.h"
#include "ui_business_interface_malayisa_hourly_pos.h"
#include "n_func.h"
#include "lds.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "lds_messagebox.h"

business_interface_malayisa_hourly_pos::business_interface_malayisa_hourly_pos(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::business_interface_malayisa_hourly_pos)
{
    ui->setupUi(this);
    ui->lineEdit_pwd->setText(n_func::f_get_sysparm("api_hourly_pos_pwd"));
    ui->lineEdit_user->setText(n_func::f_get_sysparm("api_hourly_pos_user"));
    ui->lineEdit_service->setText(n_func::f_get_sysparm("api_hourly_pos_service"));
    ui->lineEdit_machine_id->setText(n_func::f_get_sysparm("api_hourly_pos_machine_id"));
    ui->dateTimeEdit->setTime(QTime::fromString(n_func::f_get_sysparm("api_hourly_pos_time"), "hh:mm:ss"));
    ui->label_machine_cur->setText(n_func::f_get_sysparm("api_hourly_pos_machine"));
    setbuttonChecked(ui->pushButton, n_func::f_get_sysparm("api_hourly_pos_machine") == lds::terminalCode);

    //
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(tomachineCheck()));
    connect(ui->pushButton_manual_upload, SIGNAL(clicked()), this, SLOT(tomanual_upload()));
}

business_interface_malayisa_hourly_pos::~business_interface_malayisa_hourly_pos()
{
    delete ui;
}

void business_interface_malayisa_hourly_pos::took()
{
    saveData();
    this->accept();
}

void business_interface_malayisa_hourly_pos::tocancel()
{
    this->reject();
}

void business_interface_malayisa_hourly_pos::tomachineCheck()
{
    setbuttonChecked(ui->pushButton, true);
    ui->label_machine_cur->setText(lds::terminalCode);
}

void business_interface_malayisa_hourly_pos::tomanual_upload()
{
    saveData();
    switch(lds_messagebox::question(this,MESSAGE_TITLE_VOID, QObject::tr("流水上传"), QObject::tr("全部流水上传"), QObject::tr("未上传流水上传"), QObject::tr("取消"))) {
    case 0://全部流水上传
        lds_query_hddpos::updateTable("interface_malayisa_hourly_pos", qrtVariantPairList() << qrtVariantPair("ch_state", "N"), "");
        break;
    case 1://未上传流水上传
        break;
    case 2://取消
        return;
    }
    business_interface_malayisa_hourly_pos_event d;
    lds_messagebox_loading_show loading(this, QObject::tr("正在上传"));
    connect(&d, SIGNAL(signal_state(QString)), &loading, SLOT(setText(QString)));
    loading.show_delay();
    d.toExec();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, loading.text());
}

void business_interface_malayisa_hourly_pos::setbuttonChecked(lds_pushbutton *b, bool check)
{
    b->setData("checked", check);
    if(check)
        b->setIcon(QPixmap(":/image/widgets_pic/check_on.png"));
    else
        b->setIcon(QPixmap(":/image/widgets_pic/check_off.png"));
}

void business_interface_malayisa_hourly_pos::saveData()
{
    n_func::f_set_sysparm("api_hourly_pos_pwd", ui->lineEdit_pwd->text().trimmed());
    n_func::f_set_sysparm("api_hourly_pos_user", ui->lineEdit_user->text().trimmed());
    n_func::f_set_sysparm("api_hourly_pos_service", ui->lineEdit_service->text().trimmed());
    n_func::f_set_sysparm("api_hourly_pos_machine_id", ui->lineEdit_machine_id->text().trimmed());
    n_func::f_set_sysparm("api_hourly_pos_time", ui->dateTimeEdit->text().trimmed());
    if(ui->pushButton->getData("checked").toBool())
        n_func::f_set_sysparm("api_hourly_pos_machine", lds::terminalCode);
}

/*
'api_hourly_pos_machine_id', '12345678'
'api_hourly_pos_pwd', ''
'api_hourly_pos_service', ''
'api_hourly_pos_time', '00:00:00'
'api_hourly_pos_user', ''
*/
QMutex business_interface_malayisa_hourly_pos_event::mutex;
void business_interface_malayisa_hourly_pos_event::toExec()
{
    QMutexLocker lock(&mutex);

    lds_query_hddpos query;
    QString api_hourly_pos_service = lds_query_hddpos::pathValue("cey_sys_parameter/vch_value:vch_parameter='api_hourly_pos_service' ", query.db).toString();
    QString api_hourly_pos_user = lds_query_hddpos::pathValue("cey_sys_parameter/vch_value:vch_parameter='api_hourly_pos_user' ", query.db).toString();
    QString api_hourly_pos_pwd = lds_query_hddpos::pathValue("cey_sys_parameter/vch_value:vch_parameter='api_hourly_pos_pwd' ", query.db).toString();
    QString api_hourly_pos_machine_id = lds_query_hddpos::pathValue("cey_sys_parameter/vch_value:vch_parameter='api_hourly_pos_machine_id' ", query.db).toString();
    QEventLoop eloop;
    QNetworkAccessManager networkAccessManager;
    connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), &eloop, SLOT(quit()));

    if(0 == lds_query_hddpos::selectValue(" select count(0) from interface_malayisa_hourly_pos where ch_state =  'N' ").toInt()) {
        emit signal_state("NO business");
        return;
    }
    query.execSelect(" select * from interface_malayisa_hourly_pos where ch_state =  'N' ");
    while(query.next()) {
        QDate dt_operdate = query.recordValue("dt_operdate").toDate();
        emit signal_state(dt_operdate.toString(yyyyMMdd));
        QString filename = "H" + api_hourly_pos_machine_id + "_" + dt_operdate.toString("yyyyMMdd") + ".txt";
        QUrl url;
        url.setUrl(api_hourly_pos_service + "/" + filename);
        url.setUserName(api_hourly_pos_user);
        url.setPassword(api_hourly_pos_pwd);
        QNetworkRequest request(url);
        QByteArray content = dayContent(query.recordValue("dt_operdate").toDate()).toLocal8Bit();
        NetworkReplyDeleteLater reply = networkAccessManager.put(request, content);
        eloop.exec();
        if(reply.error() != QNetworkReply::NoError) {
            lds_query_hddpos::updateTable("interface_malayisa_hourly_pos", qrtVariantPairList()
                                          << qrtVariantPair("ch_state", "N")
                                          << qrtVariantPair("vch_memo", reply.errorString()), QString("dt_operdate = '%1' ").arg(dt_operdate.toString("yyyyMMdd")));
            emit signal_state(dt_operdate.toString(yyyyMMdd) + ";" + reply.errorString());
            return;
        }
        lds_query_hddpos::updateTable("interface_malayisa_hourly_pos", qrtVariantPairList()
                                      << qrtVariantPair("ch_state", "Y")
                                      << qrtVariantPair("vch_memo", "OK"), QString("dt_operdate = '%1' ").arg(dt_operdate.toString("yyyyMMdd")));
    }
    emit signal_state(QObject::tr("操作成功"));
}

QString business_interface_malayisa_hourly_pos_event::dayContent(const QDate &date)
{
    //折扣= 折扣,赠送,优惠
    //如果有抹零和低消,会出现, 算法不准确的情况
    //SSTRegistered始终等于Y
    QString between_dt = QString(" BETWEEN '%1 00:00:00' AND '%1 23:59:59' ").arg(date.toString(yyyyMMdd));
    QMap<int, PosData> hour_data_map;
    for(int k = 0; k < 24; k ++) {
        hour_data_map[k].init();
    }
    lds_query_hddpos query;
    QSqlDatabase db = query.db;
    query.execSelect(" select ch_payno	, dt_operdate, num_cost	,num_discount	,num_present	,num_service	,num_lowcost	,num_blotout	,num_free	,num_cash	,num_rate "
                     " from cey_u_checkout_master where  ch_state = 'Y'  and dt_operdate " + between_dt);
    while(query.next()) {
        QDateTime dt_operdate = query.recordValue("dt_operdate").toDateTime();
        QString ch_payno = query.recordValue("ch_payno").toString();
        QString ch_paymodeno_path = "cey_u_checkout_detail/num_realamount:ch_payno='%1' and ch_paymodeno = (select ch_paymodeno from cey_bt_paymode where vch_english = '%2') ";
        int ch_tax_mode = lds_query_hddpos::pathValue("cey_u_taxsale_master/ch_tax_mode:ch_payno = '%1' ", ch_payno, db).toInt();
        int ch_service_in = ch_tax_mode / 2;
        int ch_tax_in = ch_tax_mode % 2;
        PosData pdata;
        pdata.init(ch_payno);
        double num_cash = query.recordValue("num_cash").toDouble();
        pdata.SST = query.recordValue("num_rate").toDouble();
        pdata.Service= query.recordValue("num_service").toDouble();
        pdata.Discount = query.recordValue("num_discount").toDouble() + query.recordValue("num_present").toDouble() + query.recordValue("num_free").toDouble();
        pdata.GTO =  query.recordValue("num_cost").toDouble() - pdata.Discount + pdata.Service;
        if(ch_tax_in == 1)  // item cost inclusive of SST
            pdata.GTO = pdata.GTO - pdata.SST;
        pdata.Pax = lds_query_hddpos::pathValue("cey_u_table/int_person:ch_payno='%1' ", ch_payno, db).toInt();
        pdata.Cash = lds_query_hddpos::pathValue(ch_paymodeno_path.arg(ch_payno).arg("Cash"), db).toDouble();
        pdata.TNG = lds_query_hddpos::pathValue(ch_paymodeno_path.arg(ch_payno).arg("TNG"), db).toDouble();
        pdata.Visa = lds_query_hddpos::pathValue(ch_paymodeno_path.arg(ch_payno).arg("Visa"), db).toDouble();
        pdata.MasterCard = lds_query_hddpos::pathValue(ch_paymodeno_path.arg(ch_payno).arg("MasterCard"), db).toDouble();
        pdata.Amex = lds_query_hddpos::pathValue(ch_paymodeno_path.arg(ch_payno).arg("Amex"), db).toDouble();
        pdata.Voucher = lds_query_hddpos::pathValue(ch_paymodeno_path.arg(ch_payno).arg("Voucher"), db).toDouble();
        pdata.Other = num_cash - pdata.Cash - pdata.TNG - pdata.Visa - pdata.MasterCard - pdata.Amex - pdata.Voucher;

        hour_data_map[dt_operdate.time().hour()].add(pdata);
    }

    QString content;
    for(int k = 0; k < 24; k ++) {
        content += hour_data_map[k].lineData(date, k) + "\r\n";
    }
    content.chop(2);
    return content;
}


void business_interface_malayisa_hourly_pos_event::PosData::init()
{
    GTO = 0;
    SST = 0;
    Discount = 0;
    Service = 0;

    Pax = 0;
    Cash = 0;
    TNG = 0;
    Visa = 0;
    MasterCard = 0;

    Amex = 0;
    Voucher = 0;
    Other = 0;
}

void business_interface_malayisa_hourly_pos_event::PosData::init(const QString &ch_payno)
{
    init();
    ch_payno_list << ch_payno;
}

void business_interface_malayisa_hourly_pos_event::PosData::add(const business_interface_malayisa_hourly_pos_event::PosData &d)
{
    GTO += d.GTO;
    SST += d.SST;
    Discount += d.Discount;
    Service += d.Service;

    Pax += d.Pax;
    Cash += d.Cash;
    TNG += d.TNG;
    Visa += d.Visa;
    MasterCard += d.MasterCard;

    Amex += d.Amex;
    Voucher += d.Voucher;
    Other += d.Other;

    ch_payno_list << d.ch_payno_list;
}

QString business_interface_malayisa_hourly_pos_event::PosData::lineData(const QDate &date, int hour)
{
    lds_query_hddpos query;
    QSqlDatabase db = query.db;

    QString MachineID = lds_query_db::pathValue("cey_sys_parameter/vch_value:vch_parameter='api_hourly_pos_machine_id' ", db).toString();
    QString int_batchid = lds_query_db::pathValue("interface_malayisa_hourly_pos/int_batchid:dt_operdate='%1' ", date.toString(yyyyMMdd), db).toString();
    QString SSTRegistered = "Y";
    QString line;
    line += MachineID + "|";
    line += int_batchid + "|";
    line += date.toString("ddMMyyyy") + "|";
    line += QString::number(hour).rightJustified(2, '0') + "|";
    line += QString::number(ch_payno_list.count()) + "|";

    line += QString::number(GTO, 'f', 2) + "|";
    line += QString::number(SST, 'f', 2) + "|";
    line += QString::number(Discount, 'f', 2) + "|";
    line += QString::number(Service, 'f', 2) + "|";
    line += QString::number(Pax) + "|";

    line += QString::number(Cash, 'f', 2) + "|";
    line += QString::number(TNG, 'f', 2) + "|";
    line += QString::number(Visa, 'f', 2) + "|";
    line += QString::number(MasterCard, 'f', 2) + "|";
    line += QString::number(Amex, 'f', 2) + "|";

    line += QString::number(Voucher, 'f', 2) + "|";
    line += QString::number(Other, 'f', 2) + "|";
    line += SSTRegistered + "|";

    line.chop(1);
    return line;
}

void business_interface_malayisa_hourly_pos_thread::run()
{
    business_interface_malayisa_hourly_pos_event event;
    execEvent(&event);
}


business_interface_malayisa_hourly_pos_event::NetworkReplyDeleteLater::NetworkReplyDeleteLater(QNetworkReply *reply)
{
    this->reply = reply;
}

business_interface_malayisa_hourly_pos_event::NetworkReplyDeleteLater::~NetworkReplyDeleteLater()
{
    this->reply->abort();
    this->reply->deleteLater();
}

QNetworkReply::NetworkError business_interface_malayisa_hourly_pos_event::NetworkReplyDeleteLater::error()
{
    return this->reply->error();
}

QString business_interface_malayisa_hourly_pos_event::NetworkReplyDeleteLater::errorString()
{
    return this->reply->errorString();
}

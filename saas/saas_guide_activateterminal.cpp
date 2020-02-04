#include "saas_guide_activateterminal.h"
#include "ui_saas_guide_activateterminal.h"
#include "cjson_help.h"
#include <QtDebug>
#include <QHostInfo>
#include "lds.h"
#include "saas_pay_detail.h"
#include "lds_messagebox.h"
#include "saas_guide.h"
#include "cJSON.h"
#include "lds_network.h"

saas_guide_activateTerminal::saas_guide_activateTerminal(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::saas_guide_activateTerminal)
{
    ui->setupUi(this);

    ui->lineEdit_terminalCode->setText(lds::terminalCode);
    ui->lineEdit_terminalCode->setEnabled(false);

    ui->lineEdit_storeName->setText(QHostInfo::localHostName());
    ui->lineEdit_storeName->setEnabled(false);
}

saas_guide_activateTerminal::~saas_guide_activateTerminal()
{
    delete ui;
}

bool saas_guide_activateTerminal::toTerminalActive()
{
    QString saasType = saas_pay_detail::terminalTypeGet();//AA    FA
    QString saasSystem = saas_pay_detail::terminalSytemGet();
    QByteArray readall;
    QString errstring;
    int httpcode;
    QNetworkRequest request;
    request.setUrl(http_www_yqspos_com +"/cloudTerminal/activateTerminal.terminal");
    request.setHeader(QNetworkRequest::ContentTypeHeader,  "application/json;charset=utf-8");

    if(lds_network::managerPost(
                request,
                cJSON_help::mapTocJSONByteArray(ldsVariantMap()
                                                .append("terminalCode", ui->lineEdit_terminalCode->text())
                                                .append("storeName", ui->lineEdit_storeName->text())
                                                .append("contacts", ui->lineEdit_contacts->text())
                                                .append("telephone", ui->lineEdit_telephone->text())
                                                .append("province", ui->lineEdit_province->text())

                                                .append("city", ui->lineEdit_city->text())
                                                .append("area", ui->lineEdit_area->text())
                                                .append("street", ui->lineEdit_street->text())
                                                .append("address", ui->lineEdit_address->text())
                                                .append("saasType", saasType)//1餐饮\2超市

                                                .append("saasSystem", saasSystem)
                                                ),
                readall,
                errstring,
                httpcode
                )) {
        if(200 == httpcode) {
            httpDataAutoDel httpd = saas_guide::getTerminal();
            if(200 == httpd.httpStatus) {
                lds_messagebox::information(this,
                                            MESSAGE_TITLE_VOID,
                                            QObject::tr("激活成功") +"\n" +
                                            QObject::tr("终端号") + ":" + lds::terminalCode + "\n" +
                                            QObject::tr("截止日期") + ":" +cJSON_help::getcJSONvalue(httpd.json, "dueDate").toString() +"\n"+
                                            "saas"+QObject::tr("版本") + ":" +saas_pay_detail::codeSaaSMapGet().value(cJSON_help::getcJSONvalue(httpd.json, "type").toString()) +"\n"+
                                            QObject::tr("局域网机器个数") + ":" +cJSON_help::getcJSONvalue(httpd.json, "num").toString() +"\n"
                                            );
                return true;
            }
        }
    }
    return false;
}

#include "saas_guide.h"
#include <QStateMachine>
#include <QPropertyAnimation>
#include <QSignalTransition>
#include <QLineEdit>
#include "lds_messagebox.h"
#include "cJSON.h"
#include <QComboBox>
#include "cjson_help.h"
#include <QtDebug>
#include <QPainter>
#include <QStandardItemModel>
#include <QScrollBar>
#include "saas_guide_hard_test.h"
#include "lds.h"
#include "saas_guide_activateterminal.h"
#include "saas_guide_init_soft.h"
#include "saas_pay_detail.h"
#include "lds_network.h"
#include "w_sys_manage_cloudsync_transfer.h"

saas_guide::saas_guide(QWidget *parent) :
    QDialog(parent)
{
    ///赋值
}

saas_guide::~saas_guide()
{
}

httpDataAutoDel saas_guide::getTerminal(int mesc)
{
    httpDataAutoDel httpd;
    QString terminalCode = lds::terminalCode;
    QString retd;
    QMap<QString, QString> headInfo;

    httpd = w_sys_manage_cloudsync_transfer::sendPost2(
//            #ifdef QT_DEBUG
//                "http://192.168.1.81/cloudTerminal/getTerminal.terminal", 
//            #else
                http_www_yqspos_com +"/cloudTerminal/getTerminal.terminal", 
//            #endif
                cJSON_help::mapTocJSONByteArray("terminalCode", terminalCode),
                headInfo,
                retd,
                mesc
                );

    if(200 == httpd.httpStatus) {//正常返回
        //dueDate
        QDateTime dueDate = QDateTime::fromString(cJSON_help::getcJSONvalue(httpd.json, "dueDate").toString(), "yyyy-MM-dd hh:mm:ss");
        if(saas_pay_detail::saas_dueDate() != dueDate) {
            saas_pay_detail::saasSetValue("saas/saas_dueDate", dueDate);
        }

        //saas_online_last_run
        QDateTime curDate = QDateTime::currentDateTime();
        if(saas_pay_detail::saas_online_last_run() != curDate) {
            saas_pay_detail::saasSetValue("saas/saas_online_last_run", curDate);
        }

        //saas_type
        QString saas_type = cJSON_help::getcJSONvalue(httpd.json, "type").toString();
        if(saas_pay_detail::saas_type() != saas_type) {
            saas_pay_detail::saasSetValue("saas/saas_type", saas_type);
        }
        //saas_lan_num
        int saas_lan_num = cJSON_help::getcJSONvalue(httpd.json, "num").toInt();
        if(saas_pay_detail::saas_lan_num() != saas_lan_num) {
            saas_pay_detail::saasSetValue("saas/saas_lan_num", saas_lan_num);
        }
        //saas_qr_code_order_model
//        lds::saas_qr_code_order_model.setValue(cJSON_help::getcJSONvalue(httpd.json, "zfModel") );

    } else if(550 == httpd.httpStatus) {
        QString httpCode = cJSON_help::getcJSONvalue(httpd.json, "code").toString();
        if("-101" == httpCode//终端未激活
                || "-100" == httpCode//终端不存在
                || "-1" == httpCode//终端号异常
                ) {
            saas_pay_detail::saasSetValue("saas/saas_dueDate", "");
            saas_pay_detail::saasSetValue("saas/saas_online_last_run", "");
            saas_pay_detail::saasSetValue("saas/saas_type", "");
        }
    }
    return  httpd;
}

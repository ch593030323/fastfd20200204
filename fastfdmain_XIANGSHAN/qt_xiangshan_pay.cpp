#include "qt_xiangshan_pay.h"
#include "ui_qt_xiangshan_pay.h"
#include "ccb_pay_help.h"
#include "testwx_help.h"
#include "lds.h"
#include <QDateTime>
#include <QTimeLine>
#include "lds_messagebox.h"

qt_XIANGSHAN_pay::qt_XIANGSHAN_pay(const QString &PAYMENT, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::qt_XIANGSHAN_pay)
{
    ui->setupUi(this);
    this->ORDERID = lds::terminalCode  + lds::dt_str(QDateTime::currentDateTime(), "yyyyMMddhhmmss");
    this->PAYMENT = PAYMENT;
    ui->label_qr_url->setScaledContents(true);

    pay_check_count = 30;
}

qt_XIANGSHAN_pay::~qt_XIANGSHAN_pay()
{
    delete ui;
}

bool qt_XIANGSHAN_pay::makeUrl()
{
    QString errstring;
    QByteArray qr_url;
    if(ccb_pay_help::getPayUrl(lds::sysconf->value("system_setting/ccb_MERCHANTID").toString(),
                               lds::sysconf->value("system_setting/ccb_POSID").toString(),
                               lds::sysconf->value("system_setting/ccb_BRANCHID").toString(),
                               lds::sysconf->value("system_setting/ccb_PUB").toString(),
                               PAYMENT,
                               ORDERID,
                               qr_url,
                               errstring)) {
        ui->label_qr_url->setPixmap(testwx_help::get_rbcode(qr_url), ui->label_qr_url->width());

        return true;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring, true);
    return false;
}

void qt_XIANGSHAN_pay::toCheckPaid()
{
    QString errstring;
    if(ccb_pay_help::orderQuery( lds::sysconf->value("system_setting/ccb_MERCHANTID").toString(),
                                 lds::sysconf->value("system_setting/ccb_POSID").toString(),
                                 lds::sysconf->value("system_setting/ccb_BRANCHID").toString(),
                                 lds::sysconf->value("system_setting/ccb_QUPWD").toString(),
                                 ORDERID,
                                 errstring)) {
        this->accept();
    } else {
        ui->label_waiting->setText(QObject::tr("等待") + ":" + QString::number(pay_check_count) + "s");
        pay_check_count --;
        if(pay_check_count < 0) {
            this->reject();
        } else {
            QTimer::singleShot(1000, this, SLOT(toCheckPaid()));
        }
    }
}

#include "saas_guide_pay.h"
#include "ui_saas_guide_pay.h"
#include "w_sys_manage_cloudsync.h"
#include <QtDebug>
#include "saas_pay_detail.h"
#include "saas_guide_num_key.h"
#include "lds.h"
#include "lds_messagebox.h"
#include <QTimer>
#include "w_scr_dish_pay_widget.h"
#include "saas_guide.h"
#include "saas_guide2.h"
#include "saas_guide_pay_rbcode.h"
#include "transparentcenterdialog.h"

static const QString SAAS_UPDATE = "SAAS_UPDATE";
static const QString SAAS_EXTEND = "SAAS_EXTEND";

saas_guide_pay::saas_guide_pay(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::saas_guide_pay)
{
    ui->setupUi(this);
    ui->comboBox->addItem(QObject::tr("1年"), 12);
    ui->comboBox->addItem(QObject::tr("2年"), 24);
    ui->comboBox->addItem(QObject::tr("3年"), 36);

    ui->comboBox->addItem(QObject::tr("1个月"), 1);
    ui->comboBox->addItem(QObject::tr("2个月"), 2);
    ui->comboBox->addItem(QObject::tr("3个月"), 3);
    ui->comboBox->addItem(QObject::tr("4个月"), 4);
    ui->comboBox->addItem(QObject::tr("5个月"), 5);

    ui->comboBox->addItem(QObject::tr("6个月"), 6);
    ui->comboBox->addItem(QObject::tr("7个月"), 7);
    ui->comboBox->addItem(QObject::tr("8个月"), 8);
    ui->comboBox->addItem(QObject::tr("9个月"), 9);
    if(lds::sysconf->value("system_setting/skin_name", "obsidian").toString() == "obsidian") {
        ui->toolButton_2->setIcon(QPixmap(":/image/whiteq1.png"));ui->toolButton_2->setIconSize(QSize(30, 30));
        ui->toolButton_4->setIcon(QPixmap(":/image/whiteq4.png"));ui->toolButton_4->setIconSize(QSize(30, 30));
        ui->toolButton_3->setIcon(QPixmap(":/image/whiteq3.png"));ui->toolButton_3->setIconSize(QSize(30, 30));
        ui->toolButton->setIcon(QPixmap(":/image/whiteq5.png"));ui->toolButton->setIconSize(QSize(30, 30));
    } else {
        ui->toolButton_2->setIcon(QPixmap(":/image/q1.png"));ui->toolButton_2->setIconSize(QSize(30, 30));
        ui->toolButton_4->setIcon(QPixmap(":/image/q4.png"));ui->toolButton_4->setIconSize(QSize(30, 30));
        ui->toolButton_3->setIcon(QPixmap(":/image/q3.png"));ui->toolButton_3->setIconSize(QSize(30, 30));
        ui->toolButton->setIcon(QPixmap(":/image/q5.png"));ui->toolButton->setIconSize(QSize(30, 30));
    }
    ui->lineEdit_terminal_num->setText("1");

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePayNum()));
    QTimer::singleShot(100, this, SLOT(getPurchaseInfo()));
}

saas_guide_pay::~saas_guide_pay()
{
    delete ui;
}

bool saas_guide_pay::pay()
{
    if(payInfo.valid) {
        saas_guide_pay_rbcode dialog(payInfo.totalAmount, this);

#ifdef QT_DEBUG
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, "调试版直接成功");
#else
        if(QDialog::Accepted !=  transparentCenterDialog(&dialog).exec()) {
            return false;
        }
#endif
        //terminalReturn
        httpDataAutoDel httpd = cJSON_CreateObject();
        cJSON_help::_cJSON_AddItemToObject(httpd.json, "trade_no", cJSON_CreateString(dialog.out_trade_no.toLocal8Bit()));
        cJSON_help::_cJSON_AddItemToObject(httpd.json, "extra_common_param", cJSON_CreateString(payInfo.extra_common_param.toLocal8Bit()));
        QByteArray param = httpd.jsonToByteArray();
        QMap<QString, QString> headinfo;
        httpd =  w_sys_manage_cloudsync::get_yun_httppos(this,
                                                                   http_www_yqspos_com +"/cloudTerminal/terminalReturn.terminal",
                                                                   param,
                                                                   headinfo,
                                                                   QObject::tr("提示"),
                                                                   true,
                                                                   false
                                                                   );
        qDebug() << httpd.jsonToString() << httpd.code << httpd.message;
        if(200 == httpd.httpStatus) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return true;
        }
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    return false;
}

void saas_guide_pay::getPurchaseInfo()
{
    getUpgradeService();
    layoutUpdate();
}

void saas_guide_pay::getUpgradeService()
{
    lds_messagebox_loading_show loading(this, "", "WAITING...");
    loading.show_delay();

    httpDataAutoDel httpd = cJSON_CreateObject();
    cJSON_help::_cJSON_AddItemToObject(httpd.json, "terminalCode", cJSON_CreateString(lds::terminalCode.toLocal8Bit()));
    QByteArray param = httpd.jsonToByteArray();
    QMap<QString, QString> headinfo;
    httpd = w_sys_manage_cloudsync::get_yun_httppos(this,
                                                              http_www_yqspos_com +"/cloudTerminal/getUpgradeService.terminal",
                                                              param,
                                                              headinfo,
                                                              QObject::tr("提示"),
                                                              true,
                                                              false
                                                              );
    if(200 == httpd.httpStatus) {
        ui->textEdit_cloudActivityRuleNUMText->setHtml(cJSON_help::getcJSONvalue(httpd.json, "cloudActivityRuleNUMText").toString());
        ui->textEdit_cloudActivityRuleDiscountText->setHtml(cJSON_help::getcJSONvalue(httpd.json, "cloudActivityRuleDiscountText").toString());
        ui->lineEdit_terminal_num->setText("1");
        ui->label_7->setText(QObject::tr("服务截止日期") + ":" + cJSON_help::getcJSONvalue(httpd.json, "dateEnd").toString());
    }
}

void saas_guide_pay::updatePayNum()
{
    //    sn=标准版续期48、标准版升专业版50、专业版续期50
    //续期nums=0
    //loading
    lds_messagebox_loading_show loading(this, "", "WAITING...");
    loading.show_delay();
    //serviceSn
    int serviceSn  = 50;
    if(saas_pay_detail::isSaaSFA20()) {
        if(ui->radioButton_extend->isChecked()) {
            serviceSn = 48;
        }
    }
    //nums
    int nums = ui->lineEdit_terminal_num->text().toInt();
    if(ui->lineEdit_terminal_num->isHidden()) {
        httpDataAutoDel httpd = saas_guide::getTerminal();
        if(200 != httpd.httpStatus) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, httpd.message);
            return;
        }
        nums = saas_pay_detail::saas_lan_num();
    }
    //getSelectServiceSubtotal
    httpDataAutoDel httpd = cJSON_CreateObject();
    cJSON_help::_cJSON_AddItemToObject(httpd.json, "terminalCode", cJSON_CreateString(lds::terminalCode.toLocal8Bit()));
    cJSON_help::_cJSON_AddItemToObject(httpd.json, "serviceSn", cJSON_CreateNumber(serviceSn));
    cJSON_help::_cJSON_AddItemToObject(httpd.json, "nums", cJSON_CreateNumber(nums));
    cJSON_help::_cJSON_AddItemToObject(httpd.json, "months", cJSON_CreateNumber(ui->comboBox->curusrdata().toInt()));
    QByteArray param = httpd.jsonToByteArray();

    QMap<QString, QString> headinfo;
    httpd =  w_sys_manage_cloudsync::get_yun_httppos(this,
                                                               http_www_yqspos_com +"/cloudTerminal/getSelectServiceSubtotal.terminal",
                                                               param,
                                                               headinfo,
                                                               QObject::tr("提示"),
                                                               true,
                                                               false
                                                               );
    payInfo.valid = false;
    if(200 == httpd.httpStatus) {
        payInfo.valid = true;
        payInfo.totalAmount = cJSON_help::getcJSONvalue(httpd.json, "totalAmount").toDouble();
        payInfo.extra_common_param = cJSON_help::getcJSONvalue(httpd.json, "extra_common_param").toString();

        ui->lineEdit_pay->setText(QString().sprintf("%.02f", payInfo.totalAmount));
    }
    return ;
}

void saas_guide_pay::layoutUpdate()
{
    //    if(saas_pay_detail::isSaaSAA())
    //    {//经典版
    //        ui->radioButton_update->setChecked(true);
    //        ui->radioButton_extend->hide();
    //        ui->radioButton_update->move(0, 0); ui->radioButton_update->show();
    //        layoutUpdate(SAAS_UPDATE);
    //    }
    if(saas_pay_detail::isSaaSAA21())
    {//经典SaaS
        ui->radioButton_extend->setChecked(true);
        ui->radioButton_update->hide();
        ui->radioButton_extend->move(0, 0); ui->radioButton_extend->show();
        layoutUpdate(SAAS_EXTEND);
    }
    if(saas_pay_detail::isSaaSFA20())
    {//标准版SaaS
        ui->radioButton_extend->setChecked(true);
        ui->radioButton_extend->move(0, 0); ui->radioButton_extend->show();
        ui->radioButton_update->move(170, 0); ui->radioButton_update->show();
        layoutUpdate(SAAS_UPDATE);
    }
    if(saas_pay_detail::isSaaSFA21())
    {//专业版SaaS
        ui->radioButton_extend->setChecked(true);
        ui->radioButton_update->hide();
        ui->radioButton_extend->move(0, 0); ui->radioButton_extend->show();
        layoutUpdate(SAAS_EXTEND);
    }
}

void saas_guide_pay::layoutUpdate(const QString &saas_version)
{
    ui->toolButton_2->setText(QObject::tr("升级专业版"));

    if(SAAS_EXTEND == saas_version) {//升级
        ui->toolButton_3->move(10, 100);
        ui->label_7->move(140, 100);
        ui->comboBox->move(140, 130);
        ui->textEdit_cloudActivityRuleDiscountText->move(140, 170);

        ui->toolButton_4->hide();
        ui->lineEdit_terminal_num->hide();
        ui->label_13->hide();
        ui->toolButton_num_key->hide();
        ui->textEdit_cloudActivityRuleNUMText->hide();

        ui->toolButton_2->setText(saas_pay_detail::saasDesc());
    }
    if(SAAS_UPDATE == saas_version) {//延期
        ui->toolButton_3->move(10, 210);
        ui->label_7->move(140, 210);
        ui->comboBox->move(140, 240);
        ui->textEdit_cloudActivityRuleDiscountText->move(140, 280);

        ui->toolButton_4->show();
        ui->lineEdit_terminal_num->show();
        ui->label_13->show();
        ui->toolButton_num_key->show();
        ui->textEdit_cloudActivityRuleNUMText->show();

        ui->toolButton_2->setText(saas_pay_detail::saasDesc()+ui->radioButton_update->text());
    }

    updatePayNum();
}

void saas_guide_pay::on_toolButton_num_key_clicked()
{
    saas_guide_num_key d(ui->lineEdit_terminal_num->text());
    d.setWindowFlags(Qt::Popup);
    d.move(lds::getPopPoint(ui->lineEdit_terminal_num, &d));
    if(QDialog::Accepted == d.exec()) {
        if(d.retNum().toInt() >= 1
                && d.retNum().toInt() <= 999) {
            ui->lineEdit_terminal_num->setText(d.retNum());
            updatePayNum();
        } else {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("终端数必须在1到999之间,包含1和999"));
        }
    }
}

void saas_guide_pay::on_radioButton_extend_clicked(bool checked)
{
    if(checked) {
        layoutUpdate(SAAS_EXTEND);
    }
}

void saas_guide_pay::on_radioButton_update_clicked(bool checked)
{
    if(checked) {
        layoutUpdate(SAAS_UPDATE);
    }
}

void saas_guide_pay_update::getPurchaseInfo()
{
    saas_guide_pay::getUpgradeService();

    if(ui->radioButton_extend->isChecked()
            && false == ui->radioButton_update->isHidden()) {
        ui->radioButton_update->click();
    }
}

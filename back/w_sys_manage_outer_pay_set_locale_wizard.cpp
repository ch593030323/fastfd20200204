#include "w_sys_manage_outer_pay_set_locale_wizard.h"
#include "ui_w_sys_manage_outer_pay_set_locale_wizard.h"
#include "ftableview_delegate.h"
#include "ftableview_standmodel_sql_none.h"
#include <QFileInfo>
#include <QDir>
#include "n_func.h"
#include "w_sys_manage_outer_pay_set_tax_type.h"
#include "transparentcenterdialog.h"
#include "backheader.h"
#include "txt_ins_bill_print.h"
#include "public_printby_ch_billno.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "w_bt_dish_kitchenprinter_printerset_port.h"
#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include "printer_normal.h"
#include "lds_check_cey_sys_parameter.h"
#include "w_bt_dish_kitchenprinter_printerset_port_dialog.h"
#include "w_sys_manage_outer_pay_set_locale_wizard_txtedit.h"

bool w_sys_manage_outer_pay_set_locale_wizard::bp_country_print_img(const QString &country)
{
    return (country == "Saudi Arabia"
            || country == "Lebanon"
            || country == "Laos");
}

bool w_sys_manage_outer_pay_set_locale_wizard::bp_country_print_img()
{
    const QString country = n_func::f_get_sysparm("locale_country_name");
    return w_sys_manage_outer_pay_set_locale_wizard::bp_country_print_img(country);
}


w_sys_manage_outer_pay_set_locale_wizard::w_sys_manage_outer_pay_set_locale_wizard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_outer_pay_set_locale_wizard)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("国家和地区"));

    none_model = new ftableview_standmodel_sql_none;
    ui->tableView->setModel(none_model);
    ftableview_delegate *d = new ftableview_delegate(ui->tableView);
    d->keepConnect(ui->tableView);
    d->indexisEmptyDrawGrad = false;
    d->drawTableViewLeftTopBorder = true;
    ui->tableView->setItemDelegate(d);
    ui->tableView->updateFixed_fillData(3, 5);

    ui->pushButton_service_value->setSuffix("%");
    ui->pushButton_tax_1_value->setSuffix("%");
    ui->pushButton_tax_2_value->setSuffix("%");
    ui->pushButton_tax_3_value->setSuffix("%");

    ui->pushButton_tip_1->setText(tr("小费") + "1");
    ui->pushButton_tip_2->setText(tr("小费") + "2");
    ui->pushButton_tip_3->setText(tr("小费") + "3");

    QList<ftableview_standmodel_sql_none::noneData> list_data;
    foreach(const QFileInfo &info, QDir(LOCALE_RECEIPT_DIR).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        list_data << ftableview_standmodel_sql_none::noneData(info.fileName(),
                                                              info.fileName(),
                                                              lds::baseNamePixmap(info.filePath(), info.fileName()));
    }
    none_model->setPrepareList(list_data, QStringList() << n_func::f_get_sysparm("locale_country_name"));
    none_model->toinit();
    reTranslate();

    ui->label_printpath->setText(lds::sysconf->value("system_setting/printerpath").toString());

    lds_check_cey_sys_parameter *check_sysparm = new lds_check_cey_sys_parameter(this);
    //print_receipt_pay_test_data
    connect(ui->pushButton_receipt, SIGNAL(clicked()), this, SLOT(toreceipt_txt_edit()));

    connect(ui->pushButton_tax_1_value, SIGNAL(clicked()), this, SLOT(to_set_tax_1_value()));
    connect(ui->pushButton_tax_2_value, SIGNAL(clicked()), this, SLOT(to_set_tax_2_value()));
    connect(ui->pushButton_tax_3_value, SIGNAL(clicked()), this, SLOT(to_set_tax_3_value()));
    connect(ui->pushButton_service_value, SIGNAL(clicked()), this, SLOT(to_set_service_value()));

    connect(ui->pushButton_tax_type, SIGNAL(clicked()), this, SLOT(totax_type()));
    connect(ui->pushButton_set_print, SIGNAL(clicked()), this, SLOT(to_set_print()));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this, SLOT(to_update_setting(QModelIndex)));

    check_sysparm->buildConnect(ui->checkBox_tax_service_fix, "pay_fixed_service", SIGNAL(clicked()));//固定服务费
    check_sysparm->buildConnect(ui->checkBox_tax_tax_fix, "pay_fixed_tax", SIGNAL(clicked()));//固定税率
    check_sysparm->buildConnect(ui->checkBox_tip_enable, "tip_enable", SIGNAL(clicked()));//固定税率
    check_sysparm->buildConnect(ui->checkBox_tax_price_include_tax, "tax_price_include_tax", SIGNAL(clicked()));//固定税率
    check_sysparm->buildConnect(ui->checkBox_tax_tax_include_service, "tax_tax_include_service", SIGNAL(clicked()));//固定税率
    //暂时隐藏服务费收税,价格收税1 or 0

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->radioButton, SIGNAL(toggled(bool)), this, SLOT(toChooseTab(bool)));
    connect(ui->radioButton_2, SIGNAL(toggled(bool)), this, SLOT(toChooseTab(bool)));
    connect(ui->pushButton_tip_1, SIGNAL(clicked()), this, SLOT(to_save_tip()));
    connect(ui->pushButton_tip_2, SIGNAL(clicked()), this, SLOT(to_save_tip()));
    connect(ui->pushButton_tip_3, SIGNAL(clicked()), this, SLOT(to_save_tip()));
    //    connect(ui->pushButton_receipt_edit, SIGNAL(clicked()), this, SLOT(toreceipt_txt_edit()));



    //#ifdef QT_DEBUG
    //    lds::hideWidget(ui->checkBox_tax_service, true);
    //#endif
    ui->radioButton->setChecked(true);
}


w_sys_manage_outer_pay_set_locale_wizard::~w_sys_manage_outer_pay_set_locale_wizard()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_locale_wizard::took()
{
    if(!none_model->hasSelected()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    this->accept();
}

void w_sys_manage_outer_pay_set_locale_wizard::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_pay_set_locale_wizard::totax_type()
{
    w_sys_manage_outer_pay_set_tax_type dialog(this);
    transparentCenterDialog(&dialog).exec();
    reTranslate();
}

void w_sys_manage_outer_pay_set_locale_wizard::to_receipt()
{
    if(!none_model->hasSelected()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    QString country = none_model->currentSelected(1).value(0).toString();
    QString print_receipt_pay =  ui->pushButton_receipt->getData(Qt::UserRole).toString();
    if(false == QFile::exists(print_receipt_pay)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "NO_FILE_BE_FOUND");
        return;
    }
    to_receipt(country, lds::fileOpenReadAll(print_receipt_pay));
}

void w_sys_manage_outer_pay_set_locale_wizard::to_receipt(const QString &country, const QByteArray &byte)
{
    QString printpath = ui->label_printpath->text();
    QString print_receipt_pay_data =  LOCALE_RECEIPT_DIR + QString("/%1/print_receipt_pay_test_data.txt").arg(country);

    //print_receipt_pay_test_data
    lds_model_sqltablemodel model;
    QFile file(print_receipt_pay_data);
    file.open(QFile::ReadOnly);
    while(!file.atEnd()) {
        QString line = file.readLine();
        model.tablename_kvmap_insert_value("bill_ins_table", line.split("\t").value(0), line.split("\t").value(1));
    }
    file.close();
    model.tablename_kvmap_print("bill_ins_table");

    QSharedPointer<Printer_POS> printer = Printer_POS::get_pos_printer_by_path(printpath);
    if(printer.isNull()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "PRINTER_IS_NULL");
        return;
    }
    if(false == printer->tryOpen(printpath)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, printer->lastError());
        return;
    }
    txt_ins_bill_print d(model.tablename_kvmap_d("bill_ins_table"), byte);//lds::fileOpenReadAll(print_receipt_pay));
    foreach(const pieceIns &p,  d.bill_atfter_reslove()) {
        printer->writeHex(p.first);
        printer->write(p.second, lds::get_soft_curent_language_first());
    }
    printer->writeHex("1B6405 1D5601");//切刀
}

void w_sys_manage_outer_pay_set_locale_wizard::to_set_tax_1_name()
{
    updateTaxName("tax_name1", ui->pushButton_tax_1_value->text());
}

void w_sys_manage_outer_pay_set_locale_wizard::to_set_tax_2_name()
{
    updateTaxName("tax_name2", ui->pushButton_tax_2_value->text());
}

void w_sys_manage_outer_pay_set_locale_wizard::to_set_tax_3_name()
{
    updateTaxName("tax_name3", ui->pushButton_tax_3_value->text());
}

void w_sys_manage_outer_pay_set_locale_wizard::to_set_tax_1_value()
{
    tax_settax(ui->pushButton_tax_1_value, "int_rate","pay_fixed_tax_value");
}

void w_sys_manage_outer_pay_set_locale_wizard::to_set_tax_2_value()
{
    tax_settax(ui->pushButton_tax_2_value, "int_rate_2", "pay_fixed_tax_2_value");
}

void w_sys_manage_outer_pay_set_locale_wizard::to_set_tax_3_value()
{
    tax_settax(ui->pushButton_tax_3_value, "int_rate_3", "pay_fixed_tax_3_value");
}

void w_sys_manage_outer_pay_set_locale_wizard::to_set_service_value()
{
    lds_dialog_input_double inputdialog(this);
    inputdialog.setWindowTitle(ui->pushButton_service_value->text());
    inputdialog.ui->lineEdit->setValue(ui->pushButton_service_value->number());
    inputdialog.ui->lineEdit->setDecimals(2);
    inputdialog.ui->lineEdit->setSuffix("%");

    if(QDialog::Accepted == transparentCenterDialog(&inputdialog).exec()) {
        lds_query_hddpos  query;
        lds_query_hddpos::tran_saction();
        if(n_func::f_set_sysparm_q(query, "pay_fixed_service_value", inputdialog.ui->lineEdit->value() ).length() == 0) {
            if(query.execUpdate("cey_bt_dish", "ch_serviceflag", "Y", "")) {
                if(query.execUpdate("cey_bt_table_type", qrtVariantPairList()
                                    << qrtVariantPair("ch_serviceflag", "N")
                                    << qrtVariantPair("ch_service_mode", 4)
                                    << qrtVariantPair("int_rate", inputdialog.ui->lineEdit->value())
                                    ,   ""
                                    )) {
                    lds_query_hddpos::com_mit();
                    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
                    //
                    ui->pushButton_service_value->setNumber(inputdialog.ui->lineEdit->value());
                    return;
                }
            }
        }
        lds_query_hddpos::roll_back();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    }
}

void w_sys_manage_outer_pay_set_locale_wizard::to_set_print()
{
    w_bt_dish_kitchenprinter_printerset_port_dialog dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.setPrinterport(ui->label_printpath->text());
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        ui->label_printpath->setText(dialog.printerPort());
    }
}

void w_sys_manage_outer_pay_set_locale_wizard::to_update_setting(const QModelIndex &index)
{
    const QString country = index.data(Qt::UserRole + 1).toString();
    n_func::f_set_sysparm("locale_country_name", country);
    if(!lds_query_hddpos::selectHasNext(QString(" select vch_value from cey_sys_parameter where vch_parameter = '%1' ")
                                        .arg(LOCALE_RECEIPT_PAY_SQL_PATH(country)))) {
        n_func::f_set_sysparm(LOCALE_RECEIPT_PAY_SQL_PATH(country), lds::fileOpenReadAll(w_sys_manage_outer_pay_set_locale_wizard_txtedit::country_receipt_pay_path_list(country).value(0)));
    }
    if(country == "Malaysia") {
        lds_query_hddpos::tran_saction();

        n_func::f_set_sysparm("tax_type", "1"); //1：单一税率
        n_func::f_set_sysparm("tax_check1", "1");
        n_func::f_set_sysparm("tax_check2", "0");
        n_func::f_set_sysparm("tax_check3", "0");
        n_func::f_set_sysparm("tax_name1", "Service Tax");
        //        n_func::f_set_sysparm("tax_name2", "SGST");
        //        n_func::f_set_sysparm("tax_name3", "SGST");
        n_func::f_set_sysparm("tax_price_include_tax", "1"); //价格含税
        n_func::f_set_sysparm("tax_tax_include_service", "0"); //税含服务费
        n_func::f_set_sysparm("pay_fixed_service", "0"); //固定服务费
        n_func::f_set_sysparm("pay_fixed_tax", "1"); //固定税率
        n_func::f_set_sysparm("tip_enable", "0");
        lds_query_hddpos::com_mit();

        reTranslate();
    }
    if(country == "India") {
        lds_query_hddpos::tran_saction();

        n_func::f_set_sysparm("tax_type", "3"); //3：累加
        n_func::f_set_sysparm("tax_check1", "1");
        n_func::f_set_sysparm("tax_check2", "1");
        n_func::f_set_sysparm("tax_check3", "0");
        n_func::f_set_sysparm("tax_name1", "CGST");
        n_func::f_set_sysparm("tax_name2", "SGST");
        //        n_func::f_set_sysparm("tax_name3", "SGST");
        n_func::f_set_sysparm("tax_price_include_tax", "0"); //价格含税
        n_func::f_set_sysparm("tax_tax_include_service", "0"); //税含服务费
        n_func::f_set_sysparm("pay_fixed_service", "0"); //固定服务费
        n_func::f_set_sysparm("pay_fixed_tax", "1"); //固定税率
        n_func::f_set_sysparm("tip_enable", "0");

        lds_query_hddpos::com_mit();

        reTranslate();
    }

    if(country == "United States") {
        lds_query_hddpos::tran_saction();

        //        n_func::f_set_sysparm("tax_type", "3"); //3：累加
        //        n_func::f_set_sysparm("tax_check1", "1");
        //        n_func::f_set_sysparm("tax_check2", "1");
        //        n_func::f_set_sysparm("tax_check3", "0");
        //        n_func::f_set_sysparm("tax_name1", "CGST");
        //        n_func::f_set_sysparm("tax_name2", "SGST");
        //        n_func::f_set_sysparm("tax_name3", "SGST");
        //        n_func::f_set_sysparm("tax_price_include_tax", "0"); //价格含税
        //        n_func::f_set_sysparm("tax_tax_include_service", "0"); //税含服务费
        //        n_func::f_set_sysparm("pay_fixed_service", "0"); //固定服务费
        //        n_func::f_set_sysparm("pay_fixed_tax", "1"); //固定税率
        n_func::f_set_sysparm("tip_enable", "1");

        lds_query_hddpos::com_mit();

        reTranslate();
    }
}

void w_sys_manage_outer_pay_set_locale_wizard::toChooseTab(bool checked)
{
    if(checked) {
        if(ui->radioButton->isChecked()) {
            ui->stackedWidget->setCurrentIndex(0);
        }
        if(ui->radioButton_2->isChecked()) {
            ui->stackedWidget->setCurrentIndex(1);
        }
    }
}

void w_sys_manage_outer_pay_set_locale_wizard::to_save_tip()
{
    if(this->sender() == ui->pushButton_tip_1) {
        tip_setvalue(ui->pushButton_tip_1, "tip_int_rate_1");
    }
    if(this->sender() == ui->pushButton_tip_2) {
        tip_setvalue(ui->pushButton_tip_2, "tip_int_rate_2");
    }
    if(this->sender() == ui->pushButton_tip_3) {
        tip_setvalue(ui->pushButton_tip_3, "tip_int_rate_3");
    }
}

void w_sys_manage_outer_pay_set_locale_wizard::toreceipt_txt_edit()
{
    if(!none_model->hasSelected()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    QString errstring;
    QString country = none_model->currentSelected(1).value(0).toString();
    w_sys_manage_outer_pay_set_locale_wizard_txtedit dialog(this);
    connect(&dialog, SIGNAL(signal_receipt_print(QString,QByteArray)), this, SLOT(to_receipt(QString,QByteArray)));
    if(false == dialog.open(country, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_outer_pay_set_locale_wizard::updateTaxName(const QString &sys_parameter_key, const QString &defValue)
{
    lds_dialog_input dialog(this);
    dialog.setWindowTitle(QObject::tr("税率名称"));
    dialog.hideKey();
    //    dialog.ui->label->setText("");
    dialog.ui->lineEdit->setText(defValue);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        QString name = dialog.ui->lineEdit->text().trimmed();
        if(name.isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("税率名称为空"));
            return;
        }
        n_func::f_set_sysparm(sys_parameter_key, name);
        reTranslate();
    }
}

void w_sys_manage_outer_pay_set_locale_wizard::reTranslate()
{
    ui->checkBox_tax_price_include_tax->setChecked(n_func::f_get_sysparm("tax_price_include_tax",0).toInt() == 1);
    ui->checkBox_tax_tax_include_service->setChecked(n_func::f_get_sysparm("tax_tax_include_service",0).toInt() ==1);

    ui->checkBox_tax_service_fix->setChecked(n_func::f_get_sysparm("pay_fixed_service", "0") == "1");
    ui->checkBox_tax_tax_fix->setChecked(n_func::f_get_sysparm("pay_fixed_tax", "0") == "1");
    ui->checkBox_tip_enable->setChecked(n_func::f_get_sysparm("tip_enable", "0") == "1");

    ui->pushButton_service_value->setText(QObject::tr("服务费") );
    ui->pushButton_tax_1_value->setText( n_func::f_get_sysparm("tax_name1", QObject::tr("税率") + "1"));
    ui->pushButton_tax_2_value->setText(n_func::f_get_sysparm("tax_name2", QObject::tr("税率") + "2"));
    ui->pushButton_tax_3_value->setText(n_func::f_get_sysparm("tax_name3", QObject::tr("税率") + "3"));

    ui->pushButton_service_value->setNumber(n_func::f_get_sysparm("pay_fixed_service_value", "0").toDouble());
    ui->pushButton_tax_1_value->setNumber(n_func::f_get_sysparm("pay_fixed_tax_value", "0").toDouble());
    ui->pushButton_tax_2_value->setNumber(n_func::f_get_sysparm("pay_fixed_tax_2_value", "0").toDouble());
    ui->pushButton_tax_3_value->setNumber(n_func::f_get_sysparm("pay_fixed_tax_3_value", "0").toDouble());

    ui->pushButton_tax_type->setNumber(w_sys_manage_outer_pay_set_tax_type::taxDesc());

    ui->pushButton_tip_1->setNumber(n_func::f_get_sysparm("tip_int_rate_1", 0).toDouble());
    ui->pushButton_tip_2->setNumber(n_func::f_get_sysparm("tip_int_rate_2", 0).toDouble());
    ui->pushButton_tip_3->setNumber(n_func::f_get_sysparm("tip_int_rate_3", 0).toDouble());

}


void w_sys_manage_outer_pay_set_locale_wizard::tax_settax(public_pushbuton_number *b, const QString &field_name, const QString &pay_fixed_tax_name)
{
    if(n_func::f_get_sysparm("tax_type").toInt() < 1) {
        lds_messagebox::warning(this,MESSAGE_TITLE_VOID, QObject::tr("税率类型") + ":" + QObject::tr("不启用"));
        return;
    }

    lds_dialog_input_double inputdialog(this);
    inputdialog.setWindowTitle(b->text());
    inputdialog.ui->lineEdit->setValue(b->number());
    inputdialog.ui->lineEdit->setDecimals(2);
    inputdialog.ui->lineEdit->setSuffix("%");

    if(QDialog::Accepted == transparentCenterDialog(&inputdialog).exec()) {
        lds_query_hddpos  query;
        lds_query_hddpos::tran_saction();
        if(n_func::f_set_sysparm_q(query, pay_fixed_tax_name, inputdialog.ui->lineEdit->value() ).length() == 0) {
            if(query.execUpdate("cey_bt_dish", field_name, inputdialog.ui->lineEdit->value(), "")) {
                lds_query_hddpos::com_mit();
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
                //
                b->setNumber(inputdialog.ui->lineEdit->value());
                return;
            }
        }
        lds_query_hddpos::roll_back();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    }
}

void w_sys_manage_outer_pay_set_locale_wizard::tip_setvalue(public_pushbuton_number *b, const QString &tip_sql_name)
{
    lds_dialog_input_double inputdialog(this);
    inputdialog.setWindowTitle(b->text());
    inputdialog.ui->lineEdit->setValue(b->number());
    inputdialog.ui->lineEdit->setDecimals(2);
    inputdialog.ui->lineEdit->setSuffix("%");

    if(QDialog::Accepted == transparentCenterDialog(&inputdialog).exec()) {
        lds_query_hddpos  query;
        lds_query_hddpos::tran_saction();
        if(n_func::f_set_sysparm_q(query, tip_sql_name, inputdialog.ui->lineEdit->value() ).length() == 0) {
            lds_query_hddpos::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
            //
            b->setNumber(inputdialog.ui->lineEdit->value());
            return;
        }
        lds_query_hddpos::roll_back();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    }
}

void w_sys_manage_outer_pay_set_locale_wizard::paintEvent(QPaintEvent *e)
{
    QDialog::paintEvent(e);
    QPainter p(this);
    lds::paintDense7Pattern(&p);
}

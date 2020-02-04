#include "w_sys_manage_outer_pay_set_locale_wizard_txtedit.h"
#include "ui_w_sys_manage_outer_pay_set_locale_wizard_txtedit.h"
#include <QScrollBar>
#include <QTimer>
#include "lds.h"
#include "lds_messagebox.h"
#include <QDir>
#include <QPlainTextEdit>
#include "w_sys_manage_outer_pay_set_locale_wizard.h"
#include "n_func.h"
#include "getfilepath.h"
#include "lds_query_hddpos.h"
#include "lds_dialog_input.h"
#include "ui_lds_dialog_input_dialog.h"
#include "transparentcenterdialog.h"
#include "lds_model_sqltablemodel.h"
#include "printer_pos.h"
#include "txt_ins_bill_print.h"

QStringList w_sys_manage_outer_pay_set_locale_wizard_txtedit::country_receipt_pay_path_list(const QString &country)
{
    QStringList list;
    QString key = "print_receipt_pay_";
    foreach(const QFileInfo &info, QDir(QString() + "userdata/settings/locale" + "/" + country).entryInfoList(QStringList()<< "*.txt")) {
        if(info.baseName().mid(key.length()).toInt() > 0) {
            list << info.filePath();
        }
    }
    return list;
}

w_sys_manage_outer_pay_set_locale_wizard_txtedit::w_sys_manage_outer_pay_set_locale_wizard_txtedit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_outer_pay_set_locale_wizard_txtedit)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("编辑"));
    ui->plainTextEdit->setFont(ldsObjFont<QDialog>(this, lds::FONT_SONGTI_Family));
    lds::hideWidget(ui->scrollArea);

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(took()));
    //    connect(ui->pushButton_init, SIGNAL(clicked()), this, SLOT(toinit()));
    connect(ui->pushButton_print, SIGNAL(clicked()), this, SLOT(toprint()));
    //    connect(ui->comboBox_receipt_width, SIGNAL(currentIndexChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(ui->pushButton_init, SIGNAL(clicked()), this, SLOT(toinit()));
    connect(ui->pushButton_preview, SIGNAL(clicked()), this, SLOT(topreview()));
    connect(ui->pushButton_logo, SIGNAL(clicked()), this, SLOT(tologo()));
}


w_sys_manage_outer_pay_set_locale_wizard_txtedit::~w_sys_manage_outer_pay_set_locale_wizard_txtedit()
{
    delete ui;
}

bool w_sys_manage_outer_pay_set_locale_wizard_txtedit::open(const QString &country, QString &errstring)
{
    this->country = country;
    ui->plainTextEdit->setPlainText(n_func::f_get_sysparm(LOCALE_RECEIPT_PAY_SQL_PATH(country)));

    return true;
}

void w_sys_manage_outer_pay_set_locale_wizard_txtedit::took()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确定保存"), QObject::tr("确定"), QObject::tr("取消"))) {
        return;
    }

    saveData();

    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
    this->accept();
}

void w_sys_manage_outer_pay_set_locale_wizard_txtedit::tocancel()
{
    if(ui->plainTextEdit->document()->isModified()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0://保存
            if(false == saveData())
                return;
            this->accept();
            return;
        case 1://不保存
            this->reject();
            return;
        case 2://取消
            return;
        }
    }
    this->reject();
}

void w_sys_manage_outer_pay_set_locale_wizard_txtedit::toprint()
{
    emit signal_receipt_print(country, ui->plainTextEdit->toPlainText().toLocal8Bit());
}

void w_sys_manage_outer_pay_set_locale_wizard_txtedit::toimport()
{
    QString filepath = getFilePath::getOpenFileName(this, QObject::tr("导入文件"), lds::localdataPath + "/locale_receipt_txt_content");
    if(!filepath.isEmpty()) {
        ui->plainTextEdit->setPlainText(lds::fileOpenReadAll(filepath));
    }
}

void w_sys_manage_outer_pay_set_locale_wizard_txtedit::toexport()
{
    QString filepath = getFilePath::getSaveFileName(this, QObject::tr("导出文件"), lds::localdataPath + "/locale_receipt_txt_content" + "/example.txt");
    if(!filepath.isEmpty()) {
        lds::fileWriteByte(filepath, ui->plainTextEdit->toPlainText().toLocal8Bit());
    }
}

void w_sys_manage_outer_pay_set_locale_wizard_txtedit::toinit()
{
    lds_dialog_input dialog(this);
    dialog.showView(lds_dialog_input::viewShowCom);
    dialog.ui->label_com->setText("");
    foreach(const QString &p, country_receipt_pay_path_list(country)) {
        dialog.ui->comboBox->addItem(country + "/" + QFileInfo(p).baseName(), p);
    }
    if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
        return;
    }
    ui->plainTextEdit->selectAll();
    ui->plainTextEdit->insertPlainText(lds::fileOpenReadAll(dialog.ui->comboBox->curusrdata().toString()));
}

void w_sys_manage_outer_pay_set_locale_wizard_txtedit::topreview()
{
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
    model.tablename_kvmap_insert_value("bill_ins_table", "%gs", lds_query_hddpos::selectValue("SELECT vch_company FROM cey_sys_company;"));
    model.tablename_kvmap_print("bill_ins_table");
    txt_ins_bill_print d(model.tablename_kvmap_d("bill_ins_table"), ui->plainTextEdit->toPlainText().toLocal8Bit());//lds::fileOpenReadAll(print_receipt_pay));
    QPixmap p = QPixmap::fromImage(d.billToImage()).scaledToWidth(ui->plainTextEdit->width() - 20, Qt::SmoothTransformation);
    ui->label->setPixmap(p);
    ui->label->setFixedSize(p.size());
    if(lds::widgetIsShow(ui->plainTextEdit)) {
        lds::showWidget(ui->scrollArea, true);
        lds::showWidget(ui->plainTextEdit, false);
        ui->pushButton_preview->setText(QObject::tr("关闭"));
    } else {
        lds::showWidget(ui->scrollArea, false);
        lds::showWidget(ui->plainTextEdit, true);
        ui->pushButton_preview->setText(QObject::tr("预览"));
    }
}

void w_sys_manage_outer_pay_set_locale_wizard_txtedit::tologo()
{
    QString filepath = getFilePath::getOpenFileName(this, QObject::tr("导入文件"), lds::localdataPath);
    if(filepath.isEmpty())
        return;
    ui->plainTextEdit->insertPlainText(filepath);
}

bool w_sys_manage_outer_pay_set_locale_wizard_txtedit::saveData()
{
    lds_query_hddpos::tran_saction();
    n_func::f_set_sysparm(LOCALE_RECEIPT_PAY_SQL_PATH(country), ui->plainTextEdit->toPlainText());
    lds_query_hddpos::com_mit();
    return true;
}

#include "w_sys_manage_outer_bill_txt_set.h"
#include "ui_w_sys_manage_outer_bill_txt_set.h"
#include "getfilepath.h"
#include "lds.h"
#include "lds_messagebox.h"

w_sys_manage_outer_bill_txt_set::w_sys_manage_outer_bill_txt_set(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_outer_bill_txt_set)
{
    ui->setupUi(this);

    QFont font = this->font();
    font.setFamily(lds::FONT_SONGTI_Family);
    ui->plainTextEdit->setFont(font);
    this->setWindowTitle("");

    //
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_open, SIGNAL(clicked()), this, SLOT(toopen()));
    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(tosave()));

#ifdef QT_DEBUG
    filepath = "/home/cz/文档/build-fastfdmain_new-Desktop-Debug/userdata/settings/print/receipt_txt/pay.txt";
    ui->plainTextEdit->clear();
    ui->plainTextEdit->insertPlainText(lds::fileOpenReadAll(filepath));
    this->filepath = filepath;
#endif
}

w_sys_manage_outer_bill_txt_set::~w_sys_manage_outer_bill_txt_set()
{
    delete ui;
}

void w_sys_manage_outer_bill_txt_set::toexit()
{
    this->reject();
}

void w_sys_manage_outer_bill_txt_set::tosave()
{
    QString errstring;
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认保存"), QObject::tr("确定"), QObject::tr("取消"))) {
        if(false== lds::fileWriteByte(filepath, ui->plainTextEdit->toPlainText().toLocal8Bit(), &errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return;
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        this->accept();
        return;
    }
}

void w_sys_manage_outer_bill_txt_set::toopen()
{
    QString filepath = getFilePath::getOpenFileName(this, MESSAGE_TITLE_VOID, "userdata/settings/print/receipt_txt");
    if(filepath.length() > 0) {
        ui->plainTextEdit->clear();
        ui->plainTextEdit->insertPlainText(lds::fileOpenReadAll(filepath));
        this->filepath = filepath;
    }
}

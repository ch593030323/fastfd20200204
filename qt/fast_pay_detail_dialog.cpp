#include "fast_pay_detail_dialog.h"
#include "frontviewdialog_virtual.h"
#include "ui_fast_pay_detail_dialog.h"

fast_pay_detail_Dialog::fast_pay_detail_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_fast_pay_detail_Dialog)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(accept()));
}

fast_pay_detail_Dialog::~fast_pay_detail_Dialog()
{
    delete ui;
}

void fast_pay_detail_Dialog::setVisible(const QString &ch_tableno)
{
    bool visiabeld =( ch_tableno != "Q000");

    ui->label_18->setVisible(visiabeld);
    ui->label_19->setVisible(visiabeld);
    ui->label_lowcost->setVisible(visiabeld);
    ui->label_servicecost->setVisible(visiabeld);
}

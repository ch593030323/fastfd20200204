#include "w_sys_manage_companyinformation.h"
#include "ui_w_sys_manage_companyinformation_dialog.h"
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>
#include "lds_messagebox.h"
#include "lds_model_sqltablemodel.h"
#include <QHostAddress>
#include <QNetworkInterface>

w_sys_manage_companyinformation::w_sys_manage_companyinformation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_sys_manage_companyinformation_Dialog)
{
    ui->setupUi(this);

    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_sys_company");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->addMapping(ui->lineEdit_vch_address, tablemodel->fieldIndex("vch_address"));
    mapper->addMapping(ui->lineEdit_vch_company, tablemodel->fieldIndex("vch_company"));
    mapper->addMapping(ui->lineEdit_vch_email, tablemodel->fieldIndex("vch_email"));
    mapper->addMapping(ui->lineEdit_vch_handtel, tablemodel->fieldIndex("vch_handtel"));
    mapper->addMapping(ui->lineEdit_vch_principal, tablemodel->fieldIndex("vch_principal"));
    mapper->addMapping(ui->lineEdit_vch_tel, tablemodel->fieldIndex("vch_tel"));
    mapper->addMapping(ui->lineEdit_vch_tax_number, tablemodel->fieldIndex("vch_tax_number"));

    tablemodel->select();
    mapper->setCurrentIndex(0);


    //
    ui->lineEdit_ip->setEnabled(false);
    foreach(const QHostAddress &host, QNetworkInterface::allAddresses()) {
        if(host.toString().contains("192.168")) {
            ui->lineEdit_ip->setText(host.toString());
            break;
        }
    }

    //! toolbar
    ui->widget->setup(QStringList() << QObject::tr("保存")  << QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("保存")), SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

}

bool w_sys_manage_companyinformation::tosave()
{
    if(tablemodel->trySubmitAll()) {
        mapper->setCurrentIndex(0);
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        this->accept();
        return true;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
    return false;
}

void w_sys_manage_companyinformation::toexit()
{
    this->reject();
}

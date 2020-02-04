#include "lds_chartdialog.h"
#include "ui_lds_chartdialog.h"
#include <QAbstractItemModel>
#include <QPushButton>

lds_chartDialog::lds_chartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lds_chartDialog)
{
    ui->setupUi(this);

    //! toolbar
    ui->widget->setup(QStringList() << QObject::tr("切换") << QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("切换")), SIGNAL(clicked()),this,SLOT(toswitch()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
}

lds_chartDialog::~lds_chartDialog()
{
    delete ui;
}

void lds_chartDialog::toswitch()
{
    ui->stackedWidget->setCurrentIndex(1-ui->stackedWidget->currentIndex());
}

void lds_chartDialog::toexit()
{
    this->reject();
}

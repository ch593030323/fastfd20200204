#include "w_sys_manage_sqlcopy_usb_select.h"
#include "ui_w_sys_manage_sqlcopy_usb_select.h"
#include "ftableview_delegate.h"
#include "lds_messagebox.h"
#include "lds.h"
#include<QFileInfo>

w_sys_manage_sqlcopy_usb_select::w_sys_manage_sqlcopy_usb_select(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_sqlcopy_usb_select)
{
    ui->setupUi(this);
    setWindowTitle(QObject::tr("U盘"));

    //model_data
    QList<ftableview_standmodel_sql_none::noneData> model_data;
    foreach(const QString &k, lds::getUsbPathList()) {
        model_data << ftableview_standmodel_sql_none::noneData(QFileInfo(k).baseName(), model_data.count())
                      .append(Qt::UserRole + 2, QPixmap(":/image/q8.png"))
                      .append(Qt::UserRole + 3, k);
    }

    //model
    model = new ftableview_standmodel_sql_none;
    ftableview_delegate *d = new ftableview_delegate(ui->tableView);
    d->keepConnect(ui->tableView);
//    d->setMargin(1);
    d->indexisEmptyDrawGrad = true;

    ui->tableView->setModel(model);
    ui->tableView->setProperty("outer_stylesheet", "tableView_dish");
    ui->tableView->setItemDelegate(d);
    ui->tableView->updateFixed_fillData_addAnimation(3, 3, 150, 80);

    model->setPrepareList(model_data);
    model->toinit();

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
}

w_sys_manage_sqlcopy_usb_select::~w_sys_manage_sqlcopy_usb_select()
{
    delete ui;
}

QVariant w_sys_manage_sqlcopy_usb_select::retData(int role)
{
    if(index.isValid()) {
        return model->data(index, role);
    }
    return QVariant();
}

void w_sys_manage_sqlcopy_usb_select::took()
{
    index = ui->tableView->currentIndex();
    if(false == index.isValid()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    this->accept();
}

void w_sys_manage_sqlcopy_usb_select::tocancel()
{
    this->reject();
}

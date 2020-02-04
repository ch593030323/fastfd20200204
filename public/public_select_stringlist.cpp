#include "public_select_stringlist.h"
#include "ui_public_select_stringlist.h"
#include "ftableview_delegate.h"
#include "lds_messagebox.h"

public_select_stringlist::public_select_stringlist(const QStringList &list, QWidget *parent) :
    QDialog(parent)
{
    init();

    QList<ftableview_standmodel_sql_none::noneData> list_data;
    for(int k = 0; k < list.count(); k++) {
        list_data << ftableview_standmodel_sql_none::noneData(list[k],
                                                              k);
    }
    cook_model->setPrepareList(list_data);
    cook_model->toinit();
}

public_select_stringlist::public_select_stringlist(const QList<QPair<QString, QString> > &pairlist, QWidget *parent) :
    QDialog(parent)
{
    init();

    QList<ftableview_standmodel_sql_none::noneData> list_data;
    for(int k = 0; k < pairlist.count(); k++) {
        list_data << ftableview_standmodel_sql_none::noneData(pairlist[k].second,
                                                              pairlist[k].first);
    }
    cook_model->setPrepareList(list_data);
    cook_model->toinit();
}

public_select_stringlist::public_select_stringlist(const QList<ftableview_standmodel_sql_none::noneData> &list_data, QWidget *parent) :
    QDialog(parent)
{
    init();
    cook_model->setPrepareList(list_data);
    cook_model->toinit();
}

public_select_stringlist::~public_select_stringlist()
{
    delete ui;
}

QVariant public_select_stringlist::curSelect(int role)
{
    QModelIndex index = ui->tableView->currentIndex();
    if(index.isValid()) {
        return index.data(role);
    }
    return QVariant();
}

void public_select_stringlist::setContentSize(int rowcount, int columncount)
{
    ui->tableView->updateFixed_fillData(columncount, rowcount);
    cook_model->toinit();
}

void public_select_stringlist::took()
{
    if(cook_model->hasSelected()) {
        this->accept();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
    }
}

void public_select_stringlist::init()
{
    ui = new Ui::public_select_stringlist;
    ui->setupUi(this);

    cook_model = new ftableview_standmodel_sql_none;
    ftableview_delegate *d = new ftableview_delegate(ui->tableView);
    d->keepConnect(ui->tableView);
    d->b_shwoGrid = true;

    ui->tableView->setModel(cook_model);
    ui->tableView->setProperty("outer_stylesheet", "tableView_type");
    ui->tableView->setItemDelegate(d);
    ui->tableView->updateFixed_fillData(4, 3);

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));

    this->setWindowTitle(tr("选择"));
}

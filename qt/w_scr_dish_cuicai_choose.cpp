#include "w_scr_dish_cuicai_choose.h"
#include "lds_query_hddpos.h"
#include <QtDebug>
#include "lds_messagebox.h"
#include "public_sql.h"
#include "ftableview_delegate.h"
#include "ui_w_scr_dish_special_cook.h"

w_scr_dish_cuicai_choose::w_scr_dish_cuicai_choose(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_scr_dish_special_cook)
{
    ui->setupUi(this);
    cook_model = new ftableview_standmodel_sql_none;
    ftableview_delegate *d = new ftableview_delegate(ui->frame_cook);
    d->keepConnect(ui->frame_cook);
    d->b_shwoGrid = true;


    ui->frame_cook->setModel(cook_model);
    ui->frame_cook->setProperty("outer_stylesheet", "tableView_type");
    ui->frame_cook->setItemDelegate(d);
    ui->frame_cook->updateFixed_fillData(5, 3, 96, 80);
    ui->frame_cook->setSelectionMode(QTableView::MultiSelection);

    ui->widget->setup(QStringList() << QObject::tr("确定") << QObject::tr("取消"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("确定")),SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(QObject::tr("取消")),SIGNAL(clicked()),this,SLOT(toexit()));

    ui->label_state->clear();
}

w_scr_dish_cuicai_choose::~w_scr_dish_cuicai_choose()
{
    delete ui;
}

void w_scr_dish_cuicai_choose::reloaddata(const QMap<QString, QString> &map)
{
    QList<ftableview_standmodel_sql_none::noneData> list;
    for(QMap<QString, QString>::const_iterator k = map.begin(); k != map.end(); k++) {
        list << ftableview_standmodel_sql_none::noneData(k.value(), k.key());
    }
    cook_model->setPrepareList(list);
    cook_model->toinit();
}

QStringList w_scr_dish_cuicai_choose::hasSelected()
{
    QStringList rets;
    foreach(const QVariant &d, cook_model->currentSelected(1)) {
        rets << d.toString();
    }
    return rets;
}

void w_scr_dish_cuicai_choose::took()
{
    if(!cook_model->hasSelected()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有类别被选中"));
        return;
    }
    this->accept();
}

void w_scr_dish_cuicai_choose::toexit()
{
    this->reject();
}

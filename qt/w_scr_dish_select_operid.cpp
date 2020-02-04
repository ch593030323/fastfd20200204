#include "w_scr_dish_select_operid.h"
#include "lds_query_hddpos.h"
#include <QtDebug>
#include "lds_messagebox.h"
#include "public_sql.h"
#include "backheader.h"
#include "ftableview_delegate.h"
#include "ui_w_scr_dish_special_cook.h"

w_scr_dish_select_operid::w_scr_dish_select_operid(const QString &curOperId, QWidget *parent) :
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

    QList<ftableview_standmodel_sql_none::noneData> list;
    lds_query_hddpos  query;
    query.execSelect("select vch_operID, concat(vch_operator,'\n',vch_operID) vch_operator from ("+backheader::SELECT_OPERID_NAME_FLAG+")t");
    while(query.next()) {
        list << ftableview_standmodel_sql_none::noneData(
                    query.recordValue("vch_operator").toString(),
                    query.recordValue("vch_operID").toString()
                    );
    }
    cook_model->setPrepareList(list, QStringList() << curOperId);
    cook_model->toinit();

    ui->widget->setup(QStringList() << QObject::tr("确定") << QObject::tr("取消"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("确定")),SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->widget->index_widget(QObject::tr("取消")),SIGNAL(clicked()),this,SLOT(toexit()));

    lds::hideWidget(ui->label_state);
}

w_scr_dish_select_operid::~w_scr_dish_select_operid()
{
    delete ui;
}

QStringList w_scr_dish_select_operid::hasSelected()
{
    return cook_model->currentSelected_usr_1();
}

QString w_scr_dish_select_operid::selectOperid()
{
    return hasSelected().value(0);
}

void w_scr_dish_select_operid::took()
{
    if(!cook_model->hasSelected()) {
        switch(lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"), QObject::tr("继续退出"), QObject::tr("返回"))) {
        case 0:
            break;
        case 1:
            return;
        }
    }
    this->accept();
}

void w_scr_dish_select_operid::toexit()
{
    this->reject();
}

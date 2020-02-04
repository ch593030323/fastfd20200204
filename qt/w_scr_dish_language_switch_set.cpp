#include "w_scr_dish_language_switch_set.h"
#include "ui_w_scr_dish_language_switch_set.h"
#include "ftableview_standmodel_sql_none.h"
#include "ftableview_delegate.h"
#include "w_sys_manage_outer_pay_set.h"
#include "lds_messagebox.h"
#include "lds.h"
#include "public_sql.h"
#include "w_sys_manage_sqlcopy_soft_limit2_for_language.h"

w_scr_dish_language_switch_set::w_scr_dish_language_switch_set(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_language_switch_set)
{
    ui->setupUi(this);

    QList<ftableview_standmodel_sql_none::noneData> list_data;

    const QList<QPair<QString, QString> >& list = w_sys_manage_outer_pay_set::getLanguagelist();
    for(int k = 0; k < list.count();k ++) {
        list_data << ftableview_standmodel_sql_none::noneData(
                         list[k].second + QString("\n(%1)").arg(list[k].first),
                         list[k].first
                         );
    }

    ftableview_standmodel_sql_none *model = new ftableview_standmodel_sql_none;
    model->setPrepareList(list_data);
    ftableview_delegate *d = new ftableview_delegate(ui->tableView);
    d->keepConnect(ui->tableView);
    d->indexisEmptyDrawGrad = true;

    ui->tableView->setModel(model);
    ui->tableView->setProperty("outer_stylesheet", "tableView_type");
    ui->tableView->setItemDelegate(d);
    ui->tableView->updateFixed_fillData_addAnimation(6, 6);

    model->toinit();

    ui->pushButton_language->enabledSwitchKey(false);
    ui->pushButton_language->initKeys_conf();
    ui->label_primary->setText(w_sys_manage_outer_pay_set::getLanguageValue(ui->pushButton_language->firstKey()));
    ui->label_second->setText(w_sys_manage_outer_pay_set::getLanguageValue(ui->pushButton_language->secondKey()));

    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_language_clear, SIGNAL(clicked()),this,SLOT(to_language_clear()));
    connect(ui->lineEdit, SIGNAL(textChanged(QString)),this,SLOT(to_lookup()));
    connect(ui->tableView, SIGNAL(signalclick(QString)), this, SLOT(to_language_set(QString)));

    lds::hideWidget(ui->pushButton_language_clear);
}

w_scr_dish_language_switch_set::~w_scr_dish_language_switch_set()
{
    delete ui;
}

void w_scr_dish_language_switch_set::took()
{
    if(false == w_sys_manage_sqlcopy_soft_limit2_for_language::currentLanuageIsFree(ui->pushButton_language->firstKey(), ui->pushButton_language->secondKey())) {
        if(false == w_sys_manage_sqlcopy_soft_limit2_for_language::checkLimit(ui->pushButton_language->firstKey(), ui->pushButton_language->secondKey(), this)) {
            return;
        }
    }
    lds::set_soft_curent_language(ui->pushButton_language->firstKey());
    lds::set_soft_curent_language_second(ui->pushButton_language->secondKey());
    this->accept();
}

void w_scr_dish_language_switch_set::tocancel()
{
    this->reject();
}

void w_scr_dish_language_switch_set::to_language_clear()
{
    ui->pushButton_language->initKeys("", "");
    ui->pushButton_language->update();
}

void w_scr_dish_language_switch_set::to_language_set(const QModelIndex &index)
{
    QString key = index.data(Qt::UserRole + 1).toString();
    to_language_set(key);
}

void w_scr_dish_language_switch_set::to_language_set(const QString &key)
{
    //    QString key = index.data(Qt::UserRole + 1).toString();
    if(key == "-1") return;
    if(key == "-2") return;

    if(ui->pushButton_language->firstKey().isEmpty()) {
        if(ui->pushButton_language->secondKey() == key) return;

        ui->pushButton_language->init(key, ui->pushButton_language->secondKey());
        goto update;
    }
    if(ui->pushButton_language->secondKey().isEmpty()) {
        if(ui->pushButton_language->firstKey() == key) return;

        ui->pushButton_language->init(ui->pushButton_language->firstKey(), key);
        goto update;
    }

    ui->pushButton_language->init(key, "");

update:
    ui->label_primary->setText(w_sys_manage_outer_pay_set::getLanguageValue(ui->pushButton_language->firstKey()));
    ui->label_second->setText(w_sys_manage_outer_pay_set::getLanguageValue(ui->pushButton_language->secondKey()));
    ui->pushButton_language->update();
}

void w_scr_dish_language_switch_set::to_lookup()
{
    QString key = w_sys_manage_outer_pay_set::getLanguageKeyLike(ui->lineEdit->text().trimmed());
    ftableview_standmodel_sql_none *model = qobject_cast<ftableview_standmodel_sql_none *>(ui->tableView->model());
    model->clearSelected();
    model->select(key);
    model->initDelay();
}

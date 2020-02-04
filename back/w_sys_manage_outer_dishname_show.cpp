#include "w_sys_manage_outer_dishname_show.h"
#include "ui_w_sys_manage_outer_dishname_show.h"
#include "lds_query_hddpos.h"
#include <QtDebug>
#include "lds.h"
#include "n_func.h"
#include "lds_model_sqltablemodel.h"
#include "lds_checkbox.h"
#include "lds_messagebox.h"

w_sys_manage_outer_dishname_show::w_sys_manage_outer_dishname_show(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_outer_dishname_show)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("商品名称"));
    lds_query_hddpos query;
    ui->tableWidget->setRowCount(2 + lds_query_hddpos::selectValue("select count(0) from cey_bt_kitchen_plan").toInt());
    ui->tableWidget->appHorizontalHeader("name", QObject::tr("名称"));
    ui->tableWidget->appHorizontalHeader("combobox", QObject::tr("选择"));
    ui->tableWidget->appHorizontalHeader("path", "");
    ui->tableWidget->appHorizontalHeader("type", "");
    ui->tableWidget->hideColumn(ui->tableWidget->fieldIndex("path"));
    ui->tableWidget->hideColumn(ui->tableWidget->fieldIndex("type"));
    ui->tableWidget->setColumnWidth(ui->tableWidget->fieldIndex("name"), 200);
    ui->tableWidget->setColumnWidth(ui->tableWidget->fieldIndex("combobox"), 200);

    query.execSelect("select vch_plan_id, vch_plan_name from cey_bt_kitchen_plan");
    for(int row = 0; row < ui->tableWidget->rowCount(); row ++) {
        switch(row) {
        case 0:
            ui->tableWidget->setText(row, 0, tr("收银操作"));
            ui->tableWidget->setText(row, 1, "");
            ui->tableWidget->setText(row, 2, "w_scr_dish_main/dishnameswitch");
            ui->tableWidget->setText(row, 3, "cashier");
            break;
        case 1:
            ui->tableWidget->setText(row, 0, tr("结账打印机"));
            ui->tableWidget->setText(row, 1, "");
            ui->tableWidget->setText(row, 2, "bp_vch_dishname_No");
            ui->tableWidget->setText(row, 3, "pay");
            break;
        default:
            query.next();
            ui->tableWidget->setText(row, 0, QObject::tr("厨打") + ":" + query.recordValue("vch_plan_name").toString());
            ui->tableWidget->setText(row, 1, "");
            ui->tableWidget->setText(row, 2, query.recordValue("vch_plan_id").toString());
            ui->tableWidget->setText(row, 3, "kitchen");
            break;
        }
        //passing the ownership of the widget to the viewport.
        NCombobox *com = new NCombobox;
        ui->tableWidget->setIndexWidget(ui->tableWidget->index(row, 1), com);
    }

    //
    savedata(false);
    //
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
}

w_sys_manage_outer_dishname_show::~w_sys_manage_outer_dishname_show()
{
    delete ui;
}

void w_sys_manage_outer_dishname_show::took()
{
    if(savedata())
        this->accept();
}

void w_sys_manage_outer_dishname_show::tocancel()
{
    this->reject();
}

bool w_sys_manage_outer_dishname_show::savedata(bool save)
{
    for(int row = 0; row < ui->tableWidget->rowCount(); row ++) {
        QString path = ui->tableWidget->text(row, "path");
        QString type = ui->tableWidget->text(row, "type");
        NCombobox *combobox = static_cast<NCombobox *>(ui->tableWidget->indexWidget(ui->tableWidget->index(row, "combobox")));
        QString value = combobox->value();
        if(save && value.count() == 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，名称不能为空").arg(row + 1));
            return false;
        }

        if("cashier" == type) {
            if(save)
                lds::sysconf->setValue(path, value);
            else
                combobox->setValue(lds::sysconf->value(path).toString());
        }
        if("pay" == type) {
            if(save)
                n_func::f_set_sysparm(path, value);
            else
                combobox->setValue(n_func::f_get_sysparm(path));
        }
        if("kitchen" == type) {
            if(save)
                lds_query_hddpos::updateTable("cey_bt_kitchen_plan", qrtVariantPairList() << qrtVariantPair("vch_dishname_No", value), QString(" vch_plan_id =  '%3'; ").arg(path));
            else
                combobox->setValue(lds_query_hddpos::selectValue(QString(" select vch_dishname_No from cey_bt_kitchen_plan where vch_plan_id =  '%3'; ").arg(path)).toString());
        }
    }
    return true;
}


w_sys_manage_outer_dishname_show::NCombobox::NCombobox(QWidget *parent) : QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    b1 = new lds_CheckBox(this);
    b1->setText(QObject::tr("名称") + "1");
    b1->setCheckable(true);
    b1->setStyleSheet("QCheckBox{color:black;}");
    layout->addWidget(b1);
    b2 = new lds_CheckBox(this);
    b2->setText(QObject::tr("名称") + "2");
    b2->setCheckable(true);
    b2->setStyleSheet("QCheckBox{color:black;}");
    layout->addWidget(b2);
}

void w_sys_manage_outer_dishname_show::NCombobox::setValue(const QString &value)
{
    QStringList list = value.split(",");
    b1->setChecked(list.contains("vch_dishname"));
    b2->setChecked(list.contains("vch_dishname_2"));
}

QString w_sys_manage_outer_dishname_show::NCombobox::value()
{
    QString r;
    if(b1->isChecked())
        r += ",vch_dishname";
    if(b2->isChecked())
        r += ",vch_dishname_2";
    return r.mid(1);
}

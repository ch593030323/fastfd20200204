#include "w_sys_manage_operatorright_dish_type_right_new_type.h"
#include "ui_w_bt_dish_unitset_dialog.h"

w_sys_manage_operatorright_dish_type_right_new_type::w_sys_manage_operatorright_dish_type_right_new_type(const QString &vch_operid, QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui_w_bt_dish_unitset_Dialog;
    ui->setupUi(this);

    querymodel = new report_querymodel(this);
    querymodel->setQuery(QString(" select ch_dish_typeno, vch_dish_typename from cey_bt_dish_type where ch_dish_typeno not in (select ch_dish_typeno from cey_bt_dish_type_oper where vch_operid = '%1'); ")
                         .arg(vch_operid));
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(querymodel);
    ui->tableView->setSelectionMode(QTableView::MultiSelection);


    ui->widget->setup(QStringList()  << QObject::tr("全选") << QObject::tr("全不选") << QObject::tr("确定") << QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("全选")), SIGNAL(clicked()), ui->tableView, SLOT(toSelectAll()));
    connect(ui->widget->index_widget(QObject::tr("全不选")), SIGNAL(clicked()), ui->tableView, SLOT(toClearAll()));
    connect(ui->widget->index_widget(QObject::tr("确定")), SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()), this, SLOT(reject()));
}

w_sys_manage_operatorright_dish_type_right_new_type::~w_sys_manage_operatorright_dish_type_right_new_type()
{
    delete ui;
}

QStringList w_sys_manage_operatorright_dish_type_right_new_type::get_dish_type_list()
{
    QStringList rets;
    QList<int> rowlist = ui->tableView->getSelectedRowListAsc();
    foreach(int row, rowlist) {
        rets << querymodel->model_data(row, "ch_dish_typeno").toString();
    }
    return rets;
}

#include "w_sys_manage_operatorright_dish_type_right.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_sys_manage_operatorright_dish_type_right_new_type.h"

w_sys_manage_operatorright_dish_type_right::w_sys_manage_operatorright_dish_type_right(const QString &vch_operid, QWidget *parent)
    : QDialog(parent)
{
    combobox = new lds_ComboBox(this);
    pushbutton_new = new lds_toolbar_button("pushbutton", this);
    pushbutton_del = new lds_toolbar_button("pushbutton", this);
    lds_toolbar_button *pushbutton_ok = new lds_toolbar_button("pushbutton", this);
    tableview = new lds_tableView(this);
    label_desc = new QLabel(this);
    QHBoxLayout *hlayout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    hlayout->addStretch(1);
    hlayout->addWidget(new QLabel(QObject::tr("操作员"), this));
    hlayout->addWidget(combobox);
    hlayout->addWidget(pushbutton_new);
    hlayout->addWidget(pushbutton_del);
    hlayout->addWidget(pushbutton_ok);
    vlayout->addWidget(tableview, 1);
    vlayout->addWidget(label_desc);
    vlayout->addLayout(hlayout);
    this->setLayout(vlayout);

    pushbutton_new->setText(QObject::tr("新增"));
    pushbutton_ok->setText(QObject::tr("退出"));
    pushbutton_del->setText(QObject::tr("删除"));
    combobox->addItem(QObject::tr("全部"), "%");
    combobox->addItemsBySql(" Select vch_operID, vch_operator from cey_sys_operator ");
    combobox->setMinimumWidth(150);

    standmodel = new QStandardItemModel(this);
    standmodel->setHorizontalHeaderLabels(QStringList() << QObject::tr("操作员") << QObject::tr("大类编号") << QObject::tr("大类")/*<< QObject::tr("权限")*/);
    tableview->setModel(standmodel);
    tableview->setEditTriggers(QTableView::NoEditTriggers);
    tableview->setItemDelegateForColumn(0,  new lds_model_sqltablemodel_delegate_com(this,combobox));

    connect(pushbutton_new, SIGNAL(clicked()), this, SLOT(tonew()));
    connect(pushbutton_del, SIGNAL(clicked()), this, SLOT(todel()));
    connect(pushbutton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(refresh()));

    setWindowTitle(QObject::tr("商品权限"));
    combobox->setcomusrdata(vch_operid);
    refresh();
    resize(600, 400);
}

w_sys_manage_operatorright_dish_type_right::~w_sys_manage_operatorright_dish_type_right()
{
}

void w_sys_manage_operatorright_dish_type_right::tonew()
{
    QString vch_operid = combobox->curusrdata().toString();

    w_sys_manage_operatorright_dish_type_right_new_type dialog(vch_operid, this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        lds_query_hddpos  query;
        foreach(const QString &dish_type, dialog.get_dish_type_list()) {
            query.execInsert("cey_bt_dish_type_oper",
                             qrtVariantPairList()
                             << qrtVariantPair("vch_operid", vch_operid)
                             << qrtVariantPair("ch_dish_typeno", dish_type)
                             );
        }
        refresh();
    }
}

void w_sys_manage_operatorright_dish_type_right::todel()
{

    int row = tableview->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    QString vch_operid = standmodel->item(row, 0)->text();
    lds_query_hddpos  query;
    query.execDelete("cey_bt_dish_type_oper", QString("vch_operid = '%1' and ch_dish_typeno = '%2'")
                     .arg(vch_operid)
                     .arg(standmodel->item(row, 1)->text()));
    refresh();
}

void w_sys_manage_operatorright_dish_type_right::took()
{
    this->accept();
}

void w_sys_manage_operatorright_dish_type_right::tocancel()
{
    this->reject();
}

void w_sys_manage_operatorright_dish_type_right::refresh()
{
    QString vch_operid = combobox->curusrdata().toString();
    pushbutton_new->setEnabled(vch_operid != "%");

    standmodel->removeRows(0, standmodel->rowCount());

    QSet<QString> vch_operid_set;
    QSet<QString> ch_dish_typeno_set;
    lds_query_hddpos  query;
    //0000登录 or 该操作员没有设置全下 or 权限不允许
    query.execSelect(QString(" select *, (select a.vch_dish_typename from cey_bt_dish_type a where a.ch_dish_typeno = b.ch_dish_typeno) as vch_dish_typename  from cey_bt_dish_type_oper b where b.vch_operid like '%1' order by vch_operid asc ; ")
                     .arg(combobox->curusrdata().toString()));
    while(query.next()) {
        standmodel->appendRow(QList<QStandardItem*>()
                              << new QStandardItem(query.recordValue("vch_operid").toString())
                              << new QStandardItem(query.recordValue("ch_dish_typeno").toString())
                              << new QStandardItem(query.recordValue("vch_dish_typename").toString())
                              //                              << new QStandardItem(query.recordValue("ch_state").toString())
                              );
        vch_operid_set.insert(query.recordValue("vch_operid").toString());
        ch_dish_typeno_set.insert(query.recordValue("ch_dish_typeno").toString());
    }

    label_desc->setText(QObject::tr("操作员") + ":" + QString::number(vch_operid_set.count()) + "," +
                        QObject::tr("大类") + ":" + QString::number(ch_dish_typeno_set.count()));
    tableview->restore(0);
}

#include "w_i_inventory_query.h"
#include "ui_w_i_inventory_query.h"
#include "lds.h"
#include <QTimer>
#include "n_func.h"

QStringList w_i_inventory_query::execSelectList(const QString &sql)
{
    QStringList r;
    lds_query_hddpos query;
    query.execSelect(sql);
    while(query.next()) {
        r << query.recordValue(0).toString();
    }
    return r;
}

w_i_inventory_query::w_i_inventory_query(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_i_inventory_query)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    querymodel = new report_querymodel(this);
    ui->comboBox_report_type->addItem(QObject::tr("库存查询"), "inventory_look");
    ui->comboBox_report_type->addItem(QObject::tr("库存补仓提醒报表"), "inventory_warn");
    ui->tableView->setModel(querymodel);
    ui->tableView->setTransferHheader();
    //
    lds_query_hddpos query;
    QMenu *m = new QMenu(this);
    m->addAction(QObject::tr("全部"));
    query.execSelect(" select ch_dish_typeno from cey_bt_dish_type ");
    while(query.next()) {
        m->addAction(dish_typeno_menu(m, query.recordValue("ch_dish_typeno").toString()));
    }
    ui->pushButton_dish_type->setMenu(m);
    m->setStyleSheet(
                "QMenu::item {min-height:30px;min-width:100px;padding: 2px 25px 2px 20px;border: 1px solid transparent;}"
                "QMenu::item:selected {background: #ff546b;}"
                );
    ui->pushButton_dish_type->setText(QObject::tr("全部"));
    //
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_export, SIGNAL(clicked()), this, SLOT(toexport()));
    connect(ui->pushButton_print, SIGNAL(clicked()), this, SLOT(toprint()));
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(toselect()));
    connect(ui->comboBox_report_type, SIGNAL(currentIndexChanged(int)), this, SLOT(toselect()));
    connect(ui->pushButton_dish_type->menu(), SIGNAL(triggered(QAction*)), this, SLOT(toselect(QAction*)));

    QTimer::singleShot(10, this, SLOT(toselect()));
}

w_i_inventory_query::~w_i_inventory_query()
{
    delete ui;
}

void w_i_inventory_query::toselect()
{
    QString dish_type_select = ui->pushButton_dish_type->getData("dish_type_select").toString();
    QString dish_type = dish_type_select.split("/").value(0, "%");
    QString sub_type = dish_type_select.split("/").value(1, "%");

    bool inventory_warn = ui->comboBox_report_type->curusrdata().toString() == "inventory_warn";
    //数据库sql
    QString from_sql
            =  " FROM cey_bt_dish a"

            " LEFT OUTER JOIN cey_st_material_num b"

            " ON a.ch_dishno = b.ch_materialno"

            " where IFNULL(a.item_flag, 'N') = 'Y'"

            " and a.ch_dish_typeno like '%1%' "

            " and a.ch_sub_typeNo like '%2%' "

            " and (ch_dishno like '%3%' or vch_spell like '%3%' or vch_dishname like '%3%' or vch_barcode like '%3%' ) "

            +QString(inventory_warn ? " and  b.num_num <= a.num_item_warn": "")+

            " ORDER BY num_store desc";
    ;
    from_sql = from_sql
            .arg(dish_type)
            .arg(sub_type)
            .arg(ui->lineEdit->text().toUpper())//
            ;
    qDebug() << "from_sql" << from_sql;

    querymodel->setQuery(
                "select ch_dishno, "

                " vch_dishname, "

                " (select vch_unitname from cey_bt_unit where cey_bt_unit.ch_unitno = t.ch_unitno) as vch_unitname,"

                " num_store, "

                " num_cost as num_store_price, "

                " num_store * num_cost as num_store_total,"

                " (select vch_dish_typename from cey_bt_dish_type where cey_bt_dish_type.ch_dish_typeno = t.ch_dish_typeno) as vch_dish_typename,"

                " (select vch_sub_typename from cey_bt_dish_subtype where cey_bt_dish_subtype.ch_sub_typeno = t.ch_sub_typeno) as vch_sub_typename"

                +QString(inventory_warn ? ", num_item_warn": "")+

                " from ("

                "SELECT a.ch_dishno, a.vch_dishname, a.int_unit_rate, a.ch_unitno,"

                " a.vch_pur_unitno,"

                " a.ch_dish_typeno,"

                " a.ch_sub_typeno,"

                " IFNULL(b.num_num, 0) as num_store,"

                " IFNULL(b.cost_price, 0) as num_cost,"

                " a.num_item_warn "

                + from_sql
                + ") t"
                );
    //
    lds_query_hddpos  query;
    query.execSelect("select sum(b.num_num) as num_store, sum(b.num_num * b.cost_price) as num_store_total "
                     + from_sql);
    query.next();
    ui->tableView->updateSum(querymodel->fieldIndex("num_store"), query.recordValue("num_store"));
    ui->tableView->updateSum(querymodel->fieldIndex("num_store_total"), query.recordValue("num_store_total"));
    //
    ui->tableView->resizeColumnsToContents();
}

void w_i_inventory_query::toselect(QAction *act)
{
    QString dish_type_select = act->data().toString();
    ui->pushButton_dish_type->setData("dish_type_select", dish_type_select);
    QString dish_type  = dish_type_select.split("/").value(0, "");
    QString sub_type  = dish_type_select.split("/").value(1, "");
    if(dish_type == "") {
        ui->pushButton_dish_type->setText(QObject::tr("全部"));
    }
    if(dish_type != "" && sub_type == "") {
        ui->pushButton_dish_type->setText(
                    lds_query_hddpos::selectValue(" select vch_dish_typename from cey_bt_dish_type where ch_dish_typeno = '%1' ", dish_type).toString());
    }
    if(dish_type != "" && sub_type != "") {
        ui->pushButton_dish_type->setText(
                    lds_query_hddpos::selectValue(" select vch_dish_typename from cey_bt_dish_type where ch_dish_typeno = '%1' ", dish_type).toString() + "/" +
                    lds_query_hddpos::selectValue(" select vch_sub_typename from cey_bt_dish_subtype where ch_sub_typeno = '%1' ", sub_type).toString()
                    );
    }
    toselect();
}

void w_i_inventory_query::toexit()
{
    this->reject();
}

void w_i_inventory_query::toexport()
{
    QString title = ui->comboBox_report_type->currentText();

    w_rpt_report_dataExport dialog(ui->tableView->tableView, ui->tableView->lineView, this);
    dialog.setWindowTitle(title);
    dialog.defFilepath(title);
    transparentCenterDialog(&dialog).exec();
}

void w_i_inventory_query::toprint()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    w_rpt_report_printFormat *dialog
            = new w_rpt_report_printFormat(w_rpt_report_printFormat::show_model_quick, this->windowTitle(), lds::gs_operid, n_func::f_get_sysdatetime_str(),
                                           this->metaObject()->className(),
                                           "", "",
                                           ui->tableView,
                                           0
                                           );
    dialog->pop(b, this);
}

QAction *w_i_inventory_query::dish_typeno_menu(QMenu *parent, const QString &ch_dish_typeno)
{
    QString vch_dish_typename = lds_query_hddpos::selectValue(" select vch_dish_typename from cey_bt_dish_type where ch_dish_typeno = '%1' ", ch_dish_typeno).toString();
    lds_query_hddpos query;
    QMenu *m = 0;
    QAction *act = 0;
    query.execSelect(QString(" SELECT ch_dish_typeno, ch_sub_typeno, vch_sub_typename FROM cey_bt_dish_subtype where ch_dish_typeno = '%1'; ")
                     .arg(ch_dish_typeno));
    while(query.next()) {
        if(m == 0) {
            m = new QMenu(parent);
            m->setTitle(vch_dish_typename);
            act = m->addAction(QObject::tr("全部"));
            act->setData(ch_dish_typeno);
            m->addAction(act);
        }
        act = m->addAction(query.recordValue("vch_sub_typename").toString());
        act->setText(query.recordValue("vch_sub_typename").toString());
        act->setData(query.recordValue("ch_dish_typeno").toString() + "/" + query.recordValue("ch_sub_typeno").toString());
        m->addAction(act);
    }
    if(m == 0) {
        act = new QAction(vch_dish_typename, parent);
        act->setData(ch_dish_typeno);
        return act;
    }
    return m->menuAction();
}

#include "w_rpt_report_frame_material_saleflow2.h"
#include "ui_w_rpt_report_frame_material_saleflow2.h"
#include "n_func.h"
#include <QScrollBar>
#include "lds_model_sqltablemodel_delegate_com.h"

w_rpt_report_frame_material_saleflow2::w_rpt_report_frame_material_saleflow2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_rpt_report_frame_material_saleflow2)
{
    ui->setupUi(this);

    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(23,59,59)));
    //

    select_limit.insertFieldName("b.int_flowid");
    select_limit.insertFieldName("a.ch_payno");
    select_limit.insertFieldName("a.int_flowid");
    select_limit.insertFieldName("a.ch_dishno");
    select_limit.insertFieldName("a.num_num - a.num_back", "num_dish");
    select_limit.insertFieldName("b.ch_dishno", "ch_materialno");
    select_limit.insertFieldName("b.num_num - b.num_back", "num_material");
    select_limit.insertFieldName("a.dt_operdate");
    select_limit.insertFieldName("a.vch_operid");
    select_limit.from_sql_arg =
            " FROM"
            "    material_u_orderdish b,"
            "    cey_u_orderdish a,"
            "    cey_u_checkout_master c "
            "       where"
            "       b.int_order_flowID = a.int_flowid"
            "       and a.ch_payno = c.ch_payno"
            "       and c.ch_state = 'Y'"
            "       and a.dt_operdate >= '%1' "
            "       and a.dt_operdate <= '%2' "
            ;



    querymodel = new report_querymodel(this);
    ui->tableView->setModel(querymodel);
    ui->tableView->setTransferHheader();
    ui->tableView->lineView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //
    ui->widget->setup(QStringList() << QObject::tr("查询") << QObject::tr("导出") << QObject::tr("打印") << QObject::tr("退出") << lds_toolbar::stretch<< QObject::tr("分页") << QObject::tr("上一页") << QObject::tr("下一页"), Qt::AlignLeft);

    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()), this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()), this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()), this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()), this,SLOT(toexit()));

    connect(ui->widget->index_widget(QObject::tr("分页")), SIGNAL(clicked()), this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("上一页")), SIGNAL(clicked()), this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("下一页")), SIGNAL(clicked()), this,SLOT(toselect()));
}

w_rpt_report_frame_material_saleflow2::~w_rpt_report_frame_material_saleflow2()
{
    delete ui;
}

void w_rpt_report_frame_material_saleflow2::toselect()
{
    select_limit.set_onepagecount((ui->tableView->height() //tableview height
                                   - lds_tableView::get_hheight()//tableview horizontal header height
                                   -  lds_tableView::get_hscrollheight()//tableview horizontal scrollbar height
                                   - ui->tableView->verticalHeader()->defaultSectionSize()//tableview lineview height
                                   ) / ui->tableView->verticalHeader()->defaultSectionSize());
    select_limit.from_sql = select_limit.from_sql_arg.arg(ui->dateTimeEdit->dateTimeStr()).arg(ui->dateTimeEdit_2->dateTimeStr());
    select_limit.next();
    querymodel->setQuery(select_limit.execSql());
    ui->tableView->updateSum(querymodel->fieldIndex("num_dish"),  select_limit.field_toal("num_dish"));
    ui->tableView->updateSum(querymodel->fieldIndex("num_material"),  select_limit.field_toal("num_material"));

    if(ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("ch_dishno")) == 0) {
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_dishno"),
                                                           new lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish", "ch_dishno", "vch_dishname"));
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("ch_materialno"),
                                                           ui->tableView->itemDelegateForColumn(querymodel->fieldIndex("ch_dishno")));
        ui->tableView->setItemDelegateForColumn(querymodel->fieldIndex("vch_operid"),
                                                           new lds_model_sqltablemodel_delegate_com(this, "("+backheader::SELECT_OPERID_NAME+")t", "vch_operId", "vch_operator" ));
    }
    ui->tableView->resizeColumnsToContents();
}

void w_rpt_report_frame_material_saleflow2::toexit()
{
    this->reject();
}

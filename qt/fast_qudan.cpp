#include "fast_qudan.h"
#include "ui_fast_qudan.h"
#include <QSqlRecord>
#include "lds_messagebox.h"
#include "lds_query_hddpos.h"
#include <QSqlError>
#include "fexpandmain_2.h"
#include "backheader.h"

fast_qudan::fast_qudan(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fast_qudan)
{
    ui->setupUi(this);

    querymodel = new QSqlQueryModel;
    querymodel->setQuery("select int_flowID from cey_u_hungbill where  dt_operdate >= curdate() group by int_flowID");
    querymodel->setHeaderData(0, Qt::Horizontal, QObject::tr("挂单号"));
    ui->tableView->setModel(querymodel);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    querymodel_2 = new QSqlQueryModel;
    sql2 = QString("select a.vch_operID, b.vch_dishname,a.num_price, a.num_num - a.num_back as dish_num from cey_u_hungbill a, cey_bt_dish b where a.ch_dishno = b.ch_dishno and a.int_flowid = '%1' ");
    querymodel_2->setQuery(sql2);//is empty
    querymodel_2->setHeaderData(0, Qt::Horizontal, QObject::tr("操作员"));
    querymodel_2->setHeaderData(1, Qt::Horizontal, QObject::tr("品名"));
    querymodel_2->setHeaderData(2, Qt::Horizontal, QObject::tr("价格"));
    querymodel_2->setHeaderData(3, Qt::Horizontal, QObject::tr("数量"));
    ui->tableView_2->setModel(querymodel_2);

    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this,SLOT(toupdate2()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_del, SIGNAL(clicked()),this,SLOT(todel()));
    if(querymodel->rowCount() > 0) {
        ui->tableView->selectRow(0);
        toupdate2();
    }
}

fast_qudan::~fast_qudan()
{
    delete ui;
}

bool fast_qudan::pendTransaction(QString &errstring, fexpandmain_model_sqltablemodel_data *tablemodel, int &int_flowid_return)
{
    lds_query_hddpos  query;
    query.execSelect(QString("select Max(int_flowID) from cey_u_hungbill"));
    query.next();
    int_flowid_return = query.recordValue(0).toInt()+1;

    lds_query_hddpos::tran_saction();
    for(int row  = 0, rowcount = tablemodel->rowCount(); row < rowcount; row++) {
        if(query.execInsert("cey_u_hungbill",qrtVariantPairList()

                            << qrtVariantPair("int_flowID", int_flowid_return)
                            << qrtVariantPair("int_id", tablemodel->model_data(row, "int_id"))
                            << qrtVariantPair("ch_tableno", tablemodel->model_data(row, "ch_tableno"))
                            << qrtVariantPair("ch_dishno", tablemodel->model_data(row, "ch_dishno"))
                            << qrtVariantPair("num_price", tablemodel->model_data(row, "num_price"))

                            << qrtVariantPair("num_num", tablemodel->model_data(row, "num_num"))
                            << qrtVariantPair("num_back", tablemodel->model_data(row, "num_back"))
                            << qrtVariantPair("vch_back_operID", tablemodel->model_data(row, "vch_back_operID"))
                            << qrtVariantPair("dt_back_operdate", tablemodel->model_data(row, "dt_back_operdate"))
                            << qrtVariantPair("int_discount", tablemodel->model_data(row, "int_discount"))

                            << qrtVariantPair("vch_dis_operID", tablemodel->model_data(row, "vch_dis_operID"))
                            << qrtVariantPair("dt_dis_operdate", tablemodel->model_data(row, "dt_dis_operdate"))
                            << qrtVariantPair("ch_printflag", tablemodel->model_data(row, "ch_printflag"))
                            << qrtVariantPair("vch_print_memo", tablemodel->model_data(row, "vch_print_memo"))
                            << qrtVariantPair("ch_suitflag", tablemodel->model_data(row, "ch_suitflag"))

                            << qrtVariantPair("ch_suitno", tablemodel->model_data(row, "ch_suitno"))
                            << qrtVariantPair("ch_specialflag", tablemodel->model_data(row, "ch_specialflag"))
                            << qrtVariantPair("ch_presentflag", tablemodel->model_data(row, "ch_presentflag"))
                            << qrtVariantPair("vch_pre_operID", tablemodel->model_data(row, "vch_pre_operID"))
                            << qrtVariantPair("dt_pre_operdate", tablemodel->model_data(row, "dt_pre_operdate"))

                            << qrtVariantPair("vch_operID", tablemodel->model_data(row, "vch_operID"))
                            << qrtVariantPair("dt_operdate", tablemodel->model_data(row, "dt_operdate"))
                            << qrtVariantPair("num_price_add", tablemodel->model_data(row, "num_price_add"))
                            << qrtVariantPair("vch_plan_k3o3_id", tablemodel->model_data(row, "vch_plan_k3o3_id"))
                            << qrtVariantPair("vch_plan_k3o3_state", tablemodel->model_data(row, "vch_plan_k3o3_state"))

                            << qrtVariantPair("ch_member_belong_state", tablemodel->model_data(row, "ch_member_belong_state"))
                            << qrtVariantPair("int_rate", tablemodel->model_data(row, "int_rate")))) {
            continue;
        }

        lds_query_hddpos::roll_back();
        errstring = query.recordError();
        return false;
    }
    lds_query_hddpos::com_mit();
    printTakebill(tablemodel);
    tablemodel->removeRows(0, tablemodel->rowCount());
    return true;
}

bool fast_qudan::take(QString &errstring, fexpandmain_model_sqltablemodel_data *tablemodel, int int_flowid)
{
    lds_query_hddpos  query;
    ///
    tablemodel->keepdata_insert_prepare(0);

    query.execSelect(QString ("select a.*, b.num_price as num_price_org from cey_u_hungbill a, cey_bt_dish b where a.ch_dishno = b.ch_dishno and a.int_flowID  = '%1' ").arg(int_flowid));
    while(query.next()) {
        int row = tablemodel->rowCount();
        tablemodel->insertRow(row);

        tablemodel->model_data_set(row, "ch_billno", tablemodel->cur_ch_billno());
        tablemodel->model_data_set(row, "int_id", query.recordValue("int_id"));
        tablemodel->model_data_set(row, "ch_tableno", query.recordValue("ch_tableno"));
        tablemodel->model_data_set(row, "ch_dishno", query.recordValue("ch_dishno"));
        tablemodel->model_data_set(row, "num_price", query.recordValue("num_price"));
        tablemodel->model_data_set(row, "num_price_org", query.recordValue("num_price_org"));

        tablemodel->model_data_set(row, "num_num", query.recordValue("num_num"));
        tablemodel->model_data_set(row, "num_back", query.recordValue("num_back"));
        tablemodel->model_data_set(row, "vch_back_operID", query.recordValue("vch_back_operID"));
        tablemodel->model_data_set(row, "dt_back_operdate", query.recordValue("dt_back_operdate"));
        tablemodel->model_data_set(row, "int_discount", query.recordValue("int_discount"));

        tablemodel->model_data_set(row, "vch_dis_operID", query.recordValue("vch_dis_operID"));
        tablemodel->model_data_set(row, "dt_dis_operdate", query.recordValue("dt_dis_operdate"));
        tablemodel->model_data_set(row, "ch_printflag", query.recordValue("ch_printflag"));
        tablemodel->model_data_set(row, "vch_print_memo", query.recordValue("vch_print_memo"));
        tablemodel->model_data_set(row, "ch_suitflag", query.recordValue("ch_suitflag"));
        tablemodel->model_data_set(row, "ch_suitno", query.recordValue("ch_suitno"));
        tablemodel->model_data_set(row, "ch_waidai_flag", query.recordValue("ch_waidai_flag"));
        tablemodel->model_data_set(row, "ch_addon_flag", query.recordValue("ch_addon_flag"));
        tablemodel->model_data_set(row, "ch_addon_no", query.recordValue("ch_addon_no"));

        tablemodel->model_data_set(row, "ch_specialflag", query.recordValue("ch_specialflag"));
        tablemodel->model_data_set(row, "ch_presentflag", query.recordValue("ch_presentflag"));
        tablemodel->model_data_set(row, "vch_pre_operID", query.recordValue("vch_pre_operID"));
        tablemodel->model_data_set(row, "dt_pre_operdate", query.recordValue("dt_pre_operdate"));

        tablemodel->model_data_set(row, "vch_operID", query.recordValue("vch_operID"));
        tablemodel->model_data_set(row, "dt_operdate", query.recordValue("dt_operdate"));
        tablemodel->model_data_set(row, "num_price_add", query.recordValue("num_price_add"));
        tablemodel->model_data_set(row, "vch_plan_k3o3_id", query.recordValue("vch_plan_k3o3_id"));

        tablemodel->model_data_set(row, "vch_plan_k3o3_state", query.recordValue("vch_plan_k3o3_state"));
        tablemodel->model_data_set(row, "ch_member_belong_state", query.recordValue("ch_member_belong_state"));
        tablemodel->model_data_set(row, "int_rate", query.recordValue("int_rate"));
    }
    ///
    tablemodel->keepdata_insert_submit();

    if(query.execDelete("cey_u_hungbill", "int_flowid = " + QString::number(int_flowid))) {
        return true;
    }//
    errstring = query.recordError();
    return false;
}

void fast_qudan::printTakebill(fexpandmain_model_sqltablemodel_data *tablemodel)
{
    if(lds::sysconf->value("system_setting/kuaicanprint_on_hang", false).toBool() == false)
        return;
    QString printer_path = backheader::print_def_path();
    int int_paperwidth = tablemodel->tablename_kvmap_value("cey_bt_kitchen_print", printer_path, "vch_printip", "int_paperwidth").toInt();
    if(int_paperwidth == 0)
        int_paperwidth = 32;

    lds_query_hddpos  query;
    blockIns printdata;
    printdata += backheader::printer.drawText("i", "");//清除缓存
    query.execSelect("select vch_company from cey_sys_company limit 1;");
    if(query.next()) {
        printdata += backheader::printer.drawText("a00,s11", query.recordValue("vch_company").toString()+"\n");
    }
    printdata += backheader::printer.drawText("a10,s00", "");
    printdata += backheader::printer.drawText("", QObject::tr("操作员") + ":" +lds::gs_opername + "\n");//
    printdata += backheader::printer.drawText("", QObject::tr("操作日期") + ":" +n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss") + "\n");
    printdata += backheader::printer.drawText("", QObject::tr("数量") + ":" + QString().sprintf("%.02f", tablemodel->getKeepdata().dish_num()) + "\n");
    printdata += backheader::printer.drawText("", QObject::tr("合计") + ":" + QString().sprintf("%.02f", tablemodel->getKeepdata().dish_total()) + "\n");
    printdata += backheader::printer.drawText("-");
    printdata += backheader::printer.drawText("", left_justified(QObject::tr("名称"), int_paperwidth* 0.4));
    printdata += backheader::printer.drawText("", right_justified(QObject::tr("价格"), int_paperwidth* 0.2));
    printdata += backheader::printer.drawText("", right_justified(QObject::tr("数量"), int_paperwidth* 0.2));
    printdata += backheader::printer.drawText("", right_justified(QObject::tr("总计"), int_paperwidth* 0.2) + "\n");
    printdata += backheader::printer.drawText("-");
    for(int row = 0; row < tablemodel->rowCount(); row ++) {
        fexpandmain_keepData k = tablemodel->getKeepdata(row);
        QString ch_dishno = tablemodel->model_data(row, "ch_dishno").toString();
        double num_price = tablemodel->model_data(row, "num_price").toDouble();
        printdata += backheader::printer.drawText("", left_justified(tablemodel->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", "vch_dishname").toString(), int_paperwidth* 0.4));
        printdata += backheader::printer.drawText("", right_justified(QString().sprintf("%.02f", num_price), int_paperwidth* 0.2));
        printdata += backheader::printer.drawText("", right_justified(QString().sprintf("%.02f", k.dish_num()), int_paperwidth* 0.2));
        printdata += backheader::printer.drawText("", right_justified(QString().sprintf("%.02f", k.dish_total()) , int_paperwidth* 0.2) + "\n");
    }

    printdata += backheader::printer.drawTextEnd("1B6405 1D5601", "");//走纸5x， 切纸
    printdata =   backheader::printer.setPrintCount(printdata, 1);

    backheader::reslove_write(printdata);
}

QString fast_qudan::right_justified(const QString &in, int width)
{
    int w = lds::size_g_of_u(in);
    if(width >= w) {
        return QString(width - w, ' ') + in;
    }
    return in;
}

QString fast_qudan::left_justified(const QString &in, int width)
{
    int w = lds::size_g_of_u(in);
    if(width >= w) {
        return in + QString(width - w, ' ');
    }
    return in;
}

void fast_qudan::tocancel()
{
    this->reject();
}

void fast_qudan::took()
{
    int row =  ui->tableView->currentIndex().row();
    if( row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }

    int_flowid = querymodel->record(row).value("int_flowid").toInt();
    this->accept();
}


void fast_qudan::toupdate2()
{
    int row = ui->tableView->currentIndex().row();
    if(row >= 0) {
        querymodel_2->setQuery(sql2.arg(querymodel->record(row).value("int_flowId").toString()));
    }
}

void fast_qudan::todel()
{
    int row =  ui->tableView->currentIndex().row();
    if( row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("是否删除"), QObject::tr("是"),  QObject::tr("否"))) {
        int int_flowid = querymodel->record(row).value("int_flowid").toInt();
        lds_query_hddpos  query;
        if(query.execDelete("cey_u_hungbill", QString("int_flowid = '%1' ").arg(int_flowid))) {
            querymodel->setQuery(querymodel->query().lastQuery());
            if(querymodel->rowCount() > 0) {
                ui->tableView->selectRow(0);
                toupdate2();
            }
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return;
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        return;
    }
}



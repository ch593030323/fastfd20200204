#include "w_i_inventory_provider_arrears_pay.h"
#include "ui_w_i_inventory_provider_arrears_pay.h"
#include "lds_messagebox.h"
#include "lds_query_hddpos.h"
#include "backheader.h"
#include "n_func.h"
#include "w_i_inventory.h"

w_i_inventory_provider_arrears_pay::w_i_inventory_provider_arrears_pay(const lds_model_sqltablemodel *tablemodel, int tablemodel_row, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_i_inventory_provider_arrears_pay),
    tablemodel(tablemodel),
    tablemodel_row(tablemodel_row)
{
    ui->setupUi(this);

    QString ch_sheetType = tablemodel->model_data(tablemodel_row, "ch_sheetType").toString();
    flag = w_i_inventory::provide_PAY_FLAG(ch_sheetType);
    this->setWindowTitle(tr("供应商") + w_i_inventory::SHEET_NAME(ch_sheetType) );

    ui->doubleSpinBox_already_pay->setEnabled(false);
    ui->doubleSpinBox_already_pay->setValue(flag * tablemodel->model_data_NIBOLAN(tablemodel_row, "num_cash-num_remain"));
    ui->doubleSpinBox_amount->setEnabled(false);
    ui->doubleSpinBox_amount->setValue(flag * tablemodel->model_data(tablemodel_row, "num_cash").toDouble());
    ui->doubleSpinBox_should_pay->setFocus();
    ui->doubleSpinBox_should_pay->setValue(flag * tablemodel->model_data(tablemodel_row, "num_remain").toDouble());
    ui->doubleSpinBox_should_pay->setMaximum(ui->doubleSpinBox_should_pay->value());

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_pay, SIGNAL(clicked()), this, SLOT(topay()));
}

w_i_inventory_provider_arrears_pay::~w_i_inventory_provider_arrears_pay()
{
    delete ui;
}

void w_i_inventory_provider_arrears_pay::tocancel()
{
    this->reject();
}

void w_i_inventory_provider_arrears_pay::topay()
{
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认支付"), QObject::tr("确认"), QObject::tr("取消"))) {
        return;
    }
    QString ch_sheetType = tablemodel->model_data(tablemodel_row, "ch_sheetType").toString();
    QString ch_providerNo = tablemodel->model_data(tablemodel_row, "ch_providerNo").toString();
    QString vch_providerName = tablemodel->tablename_kvmap_value("cey_st_provider",  ch_providerNo, "ch_providerNo", "vch_providerName").toString();
    lds_query_hddpos query;
    lds_query_hddpos::tran_saction();
    double num_already_pay = flag * ui->doubleSpinBox_already_pay->value();
    double num_amount = flag * ui->doubleSpinBox_amount->value();
    double num_should_pay = flag * ui->doubleSpinBox_should_pay->value();

    double num_remain = num_amount - num_already_pay - num_should_pay;

    if(false == query.execInsert("cey_st_provider_pay_flow", qrtVariantPairList()
                                 << qrtVariantPair("ch_sheetno", tablemodel->model_data(tablemodel_row, "ch_sheetno"))
                                 << qrtVariantPair("ch_providerNo", ch_providerNo)
                                 << qrtVariantPair("num_pay", num_should_pay)
                                 << qrtVariantPair("num_remain", num_remain)
                                 << qrtVariantPair("dt_operdate", qrtFunc("now() "))
                                 )) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
        return;
    }
    if(false == query.execUpdate("cey_st_provider_pay", "num_remain", num_remain,
                                 QString(" ch_sheetno = '%1' ").arg(tablemodel->model_data(tablemodel_row, "ch_sheetno").toString()))) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
        return;
    }
    if(false == query.execUpdate("cey_st_provider", "num_remain", qrtFunc(QString(" num_remain - %1").arg(num_should_pay)), QString(" ch_providerNo = '%1' ").arg(ch_providerNo))) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
        return;
    }
    lds_query_hddpos::com_mit();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));

    //print
    blockIns printdata;
    printdata += backheader::printer.drawText("i,a00,s11", "");
    query.execSelect("select vch_company from cey_sys_company limit 1;");
    if(query.next()) {
        printdata += backheader::printer.drawText("", query.recordValue("vch_company").toString()+"\n");
    }
    printdata += backheader::printer.drawText("", this->windowTitle()+"\n");
    printdata += backheader::printer.drawText("i", "");//清除缓存
    printdata += backheader::printer.drawText("", QObject::tr("单据编号") + ":" + tablemodel->model_data(tablemodel_row, "ch_sheetno").toString() + "\n");
    printdata += backheader::printer.drawText("", QObject::tr("单据类型") + ":" + w_i_inventory::SHEET_NAME(ch_sheetType) + "\n");
    printdata += backheader::printer.drawText("", QObject::tr("供应商编号") + ":" + ch_providerNo + "\n");
    printdata += backheader::printer.drawText("", QObject::tr("供应商名称") + ":" + vch_providerName + "\n");
    printdata += backheader::printer.drawText("", QObject::tr("总金额") + ":" + lds::septNumber(num_amount) + "\n");
    printdata += backheader::printer.drawText("", QObject::tr("历史付款") + ":" + lds::septNumber(num_already_pay) + "\n");
    printdata += backheader::printer.drawText("", QObject::tr("本次支付") + ":" + lds::septNumber(num_should_pay) + "\n");
    printdata += backheader::printer.drawText("", QObject::tr("欠款") + ":" + lds::septNumber(num_remain) + "\n");

    printdata += backheader::printer.drawText("", QObject::tr("操作员") + ":" +lds::gs_opername + "\n");//
    printdata += backheader::printer.drawText("", QObject::tr("操作日期") + ":" +n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss") + "\n");

    printdata += backheader::printer.drawTextEnd("1B6405 1D5601", "");//走纸5x， 切纸
    printdata =   backheader::printer.setPrintCount(printdata, 1);

    backheader::reslove_write(printdata);
    //
    this->accept();
    return;
}

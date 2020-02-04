#include "w_scr_dish_main_separate.h"
#include "ui_w_scr_dish_main_separate.h"
#include "lds_doublespinbox.h"
#include "lds_tableview_delegate_check.h"
#include "lds_messagebox.h"

w_scr_dish_main_separate::w_scr_dish_main_separate(const fexpandmain_model_sqltablemodel_data *tablemodel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_main_separate)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("分开结算"));
    ui->lineEdit_vch_dishname->setEnabled(false);
    ui->doubleSpinBox_num_dish_org->setEnabled(false);
    this->tablemodel = tablemodel;

    standardmodel = new lds_model_sqlstandardmodel(this);
    standardmodel->setHorizontalHeaderLabels(QStringList() << "ch_state" << "vch_dishname" <<  "vch_dishname_org" <<  "ch_dishno" << "num_price" << "tablemodel_row" << "ch_type" << "num_dish" << "num_dish_org");
    standardmodel->save_set_header("ch_state", true);
    ui->tableView->setModel(standardmodel);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setTransferHheader();
    ui->tableView->justShowColumns(QList<int>() << standardmodel->fieldIndex("ch_state") << standardmodel->fieldIndex("vch_dishname") << standardmodel->fieldIndex("num_dish"));
    ui->tableView->setColumnWidth(standardmodel->fieldIndex("ch_state"), 80);
    ui->tableView->setColumnWidth(standardmodel->fieldIndex("vch_dishname"), 200);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setItemDelegateForColumn(standardmodel->fieldIndex("ch_state"), new lds_tableview_delegate_check(standardmodel->fieldIndex("ch_state"), ui->tableView, true));

    //
    for(int row = 0; row < tablemodel->rowCount(); row ++) {
        vch_print_memoData d(tablemodel->model_data(row, "vch_print_memo").toString());
        if(d.count() == 0 && tablemodel->model_data_NIBOLAN(row, "num_num-num_back") == 0) {
            continue;
        }

        int cur_row;
        cur_row = standardmodel->rowCount();
        standardmodel->insertRow(cur_row);

        standardmodel->model_data_set(cur_row, "ch_state", "N");
        standardmodel->model_data_set(cur_row, "vch_dishname", tablemodel->get_cey_bt_dish_value(tablemodel->model_data(row, "ch_dishno").toString(), "vch_dishname"));
        standardmodel->model_data_set(cur_row, "vch_dishname_org", tablemodel->get_cey_bt_dish_value(tablemodel->model_data(row, "ch_dishno").toString(), "vch_dishname"));
        standardmodel->model_data_set(cur_row, "ch_dishno", tablemodel->model_data(row, "ch_dishno").toString());
        standardmodel->model_data_set(cur_row, "tablemodel_row", row);
        standardmodel->model_data_set(cur_row, "ch_type", rowDish);
        standardmodel->model_data_set(cur_row, "num_price", tablemodel->model_data(row, "num_price"));
        standardmodel->model_data_set(cur_row, "num_dish", tablemodel->model_data_NIBOLAN(row, "num_num-num_back"));
        standardmodel->model_data_set(cur_row, "num_dish_org", tablemodel->model_data_NIBOLAN(row, "num_num-num_back"));

        while(d.next()) {
            cur_row = standardmodel->rowCount();
            standardmodel->insertRow(cur_row);

            standardmodel->model_data_set(cur_row, "ch_state", "N");
            standardmodel->model_data_set(cur_row, "vch_dishname", QObject::tr("做法") + ":" + d.nextValue("name").toString());
            standardmodel->model_data_set(cur_row, "vch_dishname_org", d.nextValue("name").toString());
            standardmodel->model_data_set(cur_row, "ch_dishno", d.nextValue("no"));
            standardmodel->model_data_set(cur_row, "tablemodel_row", row);
            standardmodel->model_data_set(cur_row, "ch_type", rowCook);
            standardmodel->model_data_set(cur_row, "num_price", d.nextValue("price"));
            standardmodel->model_data_set(cur_row, "num_dish", d.nextValue("num"));
            standardmodel->model_data_set(cur_row, "num_dish_org", d.nextValue("num"));
        }
        //
    }
    //
    ui->tableView->selectRow(0);
    toUpdateRow(ui->tableView->currentIndex());

    //
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_clearAll, SIGNAL(clicked()), this, SLOT(toClearAll()));
    connect(ui->pushButton_selectAll, SIGNAL(clicked()), this, SLOT(toSelectAll()));
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(toUpdateRow(QModelIndex)));
    connect(ui->doubleSpinBox_num_dish, SIGNAL(valueChanged(double)), this, SLOT(toUpdateTableNum(double)));
    connect(ui->pushButton_down,SIGNAL(clicked()), ui->tableView, SLOT(toPageDown()));
    connect(ui->pushButton_up,SIGNAL(clicked()), ui->tableView, SLOT(toPageUp()));
}

w_scr_dish_main_separate::~w_scr_dish_main_separate()
{
    delete ui;
}

void w_scr_dish_main_separate::took()
{
    double num_num = 0;
    row_data_map.clear();

    //fill row_data_map
    for(int row = 0; row < standardmodel->rowCount(); row ++) {
        if(standardmodel->model_data(row, "ch_state").toString() == "Y") {
            rowType ch_type = (rowType)standardmodel->model_data(row, "ch_type").toInt();
            double num_dish = standardmodel->model_data(row, "num_dish").toDouble();
            int tablemodel_row = standardmodel->model_data(row, "tablemodel_row").toInt();
            num_num += standardmodel->model_data(row, "num_dish").toDouble();

            if(false == row_data_map.contains(tablemodel_row)) {
                row_data_map.insert(tablemodel_row, separateData());
            }

            if(rowDish == ch_type) {
                row_data_map[tablemodel_row].num_dish = num_dish;
            }
            if(rowCook == ch_type) {
                row_data_map[tablemodel_row].d.append(
                            standardmodel->model_data(row, "ch_dishno").toString(),
                            standardmodel->model_data(row, "vch_dishname_org").toString(),
                            standardmodel->model_data(row, "num_dish").toDouble(),
                            standardmodel->model_data(row, "num_price").toDouble()
                            );
            }
        }
    }

    if(num_num == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return ;
    }

    //若需要退所有商品时，做法也要全部退掉
    QStringList unfull_name_list;
    for(QMap<int, separateData>::iterator k = row_data_map.begin(); k != row_data_map.end(); k++) {
        int tablemodel_row = k.key();
        separateData & dish = k.value();

        if(dish.num_dish == tablemodel->model_data_NIBOLAN(tablemodel_row, "num_num-num_back")) {
            vch_print_memoData vch_print_memo(tablemodel->model_data(tablemodel_row, "vch_print_memo").toString());
            vch_print_memo.sub(dish.d);

            if(vch_print_memo.num() != 0) {
                unfull_name_list << tablemodel->get_cey_bt_dish_value(tablemodel->model_data(tablemodel_row, "ch_dishno").toString(), "vch_dishname").toString() + ",";
                dish.d.select(tablemodel->model_data(tablemodel_row, "vch_print_memo").toString());
            }
        }
    }

    if(unfull_name_list.count() > 0) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, unfull_name_list.join(",") + "\n" + QObject::tr("做法将会全部退掉"), QObject::tr("确定"), QObject::tr("取消"))) {
            return;
        }
    }

    this->accept();
}

void w_scr_dish_main_separate::tocancel()
{
    this->reject();
}

void w_scr_dish_main_separate::toClearAll()
{
    for(int row = 0; row < standardmodel->rowCount(); row ++) {
        standardmodel->model_data_set(row, "ch_state", "N");
    }
}

void w_scr_dish_main_separate::toSelectAll()
{
    for(int row = 0; row < standardmodel->rowCount(); row ++) {
        standardmodel->model_data_set(row, "ch_state", "Y");
    }
}

void w_scr_dish_main_separate::toUpdateRow(const QModelIndex &index)
{
    ui->lineEdit_vch_dishname->setText(standardmodel->model_data(index.row(), "vch_dishname_org").toString());

    ui->doubleSpinBox_num_dish_org->setValue(standardmodel->model_data(index.row(), "num_dish_org").toDouble());
    ui->doubleSpinBox_num_dish->setRange(0, ui->doubleSpinBox_num_dish_org->value());

    double value = standardmodel->model_data(index.row(), "num_dish").toDouble();
    ui->doubleSpinBox_num_dish->setValue(value );
    ui->doubleSpinBox_num_dish->setFocus();
}

void w_scr_dish_main_separate::toUpdateTableNum(double num_dish)
{
    standardmodel->model_data_set(ui->tableView->currentIndex().row(), "num_dish", num_dish);
}

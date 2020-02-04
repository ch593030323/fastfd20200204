#include "w_bt_dish_kitchen_dish_plan.h"
#include "ui_w_bt_dish_kitchen_dish_plan.h"
#include <QPushButton>
#include <QSqlError>
#include "lds_query_hddpos.h"
#include "public_sql.h"
#include "lds_messagebox.h"
#include "w_bt_dish_kitchenprinter.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "n_func.h"
#include "lds.h"
#include "getfilepath.h"
#include "btnlanguageswicth.h"
#include "lds.h"
#include "ui_w_bt_dish_kitchen_dish_plan_new.h"

w_bt_dish_kitchen_dish_plan::w_bt_dish_kitchen_dish_plan(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_bt_dish_kitchen_dish_plan)
{
    ui->setupUi(this);
    //cey_bt_kitchen_plan

    ///taleview
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_kitchen_plan");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("vch_plan_id", QObject::tr("方案编号"));
    tablemodel->save_set_header("vch_plan_name", QObject::tr("方案名称"));
    tablemodel->save_set_header("int_print_id", QObject::tr("打印机"));
    tablemodel->save_set_header("ch_print_id_second_use", QObject::tr("启用备用打印机"));
    tablemodel->save_set_header("int_print_id_second", QObject::tr("备用打印机"));

    tablemodel->save_set_header("int_dish_type", QObject::tr("单据类型"));
    tablemodel->save_set_header("int_suit_type", QObject::tr("组合商品类型"));
    tablemodel->save_set_header("int_print_count", QObject::tr("份数"));
    tablemodel->save_set_header("int_head_block", QObject::tr("票头留白"));
    tablemodel->save_set_header("int_tail_block", QObject::tr("票尾留白"));

    /*tablemodel->save_set_header("vch_plan_title", QObject::tr("自定义表头"));
    tablemodel->save_set_header("vch_plan_tail", QObject::tr("自定义票尾"));*/
    tablemodel->save_set_header("ch_check_state", QObject::tr("判断打印机状态"));
    tablemodel->save_set_header("vch_dishname_No", QObject::tr("切换商品名称"));

    tablemodel->tablename_kvmap_insert_value("int_dish_type", "0", QObject::tr("一桌一单"));
    tablemodel->tablename_kvmap_insert_value("int_dish_type", "1", QObject::tr("一大类一单"));
    tablemodel->tablename_kvmap_insert_value("int_dish_type", "2", QObject::tr("一小类一单"));
    tablemodel->tablename_kvmap_insert_value("int_dish_type", "3", QObject::tr("一商品一单"));

    tablemodel->tablename_kvmap_insert_value("int_suit_type", "0", QObject::tr("打印在一起"));
    tablemodel->tablename_kvmap_insert_value("int_suit_type", "1", QObject::tr("只打印主商品"));
    tablemodel->tablename_kvmap_insert_value("int_suit_type", "2", QObject::tr("只打印子商品"));

    tablemodel->tablename_kvmap_insert_sql("cey_bt_kitchen_print", "int_id", "vch_printname","",  true, lds_model_sqltablemodel::SELECT_AUTO_REFRESH_KEY);

    tablemodel->tablename_kvmap_insert_value("vch_dishname_No", "vch_dishname", QObject::tr("名称")+"1");
    tablemodel->tablename_kvmap_insert_value("vch_dishname_No", "vch_dishname_2", QObject::tr("名称")+"2");
    tablemodel->tablename_kvmap_insert_value("vch_dishname_No", "vch_dishname,vch_dishname_2", QObject::tr("名称") + "1," + QObject::tr("名称") + "2");
    //
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->hideColumn(tablemodel->fieldIndex("vch_plan_tail"));
    ui->tableView->hideColumn(tablemodel->fieldIndex("vch_plan_title"));
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());

    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("int_dish_type"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel->tablename_kvmap_d("int_dish_type")));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("int_suit_type"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel->tablename_kvmap_d("int_suit_type")));

    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("int_print_id"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel->tablename_kvmap_d("cey_bt_kitchen_print")));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("int_print_id_second"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel->tablename_kvmap_d("cey_bt_kitchen_print")));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_dishname_No"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel->tablename_kvmap_d("vch_dishname_No")));

    //! toolbar
    ui->frame->setup(QStringList()
                     << QObject::tr("新增")  << QObject::tr("修改")  << QObject::tr("删除")
                     << QObject::tr("退出")
                     );
    connect(ui->frame->index_widget(QObject::tr("新增")), SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->frame->index_widget(QObject::tr("修改")), SIGNAL(clicked()),this,SLOT(tochange()));
    connect(ui->frame->index_widget(QObject::tr("删除")), SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->frame->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_bt_dish_kitchen_dish_plan::~w_bt_dish_kitchen_dish_plan()
{
    delete ui;
}

lds_tableView *w_bt_dish_kitchen_dish_plan::tableview()
{
    return ui->tableView;
}

void w_bt_dish_kitchen_dish_plan::tonew()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0508"/*厨房打印机数量*/)) {
        return;
    }
    w_bt_dish_kitchen_dish_plan_new dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_bt_dish_kitchen_dish_plan::tochange()
{
    if(ui->tableView->currentIndex().row() < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    w_bt_dish_kitchen_dish_plan_change dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_bt_dish_kitchen_dish_plan::todel()
{
    int row = ui->tableView->currentIndex().row();
    QString vch_plan_id = tablemodel->model_data(row, "vch_plan_id").toString();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"), QObject::tr("是"), QObject::tr("否"))) {
        tablemodel->removeRow(row);
        lds_query_hddpos::tran_saction();
        if(tablemodel->trySubmitAll()) {
            lds_query_hddpos  query;
            if(query.execUpdate("cey_bt_dish", "vch_kitchen_print_id1", "", qrtEqual("vch_kitchen_print_id1",vch_plan_id ))) {
                if(query.execUpdate("cey_bt_dish", "vch_kitchen_print_id2", "", qrtEqual("vch_kitchen_print_id2",vch_plan_id ))) {
                    if(query.execUpdate("cey_bt_dish", "vch_kitchen_print_id3", "", qrtEqual("vch_kitchen_print_id3",vch_plan_id ))) {

                        if(query.execUpdate("cey_bt_dish", "vch_kitchen_out_id1", "", qrtEqual("vch_kitchen_out_id1",vch_plan_id ))) {
                            if(query.execUpdate("cey_bt_dish", "vch_kitchen_out_id2", "", qrtEqual("vch_kitchen_out_id2",vch_plan_id ))) {
                                if(query.execUpdate("cey_bt_dish", "vch_kitchen_out_id3", "", qrtEqual("vch_kitchen_out_id3",vch_plan_id ))) {
                                    lds_query_hddpos::com_mit();
                                    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
        return;
    }
}

void w_bt_dish_kitchen_dish_plan::torefresh()
{
    tablemodel->select();
    ui->tableView->restore();
    if(tablemodel->rowCount() > 0) {
        ui->tableView->resizeColumnsToContents();
    }

}

void w_bt_dish_kitchen_dish_plan::toexit()
{
    this->reject();
}

/////////////////////
/// \brief w_bt_dish_kitchen_dish_plan_new::w_bt_dish_kitchen_dish_plan_new
/// \param parent
w_bt_dish_kitchen_dish_plan_new_virtual::w_bt_dish_kitchen_dish_plan_new_virtual(w_bt_dish_kitchen_dish_plan *parent) :
    QDialog(parent), ui( new Ui_w_bt_dish_kitchen_dish_plan_new), _parent(parent)
{
    ui->setupUi(this);

    this->setFixedWidth(350);
    lds_model_sqltablemodel_d::fillData(ui->comboBox_int_dish_type, _parent->tablemodel->tablename_kvmap_d("int_dish_type"));
    lds_model_sqltablemodel_d::fillData(ui->comboBox_int_suit_type, _parent->tablemodel->tablename_kvmap_d("int_suit_type"));

    lds_model_sqltablemodel_d::fillData(ui->comboBox_int_print_id_second, _parent->tablemodel->tablename_kvmap_d("cey_bt_kitchen_print"));
    lds_model_sqltablemodel_d::fillData(ui->comboBox_int_print_id, _parent->tablemodel->tablename_kvmap_d("cey_bt_kitchen_print"));

    lds_model_sqltablemodel_d::fillData(ui->comboBox_vch_dishname_No, _parent->tablemodel->tablename_kvmap_d("vch_dishname_No"));

    //!
    connect(ui->pushButton_print_set, SIGNAL(clicked()),this,SLOT(toprintset()));
    connect(ui->checkBox_ch_print_id_second_use, SIGNAL(toggled(bool)), ui->comboBox_int_print_id_second,SLOT(setVisible(bool)));
    connect(ui->comboBox_int_print_id, SIGNAL(currentIndexChanged(int)), this, SLOT(toch_check_state_enable(int)));
    ui->lineEdit_vch_plan_id->setEnabled(false);
    this->resize(500, 300);
}

w_bt_dish_kitchen_dish_plan_new_virtual::~w_bt_dish_kitchen_dish_plan_new_virtual()
{
    delete ui;
}

bool w_bt_dish_kitchen_dish_plan_new_virtual::saveCheck()
{
    if(ui->lineEdit_vch_plan_id->text().startsWith("0")) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号开头不能为0"));
        return false;
    }
    if(ui->lineEdit_vch_plan_id->text().toInt() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号必须为数字"));
        return false;
    }
    //    if(ui->lineEdit_vch_plan_id->text().length() != 2) {
    //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号必须为2位"));
    //        return false;
    //    }
    if(ui->lineEdit_vch_plan_name->text().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("名称不能为空"));
        return false;
    }
    if(ui->comboBox_int_print_id->currentText().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("打印机为空"));
        return false;
    }
    if(ui->comboBox_int_print_id_second->itemData(ui->comboBox_int_print_id_second->currentIndex()).toString()
            == ui->comboBox_int_print_id->itemData(ui->comboBox_int_print_id->currentIndex()).toString()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("备用打印机和打印机不能相同"));
        return false;
    }
    return true;
}

void w_bt_dish_kitchen_dish_plan_new_virtual::toprintset()
{
    w_bt_dish_Kitchenprinter_printermanager dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();

    int int_print_id = ui->comboBox_int_print_id->itemData(ui->comboBox_int_print_id->currentIndex()).toInt();
    int int_print_id_second = ui->comboBox_int_print_id_second->itemData(ui->comboBox_int_print_id_second->currentIndex()).toInt();

    _parent->tablemodel->select();
    ui->comboBox_int_print_id->clear();
    lds_model_sqltablemodel_d::fillData(ui->comboBox_int_print_id, _parent->tablemodel->tablename_kvmap_d("cey_bt_kitchen_print"));
    ui->comboBox_int_print_id->setCurrentIndex(ui->comboBox_int_print_id->findData(int_print_id));

    ui->comboBox_int_print_id_second->clear();
    lds_model_sqltablemodel_d::fillData(ui->comboBox_int_print_id_second, _parent->tablemodel->tablename_kvmap_d("cey_bt_kitchen_print"));
    ui->comboBox_int_print_id_second->setCurrentIndex(ui->comboBox_int_print_id_second->findData(int_print_id_second));
}

void w_bt_dish_kitchen_dish_plan_new_virtual::toch_check_state_enable(int index)
{
    QString int_print_id = ui->comboBox_int_print_id->comusrdata(index).toString();
    ui->checkBox_ch_check_state->setDisabled(lds_query_hddpos::selectValue("SELECT ch_kitchen_view FROM hddpos.cey_bt_kitchen_print where int_id = '%1' ", int_print_id).toString() == "Y");
    if(!ui->checkBox_ch_check_state->isEnabled()) {
        ui->checkBox_ch_check_state->setChecked(true);
    }
}

w_bt_dish_kitchen_dish_plan_new::w_bt_dish_kitchen_dish_plan_new(w_bt_dish_kitchen_dish_plan *parent) :
    w_bt_dish_kitchen_dish_plan_new_virtual(parent)
{
    ui->lineEdit_vch_plan_id->setText(_parent->tablemodel->max_column_add_1_bysql("vch_plan_id", "%2d"));
    ui->lineEdit_vch_plan_name->setText(QObject::tr("方案")+ui->lineEdit_vch_plan_id->text());
    ui->spinBox_int_print_count->setValue(1);
    ui->comboBox_int_print_id_second->setCurrentIndex(-1);
    ui->comboBox_int_print_id->setCurrentIndex(-1);
    ui->spinBox_int_tail_block->setValue(3);
    //! toolbard
    ui->frame->setup(QStringList()
                     << QObject::tr("保存")  << QObject::tr("退出"),  Qt::AlignRight
                     );
    ui->frame->index_widget(QObject::tr("退出"))->setFocus();
    connect(ui->frame->index_widget(QObject::tr("保存")), SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->frame->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    ui->comboBox_int_print_id_second->hide();
}

w_bt_dish_kitchen_dish_plan_new::~w_bt_dish_kitchen_dish_plan_new()
{

}

void w_bt_dish_kitchen_dish_plan_new::tosave()
{
    if(saveCheck()) {
        if(_parent->tablemodel->model_find("vch_plan_id", ui->lineEdit_vch_plan_id->text())) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号有重复"));
            return;
        }
        //!transaction
        lds_query_hddpos  query;
        lds_query_hddpos::tran_saction();

        if(query.execInsert("cey_bt_kitchen_plan",qrtVariantPairList()

                            <<qrtVariantPair("vch_plan_id", ui->lineEdit_vch_plan_id->text())
                            <<qrtVariantPair("vch_plan_name", ui->lineEdit_vch_plan_name->text())
                            <<qrtVariantPair("int_suit_type", ui->comboBox_int_suit_type->comusrdata().toInt())
                            <<qrtVariantPair("int_dish_type", ui->comboBox_int_dish_type->comusrdata().toInt())
                            <<qrtVariantPair("int_print_count", ui->spinBox_int_print_count->value())

                            <<qrtVariantPair("int_tail_block", ui->spinBox_int_tail_block->value())
                            <<qrtVariantPair("int_print_id_second", ui->comboBox_int_print_id_second->comusrdata().toInt())
                            <<qrtVariantPair("vch_plan_title", "")
                            <<qrtVariantPair("vch_plan_tail", "")
                            <<qrtVariantPair("int_print_id", ui->comboBox_int_print_id->comusrdata().toInt())

                            <<qrtVariantPair("int_head_block", ui->spinBox_int_head_block->value())
                            <<qrtVariantPair("ch_check_state",  ui->checkBox_ch_check_state->isChecked()?"Y":"N")
                            <<qrtVariantPair("vch_dishname_No", ui->comboBox_vch_dishname_No->comusrdata().toString()))) {
            lds_query_hddpos::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            this->accept();
            _parent->torefresh();
            return;
        }
        lds_query_hddpos::roll_back();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        return;
    }
}

void w_bt_dish_kitchen_dish_plan_new::toexit()
{
    this->reject();
}

w_bt_dish_kitchen_dish_plan_change::w_bt_dish_kitchen_dish_plan_change(w_bt_dish_kitchen_dish_plan *parent)
    : w_bt_dish_kitchen_dish_plan_new_virtual(parent)
{
    //!mapper
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(_parent->tablemodel);
    mapper->addMapping(ui->comboBox_int_dish_type, _parent->tablemodel->fieldIndex("int_dish_type"), "comusrdata");
    mapper->addMapping(ui->comboBox_int_print_id_second, _parent->tablemodel->fieldIndex("int_print_id_second"), "comusrdata");
    mapper->addMapping(ui->comboBox_int_print_id, _parent->tablemodel->fieldIndex("int_print_id"), "comusrdata");
    mapper->addMapping(ui->comboBox_int_suit_type, _parent->tablemodel->fieldIndex("int_suit_type"), "comusrdata");

    mapper->addMapping(ui->lineEdit_vch_plan_id, _parent->tablemodel->fieldIndex("vch_plan_id"));
    mapper->addMapping(ui->lineEdit_vch_plan_name, _parent->tablemodel->fieldIndex("vch_plan_name"));
    mapper->addMapping(ui->spinBox_int_print_count, _parent->tablemodel->fieldIndex("int_print_count"));
    mapper->addMapping(ui->spinBox_int_tail_block, _parent->tablemodel->fieldIndex("int_tail_block"));
    mapper->addMapping(ui->spinBox_int_head_block, _parent->tablemodel->fieldIndex("int_head_block"));
    mapper->addMapping(ui->checkBox_ch_print_id_second_use,_parent->tablemodel->fieldIndex("ch_print_id_second_use"),"checked1");
    mapper->addMapping(ui->comboBox_vch_dishname_No,_parent->tablemodel->fieldIndex("vch_dishname_No"),"comusrdata");


    //    mapper->addMapping(ui->textEdit_vch_plan_tail, _parent->tablemodel->fieldIndex("vch_plan_tail"), "plainText");
    //    mapper->addMapping(ui->textEdit_vch_plan_title, _parent->tablemodel->fieldIndex("vch_plan_title"), "plainText");
    mapper->addMapping(ui->checkBox_ch_check_state, _parent->tablemodel->fieldIndex("ch_check_state"), "checked1");

    mapper->setCurrentIndex(_parent->tableview()->currentIndex().row());
    ui->comboBox_int_print_id_second->setVisible(ui->checkBox_ch_print_id_second_use->isChecked());

    //! toolbard
    ui->frame->setup(QStringList()
                     << QObject::tr("保存")  << QObject::tr("退出"),  Qt::AlignRight
                     );
    ui->frame->index_widget(QObject::tr("退出"))->setFocus();
    connect(ui->frame->index_widget(QObject::tr("保存")), SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->frame->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
}

w_bt_dish_kitchen_dish_plan_change::~w_bt_dish_kitchen_dish_plan_change()
{

}

bool w_bt_dish_kitchen_dish_plan_change::tosave()
{
    if(saveData()) {
        _parent->torefresh();
        this->accept();
        return true;
    }
    return false;
}

void w_bt_dish_kitchen_dish_plan_change::toexit()
{
    if(_parent->tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!saveData()) {
                return;
            }
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }
    _parent->torefresh();
    this->reject();
}

void w_bt_dish_kitchen_dish_plan_change::toup()
{

}

void w_bt_dish_kitchen_dish_plan_change::todown()
{

}

bool w_bt_dish_kitchen_dish_plan_change::saveData()
{
    if(!saveCheck())
        return false;
    if(_parent->tablemodel->trySubmitAll()) {
        _parent->torefresh();
        mapper->setCurrentIndex(_parent->tableview()->currentIndex().row());
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    }
    //错误提示
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
    return false;
}

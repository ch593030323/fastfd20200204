#include "w_bt_dish_kitchen_view.h"
#include "ui_w_bt_dish_kitchen_view.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_bt_dish_kitchen_dish.h"
#include <QTimer>
#include "lds_tableview_delegate_check.h"
#include "backheader.h"
#include "w_bt_dish_kitchen_dish_plan.h"
#include "w_bt_dish_kitchen_view_plan.h"

w_bt_dish_kitchen_view::w_bt_dish_kitchen_view(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_bt_dish_kitchen_view)
{
    ui->setupUi(this);
    initView();
}

w_bt_dish_kitchen_view::~w_bt_dish_kitchen_view()
{
    delete ui;
}

void w_bt_dish_kitchen_view::initView()
{
    lds::setwflagFrameless(this);
    //! treewidget
    ui->treeWidget->header()->hide();
    ui->treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeWidget->setColumnCount(1);
    ///taleview
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_dish");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_dishno", QObject::tr("品码"));
    tablemodel->save_set_header("vch_dishname", QObject::tr("品名"));
    tablemodel->save_set_header("ch_kitchen_print_flag", QObject::tr("启用"), true);
    tablemodel->save_set_header("vch_kitchen_print_id1", QObject::tr("厨显"), true);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->justShowColumns(QList<int>()
                                   << tablemodel->fieldIndex("ch_dishno")<< tablemodel->fieldIndex("vch_dishname")
                                   << tablemodel->fieldIndex("ch_kitchen_print_flag")
                                   << tablemodel->fieldIndex("vch_kitchen_print_id1")
                                   );
    tablemodel->tablename_kvmap_insert_sql("cey_bt_kitchen_plan", "SELECT vch_plan_id, vch_plan_name FROM cey_bt_kitchen_view where vch_kitchen_type = 'kitchen' ", true, lds_model_sqltablemodel::SELECT_AUTO_REFRESH_KEY);

    lds_model_sqltablemodel_delegate_com *printd = new lds_model_sqltablemodel_delegate_com(this, tablemodel->tablename_kvmap_d("cey_bt_kitchen_plan"));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_kitchen_print_flag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_kitchen_print_flag"), ui->tableView, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_print_id1"), printd);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_print_id2"), printd);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_print_id3"), printd);

    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_kitchen_out_flag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_kitchen_out_flag"), ui->tableView, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_out_id1"), printd);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_out_id2"), printd);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_kitchen_out_id3"), printd);

    connect(ui->treeWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(treeselect()));
    connect(ui->pushButton_exit, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_kitchen_plan, SIGNAL(clicked()),this,SLOT(tokitchenplan()));
    connect(ui->pushButton_bath, SIGNAL(clicked()),this,SLOT(tomore()));
    connect(ui->pushButton_save, SIGNAL(clicked()),this,SLOT(tosave()));
}

void w_bt_dish_kitchen_view::refresh()
{
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

void w_bt_dish_kitchen_view::torefresh()
{
    ///treewidget
    lds_query_hddpos  query;
    ui->treeWidget->deleteAllItem();
    ui->treeWidget->rootitem = new QTreeWidgetItem(QStringList()
                                                   <<QObject::tr("全部类型"));
    ui->treeWidget->addTopLevelItem(ui->treeWidget->rootitem);

    query.execSelect("select  concat(vch_dish_typename,'[',ch_dish_typeno,']')     as 'nametype', ch_dish_typeno  from cey_bt_dish_type");
    while(query.next()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()
                                                    << query.recordValue("nametype").toString()
                                                    );
        ui->treeWidget->rootitem->addChild(item);
        //获取[]的内容
        lds_query_hddpos  query_1;
        QString no=query.recordValue("nametype").toString();
        no=no.split("[")[1];
        no.chop(1);
        query_1.execSelect(QString(
                               "select concat(vch_sub_typename,'[',ch_sub_typeno,']') as 'nametype' from cey_bt_dish_subtype where ch_dish_typeno = '%1' order by ch_sub_typeno asc;"
                               )
                           .arg(no)
                           );
        while(query_1.next()) {
            QTreeWidgetItem *item1=new QTreeWidgetItem(QStringList()
                                                       <<query_1.recordValue("nametype").toString()
                                                       );
            item->addChild(item1);
        }
    }
    ui->treeWidget->setItemSizeHint();//更新所有的 item的sizehint
    ui->treeWidget->restore();
    ///tableview
    treeselect();
    if(tablemodel->rowCount() > 0) {
        ui->tableView->resizeColumnsToContents();
    }
}

void w_bt_dish_kitchen_view::treeselect()
{
    if(0 > checkDirty()) {//取消或操作失败
        ui->treeWidget->select_beforerow();
        return;
    }

    QString ch_dish_typeno;
    QString ch_sub_typeno;
    //tree检索
    if(ui->treeWidget->currentItem()) {
        int index=ui->treeWidget->deepofitem(ui->treeWidget->currentItem());
        if(index>0) {
            if(index==1) {
                ch_dish_typeno=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
            }
            if(index==2) {
                ch_sub_typeno=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
            }
        }
    }

    QString sql_filter;
    if(ch_dish_typeno.length() > 0)
        sql_filter += QString(" ch_dish_typeno like '%1'   ").arg(ch_dish_typeno) + " and ";
    if(ch_sub_typeno.length() > 0)
        sql_filter += QString(" ch_sub_typeno like '%1'   ").arg(ch_sub_typeno) + " and ";
    sql_filter += " cey_bt_dish.ch_stopflag <> 'Y' ";
    //检索
    tablemodel->setFilter(sql_filter);
    ui->tableView->restore();
}

void w_bt_dish_kitchen_view::toexit()
{
    if(0 <= checkDirty()) {//不保存 保存成功
        this->reject();
    }
}

void w_bt_dish_kitchen_view::tokitchenplan()
{
    if(0 > checkDirty()) return;//取消或操作失败

    w_bt_dish_kitchen_view_plan dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void w_bt_dish_kitchen_view::tosave()
{
    if(saveData()) {
        this->accept();
    }
}

void w_bt_dish_kitchen_view::tomore()
{
    if(0 > checkDirty()) return;//取消或操作失败

    QString desc, sqlselect;
    if(ui->treeWidget->currentItem()) {
        int index=ui->treeWidget->deepofitem(ui->treeWidget->currentItem());
        if(index>0) {
            sqlselect=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
            if(index==1) {
                desc = QObject::tr("大类编号") + ":" + sqlselect;
            } else if(index==2) {
                desc = QObject::tr("小类编号") + ":" + sqlselect;
            }
        }
    }
    if(desc.length() > 0)
        desc += ",";
    desc += QObject::tr("厨显不支持出品方案");

    w_bt_dish_kitchen_dish_more dialog(this->tablemodel, this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.setDesc(desc);
    dialog.hideServicePart();
    transparentCenterDialog(&dialog).exec();
}

bool w_bt_dish_kitchen_view::saveData()
{
    //!等待
    lds_messagebox_loading_show loading(this, "", "WAITING...");
    loading.show_delay();
    //!~等待
    lds_query_hddpos query;
    lds_query_hddpos::tran_saction();
    for(int row = 0;  row < tablemodel->rowCount(); row ++) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpUpdate) {
            if(false == query.execUpdate("cey_bt_dish", qrtVariantPairList()
                                         << qrtVariantPair("ch_kitchen_print_flag", tablemodel->model_data(row, "ch_kitchen_print_flag"))
                                         << qrtVariantPair("vch_kitchen_print_id1", tablemodel->model_data(row, "vch_kitchen_print_id1"))
                                         << qrtVariantPair("vch_kitchen_print_id2", tablemodel->model_data(row, "vch_kitchen_print_id2"))
                                         << qrtVariantPair("vch_kitchen_print_id3", tablemodel->model_data(row, "vch_kitchen_print_id3"))

                                         << qrtVariantPair("ch_kitchen_out_flag", tablemodel->model_data(row, "ch_kitchen_out_flag"))
                                         << qrtVariantPair("vch_kitchen_out_id1", tablemodel->model_data(row, "vch_kitchen_out_id1"))
                                         << qrtVariantPair("vch_kitchen_out_id2", tablemodel->model_data(row, "vch_kitchen_out_id2"))
                                         << qrtVariantPair("vch_kitchen_out_id3", tablemodel->model_data(row, "vch_kitchen_out_id3")),
                                         QString("ch_dishno = '%1' ").arg(tablemodel->model_data(row, "ch_dishno").toString())
                                         )) {
                lds_query_hddpos::roll_back();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
                return false;
            }
        }
    }
    lds_query_hddpos::com_mit();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
    return true;
}

int w_bt_dish_kitchen_view::checkDirty()
{
    if(tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!saveData()) {
                return -1;//操作失败
            }
            return 1;//保存成功
        case 1:
            return 0;//不保存
        case 2:
            return -1;//取消
        }
    }
    return 1;//保存成功
}

#include "w_sys_manage_promotion_setting.h"
#include "ui_w_sys_manage_promotion_setting.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "public_sql.h"
#include "lds_tableview_delegate_check.h"
#include "lds.h"
#include "lds_query_hddpos.h"
#include "fexpandmain_2.h"
#include "n_func.h"
#include "ui_w_sys_manage_promotion_setting.h"
#include "backheader.h"
#include "ui_w_sys_manage_promotion_setting_bath.h"


w_sys_manage_promotion_setting::w_sys_manage_promotion_setting(QWidget *parent) :
    lds_model_mapwidget_vertical(parent),
    ui(new Ui_w_sys_managepromotion_setting)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->promotion_setting_am->setMinimumTime(QTime(0,0,0));
    ui->promotion_setting_am->setMaximumTime(QTime(12,0,0));
    ui->promotion_setting_am_2->setMinimumTime(QTime(0,0,0));
    ui->promotion_setting_am_2->setMaximumTime(QTime(12,0,0));
    ui->promotion_setting_pm->setMinimumTime(QTime(12,0,0));
    ui->promotion_setting_pm->setMaximumTime(QTime(24,0,0));
    ui->promotion_setting_pm_2->setMinimumTime(QTime(12,0,0));
    ui->promotion_setting_pm_2->setMaximumTime(QTime(24,0,0));

    //
    tablemodel_dish = new lds_model_sqltablemodel(this);
    tablemodel_dish->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel_dish->setTable("cey_bt_dish");
    
    tablemodel_dish->save_set_header("ch_dishno", QObject::tr("编号"));
    tablemodel_dish->save_set_header("vch_dishname", QObject::tr("名称"));
    tablemodel_dish->save_set_header("num_price", QObject::tr("原价"), true);
    tablemodel_dish->save_set_header("num_special_price", QObject::tr("特价"), true);
    tablemodel_dish->save_set_header("num_m_price", QObject::tr("会员价"), true);
    tablemodel_dish->save_set_header("ch_special_type", QObject::tr("策略")+"1", true);
    tablemodel_dish->save_set_header("ch_special_type2", QObject::tr("第二份半价"), true);
    tablemodel_dish->tablename_kvmap_insert_value("ch_special_type", "0", QObject::tr("默认原价"));
    tablemodel_dish->tablename_kvmap_insert_value("ch_special_type", "1", QObject::tr("默认特价"));
    tablemodel_dish->tablename_kvmap_insert_value("ch_special_type", "2", QObject::tr("可选特价"));
    tablemodel_dish->tablename_kvmap_insert_value("ch_special_type2", "0", QObject::tr("不启用"));
    tablemodel_dish->tablename_kvmap_insert_value("ch_special_type2", "1", QObject::tr("启用"));
    ui->promotion_setting_target->addItem(QObject::tr("所有人"), 0);
    ui->promotion_setting_target->addItem(QObject::tr("会员"), 1);
    ui->promotion_setting_target->addItem(QObject::tr("非会员"), 2);

    ui->tableView->setModel(tablemodel_dish);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->rePecifyHeader(tablemodel_dish->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel_dish->fieldIndex("ch_special_type"),
                                            new lds_model_sqltablemodel_delegate_com(this, tablemodel_dish->tablename_kvmap_d("ch_special_type") ));
    ui->tableView->setItemDelegateForColumn(tablemodel_dish->fieldIndex("ch_special_type2"),
                                            new lds_model_sqltablemodel_delegate_com(this, tablemodel_dish->tablename_kvmap_d("ch_special_type2") ));

    //! treewidget
    ui->treeWidget->header()->hide();
    ui->treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeWidget->setColumnCount(1);

    //
    scan_all_init_value("cey_sys_parameter", "vch_parameter", "vch_value", "promotion_setting");

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(to_ok()));
    connect(ui->pushButton_down, SIGNAL(clicked()),ui->tableView, SLOT(toPageDown()));
    connect(ui->pushButton_up, SIGNAL(clicked()),ui->tableView, SLOT(toPageUp()));
    connect(ui->pushButton_exit, SIGNAL(clicked()),this, SLOT(toexit()));
    connect(ui->treeWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(treeselect()));
    connect(ui->pushButton_bath, SIGNAL(clicked()),this, SLOT(tobath()));

    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_sys_manage_promotion_setting::~w_sys_manage_promotion_setting()
{
    delete ui;
}

bool w_sys_manage_promotion_setting::insert_sql_objectname(const QString &objectname)
{
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT vch_parameter from cey_sys_parameter where vch_parameter = '%1' ").arg(objectname));
    if(!query.next()) {
        return query.execInsert("cey_sys_parameter",qrtVariantPairList()
                                << qrtVariantPair("vch_parameter", objectname)
                                << qrtVariantPair("vch_value", ""));
    }
    return true;
}

void w_sys_manage_promotion_setting::to_ok()
{
    saveData();
    mappedSetCurrentIndex(1);
}

void w_sys_manage_promotion_setting::torefresh()
{
    ///treewidget
    lds_query_hddpos  query;
    ui->treeWidget->deleteAllItem();
    ui->treeWidget->rootitem = new QTreeWidgetItem(QStringList()
                                                   <<QObject::tr("全部类型"));
    ui->treeWidget->addTopLevelItem(ui->treeWidget->rootitem);

    query.execSelect("select  concat(vch_dish_typename,'[',ch_dish_typeno,']')     as 'nametype', ch_dish_typeno  from cey_bt_dish_type"
                     );
    while(query.next()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()
                                                    << query.record().value("nametype").toString()
                                                    );
        ui->treeWidget->rootitem->addChild(item);
        //获取[]的内容
        lds_query_hddpos  query_1;
        QString no=query.record().value("nametype").toString();
        no=no.split("[")[1];
        no.chop(1);
        query_1.execSelect(QString(
                               "select concat(vch_sub_typename,'[',ch_sub_typeno,']') as 'nametype' from cey_bt_dish_subtype where ch_dish_typeno = '%1' order by ch_sub_typeno asc;"
                               )
                           .arg(no)
                           );
        while(query_1.next()) {
            QTreeWidgetItem *item1=new QTreeWidgetItem(QStringList()
                                                       <<query_1.record().value("nametype").toString()
                                                       );
            item->addChild(item1);
        }
    }
    ui->treeWidget->setItemSizeHint();//更新所有的 item的sizehint
    ui->treeWidget->restore();
    ///tableview
    treeselect();
    if(tablemodel_dish->rowCount() > 0) {
        ui->tableView->resizeColumnsToContents();
    }
    //mapper
    mappedSetCurrentIndex(1);
}

void w_sys_manage_promotion_setting::toexit()
{
    if(dataDirty()) {
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
    this->reject();
}

void w_sys_manage_promotion_setting::treeselect()
{
    if(0 > checkDirty()) {//取消或操作失败
        ui->treeWidget->select_beforerow();
        return;
    }

    QString sql_where_app;
    QString sqlselect;
    //tree检索
    if(ui->treeWidget->currentItem()) {
        int index=ui->treeWidget->deepofitem(ui->treeWidget->currentItem());
        if(index>0) {
            sqlselect=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
            if(index==1) {
                sqlselect=QString("cey_bt_dish.ch_dish_typeno='%1'").arg(sqlselect);
            } else if(index==2) {
                sqlselect=QString("cey_bt_dish.ch_sub_typeno='%1'").arg(sqlselect);
            }
        }
    }
    if(sqlselect.isEmpty()) {

    } else {
        sqlselect +=" and ";
    }
    //是否停用
    sqlselect += " cey_bt_dish.ch_stopflag <> 'Y' ";

    if(!sql_where_app.isEmpty()) {
        sqlselect += " and " + sql_where_app;
    }
    //检索
    tablemodel_dish->setFilter(sqlselect);
    ui->tableView->resizeColumnsToContents();
}

void w_sys_manage_promotion_setting::tobath()
{
    lds_widget_auto_enable dd(ui->pushButton_bath);

    w_sys_manage_promotion_setting_bath dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.ui->label->setText(tablemodel_dish->headerData(tablemodel_dish->fieldIndex("ch_special_type"), Qt::Horizontal).toString());
    dialog.ui->label_2->setText(tablemodel_dish->headerData(tablemodel_dish->fieldIndex("ch_special_type2"), Qt::Horizontal).toString());

    const lds_model_sqltablemodel_d *d = tablemodel_dish->tablename_kvmap_d("ch_special_type");
    for(QMap<QString, int>::const_iterator k = d->getMapRowstr().begin(); k != d->getMapRowstr().end(); k++)
        dialog.ui->comboBox->addItem(d->data(k.value(), 0).toString(), k.key());

    d = tablemodel_dish->tablename_kvmap_d("ch_special_type2");
    for(QMap<QString, int>::const_iterator k = d->getMapRowstr().begin(); k != d->getMapRowstr().end(); k++)
        dialog.ui->comboBox_2->addItem(d->data(k.value(), 0).toString(), k.key());

    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        QVariant ch_special_type = dialog.ui->comboBox->itemData(dialog.ui->comboBox->currentIndex());
        QVariant ch_special_type2 = dialog.ui->comboBox_2->itemData(dialog.ui->comboBox_2->currentIndex());
        for(int row = 0; row < tablemodel_dish->rowCount() ; row ++) {
            if(tablemodel_dish->model_data(row, "ch_special_type") != ch_special_type)
                tablemodel_dish->model_data_set(row, "ch_special_type", ch_special_type);
            if(tablemodel_dish->model_data(row, "ch_special_type2") != ch_special_type2)
                tablemodel_dish->model_data_set(row, "ch_special_type2", ch_special_type2);
        }
    }
}

bool w_sys_manage_promotion_setting::saveData()
{
    mappedSubmit();
    lds_query_hddpos::tran_saction();
    if(tablemodel_dish->trySubmitAll() //每个商品的设置
            && commitData()) {
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    return false;
}

bool w_sys_manage_promotion_setting::dataDirty()
{
    return tablemodel_dish->model_is_dirty() || tablemodel->model_is_dirty();
}

//-1 取消/操作失败 0 不保存 1 保存成功
int w_sys_manage_promotion_setting::checkDirty()
{
    if(tablemodel_dish->model_is_dirty()) {
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



w_sys_manage_promotion_setting_bath::w_sys_manage_promotion_setting_bath(QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui_w_sys_manage_promotion_setting_bath;
    ui->setupUi(this);

    connect(ui->pushButton_ok, SIGNAL(clicked()), this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this,SLOT(tocancel()));
}

w_sys_manage_promotion_setting_bath::~w_sys_manage_promotion_setting_bath()
{
    delete ui;
}

void w_sys_manage_promotion_setting_bath::took()
{
    this->accept();
}

void w_sys_manage_promotion_setting_bath::tocancel()
{
    this->reject();
}

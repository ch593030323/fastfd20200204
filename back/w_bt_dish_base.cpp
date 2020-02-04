#include "w_bt_dish_base.h"
#include "ui_w_bt_dish_dialog.h"
#include <QSqlError>
#include "lds_query_hddpos.h"
#include "colordialog.h"
#include "lds_messagebox.h"
#include "getchinesespell.h"
#include "lds_tableview_delegate_check.h"
#include "backheader.h"
#include <QSqlRelation>
#include "w_bt_dish.h"
#include "w_sys_manage_basicdataexport.h"
#include <QFileInfo>
#include <QCloseEvent>
#include <QListWidget>
#include "w_bt_dish_kitchenprinter.h"
#include <QScrollBar>
#include "w_sys_manage_basicdataimport.h"
#include "lds.h"
#include <QPixmapCache>
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "lds_toolbar_button.h"
#include "getfilepath.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "ui_w_bt_dish_new_dialog_2.h"
#include "fexpandmain_2.h"
#include "w_bt_dish_base_material.h"
#include "w_st_good_input_select_dish.h"
#include "restauranttableview.h"
#include "ftableview_delegate.h"
#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include "lds_dialog_input3.h"
#include "ui_lds_dialog_input3.h"
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>
#include <QTimer>
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_tableview_delegate_doublespinbox.h"
#include "ui_w_bt_dish_typeset_dialog.h"
#include "lds_tableview_delegate_check.h"
#include "electroniccountprice_resolve.h"
#include "lds_value_scale.h"

w_bt_dish_base::w_bt_dish_base(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_bt_dish_Dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    //! treewidget
    ui->treeWidget->header()->hide();
    ui->treeWidget->setColumnCount(1);

    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_dish");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    //重新布局表头和隐藏相关表头
    tablemodel->save_set_header("ch_dishno", QObject::tr("编号"));
    tablemodel->save_set_header("ch_groupno",QObject::tr("组号"));
    tablemodel->save_set_header("vch_dishname", QObject::tr("名称"));
    tablemodel->save_set_header("ch_unitno", QObject::tr("单位"));
    tablemodel->save_set_header("num_price", QObject::tr("价格"));
    tablemodel->save_set_header("num_m_price", QObject::tr("会员特价1"));

    tablemodel->save_set_header("ch_stopflag", QObject::tr("停用"));
#ifdef QT_SCPOS_SUPPORT
#else
    tablemodel->save_set_header("ch_material_flag",QObject::tr("是否原料"));
#endif
    tablemodel->save_set_header("ch_weight",QObject::tr("可称重"));
    tablemodel->save_set_header("ch_suitflag",QObject::tr("组合商品"));
    tablemodel->save_set_header("ch_curprice", QObject::tr("时价商品"));
    tablemodel->save_set_header("ch_tempflag", QObject::tr("临时商品"));

    tablemodel->save_set_header("ch_off",QObject::tr("常用商品"));
    tablemodel->save_set_header("ch_discount", QObject::tr("折扣"));
    tablemodel->save_set_header("ch_serviceflag", QObject::tr("收服务费"));
    tablemodel->save_set_header("item_flag", QObject::tr("库存管理"));
    tablemodel->save_set_header("ch_deductflag", QObject::tr("服务提成"));
    tablemodel->save_set_header("int_rate", QObject::tr("税率设置"));

    tablemodel->save_set_header("vch_barcode", QObject::tr("条码"));


    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());

    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_material_flag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_material_flag"), this, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_weight"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_weight"), this, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("item_flag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("item_flag"), this, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_suitflag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_suitflag"), this, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_off"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_off"), this, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_discount"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_discount"), this, true));

    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_curprice"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_curprice"), this, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_tempflag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_tempflag"), this, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_stopflag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_stopflag"), this, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_serviceflag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_serviceflag"), this, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_deductflag"), new lds_tableview_delegate_check(QVector<int>() << tablemodel->fieldIndex("ch_deductflag"), this, true));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_unitno"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel, "cey_bt_unit","ch_unitno","vch_unitname"
                                                ));

    //
#ifdef QT_SCPOS_SUPPORT
    ui->comboBox_filter->addItem(QObject::tr("所有商品"), "1");
    ui->comboBox_filter->addItem(QObject::tr("库存管理"), " ifnull(item_flag, '') = 'Y' ");
#else
    ui->comboBox_filter->addItem(QObject::tr("所有商品"), "1");
    ui->comboBox_filter->addItem(QObject::tr("非原料商品"), " ifnull(ch_material_flag, '') <> 'Y' ");
    ui->comboBox_filter->addItem(QObject::tr("原料商品"), " ifnull(ch_material_flag, '') = 'Y' ");
    ui->comboBox_filter->addItem(QObject::tr("已添加原料"), " ch_dishno in (select ch_dishno from material_bt_dish) ");
    ui->comboBox_filter->addItem(QObject::tr("库存管理"), " ifnull(item_flag, '') = 'Y' ");
#endif
    ui->comboBox_filter_2->addItem(QObject::tr("启用"), " ifnull(ch_stopflag, '') <> 'Y' ");
    ui->comboBox_filter_2->addItem(QObject::tr("停用"), " ifnull(ch_stopflag, '') = 'Y' ");

    //! toolbar
    ui->widget->setup(QStringList()
                      //                      << QObject::tr("大类设置")
                      << QObject::tr("品类设置") << QObject::tr("单位设置") << QObject::tr("新增")
                      << QObject::tr("修改") << QObject::tr("删除") << QObject::tr("刷新") << QObject::tr("导出")<< QObject::tr("导入")
                      << QObject::tr("退出")
                      );
    //! connect
    connect(ui->treeWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(treeselect()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(treeselect()));

    //    connect(ui->widget->index_widget(QObject::tr("大类设置")), SIGNAL(clicked()),this,SLOT(to_dish_type()));
    connect(ui->widget->index_widget(QObject::tr("品类设置")), SIGNAL(clicked()),this,SLOT(totypeset()));
    connect(ui->widget->index_widget(QObject::tr("单位设置")), SIGNAL(clicked()),this,SLOT(tounitset()));
    connect(ui->widget->index_widget(QObject::tr("新增")), SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(QObject::tr("修改")), SIGNAL(clicked()),this,SLOT(tochange()));
    connect(ui->widget->index_widget(QObject::tr("删除")), SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(QObject::tr("刷新")), SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("导入")), SIGNAL(clicked()),this,SLOT(toimport()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(tochange()));
    connect(ui->comboBox_filter, SIGNAL(currentIndexChanged(int)),this,SLOT(torefresh()));
    connect(ui->comboBox_filter_2, SIGNAL(currentIndexChanged(int)),this,SLOT(torefresh()));
    //! refresh
    tablemodel_filter =
            " and  (ch_dishno like '%1%' or vch_spell like '%1%' or vch_dishname like '%1%') ";

    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_bt_dish_base::~w_bt_dish_base()
{
}

void w_bt_dish_base::torefresh()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->widget->index_widget(QObject::tr("退出"))->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun

    //refresh
    lds_query_hddpos  query;
    //treewidget
    ui->treeWidget->deleteAllItem();
    ui->treeWidget->rootitem=new QTreeWidgetItem(QStringList()
                                                 <<QObject::tr("全部类型"));
    ui->treeWidget->addTopLevelItem(ui->treeWidget->rootitem);

    query.execSelect("select  concat(vch_dish_typename,'[',ch_dish_typeno,']')     as 'nametype', ch_dish_typeno  from cey_bt_dish_type"
                     );
    while(query.next()) {
        QTreeWidgetItem *item=new QTreeWidgetItem(QStringList()
                                                  <<query.recordValue("nametype").toString()
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

    //
    treeselect();
}

lds_model_sqltablemodel * w_bt_dish_base::creatmodel(QObject *parent)
{
    lds_model_sqltablemodel *tablemodel=new lds_model_sqltablemodel(parent);
    tablemodel->setTable("cey_bt_dish");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    return tablemodel;
}

void w_bt_dish_base::setToolbarShown(bool f)
{
    ui->widget->setVisible(f);
}

// ch_dish_typeno = '%1'  and ch_sub_typeno = '%2'
// and (ch_dishno like '%3%' or vch_spell like '%3%' or vch_dishname like '%3%)
void w_bt_dish_base::treeselect()
{
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
    sql_filter += ui->comboBox_filter->itemData(ui->comboBox_filter->currentIndex()).toString() + " and ";
    sql_filter += ui->comboBox_filter_2->itemData(ui->comboBox_filter_2->currentIndex()).toString() + " and ";
    sql_filter += QString(" (ch_dishno like '%1%' or vch_spell like '%1%' or vch_dishname like '%1%' or vch_barcode like '%1%' ) ").arg(ui->lineEdit->text().toUpper());
    tablemodel->setFilter(sql_filter);

    ui->tableView->restore();
    ui->tableView->resizeColumnsToContents();
}

void w_bt_dish_base::to_dish_type()
{
    w_bt_dish_base_dish_type dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_bt_dish_base::totypeset()
{
    w_bt_dish_base_typeset dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void w_bt_dish_base::tounitset()
{
    w_bt_dish_base_unitset dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void w_bt_dish_base::tonew()
{
    if(ui->treeWidget->deepofitem(ui->treeWidget->currentItem())==2) {
        QString ch_dish_typeno = backheader::getinterdata(ui->treeWidget->currentItem()->parent()->text(0));
        QString ch_sub_typeno = backheader::getinterdata(ui->treeWidget->currentItem()->text(0));

        w_bt_dish_base_new dialog(this, ch_dish_typeno,ch_sub_typeno, "", "");
        connect(&dialog,SIGNAL(signalsave()),this,SLOT(treeselect()));
        dialog.setWindowTitle(_TEXT_SLOT(this));
        transparentCenterDialog(&dialog, false).exec();
    } else {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("请选择小类"));
    }
}

void w_bt_dish_base::tochange()
{
    int row=ui->tableView->currentIndex().row();
    if(row<0) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    w_bt_dish_base_change dialog(this, ui->tableView, tablemodel);
    dialog.setObjectName("w_bt_dish_base_change");
    dialog.setWindowTitle(_TEXT_SLOT(this));
    connect(&dialog, SIGNAL(signal_parent_to_refresh()),this,SLOT(torefresh()));
    transparentCenterDialog(&dialog, false).exec();
    torefresh();
}

void w_bt_dish_base::todel()
{
    int row=ui->tableView->currentIndex().row();
    if(row<0) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    QString ls_dishno=tablemodel->index(row, 0).data().toString();
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT ch_dishno from cey_u_orderdish where ch_dishno = '%1' limit 1 ").arg(ls_dishno));
    if(query.next()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("存在销售记录"));
        return;
    }
    query.execSelect(QString("select count(0) from cey_st_material_flow where ch_materialno='%1'").arg(ls_dishno));
    if(query.next()) {
        if(query.recordValue(0).toInt()>0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("存在入库记录"));
            return;
        }
    }
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"),QObject::tr("确认"),QObject::tr("取消"))) {
        tablemodel->removeRow(row);
        QString errstring;
        lds_query_hddpos::tran_saction();
        if(tablemodel->trySubmitAll(&errstring)) {
            lds_query_hddpos::com_mit();
        } else {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        }
    }
}

void w_bt_dish_base::toexport()
{
    w_sys_manage_basicdataexport dialog(this);
    dialog.reset_export_tablenames(QStringList() << tablemodel->tableName());
    dialog.reset_export_confname(QString("%1/path").arg(this->metaObject()->className()));
    dialog.setWindowTitle(QObject::tr("基础数据导出"));
    transparentCenterDialog(&dialog).exec();
}

void w_bt_dish_base::toimport()
{
    w_sys_manage_basicdataimport_sub dialog(QStringList() << tablemodel->tableName(), QString("%1/path").arg(this->metaObject()->className()), this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_bt_dish_base::toexit()
{
    lds_query_hddpos::tran_saction();
    tablemodel->trySubmitAll();
    lds_query_hddpos::com_mit();

    w_sys_manage_cloudsync_cloud2::exec_keepdata_byconf(this);

    this->reject();
}

void w_bt_dish_base::tomaterial()
{
    w_bt_dish_base_material dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_bt_dish_base::to_yun_version_diff(int d)
{
    bool f = false;
    if(d == 0) {
        f = true;
    }
}


//============================


w_bt_dish_base_typeset::w_bt_dish_base_typeset(QWidget *parent)
    : QDialog(parent),ui(new Ui_w_bt_dish_typeset_dialog)
{
    ui->setupUi(this);

    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_dish_type");
    tablemodel->save_set_header("ch_dish_typeno", QObject::tr("大类编号"));
    tablemodel->save_set_header("vch_dish_typename", QObject::tr("大类名称"));

    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    tablemodel_2=new lds_model_sqltablemodel(this);
    tablemodel_2->setTable("cey_bt_dish_subtype");
    tablemodel_2->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel_2->save_set_header("ch_sub_typeno", QObject::tr("小类编号"));
    tablemodel_2->save_set_header("vch_sub_typename", QObject::tr("小类名称"), true);
    tablemodel_2->save_set_header("ch_labelprint_flag", QObject::tr("标签打印"), true);
    tablemodel_2->save_set_header("vch_color", QObject::tr("颜色"));
    tablemodel_2->enable_vheaderdata_dirty_translate();

    lds_model_sqltablemodel_delegate_com *d;
    ui->tableView_2->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView_2->setModel(tablemodel_2);
    ui->tableView_2->rePecifyHeader(tablemodel_2->get_save_set_header_saver_column_list());
    ui->tableView_2->setItemDelegateForColumn(tablemodel_2->fieldIndex("ch_labelprint_flag"), new lds_tableview_delegate_check(QVector<int>()<<3,  ui->tableView_2, true));
    ui->tableView_2->setItemDelegateForColumn(tablemodel_2->fieldIndex("vch_color"), d = new lds_model_sqltablemodel_delegate_com(this, 255));d->enable_paintbtn_color();
    ui->tableView_2->setColumnWidth(0, 200);
    //    ui->tableView_2->horizontalHeader()->setStretchLastSection(true);
    //! tableview


    //! toolbar
    ui->widget_2->setup(QStringList()<<QObject::tr("新增")<<QObject::tr("删除")<<QObject::tr("标签批处理")<< QObject::tr("保存") << QObject::tr("退出"),Qt::AlignRight);
    connect(ui->widget_2->index_widget(QObject::tr("新增")),SIGNAL(clicked()),this,SLOT(tonew2()));
    connect(ui->widget_2->index_widget(QObject::tr("删除")),SIGNAL(clicked()),this,SLOT(todel2()));
    connect(ui->widget_2->index_widget(QObject::tr("标签批处理")),SIGNAL(clicked()),this,SLOT(tolabelselect()));
    connect(ui->widget_2->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget_2->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(tableview2update(QModelIndex)));

    ui->widget->setup(QStringList() << QObject::tr("新增") << QObject::tr("修改") << QObject::tr("删除"), Qt::AlignLeft);
    connect(ui->widget->index_widget(QObject::tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(QObject::tr("修改")),SIGNAL(clicked()),this,SLOT(tochange()));
    connect(ui->widget->index_widget(QObject::tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->tableView_2, SIGNAL(clicked(QModelIndex)), this,SLOT(toupdateColor(QModelIndex)));

    //    connect(ui->pushButton_exit,SIGNAL(clicked()),this,SLOT(toexit()));
    //! after
    lds_query_hddpos  query;
    query.execSelect("select count(0) from cey_bt_dish_type");
    query.next();
    if(0 == query.recordValue(0).toLongLong()) {
        if(!query.execInsert("cey_bt_dish_type",qrtVariantPairList()

                             << qrtVariantPair("ch_dish_typeno", "11")
                             << qrtVariantPair("vch_dish_typename", QObject::tr("默认大类")))) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+ QString::number(__LINE__));
        }
    }

    ui->checkBox_show_dish_type->setChecked(lds::sysconf->value("system_setting/dish_type_enabled", false).toBool());
    ui->widget->setEnabled(ui->checkBox_show_dish_type->isChecked());
    connect(ui->checkBox_show_dish_type,SIGNAL(toggled(bool)),this,SLOT(tosave_show_dish_type(bool)));

    ui->checkBox_dish_color_keep_subtype->setChecked(lds::sysconf->value("system_setting/dish_color_keep_subtype", false).toBool());
    connect(ui->checkBox_dish_color_keep_subtype,SIGNAL(toggled(bool)),this,SLOT(tosave_dish_color_keep_subtype(bool)));

    QTimer::singleShot(10, this, SLOT(torefresh()));
}


void w_bt_dish_base_typeset::tonew2()
{
    //tableview 中没有数据被选中 则return
    int row=ui->tableView->currentIndex().row();
    if(row<0) return;
    //新增初始化
    int rowcount=tablemodel_2->rowCount();
    QString  ch_dish_typeno = tablemodel->model_data(row, "ch_dish_typeno").toString();
    lds_query_hddpos  query;
    tablemodel_2->insertRow(rowcount);
    ///现获取最大值，然后获取右边的right
    int model_ch_sub_typeno_max_1 = 0;
    for(int row = 0, rowcount = tablemodel_2->rowCount(); row < rowcount; row ++) {
        if(tablemodel_2->model_data(row, "ch_sub_typeno").toString().startsWith(ch_dish_typeno)) {
            if(model_ch_sub_typeno_max_1 <  tablemodel_2->model_data(row, "ch_sub_typeno").toString().right(2).toInt()) {
                model_ch_sub_typeno_max_1 = tablemodel_2->model_data(row, "ch_sub_typeno").toString().right(2).toInt();
            }
        }
    }
    model_ch_sub_typeno_max_1++;

    ///现获取最大值，然后获取右边的right
    query.execSelect(QString("select right(max(ch_sub_typeno), 2)+1 FROM cey_bt_dish_subtype where left(ch_sub_typeno, 2) = '%1' ").arg(ch_dish_typeno));
    query.next();
    int sql_ch_sub_typeno_max_1 = query.recordValue(0).toInt();

    QString ch_sub_typeno_new = ch_dish_typeno + QString().sprintf("%02d", qMax(model_ch_sub_typeno_max_1, sql_ch_sub_typeno_max_1));

    tablemodel_2->model_data_set(rowcount,0,ch_sub_typeno_new);
    tablemodel_2->model_data_set(rowcount,2,ch_dish_typeno);


    ui->tableView_2->setCurrentIndex(tablemodel_2->model_index(rowcount, "vch_sub_typename"));
}

void w_bt_dish_base_typeset::todel2()
{
    int row = ui->tableView_2->currentIndex().row();
    if(row >= 0) {
        lds_query_hddpos  query;
        query.execSelect(QString("select count(0) from cey_bt_dish where ch_sub_typeno = '%1' ")
                         .arg(tablemodel_2->model_data(row, "ch_sub_typeno").toString()));
        query.next();
        if(query.recordValue(0).toLongLong() > 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("该小类下已有商品"));
            return;
        }

        tablemodel_2->removeRow(ui->tableView_2->currentIndex().row());
    }
}

bool w_bt_dish_base_typeset::tosave()
{
    return saveData();
}

void w_bt_dish_base_typeset::torefresh()
{
    tablemodel->select();
    ui->tableView->restore(0);
    ui->tableView->resizeColumnsToContents();

    tableview2update(ui->tableView->currentIndex());
}

void w_bt_dish_base_typeset::toexit()
{
    if(tablemodel_2->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!saveData()) {
                return;
            }
            this->accept();
            break;
        case 1:
            break;
        case 2:
            return;
        }
    }
    this->reject();
}

void w_bt_dish_base_typeset::tolabelselect()
{
    switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, _TEXT_SLOT(this), QObject::tr("全选"), QObject::tr("全不选"), QObject::tr("取消"))) {
    case 0:
    {
        for(int row = 0, rowcount = tablemodel_2->rowCount(); row < rowcount; row++) {
            tablemodel_2->model_data_set(row, "ch_labelprint_flag", "Y");
        }
    }
        break;
    case 1:
    {
        for(int row = 0, rowcount = tablemodel_2->rowCount(); row < rowcount; row++) {
            tablemodel_2->model_data_set(row, "ch_labelprint_flag", "N");
        }
    }
        break;
    case 2:
        break;
    }
}

void w_bt_dish_base_typeset::tableview2update(const QModelIndex &index)
{

    if(tablemodel_2->model_is_dirty()) {
        switch(lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"))) {
        case 0:
            if(!saveData()) {
                ui->tableView->select_beforerow();
                return;
            }
            break;
        case 1:
            break;
        }
    }
    tablemodel_2->setFilter(QString("ch_dish_typeno='%1'").arg(tablemodel->model_data(index.row(), 0).toString()));
    ui->tableView_2->restore(0);
    ui->tableView_2->resizeColumnsToContents();
}

void w_bt_dish_base_typeset::toupdateColor(const QModelIndex &index)
{
    if(index.column() == tablemodel_2->fieldIndex("vch_color")) {
        colordialog dialog(this);
        dialog.setWindowTitle(QObject::tr("选择颜色"));
        if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
            tablemodel_2->model_data_set(index, dialog.result_color_name());
        } else {
            if( 1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("操作取消"), QObject::tr("保存之前的颜色"), QObject::tr("清除颜色"))) {
                tablemodel_2->model_data_set(index, "");
            }
        }
    }
}

void w_bt_dish_base_typeset::tonew()
{
    lds_dialog_input3 dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.ui->label_3->hide();dialog.ui->lineEdit_3->hide();
    dialog.ui->label->setText(tablemodel->headerData(tablemodel->fieldIndex("ch_dish_typeno"), Qt::Horizontal).toString());
    dialog.ui->label_2->setText(tablemodel->headerData(tablemodel->fieldIndex("vch_dish_typename"), Qt::Horizontal).toString());
    dialog.ui->lineEdit->setEnabled(false);
    dialog.ui->lineEdit->setText((tablemodel->max_column_add_1__max_sql_add_1_dec("ch_dish_typeno", "%02d")));

    if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
        return;
    }

    if(dialog.ui->lineEdit_2->text().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("大类名称不能为空"));
        return;
    }
    //
    lds_query_hddpos::tran_saction();
    lds_query_hddpos  query;
    QString errstring;
    if(query.execInsert("cey_bt_dish_type",
                        qrtVariantPairList()
                        << qrtVariantPair("ch_dish_typeno", dialog.ui->lineEdit->text())
                        << qrtVariantPair("vch_dish_typename", dialog.ui->lineEdit_2->text()),
                        &errstring
                        )) {
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        tablemodel->select();
        ui->tableView->restore();
        return;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_bt_dish_base_typeset::tochange()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    lds_dialog_input3 dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.ui->label_3->hide();dialog.ui->lineEdit_3->hide();
    dialog.ui->label->setText(tablemodel->headerData(tablemodel->fieldIndex("ch_dish_typeno"), Qt::Horizontal).toString());
    dialog.ui->label_2->setText(tablemodel->headerData(tablemodel->fieldIndex("vch_dish_typename"), Qt::Horizontal).toString());
    dialog.ui->lineEdit->setEnabled(false);
    dialog.ui->lineEdit->setText(tablemodel->model_data(row, "ch_dish_typeno").toString());
    dialog.ui->lineEdit_2->setText(tablemodel->model_data(row, "vch_dish_typename").toString());

    if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
        return;
    }

    if(dialog.ui->lineEdit_2->text().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("大类名称不能为空"));
        return;
    }
    //
    lds_query_hddpos::tran_saction();
    lds_query_hddpos  query;
    QString errstring;
    if(query.execUpdate("cey_bt_dish_type",
                        qrtVariantPairList()
                        << qrtVariantPair("vch_dish_typename", dialog.ui->lineEdit_2->text()),
                        qrtEqual("ch_dish_typeno", dialog.ui->lineEdit->text()),
                        &errstring
                        )) {
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        tablemodel->select();
        ui->tableView->restore();
        return;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
}

void w_bt_dish_base_typeset::todel()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    lds_query_hddpos  query;
    query.execSelect(QString("select count(0) from cey_bt_dish_subtype where ch_dish_typeno = '%1' ")
                     .arg(tablemodel->model_data(row, "ch_dish_typeno").toString()));
    query.next();
    if(query.recordValue(0).toLongLong() > 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("该大类下已有小类"));
        return;
    }

    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"), QObject::tr("是"), QObject::tr("否"))) {
        return;
    }

    lds_query_hddpos::tran_saction();
    if(query.execDelete("cey_bt_dish_type", QString(" ch_dish_typeno = '%1' ;")
                        .arg(tablemodel->model_data(row, "ch_dish_typeno").toString() ))) {
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        tablemodel->select();
        ui->tableView->restore();
        return;
    }
    lds_query_hddpos::roll_back();

    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
}

void w_bt_dish_base_typeset::tosave_show_dish_type(bool f)
{
    lds::sysconf->setValue("system_setting/dish_type_enabled",f);
    ui->widget->setEnabled(f);
}

void w_bt_dish_base_typeset::tosave_dish_color_keep_subtype(bool f)
{
    lds::sysconf->setValue("system_setting/dish_color_keep_subtype",f);
}

void w_bt_dish_base_typeset::replace(const QString &ch_dish_typeno, const QString &vch_dish_typename)
{

    lds_dialog_input inputdialog(this);
    inputdialog.setWindowTitle(_TEXT_SLOT(this));
    inputdialog.ui->label->setText(tablemodel->headerData(tablemodel->fieldIndex("ch_dish_typeno"), Qt::Horizontal).toString());
    inputdialog.ui->label_2->setText(tablemodel->headerData(tablemodel->fieldIndex("vch_dish_typename"), Qt::Horizontal).toString());
    inputdialog.ui->lineEdit->setReadOnly(true);

    inputdialog.showSecondLineEdt();
    inputdialog.hideKey();

    if(inputdialog.ui->lineEdit->text().isEmpty())    inputdialog.ui->lineEdit->setText(ch_dish_typeno);
    if(inputdialog.ui->lineEdit_2->text().isEmpty())    inputdialog.ui->lineEdit_2->setText(vch_dish_typename);

    if(inputdialog.ui->lineEdit->text().isEmpty())    inputdialog.ui->lineEdit->setText((tablemodel->max_column_add_1__max_sql_add_1_dec("ch_dish_typeno", "%02d")));

    if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
        return;
    }

    if(inputdialog.ui->lineEdit_2->text().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("大类名称不能为空"));
        return;
    }
    //
    lds_query_hddpos::tran_saction();
    lds_query_hddpos  query;
    if(query.execInsertDuplicateUpdate("cey_bt_dish_type",
                                       "ch_dish_typeno",
                                       qrtVariantPairList()
                                       << qrtVariantPair("ch_dish_typeno", inputdialog.ui->lineEdit->text())
                                       << qrtVariantPair("vch_dish_typename", inputdialog.ui->lineEdit_2->text()))) {
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        tablemodel->select();
        ui->tableView->restore();
        return;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    //
}

bool w_bt_dish_base_typeset::saveData()
{
    for(int row = 0, rowcount = tablemodel_2->rowCount();row < rowcount;row++) {
        if(tablemodel_2->opera(row) == lds_model_sqltablemodel::OpDelete) continue;

        if(tablemodel_2->model_data(row, 0).toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("行数")+QString::number(row+1)+QObject::tr("小类编号不能为空"));
            return false;
        }
        if(tablemodel_2->model_data(row, 1).toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("行数")+QString::number(row+1)+QObject::tr("小类名称不能为空"));
            return false;
        }
    }
    if(tablemodel_2->trySubmitAll()) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
    return false;
}

//========================

w_bt_dish_base_unitset::w_bt_dish_base_unitset(QWidget *parent):
    QDialog(parent),
    ui(new Ui_w_bt_dish_unitset_Dialog)
{
    ui->setupUi(this);

    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_unit");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_unitno",QObject::tr("编号"));
    tablemodel->save_set_header("vch_unitname",QObject::tr("单位"), true);
    //    tablemodel->save_set_header("ch_numtype",QObject::tr("数量类型"), true);
    tablemodel->save_set_header("num_default",QObject::tr("默认数量"), true);
    tablemodel->enable_vheaderdata_dirty_translate();
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setColumnWidth(tablemodel->fieldIndex("vch_unitname"), 200);

    //获取com的数据代理
    {//单位
        QStringList value_list;
        for(int row = 0, rowcount = w_bt_dish::get_MODEL_UNIT().rowCount(); row < rowcount; row++) {
            value_list.append(w_bt_dish::get_MODEL_UNIT().item(row, 1)->text());
        }
        ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("vch_unitname"),
                                                new lds_model_sqltablemodel_delegate_com(this, value_list));
    }
    //数量
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("num_default"), new lds_tableview_delegate_doublespinbox(2, this));


    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("新增")<<QObject::tr("删除")<<QObject::tr("刷新")<<QObject::tr("保存")<<QObject::tr("退出"),Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(QObject::tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(QObject::tr("刷新")),SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    QString unitlist;
    for(int row = 0,rowcount = w_bt_dish::get_MODEL_UNIT().rowCount();row <rowcount; row++) {
        unitlist+= w_bt_dish::get_MODEL_UNIT().item(row)->text()+",";
    }
    if(lds::get_soft_curent_language_first() == _CN_) {
        ui->label_desc->setMaximumHeight(100);
        ui->label_desc->setText("["+unitlist+"]"+QObject::tr("选择默认单位时,前台称重会自动匹配对应的单位"));
    }

    QTimer::singleShot(10, this, SLOT(torefresh()));
}

void w_bt_dish_base_unitset::tonew()
{
    int rowcount=tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount, 0, tablemodel->max_column_add_1__max_sql_add_1_dec("ch_unitno", "%02d"));
    tablemodel->model_data_set(rowcount, 2, 1);
    tablemodel->model_data_set(rowcount, 3, 1);
    ui->tableView->setCurrentIndex(tablemodel->model_index(rowcount, "vch_unitname"));
}

void w_bt_dish_base_unitset::todel()
{
    int row=ui->tableView->currentIndex().row();
    if(row<0) {
        //没有数据被选中
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
        tablemodel->removeRow(row);
        return;
    }

    QString ls_typeno=tablemodel->model_data(row, 0).toString();
    lds_query_hddpos  query;
    query.execSelect(QString("select count(0) from cey_bt_dish where ch_unitno='%1'").arg(ls_typeno));
    query.next();
    if(query.recordValue(0).toLongLong()>0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("单位已被使用"));
        return;
    }
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"), QObject::tr("是"), QObject::tr("否"))) {
        return;
    }
    tablemodel->removeRow(row);
}

bool w_bt_dish_base_unitset::tosave()
{
    //有的话，则保存
    QString ls_unitno;
    QString ls_unitname;
    for(int r=0,r_count=tablemodel->rowCount();r<r_count;r++) {
        if(tablemodel->opera(r) == lds_model_sqltablemodel::OpDelete) continue;

        ls_unitno=tablemodel->model_data(r, 0).toString();
        ls_unitname=tablemodel->model_data(r, 1).toString();

        if(ls_unitno.trimmed().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，编号不能为空").arg(r + 1));
            return false;
        }
        if(ls_unitname.trimmed().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，单位不能为空").arg(r + 1));
            return false;
        }
    }
    QString errstring;
    if(tablemodel->trySubmitAll(&errstring)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        torefresh();
        return true;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
    return false;
}

void w_bt_dish_base_unitset::torefresh()
{
    tablemodel->select();
    ui->tableView->restore(0);
}

void w_bt_dish_base_unitset::toexit()
{
    if(tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!tosave()) {
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

//======================================

w_bt_dish_base_change::w_bt_dish_base_change(QWidget *parent, lds_tableView *tableview, lds_model_sqltablemodel *tablemodel):
    w_bt_dish_base_new_virtual(tablemodel, parent)
{
    this->tableview = tableview;
}

void w_bt_dish_base_change::setTypeEnabled(bool isenabled)
{
    ui->comboBox_ch_dish_typeno->setEnabled(isenabled);
    ui->comboBox_ch_sub_typeno->setEnabled(isenabled);
}

w_bt_dish_base_change::~w_bt_dish_base_change()
{

}

void w_bt_dish_base_change::refresh()
{
    mapperupdate();
}


void w_bt_dish_base_change::tosave()
{
    saveData();
}

void w_bt_dish_base_change::tocontinue()
{


}

void w_bt_dish_base_change::toexit()
{
    if(isDiry()) {
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

void w_bt_dish_base_change::delayInitData()
{
    w_bt_dish_base_new_virtual::delayInitData();
    //! datewidet
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    mapper->setModel(this->tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    addmapping(this->tablemodel, mapper);

    //! connect
    connect(ui->toolbar->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->toolbar->index_widget(QObject::tr("保存并新增")),SIGNAL(clicked()),this,SLOT(tocontinue()));
    connect(ui->toolbar->index_widget((0)/*上一页*/),SIGNAL(clicked()),this,SLOT(toup()));
    connect(ui->toolbar->index_widget((1)/*下一页*/),SIGNAL(clicked()),this,SLOT(todown()));
    connect(ui->toolbar->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->lineEdit_vch_dishname,SIGNAL(textEdited(QString)),this,SLOT(spellupdate(QString)));
    connect(ui->comboBox_ch_deducttype,SIGNAL(currentIndexChanged(int)),this,SLOT(update_ch_deducttype(int)));
    connect(ui->comboBox_ch_dish_typeno,SIGNAL(currentIndexChanged(int)),this,SLOT(updatesubdishtype()));
    //! init
    frame_setEnable(ui->checkBox_item_flag->isChecked());
    frame2_setEnable(ui->checkBox_ch_deductflag->isChecked());
    //! after
    ui->toolbar->index_widget(QObject::tr("保存并新增"))->hide();
    ui->doubleSpinBox_num_m_price->setVisible(true);
    ui->lineEdit_ch_dishno->setEnabled(false);
    enableDishImage(!ui->lineEdit_ch_dishno->isEnabled());
    static_cast<QPushButton*>(ui->toolbar->index_widget((0)/*上一页*/))->setDefault(false);static_cast<QPushButton*>(ui->toolbar->index_widget((0)/*上一页*/))->setAutoDefault(false);
    static_cast<QPushButton*>(ui->toolbar->index_widget((1)/*下一页*/))->setDefault(false);static_cast<QPushButton*>(ui->toolbar->index_widget((1)/*下一页*/))->setAutoDefault(false);
    static_cast<QPushButton*>(ui->toolbar->index_widget(QObject::tr("保存")))->setDefault(false);static_cast<QPushButton*>(ui->toolbar->index_widget(QObject::tr("保存")))->setAutoDefault(false);
    static_cast<QPushButton*>(ui->toolbar->index_widget(QObject::tr("保存并新增")))->setDefault(false);static_cast<QPushButton*>(ui->toolbar->index_widget(QObject::tr("保存并新增")))->setAutoDefault(false);
    static_cast<QPushButton*>(ui->toolbar->index_widget(QObject::tr("退出")))->setDefault(false);static_cast<QPushButton*>(ui->toolbar->index_widget(QObject::tr("退出")))->setAutoDefault(false);
    //! TIMESLOT
    mapperupdate();
    updatesubdishtype();
    setTypeEnabled(true);
}

void w_bt_dish_base_change::toup()
{
    if(isDiry()) {
        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"))) {
            if(!saveData()) return;
        }
        tablemodel->select();
    }
    tableview->goprevious();
    mapperupdate();
}

void w_bt_dish_base_change::todown()
{
    if(isDiry()) {
        if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"))) {
            if(!saveData()) return;
        }
        tablemodel->select();
    }
    tableview->gonext();
    mapperupdate();
}

void w_bt_dish_base_change::mapperupdate()
{
    ui->label_record->setText(QString("%1/%2").arg(tableview->currentIndex().row()+1).arg(tableview->verticalHeader()->count()));
    mapper->setCurrentIndex(tableview->currentIndex().row());
    dishImageupdate();
    //原料刷新
    dish_item_refresh();
    item_count_weight_refresh();
}

bool w_bt_dish_base_change::saveData()
{
    //2
    if(!saveCheck())
        return false;
    //3
    lds_query_hddpos::tran_saction();
    if(tablemodel->trySubmitAll()
            &&dish_item_save()) {
        //父类刷新，包括 父类 model 刷新
        lds_query_hddpos::com_mit();
        emit signal_parent_to_refresh();
        mapperupdate();
        //成功信息
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    }
    lds_query_hddpos::roll_back();
    //错误提示
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
    return false;
}


//=====================================
w_bt_dish_base_new::w_bt_dish_base_new(QWidget *parent, const QString &ch_dish_typeno, const QString &ch_sub_typeno, const QString &ch_groupno, const QString &ch_tempflag):
    w_bt_dish_base_new_virtual(0, parent)
{
    this->ch_dish_typeno = ch_dish_typeno;
    this->ch_sub_typeno = ch_sub_typeno;
    this->ch_groupno = ch_groupno;
    this->ch_tempflag = ch_tempflag;

    //    connect(ui->lineEdit_vch_barcode, SIGNAL(returnPressed()), this, SLOT(to_product_find_online_by_barcode()));
}

w_bt_dish_base_new::~w_bt_dish_base_new()
{

}

void w_bt_dish_base_new::tosave()
{
    if(saveData()) {
        this->accept();
    }
}

void w_bt_dish_base_new::tocontinue()
{
    //2
    if(isDiry()) {
        if(!saveData())return;
    }
    ui->lineEdit_ch_dishno->setEnabled(true);
    ui->lineEdit_ch_groupno->clear();
    enableDishImage(!ui->lineEdit_ch_dishno->isEnabled());
    //3将厨打model维持在一行
    tonewrefresh();
}

void w_bt_dish_base_new::toexit()
{
    this->reject();
}

void w_bt_dish_base_new::delayInitData()
{
    w_bt_dish_base_new_virtual::delayInitData();

    //1
    tablemodel=w_bt_dish_base::creatmodel(this);
    tablemodel->select();

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    addmapping(tablemodel, mapper);
    //! connect
    connect(ui->toolbar->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->toolbar->index_widget(QObject::tr("保存并新增")), SIGNAL(clicked()),this,SLOT(tocontinue()));
    connect(ui->toolbar->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->lineEdit_vch_dishname,SIGNAL(textEdited(QString)),this,SLOT(spellupdate(QString)));
    connect(ui->comboBox_ch_deducttype,SIGNAL(currentIndexChanged(int)),this,SLOT(update_ch_deducttype(int)));
    connect(ui->comboBox_ch_dish_typeno,SIGNAL(currentIndexChanged(int)),this,SLOT(toupdatedishnoWithmax1()));
    connect(ui->comboBox_ch_dish_typeno,SIGNAL(currentIndexChanged(int)),this,SLOT(updatesubdishtypeSubmit()));
    connect(ui->toolbar->index_widget(QObject::tr("刷新")),SIGNAL(clicked()),this,SLOT(tokeeprefresh()));
    //! init
    ui->toolbar->index_widget((0)/*上一页*/)->hide();
    ui->toolbar->index_widget((1)/*下一页*/)->hide();
    ui->label_record->setVisible(false);
    ui->lineEdit_ch_dishno->setEnabled(true);
    enableDishImage(!ui->lineEdit_ch_dishno->isEnabled());
    frame2_setEnable(ui->checkBox_ch_deductflag->isChecked());
    frame_setEnable(ui->checkBox_ch_tempflag->isChecked());
    update_ch_deducttype(ui->comboBox_ch_deducttype->currentIndex());
    if(ch_dish_typeno.isEmpty()) ui->comboBox_ch_dish_typeno->setCurrentIndex(0);
    else ui->comboBox_ch_dish_typeno->setcomusrdata(ch_dish_typeno);
    if(ch_sub_typeno.isEmpty()) ui->comboBox_ch_sub_typeno->setCurrentIndex(0);
    else ui->comboBox_ch_sub_typeno->setcomusrdata(ch_sub_typeno);
    if(ch_tempflag.length() > 0) ui->checkBox_ch_tempflag->setChecked1(ch_tempflag);

    if(ch_groupno.length() > 0) {
        ui->lineEdit_ch_groupno->setEnabled(false);
        ui->lineEdit_ch_groupno->setText(ch_groupno);
    }

    ui->comboBox_ch_unitno->setcomusrdata(ui->comboBox_ch_unitno->comusrdata(0));
    ui->comboBox_vch_pur_unitno->setcomusrdata(ui->comboBox_vch_pur_unitno->comusrdata(0));

    tokeeprefresh();
}

void w_bt_dish_base_new::toupdatedishnoWithmax1()
{
    //1
    QString ch_dish_typeno = ui->comboBox_ch_dish_typeno->comusrdata().toString();
    QString nextno=ch_dish_typeno+"000";
    QString maxno;

    lds_query_hddpos  query;
    query.execSelect(QString("select max(ch_dishno) from cey_bt_dish where ch_dishno like '%1%' ")
                     .arg(ch_dish_typeno)
                     );
    query.next();
    maxno = query.recordValue(0).toString();
    nextno = ch_dish_typeno + QString().sprintf("%03d",maxno.right(3).toInt()+1);
    //2
    editorsetValue(ui->lineEdit_ch_dishno, nextno,  "ch_dishno");
}

void w_bt_dish_base_new::tokeeprefresh()
{
    //1
    QString ch_dishno;
    if(!ui->lineEdit_ch_dishno->isEnabled()) {
        ch_dishno  = ui->lineEdit_ch_dishno->text();
    }
    //2
    torefresh();
    //3
    if(!ch_dishno.isEmpty()) {
        editorsetValue(ui->lineEdit_ch_dishno, ch_dishno, "ch_dishno");
    }
    //    if(ui->lineEdit_num_item_warn->text().isEmpty()) {
    //        editorsetValue(ui->lineEdit_num_item_warn, "0", "num_item_warn");
    //    }
    if(ui->spinBox_int_unit_rate->value() == 0) {
        editorsetValue(ui->spinBox_int_unit_rate, 1, "int_unit_rate");
    }

    if(n_func::f_get_sysparm("pay_fixed_tax", "0") == "1") {
        QString ch_serviceflag =n_func::f_get_sysparm("pay_fixed_service", "0") == "1" ? "Y" : "N";

        editorsetValue(ui->doubleSpinBox_int_rate, n_func::f_get_sysparm("pay_fixed_tax_value", 0).toInt(), "int_rate");
        editorsetValue(ui->doubleSpinBox_int_rate, n_func::f_get_sysparm("pay_fixed_tax_2_value", 0).toInt(), "int_rate_2");
        editorsetValue(ui->doubleSpinBox_int_rate, n_func::f_get_sysparm("pay_fixed_tax_3_value", 0).toInt(), "int_rate_3");
        editorsetValue(ui->checkBox_ch_serviceflag, ch_serviceflag, "ch_serviceflag");
    }
}

void w_bt_dish_base_new::tonewrefresh()
{
    torefresh();
    toupdatedishnoWithmax1();
    torefresh();
}

void w_bt_dish_base_new::torefresh()
{
    QString ch_dish_typeno =ui->comboBox_ch_dish_typeno->comusrdata().toString();
    QString ch_sub_typeno =ui->comboBox_ch_sub_typeno->comusrdata().toString();
    QString ch_unitno =ui->comboBox_ch_unitno->comusrdata().toString();
    QString vch_pur_unitno =ui->comboBox_vch_pur_unitno->comusrdata().toString();
    QString ch_tempflag = ui->checkBox_ch_tempflag->isChecked1();
    QString ch_groupno =ui->lineEdit_ch_groupno->text();
    QString item_flag = "N";
#ifdef QT_SCPOS_SUPPORT
    item_flag = "Y";
#endif
    if(lds::sysconf->value("w_sys_manage_outer_pay_set/scale_led_mode", false).toBool()) {
        item_flag = "Y";
    }

    //2
    tablemodel->setFilter(QString("ch_dishno = '%1' ").arg(ui->lineEdit_ch_dishno->text()));
    if(tablemodel->rowCount() == 0) {
        tablemodel->insertRow(0);
    }
    mapper->setCurrentIndex(0);
    mapper->submit();
    //3
    editorsetValue(ui->checkBox_ch_discount, "Y", "ch_discount");
    if(!ui->checkBox_ch_tempflag->isEnabled()) {
        editorsetValue(ui->checkBox_ch_tempflag, "Y", "ch_tempflag");
    }
    editorsetValue(ui->comboBox_ch_dish_typeno, ch_dish_typeno, "ch_dish_typeno");
    editorsetValue(ui->comboBox_ch_sub_typeno, ch_sub_typeno, "ch_sub_typeno");
    editorsetValue(ui->comboBox_ch_unitno,ch_unitno,"ch_unitno");
    editorsetValue(ui->comboBox_vch_pur_unitno,vch_pur_unitno,"vch_pur_unitno");
    editorsetValue(ui->checkBox_ch_tempflag, ch_tempflag, "ch_tempflag");
    editorsetValue(ui->lineEdit_ch_groupno, ch_groupno, "ch_groupno");
    editorsetValue(ui->checkBox_item_flag, item_flag, "item_flag");
    //原料刷新
    dish_item_refresh();
}

bool w_bt_dish_base_new::saveData()
{
    //2
    if(!saveCheck()) return false;
    //3
    lds_query_hddpos::tran_saction();
    if(tablemodel->trySubmitAll()
            &&dish_item_save()) {
        lds_query_hddpos::com_mit();
        ui->lineEdit_ch_dishno->setEnabled(false);
        enableDishImage(!ui->lineEdit_ch_dishno->isEnabled());
        emit signalsave();
        tablemodel->setFilter(QString("ch_dishno = '%1' ").arg(ui->lineEdit_ch_dishno->text()));
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    } else {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误")+QObject::tr("或者编号有重复") + QString::number(__LINE__));
        return false;
    }
}

//=========================
w_bt_dish_base_dish_type::w_bt_dish_base_dish_type(w_bt_dish_base *parent):
    QDialog(parent),
    ui(new Ui_w_bt_dish_unitset_Dialog),
    _parent(parent)
{
    ui->setupUi(this);
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_dish_type");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_dish_typeno", QObject::tr("大类编号"));
    tablemodel->save_set_header("vch_dish_typename", QObject::tr("大类名称"), true);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("新增")<<QObject::tr("删除")<<QObject::tr("刷新")<<QObject::tr("保存")<<QObject::tr("退出"),Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(QObject::tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(QObject::tr("刷新")),SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));

    lds_query_hddpos  query;
    query.execSelect("select count(0) from cey_bt_dish_type");
    query.next();
    if(0 == query.recordValue(0).toLongLong()) {
        if(!query.execInsert("cey_bt_dish_type",qrtVariantPairList()

                             << qrtVariantPair("ch_dish_typeno", "11")
                             << qrtVariantPair("vch_dish_typename", QObject::tr("默认大类")))) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+QString::number(__LINE__));
        }
    }
    //!refresh
    torefresh();
}

void w_bt_dish_base_dish_type::tonew()
{
    int rowcount=tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount,  "ch_dish_typeno",  tablemodel->max_column_add_1__max_sql_add_1_dec("ch_dish_typeno", "%02d"));

    ui->tableView->setCurrentIndex(tablemodel->model_index(rowcount, "vch_dish_typename"));
}

void w_bt_dish_base_dish_type::todel()
{
    lds_query_hddpos  query;
    query.execSelect(QString("select count(0) from cey_bt_dish_subtype where ch_dish_typeno = '%1' ")
                     .arg(tablemodel->model_data(ui->tableView->currentIndex().row(), "ch_dish_typeno").toString()));
    query.next();
    if(query.recordValue(0).toLongLong() > 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("该大类下已有小类"));
        return;
    }
    tablemodel->removeRow(ui->tableView->currentIndex().row());
    tablemodel->trySubmitAll();
}

bool w_bt_dish_base_dish_type::tosave()
{
    if(tablemodel->trySubmitAll()) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
        return false;
    }
}

void w_bt_dish_base_dish_type::torefresh()
{
    tablemodel->select();
    if(tablemodel->rowCount() > 0) {
        ui->tableView->selectRow(0);
    }
}

void w_bt_dish_base_dish_type::toexit()
{
    if(tablemodel->model_is_dirty()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0:
            if(!tosave()) {
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

w_bt_dish_base_new_virtual::w_bt_dish_base_new_virtual(lds_model_sqltablemodel *tablemodel, QWidget *parent)
    : QDialog(parent)
{
    this->tablemodel = tablemodel;
    ui = new Ui_w_bt_dish_new_dialog_2;
    ui->setupUi(this);
    resize(800, 560);
    ui->tableView_pic->updateFixed(1,1);
    ui->tableView_pic->setProperty("outer_stylesheet", "tableView_dish");
    ui->doubleSpinBox_num_count_weight->setDecimals(3);
    ui->checkBox_ch_serviceflag->setDisabled(n_func::f_get_sysparm("pay_fixed_service", "0") == "1");
    ui->doubleSpinBox_int_rate->setDisabled(n_func::f_get_sysparm("pay_fixed_tax", "0") == "1");
    ui->doubleSpinBox_int_rate_2->setDisabled(n_func::f_get_sysparm("pay_fixed_tax", "0") == "1");
    ui->doubleSpinBox_int_rate_3->setDisabled(n_func::f_get_sysparm("pay_fixed_tax", "0") == "1");
    ui->label_count_weight_unit->setText("KG");
    ui->lineEdit_ch_dishno->setMaxLength(7);
#ifdef QT_SCPOS_SUPPORT
    lds::hideWidget(ui->pushButton_item_set);
    lds::hideWidget(ui->checkBox_ch_material_flag);
#endif
    showTaxWidget(false);
    lds::showWidget(ui->label_loading, false);

    QTimer::singleShot(100, this, SLOT(delayInitData()));
}

w_bt_dish_base_new_virtual::~w_bt_dish_base_new_virtual()
{
    delete ui;
    delete model_dish;
}

void w_bt_dish_base_new_virtual::delayInitData()
{
    //tableView_pic
    model_dish = new ftableview_standmodel_sql(ftableview_standmodel_sql::TYPE_icon);
    ui->tableView_pic->setModel(model_dish);
    model_dish_d = new ftableview_delegate(ui->tableView_pic);
    model_dish_d->indexisEmptyDrawGrad = true;
    model_dish_d->setColorLight(3);
    model_dish_d->keepConnect(ui->tableView_pic);
    ui->tableView_pic->setItemDelegate(model_dish_d);
    ui->tableView_pic->fillData_addAnimation(1,1);
    ui->tableView_pic->enable_release_clear_selection(true);
    model_dish->setData(model_dish->index(0, 0), QPixmap(":/image/pig.png"), Qt::UserRole + 2);

    //重新定义key和value的列号
    imagedishdir = lds::sysconf->value("system_setting/bofanglujing").toString();
    //1
    ui->comboBox_vch_pur_unitno->addItemsBySql("select ch_unitno, vch_unitname from cey_bt_unit ");
    ui->comboBox_ch_unitno->addItemsBySql("select ch_unitno, vch_unitname from cey_bt_unit ");
    ui->comboBox_ch_deducttype->addItemsByModel(&w_bt_dish::get_MODEL_DISH_DEDUCTTYPE());
    ui->comboBox_ch_dish_typeno->addItemsBySql("select ch_dish_typeno, vch_dish_typename from cey_bt_dish_type ");
    ui->comboBox_ch_sub_typeno->addItemsBySql("select ch_sub_typeno, vch_sub_typename from cey_bt_dish_subtype ");
    //3
    dish_item_model = 0;
    //
    connect(ui->tableView_pic,SIGNAL(signalclick(QString)),this,SLOT(toupdateDishImage()));
    connect(ui->toolButton_clearpic,SIGNAL(clicked()),this,SLOT(touclearDishImage()));
    connect(ui->pushButton_ch_groupno, SIGNAL(clicked()),this,SLOT(toshow_group_info()));
    connect(ui->pushButton_advanced, SIGNAL(clicked()), this, SLOT(totab_advance()));
    connect(ui->pushButton_basic, SIGNAL(clicked()), this, SLOT(totab_basic()));
    connect(ui->pushButton_item_set, SIGNAL(clicked()), this, SLOT(totab_item_set()));
    connect(ui->checkBox_item_flag, SIGNAL(toggled(bool)),this, SLOT(frame_setEnable(bool)));
    connect(ui->checkBox_ch_deductflag, SIGNAL(toggled(bool)),this, SLOT(frame2_setEnable(bool)));
    connect(ui->checkBox_ch_material_flag, SIGNAL(clicked(QString)), this,SLOT(toupdateItemFlag(QString)));

    connect(ui->pushButton_item_del,SIGNAL(clicked()),this,SLOT(toitemdel()));
    connect(ui->pushButton_item_news,SIGNAL(clicked()),this,SLOT(toitemnews()));
    connect(ui->pushButton_item_change,SIGNAL(clicked()),this,SLOT(toitem_change()));
    connect(ui->doubleSpinBox_num_price,SIGNAL(editingFinished()),this,SLOT(mpriceupdate()));
    connect(ui->toolButton_color,SIGNAL(clicked()),this,SLOT(toupdateDishColor()));

    connect(ui->pushButton_ch_unitno, SIGNAL(clicked(bool)), this,SLOT(to_ch_unitno()));
    connect(ui->pushButton_ch_dish_typeno, SIGNAL(clicked(bool)), this,SLOT(to_ch_dish_typeno()));
    connect(ui->pushButton_ch_sub_typeno, SIGNAL(clicked(bool)), this,SLOT(to_ch_sub_typeno()));
    connect(ui->pushButton_vch_pur_unitno, SIGNAL(clicked(bool)), this,SLOT(to_vch_pur_unitno()));

    connect(ui->checkBox_ch_suitflag, SIGNAL(clicked(bool)), this,SLOT(toSaaSCheck(bool)));
    connect(ui->checkBox_item_flag, SIGNAL(clicked(bool)), this,SLOT(toSaaSCheck(bool)));
    connect(ui->checkBox_ch_material_flag, SIGNAL(clicked(bool)), this,SLOT(toSaaSCheck(bool)));
    connect(ui->checkBox_ch_curprice, SIGNAL(clicked(bool)), this,SLOT(toSaaSCheck(bool)));

    connect(ui->checkBox_ch_tempflag, SIGNAL(clicked(bool)), this,SLOT(toSaaSCheck(bool)));
    connect(ui->checkBox_ch_guqing_flag, SIGNAL(clicked(bool)), this,SLOT(toSaaSCheck(bool)));
    connect(ui->checkBox_ch_serviceflag, SIGNAL(clicked(bool)), this,SLOT(toSaaSCheck(bool)));
    connect(ui->checkBox_ch_deductflag, SIGNAL(clicked(bool)), this,SLOT(toSaaSCheck(bool)));
    connect(ui->checkBox_ch_weight, SIGNAL(clicked(bool)), this, SLOT(toSetCountWeight(bool)));
    connect(ui->checkBox_ch_count, SIGNAL(clicked(bool)), this, SLOT(toSetCountWeight(bool)));

    connect(ui->pushButton_rate_more, SIGNAL(clicked()), this, SLOT(toShowTax()));
    connect(ui->pushButton_rate1, SIGNAL(clicked()), this, SLOT(toChangeTax1Name()));
    connect(ui->pushButton_rate2, SIGNAL(clicked()), this, SLOT(toChangeTax2Name()));
    connect(ui->pushButton_rate3, SIGNAL(clicked()), this, SLOT(toChangeTax3Name()));
    //! toolbar
    ui->toolbar->setup(QStringList()<<QObject::tr("上一行")<<QObject::tr("下一行") << QObject::tr("保存")<<QObject::tr("保存并新增")<<QObject::tr("退出"), Qt::AlignRight);


    //    ui->lineEdit_vch_subno->hide();
    //    ui->label_vch_subno->hide();
    ui->checkBox_ch_lowflag->hide();

    //vch_guqing_num, vch_guqing_sale
    ui->pushButton_basic->setCheckable(true);
    ui->pushButton_basic->disableFrame();

    ui->pushButton_advanced->setCheckable(true);
    ui->pushButton_advanced->disableFrame();

    ui->pushButton_item_set->setCheckable(true);
    ui->pushButton_item_set->disableFrame();

    //init
    showStackedIndex(0);
    ui->pushButton_basic->setChecked(true);

    fexpandmain_2::guqing_try_clear();
    ui->lineEdit_vch_dishname_for_item->setEnabled(false);
    ui->toolButton_color->setIcon(restraurantDelegate::getPixmapByStateList(QList<tableState>()<<tableState_KAITAI <<  tableState_YUDA));

    ui->toolButton_clearpic->setFixedHeight(30);
    ui->toolButton_color->setFixedHeight(30);

    showTaxWidget();
}

void w_bt_dish_base_new_virtual::spellupdate(const QString &v)
{
    QString spell=getChineseSpell::ChineseSpell(v);
    spell=spell.toUpper();
    editorsetValue(ui->lineEdit_vch_spell, spell, "vch_spell");
    //    editorsetValue(ui->lineEdit_vch_subno, spell, "vch_subno");
}

void w_bt_dish_base_new_virtual::mpriceupdate(double v)
{
    editorsetValue(ui->doubleSpinBox_num_m_price, v, "num_m_price");
}

void w_bt_dish_base_new_virtual::mpriceupdate()
{
    mpriceupdate(ui->doubleSpinBox_num_price->value());
}

void w_bt_dish_base_new_virtual::update_ch_deducttype(int type)
{
    if(type==0) {
        ui->spinBox_int_deduct->setEnabled(true);
        ui->spinBox_num_deduct->setEnabled(false);
    } else if(type==1) {
        ui->spinBox_int_deduct->setEnabled(false);
        ui->spinBox_num_deduct->setEnabled(true);
    }
}

void w_bt_dish_base_new_virtual::updatesubdishtype()
{
    QString ud = ui->comboBox_ch_sub_typeno->comusrdata().toString();
    updatesubdishtypemodel(ui->comboBox_ch_dish_typeno->comusrdata().toString());
    ui->comboBox_ch_sub_typeno->setcomusrdata(ud);

}

void w_bt_dish_base_new_virtual::updatesubdishtypeSubmit()
{
    QString ud = ui->comboBox_ch_sub_typeno->comusrdata().toString();
    updatesubdishtypemodel(ui->comboBox_ch_dish_typeno->comusrdata().toString());
    editorsetValue(ui->comboBox_ch_sub_typeno, ud, "ch_sub_typeno");
}

void w_bt_dish_base_new_virtual::updatesubdishtypemodel(const QString &ch_dish_typeno)
{
    ui->comboBox_ch_sub_typeno->clear();
    ui->comboBox_ch_sub_typeno->addItemsBySql(QString("select ch_sub_typeno, vch_sub_typename from cey_bt_dish_subtype where ch_dish_typeno = '%1' ")
                                              .arg(ch_dish_typeno));
}

void w_bt_dish_base_new_virtual::addmapping(lds_model_sqltablemodel *model, QDataWidgetMapper *mapper)
{
    mapper->addMapping(ui->lineEdit_ch_dishno, model->fieldIndex("ch_dishno"));
    mapper->addMapping(ui->spinBox_int_deduct, model->fieldIndex("int_deduct"));
    mapper->addMapping(ui->spinBox_num_deduct, model->fieldIndex("num_deduct"));
    mapper->addMapping(ui->doubleSpinBox_num_m_price, model->fieldIndex("num_m_price"));
    mapper->addMapping(ui->doubleSpinBox_num_price, model->fieldIndex("num_price"));
    mapper->addMapping(ui->spinBox_num_sale_price, model->fieldIndex("num_sale_price"));
    mapper->addMapping(ui->lineEdit_vch_dishname, model->fieldIndex("vch_dishname"));
    mapper->addMapping(ui->lineEdit_vch_dishname_2, model->fieldIndex("vch_dishname_2"));
    mapper->addMapping(ui->lineEdit_vch_spell, model->fieldIndex("vch_spell"));
    //    mapper->addMapping(ui->lineEdit_vch_subno, model->fieldIndex("vch_subno"));
    mapper->addMapping(ui->comboBox_ch_deducttype, 20, "comusrdata");
    mapper->addMapping(ui->comboBox_vch_pur_unitno,28, "comusrdata");
    mapper->addMapping(ui->comboBox_ch_dish_typeno,4, "comusrdata");
    mapper->addMapping(ui->comboBox_ch_sub_typeno,14, "comusrdata");
    mapper->addMapping(ui->checkBox_ch_curprice,model->fieldIndex("ch_curprice"),"checked1");
    mapper->addMapping(ui->checkBox_ch_deductflag,model->fieldIndex("ch_deductflag"),"checked1");
    mapper->addMapping(ui->checkBox_ch_discount,model->fieldIndex("ch_discount"),"checked1");
    mapper->addMapping(ui->checkBox_ch_lowflag,model->fieldIndex("ch_lowflag"),"checked1");
    mapper->addMapping(ui->checkBox_ch_off,model->fieldIndex("ch_off"),"checked1");
    mapper->addMapping(ui->checkBox_ch_serviceflag,model->fieldIndex("ch_serviceflag"),"checked1");
    mapper->addMapping(ui->checkBox_ch_suitflag,model->fieldIndex("ch_suitflag"),"checked1");
    mapper->addMapping(ui->checkBox_item_flag,model->fieldIndex("item_flag"),"checked1");
    mapper->addMapping(ui->checkBox_ch_stopflag,model->fieldIndex("ch_stopflag"),"checked1");
    mapper->addMapping(ui->checkBox_ch_tempflag,model->fieldIndex("ch_tempflag"),"checked1");
    mapper->addMapping(ui->checkBox_ch_weight,model->fieldIndex(("ch_weight")));
    mapper->addMapping(ui->lineEdit_vch_barcode,model->fieldIndex(("vch_barcode")));
    mapper->addMapping(ui->spinBox_int_unit_rate,model->fieldIndex("int_unit_rate"));
    mapper->addMapping(ui->doubleSpinBox_int_rate,model->fieldIndex(("int_rate")));
    mapper->addMapping(ui->doubleSpinBox_int_rate_2,model->fieldIndex(("int_rate_2")));
    mapper->addMapping(ui->doubleSpinBox_int_rate_3,model->fieldIndex(("int_rate_3")));


    mapper->addMapping(ui->comboBox_ch_unitno,model->fieldIndex("ch_unitno"),"comusrdata");
    mapper->addMapping(ui->lineEdit_ch_groupno,model->fieldIndex(("ch_groupno")));
    mapper->addMapping(ui->spinBox_num_item_warn, model->fieldIndex(("num_item_warn")));

    mapper->addMapping(ui->doubleSpinBox_num_guqing_num,model->fieldIndex(("num_guqing_num")));
    mapper->addMapping(ui->checkBox_ch_guqing_flag,model->fieldIndex("ch_guqing_flag"),"checked1");
    mapper->addMapping(ui->checkBox_ch_material_flag,model->fieldIndex("ch_material_flag"),"checked1");
    mapper->addMapping(ui->checkBox_ch_count, model->fieldIndex("ch_count"),"checked1");
    mapper->addMapping(ui->checkBox_ch_addon_flag, model->fieldIndex("ch_addon_flag"),"checked1");
    mapper->addMapping(ui->doubleSpinBox_num_count_weight,model->fieldIndex("num_count_weight"));
}

void w_bt_dish_base_new_virtual::editorsetValue(QLineEdit *editor, const QString &v, const QString &fieldName)
{
    QModelIndex index = tablemodel->index(mapper->currentIndex(), tablemodel->fieldIndex(fieldName));
    if(index.isValid()) {
        QString prevalue = tablemodel->model_data(index).toString();
        if(prevalue != v) {
            editor->setText(v);
            tablemodel->model_data_set(index, v);
        }
    }
}

void w_bt_dish_base_new_virtual::editorsetValue(lds_ComboBox *editor, const QString &v, const QString &fieldName)
{
    QModelIndex index = tablemodel->index(mapper->currentIndex(), tablemodel->fieldIndex(fieldName));
    if(index.isValid()) {
        QString prevalue = tablemodel->model_data(index).toString();
        if(prevalue != v) {
            editor->setcomusrdata(v);
            tablemodel->model_data_set(index, v);
        }
    }
}

void w_bt_dish_base_new_virtual::editorsetValue(lds_ComboBox *editor, int v, const QString &fieldName)
{
    QModelIndex index = tablemodel->index(mapper->currentIndex(), tablemodel->fieldIndex(fieldName));
    if(index.isValid()) {
        int prevalue = tablemodel->model_data(index).toInt();
        if(prevalue != v) {
            editor->setCurrentIndex(v);
            tablemodel->model_data_set(index, editor->comusrdata());
        }
    }
}

void w_bt_dish_base_new_virtual::editorsetValue(lds_CheckBox *editor, const QString &v, const QString &fieldName)
{
    QModelIndex index = tablemodel->index(mapper->currentIndex(), tablemodel->fieldIndex(fieldName));
    if(index.isValid()) {
        QString prevalue = tablemodel->model_data(index).toString();
        if(prevalue != v) {
            tablemodel->model_data_set(index, v);
            editor->setChecked1(v);
        }
    }
}

void w_bt_dish_base_new_virtual::editorsetValue(lds_checkbox_unwrap *editor, const QString &v, const QString &fieldName)
{
    QModelIndex index = tablemodel->index(mapper->currentIndex(), tablemodel->fieldIndex(fieldName));
    if(index.isValid()) {
        QString prevalue = tablemodel->model_data(index).toString();
        if(prevalue != v) {
            tablemodel->model_data_set(index, v);
            editor->setChecked1(v);
        }
    }
}

void w_bt_dish_base_new_virtual::editorsetValue(QDoubleSpinBox *editor, double v, const QString &fieldName)
{
    QModelIndex index = tablemodel->index(mapper->currentIndex(), tablemodel->fieldIndex(fieldName));
    if(index.isValid()) {
        double prevalue = tablemodel->model_data(index).toDouble();
        if(prevalue != v) {
            editor->setValue(v);
            tablemodel->model_data_set(index, v);
        }
    }
}

void w_bt_dish_base_new_virtual::editorsetValue(QSpinBox *editor, int v, const QString &fieldName)
{
    QModelIndex index = tablemodel->index(mapper->currentIndex(), tablemodel->fieldIndex(fieldName));
    if(index.isValid()) {
        int prevalue = tablemodel->model_data(index).toInt();
        if(prevalue != v) {
            editor->setValue(v);
            tablemodel->model_data_set(index, v);
        }
    }
}

bool w_bt_dish_base_new_virtual::isDiry()
{
    return tablemodel->model_is_dirty()
            || (dish_item_model==0 ? false : dish_item_model->model_is_dirty());
}

bool w_bt_dish_base_new_virtual::saveCheck()
{
    lds_query_hddpos  query;

    if(!ui->lineEdit_vch_barcode->text().isEmpty()) {
        query.execSelect(QString("select count(0) from cey_bt_dish where vch_barcode = '%1' and ch_dishno <> '%2' ")
                         .arg(ui->lineEdit_vch_barcode->text())
                         .arg(ui->lineEdit_ch_dishno->text())
                         );
        query.next();
        if(query.recordValue(0).toLongLong() > 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("条码不能重复"));
            return false;
        }
    }
    //开头不能为0
#ifdef QT_SCPOS_SUPPORT
#else
    if(ui->lineEdit_ch_dishno->text().startsWith("0")) {
        if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号开头为0，若使用点菜宝则禁止编号开头为0，是否继续"), QObject::tr("继续"), QObject::tr("取消"))) {
            return false;
        }
    }
    if(ui->lineEdit_ch_dishno->text().length() != 5) {
        if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号长度不足5位，若使用点菜宝则长度必须为5位，是否继续"), QObject::tr("继续"), QObject::tr("取消"))) {
            return false;
        }
    }
#endif
    if(ui->lineEdit_ch_dishno->text().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号不能为空"));
        return false;
    }
    if(b_enable_material
            && ui->comboBox_ch_dish_typeno->comusrdata().toString().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("大类不能为空"));
        return false;
    }
    if(b_enable_material
            && ui->comboBox_ch_sub_typeno->comusrdata().toString().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("小类不能为空"));
        return false;
    }
    if(ui->lineEdit_vch_dishname->text().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("名称不能为空"));
        return false;
    }
    if(ui->comboBox_ch_unitno->currentText().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("单位不能为空"));
        return false;
    }
    if(ui->lineEdit_ch_groupno->text().trimmed().isEmpty()) {
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("组号不能为空，将改为编号？"), QObject::tr("确认"), QObject::tr("取消"))) {
            return false;
        }
        editorsetValue(ui->lineEdit_ch_groupno, ui->lineEdit_ch_dishno->text(), "ch_groupno");
    }
    query.execSelect(QString("SELECT count(0) from cey_bt_dish where ch_dishno = '%1' ")
                     .arg(ui->lineEdit_ch_groupno->text()));
    query.next();
    if(query.recordValue(0).toInt() == 0) {
        if(ui->lineEdit_ch_groupno->text() != ui->lineEdit_ch_dishno->text()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("组号必须为编号之一"));
            return false;
        }
    }

    if(dish_item_model) {
        for(int row  = 0; row < dish_item_model->rowCount(); row++) {
            if(dish_item_model->opera(row) != lds_model_sqltablemodel::OpDelete
                    && dish_item_model->model_data(row, "ch_itemno").toString().isEmpty()) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("原料不能空"));
                return false;
            }
        }
    }
    if(ui->checkBox_item_flag->isChecked() && ui->spinBox_int_unit_rate->value() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("大小包装比率不能等于零"));
        return false;
    }

    //
    bool material_has_be_added = false;
    if(dish_item_model) {
        if(dish_item_model->rowCountOp(lds_model_sqltablemodel::OpExceptDelete) > 0) {
            material_has_be_added = true;
        }
    }
    if(0 == dish_item_model) {
        lds_query_hddpos  query;
        query.execSelect(QString("SELECT ch_itemno from material_bt_dish where ch_dishno = '%1';  ")
                         .arg(ui->lineEdit_ch_dishno->text()));
        if(query.next()) {
            material_has_be_added = true;
        }
    }
    if(ui->checkBox_ch_material_flag->isChecked() && material_has_be_added) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("原料不能添加原料"));
        return false;
    }
    if(ui->checkBox_ch_weight->isChecked()
            && ui->checkBox_ch_count->isChecked()) {
        if(ui->doubleSpinBox_num_count_weight->value() == 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("计数称重不能等于零"));
            return false;
        }
    }
    return true;
}

void w_bt_dish_base_new_virtual::dishImageupdate()
{
    QPixmap p(imagedishdir+"/"+ui->lineEdit_ch_dishno->text());
    if(p.isNull()) {
        model_dish->setData(model_dish->index(0, 0), QPixmap(":/image/pig.png"), Qt::UserRole + 2);
    } else {
        model_dish->setData(model_dish->index(0, 0), p, Qt::UserRole + 2);
    }
    model_dish->setData(model_dish->index(0, 0), ui->lineEdit_ch_dishno->text(), Qt::UserRole + 1);
    model_dish->setData(model_dish->index(0, 0), ui->lineEdit_vch_dishname->text(), Qt::UserRole);
    model_dish->model_data_set(0, 0, tablemodel->model_data(mapper->currentIndex(), "vch_color"), Qt::UserRole + 9);
}

void w_bt_dish_base_new_virtual::enableDishImage(bool enabled)
{
    Q_UNUSED(enabled);
    model_dish->model_data_set(0, 0, tablemodel->model_data(mapper->currentIndex(), "vch_color"), Qt::UserRole + 9);
    //    ui->tableView_pic->setEnabled(enabled);
    //    ui->toolButton_clearpic->setEnabled(enabled);
    //    ui->toolButton_color->setEnabled(enabled);
}

void w_bt_dish_base_new_virtual::toshow_group_info()
{
    lds_query_hddpos  query;
    query.execSelect(QString(" select ch_dishno, vch_dishname from cey_bt_dish  where ch_groupno = '%1' and ch_groupno <> ''  "
                             " and ch_stopflag <> 'Y' and ch_tempflag <> 'Y' "
                             )
                     .arg(ui->lineEdit_ch_groupno->text()));
    QString ch_groupno_info;
    while(query.next()) {
        ch_groupno_info += query.recordValue("vch_dishname").toString()
                + "-" + query.recordValue("ch_dishno").toString() + "\n";
    }
    if(!ch_groupno_info.isEmpty()) ch_groupno_info.chop(1);

    lds_messagebox::information(this, MESSAGE_TITLE_VOID,  QObject::tr("当前组号信息") + "\n" +  ch_groupno_info);
}

void w_bt_dish_base_new_virtual::totab_advance()
{
    if(ui->pushButton_advanced->isChecked() == false) {
        ui->pushButton_advanced->setChecked(true);
    }
    ui->pushButton_basic->setChecked(false);
    ui->pushButton_item_set->setChecked(false);

    showStackedIndex(1);
    item_count_weight_refresh();
}

void w_bt_dish_base_new_virtual::totab_basic()
{
    if(ui->pushButton_basic->isChecked() == false) {
        ui->pushButton_basic->setChecked(true);
    }
    ui->pushButton_advanced->setChecked(false);
    ui->pushButton_item_set->setChecked(false);
    showStackedIndex(0);
}

void w_bt_dish_base_new_virtual::totab_item_set()
{
    if(ui->pushButton_item_set->isChecked() == false) {
        ui->pushButton_item_set->setChecked(true);
    }
    if(dish_item_model == 0) {
        dish_item_model = new lds_model_sqltablemodel(this);
        dish_item_model->setTable("material_bt_dish");
        dish_item_model->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
        dish_item_model->save_set_header(dish_item_model->fieldIndex("ch_itemno"), QObject::tr("原料"));
        dish_item_model->save_set_header(dish_item_model->fieldIndex("num_num"), QObject::tr("数量"), true);
        dish_item_model->enable_vheaderdata_dirty_translate();

        ui->tableView->setModel(dish_item_model);
        ui->tableView->justShowColumns(QList<int>()
                                       << dish_item_model->fieldIndex("ch_itemno")
                                       << dish_item_model->fieldIndex("num_num"));
        ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
        ui->tableView->rePecifyHeader(dish_item_model->get_save_set_header_saver_column_list());
        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->setColumnWidth(dish_item_model->fieldIndex("ch_itemno"), 500);
        ui->tableView->setItemDelegateForColumn(dish_item_model->fieldIndex("ch_itemno"),
                                                new lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish", "ch_dishno", "vch_dishname"));
        ui->tableView->setItemDelegateForColumn(dish_item_model->fieldIndex("num_num"),
                                                new lds_tableview_delegate_doublespinbox(3, this));
    }
    dish_item_model->setFilter(QString(" ch_dishno = '%1' ").arg(ui->lineEdit_ch_dishno->text()));

    QString vch_dishname;
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT ch_dishno, vch_dishname, ch_material_flag  from cey_bt_dish where ch_dishno = '%1' ").arg(ui->lineEdit_ch_dishno->text()));
    if(!query.next()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请先保存"));
        goto return_to_basic;
    }
    vch_dishname = query.recordValue("vch_dishname").toString();


    goto next;
return_to_basic:
    totab_basic();
    return;
next:
    ui->lineEdit_vch_dishname_for_item->setText(vch_dishname);
    ui->pushButton_advanced->setChecked(false);
    ui->pushButton_basic->setChecked(false);
    showStackedIndex(2);
}

void w_bt_dish_base_new_virtual::toitemdel()
{
    int row = ui->tableView->currentIndex().row();
    int column = ui->tableView->currentIndex().column();
    if(row >= 0) {
        //删除部分
        dish_item_model->removeRow(row);
        if(dish_item_model->opera(row) == lds_model_sqltablemodel::OpDelete) {//new的del,会跳过
            int selectrow = qMin(row+1, dish_item_model->rowCount());
            ui->tableView->setCurrentIndex(dish_item_model->index(selectrow, column));
        }
    }
}

void w_bt_dish_base_new_virtual::toitemnews()
{
    w_st_good_input_select_dish dialog(this);
    dialog.setWindowTitle(QObject::tr("物料"));
    dialog.filter_by_material();
    if(QDialog::Accepted ==transparentCenterDialog(&dialog).exec()) {
        foreach(int k, dialog.ret_row_list) {
            int rowcount = dish_item_model->rowCount();
            dish_item_model->insertRow(rowcount);
            dish_item_model->model_data_set(rowcount, "ch_dishno", ui->lineEdit_ch_dishno->text());
            dish_item_model->model_data_set(rowcount, "ch_itemno", dialog.get_tablemodel_value(k, "ch_dishno"));
            dish_item_model->model_data_set(rowcount, "num_num", 1);
        }
    }
}

void w_bt_dish_base_new_virtual::toitem_change()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    w_bt_dish_base_change_by_ch_dishno dialog(this, dish_item_model->model_data(row, "ch_itemno").toString());
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.ui->checkBox_ch_material_flag->setEnabled(false);
    dialog.ui->pushButton_item_set->hide();
    transparentCenterDialog(&dialog).exec();
}

bool w_bt_dish_base_new_virtual::dish_item_save()
{
    return (dish_item_model==0? true : dish_item_model->trySubmitAll());
}

void w_bt_dish_base_new_virtual::dish_item_refresh()
{
    if(ui->pushButton_item_set->isChecked())
        dish_item_model->setFilter(QString("ch_dishno = '%1' ").arg(ui->lineEdit_ch_dishno->text()));
}

void w_bt_dish_base_new_virtual::item_count_weight_refresh()
{
    if(ui->checkBox_ch_count->isChecked()) {
        lds::showWidget(ui->checkBox_ch_weight, false);
        lds::showWidget(ui->checkBox_ch_count, true);
        lds::showWidget(ui->doubleSpinBox_num_count_weight, true);
        lds::showWidget(ui->label_count_weight_unit, true);
    } else {
        lds::showWidget(ui->checkBox_ch_weight, true);
        lds::showWidget(ui->checkBox_ch_count, false);
        lds::showWidget(ui->doubleSpinBox_num_count_weight, false);
        lds::showWidget(ui->label_count_weight_unit, false);
    }
    ui->label_count_weight_unit->setText("G");
}

void w_bt_dish_base_new_virtual::enable_material()
{
    b_enable_material = true;
}

void w_bt_dish_base_new_virtual::frame_setEnable(bool e)
{
    ui->pushButton_vch_pur_unitno->setEnabled(e);
    ui->comboBox_vch_pur_unitno->setEnabled(e);

    ui->label_16->setEnabled(e);
    ui->spinBox_num_sale_price->setEnabled(e);

    ui->label_int_unit_rate->setEnabled(e);
    ui->spinBox_int_unit_rate->setEnabled(e);

    ui->label_2->setEnabled(e);
    ui->spinBox_num_item_warn->setEnabled(e);

}

void w_bt_dish_base_new_virtual::frame2_setEnable(bool e)
{
    ui->label_11->setEnabled(e);
    ui->comboBox_ch_deducttype->setEnabled(e);

    ui->label_12->setEnabled(e);
    ui->spinBox_int_deduct->setEnabled(e);

    ui->label_13->setEnabled(e);
    ui->spinBox_num_deduct->setEnabled(e);
}

void w_bt_dish_base_new_virtual::to_ch_unitno()
{
    w_bt_dish_base_unitset dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();


    //save
    QVariant unitno = ui->comboBox_ch_unitno->curusrdata();
    QVariant pur_unitno = ui->comboBox_vch_pur_unitno->curusrdata();

    //set
    ui->comboBox_vch_pur_unitno->clear();
    ui->comboBox_vch_pur_unitno->addItemsBySql("select ch_unitno, vch_unitname from cey_bt_unit ");
    ui->comboBox_ch_unitno->clear();
    ui->comboBox_ch_unitno->addItemsBySql("select ch_unitno, vch_unitname from cey_bt_unit ");

    //restore
    ui->comboBox_ch_unitno->setcomusrdata(unitno);
    ui->comboBox_vch_pur_unitno->setcomusrdata(pur_unitno);

}

void w_bt_dish_base_new_virtual::to_ch_dish_typeno()
{
    w_bt_dish_base_typeset dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();

    //save
    QVariant typeno = ui->comboBox_ch_dish_typeno->curusrdata();
    QVariant subtypeno = ui->comboBox_ch_sub_typeno->curusrdata();
    //1
    ui->comboBox_ch_dish_typeno->clear();
    ui->comboBox_ch_dish_typeno->addItemsBySql("select ch_dish_typeno, vch_dish_typename from cey_bt_dish_type ");
    ui->comboBox_ch_sub_typeno->clear();
    ui->comboBox_ch_sub_typeno->addItemsBySql("select ch_sub_typeno, vch_sub_typename from cey_bt_dish_subtype ");
    //restore
    ui->comboBox_ch_dish_typeno->setcomusrdata(typeno);
    ui->comboBox_ch_sub_typeno->setcomusrdata(subtypeno);

}

void w_bt_dish_base_new_virtual::to_ch_sub_typeno()
{
    to_ch_dish_typeno();
}

void w_bt_dish_base_new_virtual::to_vch_pur_unitno()
{
    to_ch_unitno();
}

void w_bt_dish_base_new_virtual::toSaaSCheck(bool check)
{
    if(false == check) return;

    if(ui->checkBox_ch_suitflag == this->sender()
            && !saas_pay_detail::SaaSFuncCheck(this, "0401"/*组合商品*/)) {
        qobject_cast<QCheckBox *>(this->sender())->setChecked(false);
        return;
    }
    if(ui->checkBox_item_flag == this->sender()
            && !saas_pay_detail::SaaSFuncCheck(this, "0402"/*库存管理*/)) {
        qobject_cast<QCheckBox *>(this->sender())->setChecked(false);
        return;
    }
    if(ui->checkBox_ch_material_flag == this->sender()
            && !saas_pay_detail::SaaSFuncCheck(this, "0403"/*商品原料*/)) {
        qobject_cast<QCheckBox *>(this->sender())->setChecked(false);
        return;
    }
    if(ui->checkBox_ch_curprice == this->sender()
            && !saas_pay_detail::SaaSFuncCheck(this, "0404"/*时价商品*/)) {
        qobject_cast<QCheckBox *>(this->sender())->setChecked(false);
        return;
    }
    if(ui->checkBox_ch_tempflag == this->sender()
            && !saas_pay_detail::SaaSFuncCheck(this, "0405"/*临时商品*/)) {
        qobject_cast<QCheckBox *>(this->sender())->setChecked(false);
        return;
    }
    if(ui->checkBox_ch_guqing_flag == this->sender()
            && !saas_pay_detail::SaaSFuncCheck(this, "0406"/*商品沽清*/)) {
        qobject_cast<QCheckBox *>(this->sender())->setChecked(false);
        return;
    }
    if(ui->checkBox_ch_serviceflag == this->sender()
            && !saas_pay_detail::SaaSFuncCheck(this, "0407"/*商品服务费*/)) {
        qobject_cast<QCheckBox *>(this->sender())->setChecked(false);
        return;
    }
    if(ui->checkBox_ch_deductflag == this->sender()
            && !saas_pay_detail::SaaSFuncCheck(this, "0408"/*商品营业员提成*/)) {
        qobject_cast<QCheckBox *>(this->sender())->setChecked(false);
        return;
    }
}

void w_bt_dish_base_new_virtual::showStackedIndex(int index)
{
    switch(index) {
    case 0:
        lds::showWidget(ui->frame_basic);ui->frame_basic->move(10, 10);
        lds::hideWidget(ui->frame_advanced);
        lds::hideWidget(ui->frame_item_set);
        break;
    case 1:
        lds::hideWidget(ui->frame_basic);
        lds::showWidget(ui->frame_advanced);ui->frame_advanced->move(20, 50);
        lds::hideWidget(ui->frame_item_set);
        break;
    case 2:
        lds::hideWidget(ui->frame_basic);
        lds::hideWidget(ui->frame_advanced);
        lds::showWidget(ui->frame_item_set);ui->frame_item_set->move(20, 50);
        break;
    }
}

void w_bt_dish_base_new_virtual::toSetCountWeight(bool check)
{
    if(ui->checkBox_ch_weight == this->sender() && check) {
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("启用称重计数"), QObject::tr("是"), QObject::tr("否"))) {
            editorsetValue(ui->checkBox_ch_count, "Y", "ch_count");
            //
            lds_dialog_input_double dialog(this);
            dialog.setWindowTitle(lds::sysconf->value("system_setting/dianzichengxinghao").toString());
            if(dialog.windowTitle().isEmpty()) {
                dialog.setWindowTitle(QObject::tr("没有类型"));
            }
            dialog.setWindowTitle(QObject::tr("计价秤") + ":" + dialog.windowTitle());
            dialog.ui->label->setText(QObject::tr("取重") + "(G)");
            dialog.ui->lineEdit->setDecimals(3);
            //
            QString errstring;
            ElectronicCountPrice_resolve countprice;
            if(false == countprice.scaleOpen(lds::sysconf->value("system_setting/dianzichengxinghao").toString(),
                                             lds::sysconf->value("system_setting/dianzichengduankou").toString(),
                                             lds::sysconf->value("system_setting/dianzicheng_baudrate").toInt(),
                                             &errstring)) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            }
            lds_value_scale scale(1000, this);
            scale.make_connect(&countprice,SIGNAL(weighComming(double)), dialog.ui->lineEdit,SLOT(setValue(double)));
            //
            if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
                editorsetValue(ui->doubleSpinBox_num_count_weight, dialog.ui->lineEdit->value(), "num_count_weight");
            }
        } else {
            editorsetValue(ui->checkBox_ch_count, "N", "ch_count");
        }
    }
    item_count_weight_refresh();
}

void w_bt_dish_base_new_virtual::toShowTax()
{
    bool isshow = n_func::f_get_sysparm("bt_show_more_tax", "0") == "1";
    n_func::f_set_sysparm("bt_show_more_tax", isshow ? "0" : "1");

    showTaxWidget();
}

void w_bt_dish_base_new_virtual::toChangeTax1Name()
{
    updateTaxName("tax_name1", ui->pushButton_rate1->text());
}

void w_bt_dish_base_new_virtual::toChangeTax2Name()
{
    updateTaxName("tax_name2", ui->pushButton_rate2->text());
}

void w_bt_dish_base_new_virtual::toChangeTax3Name()
{
    updateTaxName("tax_name3", ui->pushButton_rate3->text());
}

void w_bt_dish_base_new_virtual::to_product_find_online_by_barcode()
{
    QString url = "http://yun.scpos.com/CommondityAdapterServer/product/product-find";
    QString body = QString("{'no':'00000000','product':'','itemcode':'%1',"
                           "'cust_no':'','address':'','province':'','city':'','contacts':'',"
                           "'user_name':'','qq':'00000000','mobile':'','email':''}").
            arg(ui->lineEdit_vch_barcode->text());
    QNetworkRequest request;
    QByteArray readall;
    QString errstring;
    int httpcode;


    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    if(false == lds_network::managerPost(request, body.toLocal8Bit(), readall, errstring, httpcode)) {
        return ;
    }
    if(200 != httpcode) {
        errstring = cJSON_help::getcJSONvalue(readall, "message").toString();
        return ;
    }
    return ;
}

void w_bt_dish_base_new_virtual::toupdateDishImage()
{
    if(imagedishdir.isEmpty() || !QDir(imagedishdir).exists()) {
        if(1 == lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("图片(文件夹)为空"), QObject::tr("选择文件夹"), QObject::tr("取消"))) {
            return;
        }
        QString filepath = getFilePath::getExistingDirectory(this, QObject::tr("选择文件夹"), ".",QFileDialog::ShowDirsOnly);
        if(filepath.isEmpty()){
            return;
        }
        lds::sysconf->setValue("system_setting/bofanglujing", filepath);
        imagedishdir = filepath;
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("选择图片"), QObject::tr("确定"), QObject::tr("取消")))
            return;
    }
    if(!ui->lineEdit_ch_dishno->isEnabled()) {
        QString filepath = getFilePath::getOpenFileName(this, QObject::tr("选择图片"),
                                                #if defined(Q_OS_WIN) || defined(QT_DEBUG)
                                                        lds::sysconf->value("w_bt_dish_base_new_virtual/curfilepath", lds::localdataPath +"/imagepath0").toString()
                                                #else
                                                        lds::sysconf->value("w_bt_dish_base_new_virtual/curfilepath", "/userdata/resource/imagepath0").toString()
                                                #endif
                                                        );
        QString curimagebaseName = ui->lineEdit_ch_dishno->text();
        if(!filepath.isEmpty()) {
            QPixmap p(filepath);
            //删除存在的图片
            if(!p.isNull()) {
                QStringList files = QDir(imagedishdir).entryList();
                for(int begin = 0; begin < files.count();begin++) {
                    if(files[begin].startsWith(curimagebaseName+".")) {
                        QFile file(imagedishdir+"/"+files[begin]);
                        if(!file.remove()) {
                            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+file.errorString());
                            return;
                        }
                    }
                }
                //保存
                if(p.save(imagedishdir+"/"+curimagebaseName+"."+QFileInfo(filepath).suffix())) {
                    dishImageupdate();
                    lds::sysconf->setValue("w_bt_dish_base_new_virtual/curfilepath", filepath);
                    return;
                }
            }
            goto end;
        }
        return;
end:
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    }
}

void w_bt_dish_base_new_virtual::touclearDishImage()
{
    switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("清除"), QObject::tr("清除图片"), QObject::tr("清除颜色"), QObject::tr("取消"))) {
    case 0://清除商品图片
    {

        QString curimagebaseName = ui->lineEdit_ch_dishno->text();
        QStringList files = QDir(imagedishdir).entryList();
        for(int begin = 0; begin < files.count();begin++) {
            if(files[begin].startsWith(curimagebaseName+".")) {
                QFile file(imagedishdir+"/"+files[begin]);
                if(!file.remove()) {
                    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+file.errorString());
                    return;
                }
            }
            QPixmapCache::clear();
        }

        dishImageupdate();
    }
        break;
    case 1://清除商品颜色
    {
        //tablemodel
        tablemodel->model_data_set(mapper->currentIndex(), "vch_color", "");
        //tableview
        model_dish->model_data_set(0, 0, tablemodel->model_data(mapper->currentIndex(), "vch_color"), Qt::UserRole + 9);
    }
        break;
    case 2:
        return;
    }
}

void w_bt_dish_base_new_virtual::toupdateDishColor()
{
    colordialog dialog(this);
    dialog.setWindowTitle(QObject::tr("选择颜色"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        //tablemodel
        tablemodel->model_data_set(mapper->currentIndex(), "vch_color", dialog.result_color_name());
        //tableview
        model_dish->model_data_set(0, 0, tablemodel->model_data(mapper->currentIndex(), "vch_color"), Qt::UserRole + 9);
    }
}

void w_bt_dish_base_new_virtual::toupdateItemFlag(const QString &f)
{
    editorsetValue(ui->checkBox_item_flag, f, "item_flag");
}

void w_bt_dish_base_new_virtual::showTaxWidget()
{
    bool isshow = n_func::f_get_sysparm("bt_show_more_tax", "0") == "1";
    showTaxWidget(isshow);
}

void w_bt_dish_base_new_virtual::showTaxWidget(bool isshow)
{
    lds::showWidget(ui->pushButton_rate2, isshow);
    lds::showWidget(ui->pushButton_rate3, isshow);
    lds::showWidget(ui->doubleSpinBox_int_rate_2, isshow);
    lds::showWidget(ui->doubleSpinBox_int_rate_3, isshow);

    ui->pushButton_rate1->setText(n_func::f_get_sysparm("tax_name1", QObject::tr("税率") + "1"));
    ui->pushButton_rate2->setText(n_func::f_get_sysparm("tax_name2", QObject::tr("税率") + "2"));
    ui->pushButton_rate3->setText(n_func::f_get_sysparm("tax_name3", QObject::tr("税率") + "3"));

    if(isshow) {
        ui->pushButton_rate_more->setText(QObject::tr("收起"));
    } else {
        ui->pushButton_rate_more->setText(QObject::tr("更多"));
    }
}

void w_bt_dish_base_new_virtual::updateTaxName(const QString &sys_parameter_key, const QString &defValue)
{
    lds_dialog_input dialog(this);
    dialog.setWindowTitle(QObject::tr("税率名称"));
    dialog.hideKey();
    dialog.ui->label->setText("");
    dialog.ui->lineEdit->setText(defValue);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        QString name = dialog.ui->lineEdit->text().trimmed();
        if(name.isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("税率名称为空"));
            return;
        }
        n_func::f_set_sysparm(sys_parameter_key, name);
        showTaxWidget();
    }
}

//修改通过ch_dishno
w_bt_dish_base_change_by_ch_dishno::w_bt_dish_base_change_by_ch_dishno(QWidget *parent, const QString &ch_dishno):
    w_bt_dish_base_new_virtual(0, parent)
{
    this->setWindowTitle(QObject::tr("修改"));
    _ch_dishno = ch_dishno;
}

w_bt_dish_base_change_by_ch_dishno::~w_bt_dish_base_change_by_ch_dishno()
{

}

void w_bt_dish_base_change_by_ch_dishno::tosave()
{
    if(saveData()) {
        this->accept();
    }
}

void w_bt_dish_base_change_by_ch_dishno::tocontinue()
{

}

void w_bt_dish_base_change_by_ch_dishno::toexit()
{
    if(isDiry()) {
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

void w_bt_dish_base_change_by_ch_dishno::mapperupdate()
{
    mapper->setCurrentIndex(0);
    dishImageupdate();
    //原料刷新
    dish_item_refresh();
}

bool w_bt_dish_base_change_by_ch_dishno::saveData()
{
    //2
    if(!saveCheck()) return false;
    //3
    lds_query_hddpos::tran_saction();
    if(tablemodel->trySubmitAll()
            &&dish_item_save()) {
        lds_query_hddpos::com_mit();
        //父类刷新，包括 父类 model 刷新
        mapperupdate();
        //成功信息
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    }
    lds_query_hddpos::roll_back();
    //错误提示
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
    return false;
}

void w_bt_dish_base_change_by_ch_dishno::delayInitData()
{
    w_bt_dish_base_new_virtual::delayInitData();
    //1
    tablemodel=w_bt_dish_base::creatmodel(this);
    tablemodel->setFilter(QString("ch_dishno = '%1' ")
                          .arg(_ch_dishno));

    //2
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    addmapping(tablemodel, mapper);

    //3
    connect(ui->toolbar->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->toolbar->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->lineEdit_vch_dishname,SIGNAL(textEdited(QString)),this,SLOT(spellupdate(QString)));
    connect(ui->comboBox_ch_deducttype,SIGNAL(currentIndexChanged(int)),this,SLOT(update_ch_deducttype(int)));
    connect(ui->comboBox_ch_dish_typeno,SIGNAL(currentIndexChanged(int)),this,SLOT(updatesubdishtype()));
    //! init
    frame_setEnable(ui->checkBox_item_flag->isChecked());
    frame2_setEnable(ui->checkBox_ch_deductflag->isChecked());
    ui->doubleSpinBox_num_m_price->setVisible(true);
    ui->lineEdit_ch_dishno->setEnabled(false);
    enableDishImage(!ui->lineEdit_ch_dishno->isEnabled());
    static_cast<QPushButton*>(ui->toolbar->index_widget(QObject::tr("保存")))->setDefault(false);static_cast<QPushButton*>(ui->toolbar->index_widget(QObject::tr("保存")))->setAutoDefault(false);
    static_cast<QPushButton*>(ui->toolbar->index_widget(QObject::tr("退出")))->setDefault(false);static_cast<QPushButton*>(ui->toolbar->index_widget(QObject::tr("退出")))->setAutoDefault(false);
    //4
    ui->toolbar->index_widget((0)/*上一页*/)->setVisible(false);
    ui->toolbar->index_widget((1)/*上一页*/)->setVisible(false);
    ui->toolbar->index_widget(QObject::tr("保存并新增"))->setVisible(false);

    mapperupdate();
    //    ui->comboBox_ch_dish_typeno->setEnabled(false);
    //    ui->comboBox_ch_sub_typeno->setEnabled(false);
    ui->checkBox_ch_stopflag->setEnabled(false);
    ui->checkBox_ch_tempflag->setEnabled(false);
}

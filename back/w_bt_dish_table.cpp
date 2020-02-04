//餐桌资料
#include "w_bt_dish_table.h"
#include "ui_w_bt_dish_dialog.h"
#include "lds_query_hddpos.h"
#include "ui_w_bt_dish_table_new2.h"
#include "lds_model_sqltablemodel.h"
#include "lds_messagebox.h"
#include "backheader.h"
#include "w_bt_dish.h"
#include <QSqlError>
#include <QTreeWidgetItem>
#include "w_sys_manage_basicdataexport.h"
#include "w_sys_manage_basicdataimport.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "n_func.h"
#include "ui_w_bt_dish_table_new_dialog.h"
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>
#include "ui_w_bt_dish_unitset_dialog.h"
#include "ui_w_bt_dish_table_tableset_dialog.h"
#include "ui_w_bt_dish_table_news_dialog.h"
#include "w_bt_dish_table_tableset2.h"

w_bt_dish_table::w_bt_dish_table(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_bt_dish_Dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->label->setText(QObject::tr("检索"));
    ui->lineEdit->setPlaceholderText(QObject::tr("餐桌号")+","+QObject::tr("餐桌名"));
    ui->comboBox_filter->setVisible(false);
    //! toolbar
    ui->widget->setup(QStringList()
                      << QObject::tr("餐区设置") << QObject::tr("桌类设置") << QObject::tr("新增") << QObject::tr("批量新增")
                      << QObject::tr("修改") << QObject::tr("删除") <<  QObject::tr("刷新")
                      << QObject::tr("导出") << QObject::tr("导入") << QObject::tr("退出")
                      );
    //! treewidget
    ui->treeWidget->header()->hide();
    ui->treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeWidget->setColumnCount(1);
    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_table");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_tableno", QObject::tr("餐桌号"));
    tablemodel->save_set_header("vch_tablename", QObject::tr("餐桌名称"));
    tablemodel->save_set_header("ch_typeno", QObject::tr("餐桌类型"));
    tablemodel->save_set_header("int_person", QObject::tr("人数"));
    tablemodel->save_set_header("ch_areano", QObject::tr("所属区域"));
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());

    tablemodel->tablename_kvmap_insert_sql("cey_bt_table_area", "ch_areano", "vch_areaname", "", false, lds_model_sqltablemodel::SELECT_AUTO_REFRESH_KEY);
    tablemodel->tablename_kvmap_insert_sql("cey_bt_table_type", "ch_typeno", "vch_typename", "", false, lds_model_sqltablemodel::SELECT_AUTO_REFRESH_KEY);

    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_areano"), new lds_model_sqltablemodel_delegate_com(this, tablemodel->tablename_kvmap_d("cey_bt_table_area")));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_typeno"), new lds_model_sqltablemodel_delegate_com(this, tablemodel->tablename_kvmap_d("cey_bt_table_type")));

    //! refresh
    //重新布局表头和隐藏相关表头

    //! connect
    connect(ui->treeWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(treeselect()));
    connect(ui->widget->index_widget(QObject::tr("新增")), SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(QObject::tr("修改")), SIGNAL(clicked()),this,SLOT(tochange()));
    connect(ui->widget->index_widget(QObject::tr("删除")), SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(QObject::tr("批量新增")), SIGNAL(clicked()),this,SLOT(tonews()));
    connect(ui->widget->index_widget(QObject::tr("桌类设置")), SIGNAL(clicked()),this,SLOT(totable()));
    connect(ui->widget->index_widget(QObject::tr("餐区设置")), SIGNAL(clicked()),this,SLOT(toarea()));
    connect(ui->widget->index_widget(QObject::tr("刷新")), SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("导入")), SIGNAL(clicked()),this,SLOT(toimport()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(treeselect()));

    ui->comboBox_filter_2->hide();
    ui->comboBox_filter->hide();
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_bt_dish_table::~w_bt_dish_table()
{
}

bool w_bt_dish_table::tableDelTransaction(const QString &ch_tableno, QString *errstring)
{
    lds_query_hddpos  query;
    query.execSelect(QString("select count(0) from cey_u_table where ch_tableno='%1'").arg(ch_tableno));
    query.next();
    if(query.recordValue(0).toInt()>0) {
        *errstring  = QObject::tr("指定餐桌记录已经营业操作");
        return false;
    }

    lds_query_hddpos::tran_saction();
    if(query.execDelete("cey_bt_table", qrtEqual("ch_tableno", ch_tableno), errstring)) {
        lds_query_hddpos::com_mit();
        return true;
    }
    lds_query_hddpos::roll_back();
    return false;
}

void w_bt_dish_table::torefresh()
{
    //yun
    w_sys_manage_cloudsync_with_time_running::exec_no_up_then_down_byconf(this);
    ui->widget->index_widget(QObject::tr("退出"))->show_flag_top_right(w_sys_manage_cloudsync_with_time_running::q_bt_get_ready_total_byconf());
    //~yun

    //treewidget
    lds_query_hddpos  query;
    ui->treeWidget->deleteAllItem();
    ui->treeWidget->rootitem=new QTreeWidgetItem(QStringList()
                                                 <<QObject::tr("全部类型"));
    ui->treeWidget->addTopLevelItem(ui->treeWidget->rootitem);
    query.execSelect("select concat(vch_typename,'[',ch_typeno,']') as 'nametype'  from cey_bt_table_type order by cey_bt_table_type.ch_typeno asc");
    while(query.next()) {
        QTreeWidgetItem *item=new QTreeWidgetItem(QStringList()
                                                  <<QString("%1")
                                                  .arg(query.recordValue("nametype").toString())
                                                  );
        ui->treeWidget->rootitem->addChild(item);
    }
    ui->treeWidget->setItemSizeHint();//更新所有的 item的sizehint
    ui->treeWidget->restore();
    //tableview
    treeselect();
}

void w_bt_dish_table::setToolbarShown(bool shown)
{
    ui->widget->setVisible(shown);
}

void w_bt_dish_table::tonew()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0201"/*餐桌数量*/)) {
        return;
    }

    if(ui->treeWidget->deepofitem(ui->treeWidget->currentItem())==1) {
        QString tabletype=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
        w_bt_dish_table_new2 dialog(this);
        dialog.update_ch_typeno(tabletype);
        dialog.setWindowTitle(_TEXT_SLOT(this));
        transparentCenterDialog(&dialog).exec();
        torefresh();
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请先选择餐桌类型"));
    }
}

void w_bt_dish_table::tochange()
{
    int row=ui->tableView->currentIndex().row();
    if(row>=0) {
        w_bt_dish_table_change dialog(tablemodel, ui->tableView, this);
        dialog.setWindowTitle(_TEXT_SLOT(this));
        transparentCenterDialog(&dialog).exec();
        torefresh();
    }
}

void w_bt_dish_table::todel()
{
    int row=ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"), QObject::tr("确认"), QObject::tr("取消"))) {
        return;
    }

    QString ls_dishno=tablemodel->index(row, 0).data().toString();
    QString errstring;
    if(!tableDelTransaction(ls_dishno, &errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    tablemodel->select();
}

void w_bt_dish_table::tonews()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0201"/*餐桌数量*/)) {
        return;
    }

    w_bt_dish_table_news dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));transparentCenterDialog(&dialog).exec();
}

void w_bt_dish_table::totable()
{
    w_bt_dish_table_tableset2 dialog(this);
    dialog.setFixedWidth(750);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void w_bt_dish_table::toarea()
{
    w_bt_dish_table_areaset dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void w_bt_dish_table::toexport()
{
    w_sys_manage_basicdataexport dialog(this);
    dialog.reset_export_tablenames(QStringList() << tablemodel->tableName());
    dialog.reset_export_confname(QString("%1/path").arg(this->metaObject()->className()));
    dialog.setWindowTitle(QObject::tr("基础数据导出"));
    transparentCenterDialog(&dialog).exec();
}

void w_bt_dish_table::toimport()
{
    w_sys_manage_basicdataimport_sub dialog(QStringList() << tablemodel->tableName(), QString("%1/path").arg(this->metaObject()->className()), this);
    dialog.setWindowTitle(QObject::tr("基础数据导入"));
    transparentCenterDialog(&dialog).exec();
}

void w_bt_dish_table::toexit()
{
    w_sys_manage_cloudsync_cloud2::exec_keepdata_byconf(this);
    this->reject();
}

void w_bt_dish_table::treeselect()
{
    QString sqlselect;
    QString ch_typeno;
    if(ui->treeWidget->currentItem()) {
        int index=ui->treeWidget->deepofitem(ui->treeWidget->currentItem());
        if(index==1) {
            ch_typeno=backheader::getinterdata(ui->treeWidget->currentItem()->text(0));
        }

        sqlselect+=QString("(cey_bt_table.ch_typeno like '%1%' )").arg(ch_typeno);
        //过滤
        sqlselect +=QString("and (cey_bt_table.ch_tableno like '%1%' or cey_bt_table.vch_tablename like '%1%') ")
                .arg(ui->lineEdit->text())
                ;
        tablemodel->setFilter(sqlselect);
        ui->tableView->restore(0);
        ui->tableView->resizeColumnsToContents();
    }
}

//==============================

w_bt_dish_table_change::w_bt_dish_table_change(lds_model_sqltablemodel *tablemodel, lds_tableView *tableview, QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui_w_bt_dish_table_new_Dialog;
    this->tablemodel = tablemodel;
    this->tableview = tableview;
    ui->setupUi(this);
    //! datawidget
    lds_model_sqltablemodel_d::fillData(ui->comboBox_ch_areano, tablemodel->tablename_kvmap_d("cey_bt_table_area"));
    lds_model_sqltablemodel_d::fillData(ui->comboBox_ch_typeno, tablemodel->tablename_kvmap_d("cey_bt_table_type"));

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->addMapping(ui->lineEdit_ch_tableno, tablemodel->fieldIndex("ch_tableno"));
    mapper->addMapping(ui->lineEdit_int_person, tablemodel->fieldIndex("int_person"));
    mapper->addMapping(ui->lineEdit_vch_memo, tablemodel->fieldIndex("vch_memo"));
    mapper->addMapping(ui->lineEdit_vch_tablename, tablemodel->fieldIndex("vch_tablename"));
    mapper->addMapping(ui->comboBox_ch_typeno, tablemodel->fieldIndex("ch_typeno"), "comusrdata");
    mapper->addMapping(ui->comboBox_ch_areano, tablemodel->fieldIndex("ch_areano"), "comusrdata");

    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("上一行")<<QObject::tr("下一行")<<QObject::tr("保存")<<QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("上一行")),SIGNAL(clicked()),this,SLOT(toup()));
    connect(ui->widget->index_widget(QObject::tr("下一行")),SIGNAL(clicked()),this,SLOT(todown()));
    connect(ui->widget->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("继续")),SIGNAL(clicked()),this,SLOT(tocontinue()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(tableview, SIGNAL(selectChanged(int)), mapper, SLOT(setCurrentIndex(int)));
    //这样做可以屏蔽pushbutton的enter时间,当有lineeidt需要输入条码是,会自动点击按钮
    ui->widget->setDefaultFalseAutoDefaultFalse(QObject::tr("上一行"));
    ui->widget->setDefaultFalseAutoDefaultFalse(QObject::tr("下一行"));
    ui->widget->setDefaultFalseAutoDefaultFalse(QObject::tr("保存"));
    ui->widget->setDefaultFalseAutoDefaultFalse(QObject::tr("继续"));
    ui->widget->setDefaultFalseAutoDefaultFalse(QObject::tr("退出"));
    //! after
    ui->lineEdit_ch_tableno->setEnabled(false);
    ui->comboBox_ch_typeno->setEnabled(false);
    //init
    ui->label_record->setText(QString("%1/%2").arg(tableview->currentIndex().row()+1).arg(tableview->verticalHeader()->count()));
    mapper->setCurrentIndex(tableview->currentIndex().row());
}

void w_bt_dish_table_change::tosave()
{
    saveData();
}

void w_bt_dish_table_change::toexit()
{
    int row = tableview->currentIndex().row();
    if(tablemodel->model_is_dirty(row)) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"),QObject::tr("取消"))) {
        case 0://保存
            if(!saveData()) {
                return;
            }
            break;
        case 1://不保存
            break;
        case 2://取消
            return;
        }
    }

    this->reject();
}

void w_bt_dish_table_change::toup()
{
    int row = tableview->currentIndex().row();
    if(tablemodel->model_is_dirty(row)) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"))) {
        case 0:
            if(!saveData()) {
                return;
            }
            break;
        case 1:
            tablemodel->revertRow(row);
            break;
        }
    }
    tableview->goprevious();
}

void w_bt_dish_table_change::todown()
{
    int row = tableview->currentIndex().row();
    if(tablemodel->model_is_dirty(row)) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有数据未保存,确认保存?"), QObject::tr("保存"),QObject::tr("不保存"))) {
        case 0:
            if(!saveData()) {
                return;
            }
            break;
        case 1:
            tablemodel->revertRow(row);
            break;
        }
    }
    tableview->gonext();
}

bool w_bt_dish_table_change::saveData()
{
    if(tablemodel->trySubmitAll()) {
        tableview->restore();
        mapper->setCurrentIndex(tableview->currentIndex().row());
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    }
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    return false;
}

//==============================
w_bt_dish_table_news::w_bt_dish_table_news(QWidget *parent):
    QDialog(parent),
    ui(new Ui_w_bt_table_news_Dialog),
    _parent(static_cast<w_bt_dish_table *>(parent))
{
    ui->setupUi(this);
    //! toolbar
    ui->widget->setup(QStringList() << QObject::tr("生成") << QObject::tr("退出"), Qt::AlignRight);

    //! area type
    lds_model_sqltablemodel_d::fillData(ui->comboBox_ch_areano, _parent->tablemodel->tablename_kvmap_d("cey_bt_table_area"));
    lds_model_sqltablemodel_d::fillData(ui->comboBox_ch_typeno, _parent->tablemodel->tablename_kvmap_d("cey_bt_table_type"));

    QString ch_typeno;
    if(_parent->ui->treeWidget->currentItem()) {
        int index=_parent->ui->treeWidget->deepofitem(_parent->ui->treeWidget->currentItem());
        if(index==1) {
            ch_typeno=backheader::getinterdata(_parent->ui->treeWidget->currentItem()->text(0));
        }
    }
    ui->comboBox_ch_typeno->setCurrentIndex(ui->comboBox_ch_typeno->findData(ch_typeno));

    //! connect
    connect(ui->widget->index_widget(QObject::tr("生成")), SIGNAL(clicked()),this,SLOT(togenrate()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

    ui->lineEdit_int_len->setEnabled(false);
}

void w_bt_dish_table_news::togenrate()
{
    //clicked (none) returns long [pbm_bnclicked]
    //long ancestorreturnvalue
    QString ls_sql;
    QString ls_code_start;
    QString ls_code_end;
    QString ls_ch_typeno;
    QString ls_ch_areano;
    QString ls_msg;
    QString ls_ch_tableno;
    QString ls_ch_tablename;
    QString ls_filter;
    QString ls_qz;
    QString ls_ch_temp;
    QString errtext;
    //    qlonglong ll_i;
    qlonglong ll_code_start;
    qlonglong ll_code_end;
    double ll_int_person;
    //    qlonglong ll_exist;
    //    int li_ret;
    int li_len;

    bool ok;
    ui->lineEdit_code_start->text().toInt(&ok);
    if(!ok) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, ui->label_code_start->text() + "," + QObject::tr("必须为数字"));
        return;
    }
    ui->lineEdit_code_end->text().toInt(&ok);
    if(!ok) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, ui->label_code_end->text() +  "," + QObject::tr("必须为数字"));
        return;
    }

    //    IF PARENT.dw_set.accepttext() = -1 THEN RETURN
    ls_qz =ui->lineEdit_code_qz->text();

    ll_code_end =ui->lineEdit_code_end->text().toLongLong();
    ll_code_start =ui->lineEdit_code_start->text().toLongLong();
    ls_ch_typeno =ui->comboBox_ch_typeno->comusrdata().toString();
    ls_ch_areano =ui->comboBox_ch_areano->comusrdata().toString();
    ll_int_person =ui->lineEdit_int_person->value();// PARENT.dw_set.getitemnumber(1,"int_person")
    ls_code_start = QString::number(ll_code_start);
    ls_code_end = QString::number(ll_code_end);
    li_len = ui->lineEdit_int_len->text().toInt();//PARENT.dw_set.getitemnumber(1,"int_len")
    ls_ch_temp = "N";
    ls_filter = ui->lineEdit_vch_filter->text().trimmed();//trim(PARENT.dw_set.object.vch_filter[1])
    if(ls_filter.isEmpty()) {

    }
    if(!ls_qz.isEmpty() && ls_qz.toInt() == 0) {
        ls_msg=QObject::tr("编号必须为数字");
    } else if(ls_ch_typeno.isEmpty()) {
        ls_msg=QObject::tr("类型不能为空");
    } else if(ls_ch_areano.isEmpty()) {
        ls_msg=QObject::tr("区域不能为空");
    } else if(ui->lineEdit_code_start->text().isEmpty()) {
        ls_msg=QObject::tr("起始编号不能为空");
    } else if(ui->lineEdit_code_end->text().isEmpty()) {
        ls_msg=QObject::tr("结束编号不能为空");
    } else if(ls_code_start.length()>4||ls_code_end.length()>4) {
        ls_msg=QObject::tr("编号位数不能为空且不能大于4位");
    }else if(li_len==0) {
        ls_msg=QObject::tr("编号长度不能为空");
    } else if(ll_code_start > ll_code_end) {
        ls_msg=QObject::tr("起始编号不能大于结束编号");
    } else if(qAbs(ll_code_end - ll_code_start) > 1000) {
        ls_msg=QObject::tr("一次批量创建的餐桌不超过1千条");
    } else if(ls_qz.length()>2) {
        ls_msg=QObject::tr("前缀不能超过2位");
    } else if((ls_qz.length() + ls_code_end.length())>4) {
        ls_msg=QObject::tr("前缀位数加编号位数不能超过4位");
    }

    if(ls_msg.isEmpty()) {

    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,ls_msg);
        return;
    }

    //! <--transaction begin-->
    ls_filter.replace(",", "");
    ls_filter="["+ls_filter+"]";
    lds_query_hddpos::tran_saction();
    lds_query_hddpos  query;
    for(long ll_i=ll_code_start; ll_i<=ll_code_end;ll_i++) {//0
        ls_ch_tableno=ls_qz + QString::number(ll_i).rightJustified(li_len-ls_qz.length(), '0').right(li_len-ls_qz.length());
        ls_ch_tablename=QObject::tr("桌")+ls_ch_tableno;

        if(!ls_filter.isEmpty() && ls_ch_tableno.contains(QRegExp(ls_filter))) {
            continue;
        }
        if(!saas_pay_detail::SaaSFuncCheck(this, "0201"/*餐桌数量*/)) {
            lds_query_hddpos::roll_back();
            return;
        }
        query.execSelect(QString("select count(0)  from cey_bt_table   where ch_tableno ='%1'").arg(ls_ch_tableno));
        query.next();
        if(query.recordValue(0).toLongLong() > 0) {
            errtext=QObject::tr("批量范围中的编号已经存在") + ":" + ls_ch_tableno;
            break;
        }
        if(query.execInsert("cey_bt_table",qrtVariantPairList()

                            << qrtVariantPair("ch_tableno", ls_ch_tableno)
                            << qrtVariantPair("vch_tablename", ls_ch_tablename)
                            << qrtVariantPair("ch_typeno", ls_ch_typeno)
                            << qrtVariantPair("ch_areano", ls_ch_areano)
                            << qrtVariantPair("int_person", ll_int_person))) {
            continue;
        }
        errtext=QObject::tr("数据库错误");
        break;
    }//0
    if(errtext.isEmpty()) {
        lds_query_hddpos::com_mit();
        _parent->torefresh();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID,  QObject::tr("操作成功"));
    } else {
        lds_query_hddpos::roll_back();
        _parent->torefresh();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,errtext);
    }
    //! <--transaction end-->
}

void w_bt_dish_table_news::toexit()
{
    this->reject();
}

//=================================

w_bt_dish_table_tableset::w_bt_dish_table_tableset(QWidget *parent):
    QDialog(parent),
    ui(new Ui_w_bt_dish_table_tableset_Dialog),
    _parent(static_cast<w_bt_dish_table* >(parent))
{
    ui->setupUi(this);
    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("新增")<<QObject::tr("删除")<<QObject::tr("保存")<<QObject::tr("退出"), Qt::AlignRight);
    //! tableview
    ui->comboBox_ch_low_mode->addItemsByModel(&w_bt_dish::get_MODEL_DISH_LOW_MODE());
    ui->comboBox_ch_service_mode->addItemsByModel(&w_bt_dish::get_MODEL_DISH_SERVICE_MODE());
    ui->comboBox_ch_serviceflag->addItemsByModel(&w_bt_dish::get_MODEL_TABLE_SERVICE_FLAG_MODE());


    //几个写死的com 和radiobuton 的内容
    //! init
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_table_type");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->select();
    tablemodel->save_set_header("ch_typeno",  QObject::tr("编号"));
    tablemodel->save_set_header("vch_typename",  QObject::tr("桌类名称"));
    ui->tableView->setModel(tablemodel);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setColumnWidth(0, 50);
    ui->tableView->justShowColumns(QList<int>() << tablemodel->fieldIndex("ch_typeno") << tablemodel->fieldIndex("vch_typename"));

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->addMapping(ui->comboBox_ch_low_mode, tablemodel->fieldIndex("ch_low_mode"), "comusrdata");
    mapper->addMapping(ui->comboBox_ch_serviceflag, tablemodel->fieldIndex("ch_serviceflag"), "comusrdata");//将bool 转换为Y N
    mapper->addMapping(ui->comboBox_ch_service_mode, tablemodel->fieldIndex("ch_service_mode"), "comusrdata");
    mapper->addMapping(ui->lineEdit_ch_typeno, tablemodel->fieldIndex("ch_typeno"));
    mapper->addMapping(ui->spinBox_int_begin_time, tablemodel->fieldIndex("int_begin_time"));
    mapper->addMapping(ui->spinBox_int_over, tablemodel->fieldIndex("int_over"));
    mapper->addMapping(ui->spinBox_int_rate, tablemodel->fieldIndex("int_rate"));
    mapper->addMapping(ui->spinBox_int_start_minute, tablemodel->fieldIndex("int_start_minute"));
    mapper->addMapping(ui->spinBox_int_unit_minute, tablemodel->fieldIndex("int_unit_minute"));
    mapper->addMapping(ui->doubleSpinBox_num_lowcost, tablemodel->fieldIndex("num_lowcost"));
    //    mapper->addMapping(ui->doubleSpinBox_num_max_service, tablemodel->fieldIndex("num_max_service"));
    mapper->addMapping(ui->doubleSpinBox_num_ration, tablemodel->fieldIndex("num_ration"));
    mapper->addMapping(ui->spinBox_num_start_amount, tablemodel->fieldIndex("num_start_amount"));
    mapper->addMapping(ui->spinBox_num_unit_amount, tablemodel->fieldIndex("num_unit_amount"));
    mapper->addMapping(ui->lineEdit_vch_typename, tablemodel->fieldIndex("vch_typename"));

    //! connect
    connect(ui->widget->index_widget(QObject::tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(QObject::tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->comboBox_ch_service_mode,SIGNAL(currentIndexChanged(int)),this,SLOT(update_enabled(int)));
    connect(ui->tableView,SIGNAL(selectChanged(int)),mapper,SLOT(setCurrentIndex(int)));
    //! after
    ui->tableView->selectRow(_parent->ui->treeWidget->get_0_Index());
    mapper->setCurrentIndex(qMax(0, ui->tableView->currentIndex().row()));
    update_enabled(ui->comboBox_ch_service_mode->currentIndex());
}


void w_bt_dish_table_tableset::tonew()
{
    int rowcount=tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_typeno"), tablemodel->max_column_add_1__max_sql_add_1_dec("ch_typeno","%02d"));
    ui->comboBox_ch_low_mode->setCurrentIndex(0);
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_low_mode"),ui->comboBox_ch_low_mode->comusrdata().toString());
    ui->comboBox_ch_service_mode->setCurrentIndex(0);
    tablemodel->model_data_set(rowcount, tablemodel->fieldIndex("ch_service_mode"),ui->comboBox_ch_service_mode->comusrdata().toString());
    ui->tableView->selectRow(rowcount);
}

void w_bt_dish_table_tableset::todel()
{
    //
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }
    QString  ch_typeno = tablemodel->model_data(row, "ch_typeno").toString();
    //
    lds_query_hddpos  query;
    query.execSelect(QString("select count(ch_tableno) from cey_bt_table where ch_typeno = '%1' ").arg(ch_typeno));
    query.next();
    if(query.recordValue(0).toInt() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("该类别下已有餐桌"));
        return;
    }

    //
    tablemodel->removeRow(row);
    tablemodel->trySubmitAll();
    ui->tableView->restore();
}

bool w_bt_dish_table_tableset::tosave()
{
    mapper->submit();
    int premapperindex=mapper->currentIndex();
    if(tablemodel->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请先新增餐桌类型"));
        return false;
    }
    for(int row=0,rowcount=tablemodel->rowCount();row<rowcount;row++) {
        if(tablemodel->model_data(row, "ch_typeno").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，编号不能为空").arg(row + 1));
            return false;
        }
        if(tablemodel->model_data(row, "vch_typename").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，桌类名称不能为空").arg(row + 1));
            return false;
        }

        if(tablemodel->model_data(row, "ch_service_mode").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，服务费模式不能为空").arg(row + 1));
            return false;
        }

        if(tablemodel->model_data(row, "ch_low_mode").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，低消方式不能为空").arg(row + 1));
            return false;
        }

        //============

        if(tablemodel->index(row, tablemodel->fieldIndex("ch_service_mode")).data().toInt() == 1
                &&tablemodel->index(row, tablemodel->fieldIndex("int_unit_minute")).data().toInt()<=0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，超钟收费单位不能小于等于0").arg(row + 1));
            return false;
        }

        //===============
    }
    if(tablemodel->trySubmitAll()) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        _parent->torefresh();
        mapper->setCurrentIndex(premapperindex);
        return true;
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
        return false;
    }
}

void w_bt_dish_table_tableset::toexit()
{
    this->reject();
}

void w_bt_dish_table_tableset::update_enabled(int index)
{
    switch(index) {
    case 0:
        ui->frame_2->setEnabled(false);
        ui->frame_3->setEnabled(false);
        ui->frame_4->setEnabled(false);
        break;
    case 1:
        ui->frame_2->setEnabled(true);
        ui->frame_3->setEnabled(false);
        ui->frame_4->setEnabled(false);
        break;
    case 2:
    case 4:
        ui->frame_2->setEnabled(false);
        ui->frame_3->setEnabled(true);
        ui->frame_4->setEnabled(false);
        break;
    case 3:
        ui->frame_2->setEnabled(false);
        ui->frame_3->setEnabled(false);
        ui->frame_4->setEnabled(true);
        break;
    }
}

//===========================

w_bt_dish_table_areaset::w_bt_dish_table_areaset(QWidget *parent):
    QDialog(parent),
    ui(new Ui_w_bt_dish_unitset_Dialog)
{
    ui->setupUi(this);
    ui->label_desc->hide();

    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_table_area");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header(tablemodel->fieldIndex("ch_areano"), QObject::tr("编号"));
    tablemodel->save_set_header(tablemodel->fieldIndex("vch_areaname"), QObject::tr("区域名称"), 1);

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setModel(tablemodel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setModel(tablemodel);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->horizontalHeader()->setStretchLastSection(true);


    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("新增")<<QObject::tr("删除")<<QObject::tr("刷新")<<QObject::tr("保存")<<QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(QObject::tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(QObject::tr("刷新")),SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));


    torefresh();
}

void w_bt_dish_table_areaset::tonew()
{
    int rowcount=tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount, 0, tablemodel->max_column_add_1__max_sql_add_1_dec("ch_areano","%02d"));
    ui->tableView->setCurrentIndex(tablemodel->model_index(rowcount, "vch_areaname"));
}

void w_bt_dish_table_areaset::todel()
{
    int row=ui->tableView->currentIndex().row();
    if(row<0) {
        //没有数据被选中
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    QString ls_typeno=tablemodel->model_data(row, 0).toString();
    lds_query_hddpos  query;
    query.execSelect(QString("select count(0) from cey_bt_table where ch_areano='%1'").arg(ls_typeno));
    query.next();
    if(query.recordValue(0).toLongLong()>0) {
        //指定单位已存在商品记录!
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("指定区域已存在餐桌记录"));
        return;
    }

    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"),QObject::tr("确认"),QObject::tr("取消"))) {
        tablemodel->removeRow(row);
        tablemodel->trySubmitAll();
    }
}

void w_bt_dish_table_areaset::torefresh()
{
    tablemodel->select();
    if(tablemodel->rowCount() > 0) {
        ui->tableView->selectRow(0);
    }
}

bool w_bt_dish_table_areaset::tosave()
{
    QString ls_areano;
    QString ls_areaname;
    for(int r=0,r_count=tablemodel->rowCount();r<r_count;r++) {
        ls_areano=tablemodel->model_data(r, 0).toString();
        ls_areaname=tablemodel->model_data(r, 1).toString();
        if(ls_areano.trimmed().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("第%1行，区域代码不能为空").arg(r + 1));
            return false;
        }
        if(ls_areaname.trimmed().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("第%1行，区域名称不能为空").arg(r + 1));
            return false;
        }

        if(findareano(ls_areano, r)!=-1) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("区域代码有重复"));
            return false;
        }
    }

    if(tablemodel->trySubmitAll()) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("数据保存成功"));
        return true;
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
        return false;
    }
}

void w_bt_dish_table_areaset::toexit()
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

int w_bt_dish_table_areaset::findareano(const QString &areano, int exceptrow)
{
    for(int r=0,r_count=tablemodel->rowCount();r<r_count;r++) {
        if(r!=exceptrow
                &&tablemodel->model_data(r, 0).toString() == areano)
            return r;
    }
    return -1;
}




w_bt_dish_table_tableset_cut::w_bt_dish_table_tableset_cut(QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui_w_bt_dish_unitset_Dialog;
    ui->setupUi(this);
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_table_type");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_typeno", QObject::tr("编号"));
    tablemodel->save_set_header("vch_typename", QObject::tr("桌类名称"), 1);
    //    tablemodel->save_set_header("num_max_service", QObject::tr("最高服务费"), 1);

    bool pay_fixed_service = n_func::f_get_sysparm("pay_fixed_service", "0") == "1";
    tablemodel->save_set_header("ch_service_mode", QObject::tr("服务费模式"), pay_fixed_service?0:1);
    tablemodel->save_set_header("num_ration", QObject::tr("服务费") + "%", pay_fixed_service?0:2);

    tablemodel->save_set_header("ch_low_mode", QObject::tr("低消方式"), 1);
    tablemodel->save_set_header("num_lowcost", QObject::tr("低消金额"), 1);


    ui->tableView->setModel(tablemodel);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->resizeColumnToContents(0);
    ui->tableView->resizeColumnToContents(1);
    ui->tableView->resizeColumnToContents(2);
    ui->tableView->setColumnWidth(3, 200);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_service_mode"), new lds_model_sqltablemodel_delegate_com(this, &w_bt_dish::get_MODEL_DISH_SERVICE_MODE_CUT()));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_low_mode"), new lds_model_sqltablemodel_delegate_com(this, &w_bt_dish::get_MODEL_DISH_LOW_MODE()));

    ///
    ui->widget->setup(QStringList()<<QObject::tr("新增")<<QObject::tr("删除")<<QObject::tr("刷新")<<QObject::tr("保存")<<QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("新增")),SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->widget->index_widget(QObject::tr("删除")),SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->widget->index_widget(QObject::tr("刷新")),SIGNAL(clicked()),this,SLOT(torefresh()));
    connect(ui->widget->index_widget(QObject::tr("保存")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));


    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_bt_dish_table_tableset_cut::~w_bt_dish_table_tableset_cut()
{
    delete ui;
}

void w_bt_dish_table_tableset_cut::tonew()
{
    bool pay_fixed_service = n_func::f_get_sysparm("pay_fixed_service", "0") == "1";
    int ch_service_mode = 0;
    double num_ration = 0;
    if(pay_fixed_service) {
        ch_service_mode = 4;
        num_ration = n_func::f_get_sysparm("pay_fixed_service_value", 0).toDouble();
    }

    int rowcount = tablemodel->rowCount();
    tablemodel->insertRow(rowcount);
    tablemodel->model_data_set(rowcount, "ch_typeno", tablemodel->max_column_add_1__max_sql_add_1_dec("ch_typeno", "%02d"));
    tablemodel->model_data_set(rowcount, "ch_service_mode", ch_service_mode);
    tablemodel->model_data_set(rowcount, "int_rate", 0);
    tablemodel->model_data_set(rowcount, "num_ration", num_ration);
    //    tablemodel->model_data_set(rowcount, "num_max_service", 0);
    tablemodel->model_data_set(rowcount, "ch_low_mode", 1);//默认整桌低消
    tablemodel->model_data_set(rowcount, "num_lowcost", 0);

    ui->tableView->setCurrentIndex(tablemodel->model_index(rowcount, "vch_typename"));
}

void w_bt_dish_table_tableset_cut::todel()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有行被选中"));
        return;
    }

    QString  ch_typeno = tablemodel->model_data(row, "ch_typeno").toString();
    lds_query_hddpos  query;
    query.execSelect(QString("select count(ch_tableno) from cey_bt_table where ch_typeno = '%1' ").arg(ch_typeno));
    query.next();
    if(query.recordValue(0).toInt() > 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("该类别下已有餐桌"));
        return;
    }

    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"),QObject::tr("确认"),QObject::tr("取消"))) {
        tablemodel->removeRow(row);
        tablemodel->trySubmitAll();
    }
}

void w_bt_dish_table_tableset_cut::tosave()
{
    QString errstring;
    for(int row=0,rowcount=tablemodel->rowCount();row<rowcount;row++) {
        if(tablemodel->model_data(row, "ch_typeno").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，编号不能为空").arg(row+1));
            return;
        }
        if(tablemodel->model_data(row, "vch_typename").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，桌类名称不能为空").arg(row+1));
            return;
        }

        if(tablemodel->model_data(row, "ch_service_mode").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，服务费模式不能为空").arg(row+1));
            return;
        }

        if(tablemodel->model_data(row, "ch_low_mode").toString().isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，低消方式不能为空").arg(row+1));
            return;
        }

        //============

        if(tablemodel->index(row, tablemodel->fieldIndex("ch_service_mode")).data().toInt() == 1
                &&tablemodel->index(row, tablemodel->fieldIndex("int_unit_minute")).data().toInt()<=0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("第%1行，超钟收费单位不能小于等于0").arg(row+1));
            return;
        }
    }

    ////////////////////////////////
    lds_query_hddpos  query;
    lds_query_hddpos::tran_saction();
    if(!tablemodel->trySubmitAll()) {
        goto rollback;
    }

    if(query.execUpdate("cey_bt_table_type",
                        "int_rate", qrtFunc("( if(ch_service_mode ='4', num_ration, 0 ))"),
                        "num_ration", qrtFunc("(if( ch_service_mode = '3', num_ration, 0 )) "),
                        "", &errstring)) {

        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        torefresh();
        return;
    }
rollback:
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    return;

}

void w_bt_dish_table_tableset_cut::toexit()
{
    this->reject();
}

void w_bt_dish_table_tableset_cut::torefresh()
{
    tablemodel->select();
    for(int row = 0, rowcount = tablemodel->rowCount(); row < rowcount; row ++) {
        if(tablemodel->model_data(row, "ch_service_mode").toString() == "4") {
            tablemodel->model_data_set(row, "num_ration", tablemodel->model_data(row, "int_rate").toFloat());
            continue;
        }
        if(tablemodel->model_data(row, "ch_service_mode").toString() == "3") continue;

        tablemodel->model_data_set(row, "num_ration", 0);
    }

    ui->tableView->resizeColumnsToContents(true);
}


w_bt_dish_table_new2::w_bt_dish_table_new2(QWidget *parent)
    :    w_bt_dish_table_virtual(parent)
{
    torefresh();
    ui->lineEdit_int_person->setValue(1);
    ui->pushButton_del->hide();

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(tosave()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_area, SIGNAL(clicked()),this,SLOT(toarea()));
    connect(ui->pushButton_type, SIGNAL(clicked()),this,SLOT(totype()));
    connect(ui->comboBox_ch_typeno, SIGNAL(currentIndexChanged(int)), this,SLOT(torefreshTableno(int)));
}

void w_bt_dish_table_new2::toexit()
{
    this->reject();
}

bool w_bt_dish_table_new2::tosave()
{
    if(!saveCheck()) {
        return false;
    }
    lds_query_hddpos  query;
    QString ch_tableno=ui->lineEdit_ch_tableno->text();
    query.execSelect(QString("SELECT count(0) from cey_bt_table where ch_tableno = '%1' ")
                     .arg(ch_tableno));
    query.next();
    if(query.recordValue(0).toInt() > 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("餐桌号有重复"));
    }

    lds_query_hddpos::tran_saction();

    if(query.execInsert("cey_bt_table",qrtVariantPairList()

                        << qrtVariantPair("ch_tableno", ui->lineEdit_ch_tableno->text())
                        << qrtVariantPair("vch_tablename", ui->lineEdit_vch_tablename->text())
                        << qrtVariantPair("ch_typeno", ui->comboBox_ch_typeno->curusrdata())
                        << qrtVariantPair("int_person", ui->lineEdit_int_person->value())
                        << qrtVariantPair("ch_areaNo", ui->comboBox_ch_areano->curusrdata())

                        << qrtVariantPair("dt_datetime", n_func::f_get_sysdatetime()))) {

        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        this->accept();
        return true;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  query.recordError());
    return false;
}

void w_bt_dish_table_new2::toarea()
{
    w_bt_dish_table_areaset dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void w_bt_dish_table_new2::totype()
{
    w_bt_dish_table_tableset_cut dialog(this);
    dialog.setFixedWidth(750);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
    torefresh();
}

void w_bt_dish_table_new2::torefresh()
{
    //save
    QVariant typeno = ui->comboBox_ch_typeno->curusrdata();
    QVariant areano = ui->comboBox_ch_areano->curusrdata();

    //set
    lds_query_hddpos  query;
    ui->comboBox_ch_typeno->clear();
    query.execSelect("select ch_typeno, vch_typename from cey_bt_table_type; ");
    while(query.next())
        ui->comboBox_ch_typeno->addItem(query.recordValue(1).toString(), query.recordValue(0));

    ui->comboBox_ch_areano->clear();
    query.execSelect("select ch_areano, vch_areaname from cey_bt_table_area; ");
    while(query.next())
        ui->comboBox_ch_areano->addItem(query.recordValue(1).toString(), query.recordValue(0));
    //restore
    if(typeno.isValid())
        ui->comboBox_ch_typeno->setcomusrdata(typeno);

    if(areano.isValid())
        ui->comboBox_ch_areano->setcomusrdata(areano);


    torefresh_ch_tableno(ui->comboBox_ch_typeno->curusrdata());
}

void w_bt_dish_table_new2::torefreshTableno(int index)
{
    torefresh_ch_tableno(ui->comboBox_ch_typeno->itemData(index));
}

void w_bt_dish_table_new2::torefresh_ch_tableno(const QVariant &ch_typeno)
{
    lds_query_hddpos  query;
    query.execSelect(QString("select max(ch_tableno) from cey_bt_table where left(ch_tableno, 2) = '%1' ")
                     .arg(ch_typeno.toString()));
    query.next();
    ui->lineEdit_ch_tableno->setText(ch_typeno.toString() +
                                     QString().sprintf("%02d", query.recordValue(0).toString().right(2).toInt()+1));
}


w_bt_dish_table_change2::w_bt_dish_table_change2(const QString &ch_tableno, QWidget *parent)
    : w_bt_dish_table_new2(parent)
{
    update_ch_tableno(ch_tableno);

    connect(ui->pushButton_del, SIGNAL(clicked()),this,SLOT(toDel()));
}

void w_bt_dish_table_change2::update_ch_tableno(const QString &ch_tableno)
{
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT ch_typeno, ch_areano, vch_tablename from cey_bt_table where ch_tableno = '%1' ")
                     .arg(ch_tableno));
    query.next();
    ui->comboBox_ch_typeno->setcomusrdata(query.recordValue("ch_typeno").toString());
    ui->comboBox_ch_areano->setcomusrdata(query.recordValue("ch_areano").toString());
    ui->lineEdit_vch_tablename->setText(query.recordValue("vch_tablename").toString());
    ui->lineEdit_ch_tableno->setText(ch_tableno);
    ui->lineEdit_ch_tableno->setEnabled(false);
    ui->comboBox_ch_typeno->setEnabled(false);
}

bool w_bt_dish_table_change2::tosave()
{
    if(!saveCheck()) {
        return false;
    }
    lds_query_hddpos  query;

    lds_query_hddpos::tran_saction();
    if(query.execUpdate("cey_bt_table",qrtVariantPairList()

                        << qrtVariantPair("vch_tablename", ui->lineEdit_vch_tablename->text())
                        << qrtVariantPair("ch_typeno", ui->comboBox_ch_typeno->curusrdata())
                        << qrtVariantPair("int_person", ui->lineEdit_int_person->value())
                        << qrtVariantPair("ch_areaNo", ui->comboBox_ch_areano->curusrdata()),
                        QString(" ch_tableno = '%1' ").arg(ui->lineEdit_ch_tableno->text())
                        )) {

        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        this->accept();
        return  true;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  query.recordError());

    return false;
}

void w_bt_dish_table_change2::torefresh_ch_tableno(const QVariant &/*ch_typeno*/)
{

}

void w_bt_dish_table_change2::toDel()
{
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"), QObject::tr("确定"), QObject::tr("取消")) ) {
        QString errstring;
        if(!w_bt_dish_table::tableDelTransaction(ui->lineEdit_ch_tableno->text(), &errstring)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        this->accept();
    }
}


w_bt_dish_table_virtual::w_bt_dish_table_virtual(QWidget *parent)
    : QDialog(parent),
      ui(new Ui_w_bt_dish_table_new2)
{
    ui->setupUi(this);
}

w_bt_dish_table_virtual::~w_bt_dish_table_virtual()
{
    delete ui;
}

void w_bt_dish_table_virtual::update_ch_typeno(const QString &ch_typeno)
{
    ui->comboBox_ch_typeno->setcomusrdata(ch_typeno);
}

void w_bt_dish_table_virtual::update_ch_areano(const QString &ch_areano)
{
    ui->comboBox_ch_areano->setcomusrdata(ch_areano);
}

bool w_bt_dish_table_virtual::saveCheck()
{
    QString ch_tableno=ui->lineEdit_ch_tableno->text();

    if(ui->comboBox_ch_typeno->currentIndex() < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("餐桌类型不能为空"));
        return false;
    }
    if(ui->comboBox_ch_areano->currentIndex() < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("餐区不能为空"));
        return false;
    }

    if(ch_tableno.startsWith("0")) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号开头不能为0"));
        return false;
    }
    if(ch_tableno.toInt() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("编号必须为数字"));
        return false;
    }
    if(ch_tableno.trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("餐桌号不能为空"));
        return false;
    }
    if(ui->lineEdit_vch_tablename->text().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("餐桌名不能为空"));
        return false;
    }
    return true;
}


w_bt_dish_table_new3::w_bt_dish_table_new3(QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui_w_bt_dish_unitset_Dialog;
    ui->setupUi(this);

    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_table");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_tableno",QObject::tr("餐桌号"));
    tablemodel->save_set_header("vch_tablename",QObject::tr("餐桌名"), true);
    tablemodel->save_set_header("ch_typeno",QObject::tr("餐桌类型"), true);
    tablemodel->save_set_header("ch_areano",QObject::tr("餐桌区域"), true);
    tablemodel->save_set_header("int_person",QObject::tr("人数"), true);
    tablemodel->enable_vheaderdata_dirty_translate();
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

    QTimer::singleShot(0, this, SLOT(torefresh()));

}

w_bt_dish_table_new3::~w_bt_dish_table_new3()
{
    delete ui;
}

void w_bt_dish_table_new3::tonew()
{
    int row = tablemodel->rowCount();
    tablemodel->insertRow(row);
    //
    //    lds_query_hddpos  query;
    //    query.execSelect(QString("select max(ch_tableno) from cey_bt_table where left(ch_tableno, 2) = '%1' ")
    //               .arg(ch_typeno.toString()));
    //    query.next();
    //    ui->lineEdit_ch_tableno->setText(ch_typeno.toString() +
    //                                     QString().sprintf("%02d", query.recordValue(0).toString().right(2).toInt()+1));

    //    tablemodel->model_data_set(row, "ch_tableno", );
}

void w_bt_dish_table_new3::todel()
{
    tablemodel->removeRow(ui->tableView->currentIndex().row());
}

bool w_bt_dish_table_new3::tosave()
{
    return tablemodel->trySubmitAll();
}

void w_bt_dish_table_new3::torefresh()
{
    tablemodel->select();
}

void w_bt_dish_table_new3::toexit()
{
    this->reject();
}

#include "w_scr_dish_num_cook_dialog.h"
#include "ui_w_scr_dish_num_cook_dialog.h"
#include "lds_query_hddpos.h"
#include "w_bt_dish_cookset.h"
#include <QPointer>
#include <QtDebug>
#include <QKeyEvent>
#include "public_sql.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "lds.h"
#include "fexpandmain_2.h"
#include "take_weight.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "lds_menu.h"
#include "ftableview_delegate.h"
#include "w_scr_dish_select_operid.h"
#include "backheader.h"

void w_scr_dish_num_cook_Dialog::static_to_present(QWidget *parent, fexpandmain_model_sqltablemodel  *tablemodel, int row, float num_num)
{
    QDateTime dt = n_func::f_get_sysdatetime();
    lds_query_hddpos  query;
    //权限，赠送上限
    if(lds::gs_operid != "0000") {
        query.execSelect(QString("select num_present from cey_sys_operator where vch_operID ='%1' and ch_flag = 'Y' ").arg(lds::gs_operid));
        query.next();
        float dec_presenttop = query.recordValue("num_present").toFloat();
        if(num_num > dec_presenttop) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("赠送金额超过赠送上限"));
            return ;
        }
    }
    //赠送
    if(tablemodel->model_data(row, "ch_presentflag").toString() != "Y") {
        if(0==lds_messagebox::question(parent, MESSAGE_TITLE_VOID, QObject::tr("商品赠送"), QObject::tr("确认"), QObject::tr("取消"))) {
            tablemodel->model_data_set(row, "ch_presentflag", "Y");
            tablemodel->model_data_set(row, "vch_pre_operID", lds::gs_operid);
            tablemodel->model_data_set(row, "dt_pre_operdate", dt);
            tablemodel->model_data_set(row, "int_discount", 100);
            return ;
        }
    } else {
        if(0==lds_messagebox::question(parent, MESSAGE_TITLE_VOID, QObject::tr("此商品已经被赠送,是否取消赠送?"), QObject::tr("确认"), QObject::tr("取消"))) {
            if(tablemodel->model_data(row, "vch_pre_operID").toString() == lds::gs_operid) {
                tablemodel->model_data_set(row, "ch_presentflag", "N");
                tablemodel->model_data_set(row, "vch_pre_operID", "");
                tablemodel->model_data_set(row, "dt_pre_operdate", DATETIME_VOID);
            } else {
                lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("此商品已经被赠送") + "," + QObject::tr("赠送人") + ":" + tablemodel->model_data(row, "vch_pre_operID").toString());
                return;
            }
        }
    }
}

w_scr_dish_num_cook_Dialog::w_scr_dish_num_cook_Dialog(const fexpandmain_model_sqltablemodel_data *model, int row, QWidget *parent) :
    QDialog(parent)
{
    init_ui();
    refresh(model, row);
}

void w_scr_dish_num_cook_Dialog::enable_cook(bool f) const
{
    ui->pushButton_clearcook->setEnabled(f);
    ui->pushButton_take_weight->setEnabled(f);
    ui->frame_cook->setEnabled(f);
    ui->pushButton_tab_cook->setEnabled(f);
}

void w_scr_dish_num_cook_Dialog::cook_refresh()
{
    lds_query_hddpos  query;
    QString ch_dishno = tablemodel->model_data(0, "ch_dishno").toString();
    QList<ftableview_standmodel_sql_none::noneData> list_data;
    QString tablename;
    if(lds::sysconf->value("w_scr_dish_num_cook_Dialog/cook_tab", "cook").toString() == "cook") {
        tablename = "cook";
        query.execSelect(QString(" select count(0) from cey_bt_cp_memo a, cey_bt_cp_dish b where a.ch_no = b.ch_no and b.ch_dishno = '%1' ").arg(ch_dishno));
        query.next();
        if(query.recordValue(0).toInt() > 0) {
            query.execSelect(QString(" select a.ch_no, a.vch_cp_memo, a.num_add_price from cey_bt_cp_memo a, cey_bt_cp_dish b where a.ch_no = b.ch_no and b.ch_dishno = '%1' ").arg(ch_dishno));
            while(query.next()) {
                list_data << ftableview_standmodel_sql_none::noneData(
                                 QString("%1\n%2").arg(query.recordValue("vch_cp_memo").toString(), query.recordValue("num_add_price").toString()),
                                 query.recordValue("ch_no").toString()
                                 );
            }
        } else {
            query.execSelect(QString(" select a.ch_no, a.vch_cp_memo, a.num_add_price from cey_bt_cp_memo a"));
            while(query.next()) {
                list_data << ftableview_standmodel_sql_none::noneData(
                                 QString("%1\n%2").arg(query.recordValue("vch_cp_memo").toString(), query.recordValue("num_add_price").toString()),
                                 query.recordValue("ch_no").toString()
                                 );
            }
        }
    }
    if(lds::sysconf->value("w_scr_dish_num_cook_Dialog/cook_tab", "cook").toString() == "addon") {
        tablename = "addon";
        query.execSelect(QString("select ch_dishno, vch_dishname, num_price from cey_bt_dish where ch_addon_flag = 'Y' "));//  select a.ch_no, a.vch_cp_memo, a.num_add_price from cey_bt_cp_memo a, cey_bt_cp_dish b where a.ch_no = b.ch_no and b.ch_dishno = '%1' ").arg(ch_dishno));
        while(query.next()) {
            list_data << ftableview_standmodel_sql_none::noneData(
                             QString("%1\n%2").arg(query.recordValue("vch_dishname").toString(), query.recordValue("num_price").toString()),
                             query.recordValue("ch_dishno").toString()
                             );
        }
    }

    cook_model->setPrepareList(list_data);
    //
    const lds_model_sqltablemodel_d *d = tablemodel->tablename_kvmap_d(tablename);
    for(int row = 0; row < d->rowCount(); row ++) {
        QString no = d->data(row, "no").toString();
        double num = d->data(row, "num").toDouble();
        cook_model->select(no);
        cook_model->page_record_set_data(no, num, Qt::UserRole + 3);//做法数量
    }
    tablemodel->tablename_kvmap_print();
    //
    cook_model->toinit();
}

QMap<QString, double> w_scr_dish_num_cook_Dialog::cur_cook_data()
{
    QMap<QString, double> map;
    ftableview_standmodel_virtual::usrVarMapIterator mapitr(cook_model->currentSelected());
    while(mapitr.hasNext()) {
        mapitr.next();
        //
        QString ch_no = mapitr.value().value(1/*做法编号*/).toString();
        int cook_num = mapitr.value().value(3/*做法数量*/).toInt();
        cook_num = cook_num == 0 ? 1 : cook_num;
        //
        map.insert(ch_no, cook_num);
    }

    return map;
}

w_scr_dish_num_cook_Dialog::~w_scr_dish_num_cook_Dialog()
{
    delete ui;
}

void w_scr_dish_num_cook_Dialog::init_ui()
{
    ui = new Ui::w_scr_dish_num_cook_Dialog;
    ui->setupUi(this);
    tablemodel = new fexpandmain_model_sqltablemodel(this);
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    frommodel =  0;
    cook_model = new ftableview_standmodel_sql_none;
    ftableview_delegate *d = new ftableview_delegate(ui->frame_cook);
    d->keepConnect(ui->frame_cook);
    d->b_shwoGrid = true;
    d->set_pay_unit_prefix(n_func::f_get_sysparm("pay_unit_prefix"));

    ui->frame_cook->setModel(cook_model);
    ui->frame_cook->setProperty("outer_stylesheet", "tableView_type");
    ui->frame_cook->setItemDelegate(d);
    ui->frame_cook->updateFixed_fillData(4, 4, 85, 0);
    ui->frame_cook->setSelectionMode(QTableView::MultiSelection);
    ui->frame_cook->enable_mouse_press_after_select();
    connect(ui->frame_cook, SIGNAL(signal_enable_mouse_press_after_select(QModelIndex)), SLOT(tochangenum(QModelIndex)));
    //
    ui->lineEdit_vch_dishname->setEnabled(false);
    ui->lineEdit_vch_operID->setEnabled(false);
    ui->lineEdit_vch_operID->setcomMapBySql(backheader::SELECT_OPERID_NAME);
    //    connect(ui->pushButton_allcook, SIGNAL(clicked()),this,SLOT(toallcook()));
    connect(ui->pushButton_clearcook, SIGNAL(clicked()),this,SLOT(toclearcook()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_present, SIGNAL(clicked()),this,SLOT(topresent()));
    connect(ui->pushButton_take_weight, SIGNAL(clicked()),this,SLOT(totakeweight()));
    connect(ui->pushButton_cookset, SIGNAL(clicked()),this,SLOT(tocookset()));
    connect(ui->pushButton_tab_addon, SIGNAL(clicked()),this,SLOT(toswtichTab()));
    connect(ui->pushButton_tab_cook, SIGNAL(clicked()),this,SLOT(toswtichTab()));
    connect(ui->toolButton_vch_operID, SIGNAL(clicked()), this, SLOT(tochangevch_operID()));

    label_info  = new QTextEdit(this);
    label_info->setMinimumSize(300, 300);
    label_info->setStyleSheet("color:#ff546b;");
    lds_menu *menu=new lds_menu(this);
    QWidgetAction *widgetaction = new QWidgetAction(ui->pushButton_more);
    widgetaction->setDefaultWidget(label_info);
    menu->addAction(widgetaction);
    ui->pushButton_more->setMenu(menu);

    ui->pushButton_more->hide();
    label_info->hide();
    if(lds::get_soft_curent_language_first() == _CN_) {
        label_info->show();
        ui->pushButton_more->show();
    }

    //    ui->frame_cook->enable_dollar();
    ui->pushButton_present->hide();
    //    ui->doubleSpinBox_num_num->installEventFilter(this);
    //    ui->doubleSpinBox_num_price->installEventFilter(this);
    //    ui->lineEdit_vch_dishname->installEventFilter(this);
    //    ui->spinBox_int_discount->installEventFilter(this);
    //    ui->pushButton_allcook->installEventFilter(this);
    //    ui->pushButton_cancel->installEventFilter(this);
    //    ui->pushButton_clearcook->installEventFilter(this);
    //    ui->pushButton_more->installEventFilter(this);
    //    ui->pushButton_ok->installEventFilter(this);
    //    ui->pushButton_present->installEventFilter(this);
    //    ui->pushButton_take_weight->installEventFilter(this);

    if(public_sql::discount_off_mode()) {
        ui->spinBox_int_discount->enableValueMaxMinus();
    }
    ui->spinBox_int_discount->setRange(0, 100);
    //
    ui->pushButton_tab_cook->setChecked(lds::sysconf->value("w_scr_dish_num_cook_Dialog/cook_tab", "cook").toString() == "cook");
    ui->pushButton_tab_addon->setChecked(lds::sysconf->value("w_scr_dish_num_cook_Dialog/cook_tab", "cook").toString() == "addon");
    //
    setWindowTitle(QObject::tr("详细"));
}

void w_scr_dish_num_cook_Dialog::refresh(const fexpandmain_model_sqltablemodel_data *model, int row)
{
    ui->doubleSpinBox_num_num->setDecimals(model->get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt());
    ui->doubleSpinBox_num_price->setDecimals(model->get_cey_sys_parameter_value("bp_fmt_price_dec", 2).toInt());

    //init
    if(frommodel == 0) {
        frommodel = model;
        tablemodel->setTable(frommodel->tableName());
        frommodel_row.assignValue(row);
    }

    //refresh
    tablemodel->removeRows(0, tablemodel->rowCount());
    //1
    tablemodel->insertRow(0);
    for(int c = 0, cc = frommodel->columnCount(); c < cc; c++) {
        tablemodel->model_data_set(0, c,  frommodel->model_data(row, c));
    }
    refresh_sub(row);
}

double w_scr_dish_num_cook_Dialog::get_cur_dish_num()
{
    return ui->doubleSpinBox_num_num->value();
}

const fexpandmain_model_sqltablemodel *w_scr_dish_num_cook_Dialog::model()
{
    return tablemodel;
}

void w_scr_dish_num_cook_Dialog::refresh_sub(int row)
{
    //
    bool flag;
    label_info->clear();
    //2
    QString ch_dishno = frommodel->model_data(row, "ch_dishno").toString();
    ui->doubleSpinBox_num_num->setValue(frommodel->model_data_NIBOLAN(row, "num_num - num_back"));//这边若是已经落单，则无法修改数量，
    ui->doubleSpinBox_num_price->setValue(frommodel->model_data(row, "num_price").toDouble());
    ui->lineEdit_vch_dishname->setText(frommodel->get_vch_dishname_by_map(ch_dishno));
    ui->spinBox_int_discount->setValue(frommodel->model_data(row, "int_discount").toInt());

    ui->lineEdit_vch_operID->setcomKey(frommodel->model_data(row, "vch_operID").toString());

    ///3
    tablemodel->tablename_kvmap_insert_table("cook");
    tablemodel->tablename_kvmap_insert_table("addon");
    //cook
    vch_print_memoData vch_print_memo;
    vch_print_memo.select(frommodel->model_data(frommodel_row, "vch_print_memo").toString());
    while(vch_print_memo.next()) {
        QString no = vch_print_memo.nextValue("no").toString();
        double num = vch_print_memo.nextValue("num").toDouble();
        tablemodel->tablename_kvmap_insert_field("cook", no, "no", no);
        tablemodel->tablename_kvmap_insert_field("cook", no, "num", num);
    }
    //addon
    if(frommodel->model_data(frommodel_row, "ch_addon_flag").toString() == "P") {
        for(int row = frommodel_row + 1; row < frommodel->rowCount() && frommodel->model_data(row, "ch_addon_flag").toString() == "Y"; row ++) {
            QString no = frommodel->model_data(row, "ch_dishno").toString();
            double num = frommodel->model_data_NIBOLAN(row, "num_num-num_back");
            tablemodel->tablename_kvmap_insert_field("addon", no, "no", no);
            tablemodel->tablename_kvmap_insert_field("addon", no, "num", num);
        }
    }
    cook_refresh();
    ///~

    //5
    QString errstring;

    if(!errstring.isEmpty()) label_info->append(errstring);
    //折扣
    flag = true;
    if(!frommodel->can_discount(row, &errstring)) {
        flag = false;
        label_info->append(errstring);
    }
    ui->spinBox_int_discount->setEnabled(flag);
    //做法
    if(!frommodel->can_cook(row, &errstring)) {
        enable_cook(false);
        label_info->append(errstring);
    }
    if(lds_query_hddpos::selectValue(QString(" select count(0) from cey_bt_cp_memo a, cey_bt_cp_dish b where a.ch_no = b.ch_no and b.ch_dishno = '%1' ").arg(ch_dishno)).toInt() == 0) {
        lds::hideWidget(ui->pushButton_cookset);
    }
    //加料
    if(!frommodel->can_addon(row, &errstring)) {
        ui->pushButton_tab_addon->setEnabled(false);
        label_info->append(errstring);
    }
    if(!ui->pushButton_tab_addon->isEnabled()) {
        if(ui->pushButton_tab_addon->isChecked()) {
            ui->pushButton_tab_cook->click();
        }
    }
    //价格
    flag = true;
    if(!frommodel->can_change_price(row, &errstring)) {
        flag = false;
        label_info->append(errstring);
    }
    ui->doubleSpinBox_num_price->setEnabled(flag);
    //数量
    flag  = true;
    if(!frommodel->can_change_num(row, &errstring)) {
        flag = false;
        label_info->append(errstring);
    }
    ui->doubleSpinBox_num_num->setEnabled(flag);
    ui->pushButton_take_weight->setEnabled(flag);
    lds::showWidget(ui->toolButton_vch_operID, flag);

    if(lds::sysconf->value("w_sys_manage_outer_pay_set/scale_led_mode", false).toBool()) {//非称重模式
        ui->pushButton_take_weight->setEnabled(false);
    }
    //赠送
    flag = true;
    if(!frommodel->can_present(row, &errstring)) {
        flag = false;
        label_info->append(errstring);
    }
    ui->pushButton_present->setEnabled(flag);
    ui->pushButton_present->set_rt_checked(frommodel->model_data(row, "ch_presentflag").toString() == "Y");
    //操作员
}

void w_scr_dish_num_cook_Dialog::toallcook()
{
}

void w_scr_dish_num_cook_Dialog::toclearcook()
{
    cook_model->clearSelected();
}

void w_scr_dish_num_cook_Dialog::took()
{
    saveCurdata();

    lds_query_hddpos query;
    float num_num = ui->doubleSpinBox_num_num->value();
    if(num_num == 0) num_num = 1;
    //num_num
    if(ui->doubleSpinBox_num_num->value() <= 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数量不能小于等于0"));
        ui->doubleSpinBox_num_num->setFocus();
        return;
    }
    QString discount_desc;
    if(false == public_sql::can_discount_limit(this, ui->spinBox_int_discount->trueValue(), QObject::tr("折扣"), discount_desc)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, discount_desc);
        return;
    }

    //将数据提交给model
    tablemodel->model_data_set(0, "num_num", ui->doubleSpinBox_num_num->value() + tablemodel->model_data(0, "num_back").toFloat());
    tablemodel->model_data_set(0, "num_price", ui->doubleSpinBox_num_price->value());
    tablemodel->model_data_set(0, "int_discount", ui->spinBox_int_discount->value());

    ///
    //cook
    const  lds_model_sqltablemodel_d *d = tablemodel->tablename_kvmap_d("cook");
    vch_print_memoData vch_print_memo;
    for(int row = 0; row < d->rowCount(); row ++)
        vch_print_memo.duplicate(query.db, d->data(row, "no").toString(), d->data(row, "num").toDouble());
    tablemodel->model_data_set(0, "vch_print_memo", vch_print_memo.sqlValue());
    tablemodel->model_data_set(0, "num_price_add",  vch_print_memo.total());
    if(tablemodel->model_data(0, "ch_suitflag").toString() == "Y" ) {
        tablemodel->model_data_set(0, "num_price_add", 0);
    }
    //
    tablemodel->model_data_set(0, "vch_operID",  ui->lineEdit_vch_operID->comKey());
    //关闭
    this->accept();
}

void w_scr_dish_num_cook_Dialog::tocancel()
{
    this->reject();
}

void w_scr_dish_num_cook_Dialog::topresent()
{
    w_scr_dish_num_cook_Dialog::static_to_present(this,
                                                  tablemodel,
                                                  0,
                                                  ui->doubleSpinBox_num_num->value());
    ui->pushButton_present->set_rt_checked(tablemodel->model_data(0, "ch_presentflag").toString() == "Y");
}

void w_scr_dish_num_cook_Dialog::totakeweight()
{
    //没有数据被选中
    take_weight dialog(tablemodel->model_data(0, "ch_dishno").toString(),
                       ui->doubleSpinBox_num_price->value(),
                       ui->doubleSpinBox_num_num->value(),
                       ui->spinBox_int_discount->value(),
                       this);
    dialog.setWindowTitle(QObject::tr("取重"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        ui->doubleSpinBox_num_num->setValue(dialog._weight);
    }
}

void w_scr_dish_num_cook_Dialog::tocookset()
{
    QString ch_dishno = tablemodel->model_data(0, "ch_dishno").toString();
    w_bt_dish_cookset_typeset dialog(this);
    dialog.enableSaveAndAppDish(ch_dishno);
    dialog.setWindowTitle(tablemodel->tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", "vch_dishname").toString());
    transparentCenterDialog(&dialog).exec();
    cook_refresh();
}

void w_scr_dish_num_cook_Dialog::tomore()
{
    static QLabel *label = new QLabel;
    if(ui->pushButton_more->menu() == 0) {
        label->setMinimumSize(300, 300);
        label->setStyleSheet("color:#ff546b; font-size:16pt;");
        lds_menu *menu=new lds_menu(this);
        QWidgetAction *widgetaction = new QWidgetAction(ui->pushButton_more);
        widgetaction->setDefaultWidget(label);
        menu->addAction(widgetaction);
        ui->pushButton_more->setMenu(menu);
    }
}

void w_scr_dish_num_cook_Dialog::tochangenum(const QModelIndex &index)
{
    QString key = index.data(Qt::UserRole + 1).toString();
    if(key == "-1"
            || key == "-2"
            || key == "-3"
            || key.isEmpty()
            ) {
        return;
    }

    int cook_num = index.data(Qt::UserRole + 3/*组数量*/).toInt();
    if(0 == cook_num) cook_num = 1;

    lds_dialog_input inputdialog(this);
    inputdialog.setDotEnabled(false);
    inputdialog.ui->label->setText(QObject::tr("数量"));
    inputdialog.setWindowTitle(QObject::tr("修改数量"));
    inputdialog.ui->lineEdit->setText(QString::number(cook_num));
    if(QDialog::Accepted == transparentCenterDialog(&inputdialog).exec()) {
        float num = inputdialog.ui->lineEdit->text().toInt();
        cook_model->setData(index, num, Qt::UserRole+3/*组数量*/);
        if(num == 0) {
            ui->frame_cook->setCurrentIndex(index);
        }
        return;
    }
}

void w_scr_dish_num_cook_Dialog::toswtichTab()
{
    //save
    saveCurdata();
    //init
    ui->pushButton_tab_addon->setChecked(false);
    ui->pushButton_tab_cook->setChecked(false);
    lds::sysconf->setValue("w_scr_dish_num_cook_Dialog/cook_tab", "");

    //
    if(this->sender() == ui->pushButton_tab_addon) {
        ui->pushButton_tab_addon->setChecked(true);
        lds::sysconf->setValue("w_scr_dish_num_cook_Dialog/cook_tab", "addon");
    }
    if(this->sender() == ui->pushButton_tab_cook) {
        ui->pushButton_tab_cook->setChecked(true);
        lds::sysconf->setValue("w_scr_dish_num_cook_Dialog/cook_tab", "cook");
    }
    //refresh
    cook_refresh();
}

void w_scr_dish_num_cook_Dialog::tochangevch_operID()
{
    w_scr_dish_select_operid dialog(ui->lineEdit_vch_operID->comKey(), this);
    dialog.setWindowTitle(QObject::tr("操作员"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        ui->lineEdit_vch_operID->setcomKey(dialog.selectOperid());
    }
}

void w_scr_dish_num_cook_Dialog::saveCurdata()
{
    QString tablename = "cook";
    if(lds::sysconf->value("w_scr_dish_num_cook_Dialog/cook_tab", "cook").toString() == "addon") {
        tablename = "addon";
    }
    tablemodel->tablename_kvmap_delete(tablename);
    //
    QMap<QString, double> map = cur_cook_data();
    for(QMap<QString, double>::iterator k = map.begin(); k != map.end(); k ++) {
        tablemodel->tablename_kvmap_insert_field(tablename, k.key(), "no", k.key());
        tablemodel->tablename_kvmap_insert_field(tablename, k.key(), "num", k.value());
    }
    tablemodel->tablename_kvmap_print();
}

QString w_scr_dish_num_cook_Dialog::operName(const QString &vch_operid)
{
    if(vch_operid == "0000")
        return "system";
    return lds_query_hddpos::selectValue(QString(" select vch_operator from cey_sys_operator where vch_operID = '%1' ").arg(vch_operid)).toString();
}

vch_print_memoData w_scr_dish_num_cook_Dialog::print_memo(const QMap<QString, double> &map)
{
    lds_query_hddpos query;
    vch_print_memoData d;
    for(QMap<QString, double>::const_iterator k = map.begin(); k != map.end(); k ++) {
        d.duplicate(query.db, k.key(), k.value());
    }
    return d;
}

//bool w_scr_dish_num_cook_Dialog::eventFilter(QObject *o, QEvent *e)
//{
//    if(QEvent::KeyPress == e->type()) {
//        QKeyEvent *ke = ((QKeyEvent *)e);
//        const int kvalue = ke->key();
//        if( kvalue == Qt::Key_Return
//                || kvalue == Qt::Key_Enter
//                ) {
//            took();
//            e->ignore();
//            return true;
//        }
//    }
//    return QDialog::eventFilter(o, e);
//}

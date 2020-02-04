#include "w_sys_manage_outer_bill_preview.h"
#include "ui_w_sys_manage_outer_bill_preview.h"
#include "ftableview_delegate.h"
#include "ftableview_standmodel_sql_none.h"
#include "n_func.h"
#include "lds.h"
#include "getfilepath.h"
#include "public_printby_ch_billno.h"
#include "lds_menu.h"
#include "lds.h"
#include "lds_messagebox.h"
#include <QScrollBar>
#include <QTimer>
#include "transparentcenterdialog.h"
#include "public_printby_ch_billno_over_view.h"

static QString map_filter = "bp_fmt";
static QString ifEmpty(const QString &text, const QString &def) {
    if(text.trimmed().isEmpty()) {
        return def;
    }
    return text;
}

static void print(QMap<QString, int> &map, int &k, const QObjectList &children) {
    foreach(const QObject *x, children) {
        if(x->objectName().startsWith("bp_fmt_")
                && !x->objectName().endsWith("_len")
                && !x->objectName().endsWith("_row")
                && !x->objectName().endsWith("_hheader_order")
                && !x->objectName().endsWith("_left")
                && !x->objectName().endsWith("_en")
                && !x->objectName().endsWith("_enable")
                && !x->objectName().endsWith("_print_prepare")
                && !x->objectName().endsWith("_fmt")
                ) {
            map.insert(x->objectName().mid(7), k);
            k ++;
        } else if(x->objectName() == "bp_fmt_title_enable") {
            map.insert(x->objectName().mid(7), k);
            k ++;
        }

        print(map, k, x->children());
    }
}

w_sys_manage_outer_bill_preview::w_sys_manage_outer_bill_preview(QWidget *parent) :
    lds_model_mapwidget_vertical(parent),
    ui(new Ui::w_sys_manage_outer_bill_preview)
{
    ui->setupUi(this);

#ifdef QT_DEBUG
    int k = 1;
    int x = 1;
    QMap<QString, int> map;
    print(map, k, this->children());
    for(QMap<QString, int>::iterator k = map.begin(); k != map.end(); k++) {
        qDebug() << x << k.key();
        x ++;
    }
#endif

    ui->bp_fmt_series_group->addItem(QObject::tr("不分组"), "0");
    ui->bp_fmt_series_group->addItem(QObject::tr("按大类"), "1");
    ui->bp_fmt_series_group->addItem(QObject::tr("按小类"), "2");

    ui->bp_fmt_dishname_row->addItem("1", 1);
    ui->bp_fmt_dishname_row->addItem("2", 2);

    ui->bp_fmt_amount_row->addItem("1", 1);
    ui->bp_fmt_amount_row->addItem("2", 2);

    ui->bp_fmt_price_row->addItem("1", 1);
    ui->bp_fmt_price_row->addItem("2", 2);

    ui->bp_fmt_unit_row->addItem("1", 1);
    ui->bp_fmt_unit_row->addItem("2", 2);

    ui->bp_fmt_disdetail_row->addItem("1", 1);
    ui->bp_fmt_disdetail_row->addItem("2", 2);

    ui->bp_fmt_total_row->addItem("1", 1);
    ui->bp_fmt_total_row->addItem("2", 2);

    ui->bp_fmt_num_row->addItem("1", 1);
    ui->bp_fmt_num_row->addItem("2", 2);

    ui->bp_fmt_table_fmts->addItem(QObject::tr("餐桌编号"), "no");
    ui->bp_fmt_table_fmts->addItem(QObject::tr("餐桌"), "name");
    ui->bp_fmt_table_fmts->addItem(QObject::tr("餐桌")+"("+QObject::tr("编号")+")", "name(no)");

    ui->bp_fmt_operid_fmts->addItem(QObject::tr("工号"), "no");
    ui->bp_fmt_operid_fmts->addItem(QObject::tr("名称"), "name");
    ui->bp_fmt_operid_fmts->addItem(QObject::tr("名称")+"("+QObject::tr("工号")+")", "name(no)");

    ui->bp_fmt_table_len->addItem("00", "00");
    ui->bp_fmt_table_len->addItem("01", "01");
    ui->bp_fmt_table_len->addItem("10", "10");
    ui->bp_fmt_table_len->addItem("11", "11");
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_payno_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_operid_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_person_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_time_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_serial_no_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_memo_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_address_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_tel_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_linkman_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_waiter_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_times_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_waitername_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_content_len);
    //    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_num_len);
    //    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_dishname_len);
    //    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_unit_len);
    //    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_amount_len);
    //    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_price_len);
    //    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_disdetail_len);
    //    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_total_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_cost_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_discount_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_present_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_service_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_tax_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_lowcost_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_blotout_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_receive_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_cash_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_pay_detail_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_avg_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_change_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_free_len);

    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_m_no_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_m_name_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_m_amount_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_m_adress_len);
    ui->bp_fmt_table_len->copyItemsTo(ui->bp_fmt_m_point_len);
    //
    //dishname,num,unit,price,total,disdetail,amount
    bp_fmt_hheader_rect.append(QList<QRect>());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_dishname->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_dishname_len->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_dishname_left->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_dishname_row->geometry());
    bp_fmt_hheader_rect.last().append(ui->pushButton_fmt_dishname_switch->geometry());
    //
    bp_fmt_hheader_rect.append(QList<QRect>());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_num->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_num_len->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_num_left->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_num_row->geometry());
    bp_fmt_hheader_rect.last().append(ui->pushButton_fmt_num_switch->geometry());
    //
    bp_fmt_hheader_rect.append(QList<QRect>());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_unit->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_unit_len->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_unit_left->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_unit_row->geometry());
    bp_fmt_hheader_rect.last().append(ui->pushButton_fmt_unit_switch->geometry());
    //
    bp_fmt_hheader_rect.append(QList<QRect>());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_price->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_price_len->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_price_left->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_price_row->geometry());
    bp_fmt_hheader_rect.last().append(ui->pushButton_fmt_price_switch->geometry());
    //
    bp_fmt_hheader_rect.append(QList<QRect>());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_total->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_total_len->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_total_left->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_total_row->geometry());
    bp_fmt_hheader_rect.last().append(ui->pushButton_fmt_total_switch->geometry());
    //
    bp_fmt_hheader_rect.append(QList<QRect>());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_disdetail->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_disdetail_len->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_disdetail_left->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_disdetail_row->geometry());
    bp_fmt_hheader_rect.last().append(ui->pushButton_fmt_disdetail_switch->geometry());
    //
    bp_fmt_hheader_rect.append(QList<QRect>());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_amount->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_amount_len->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_amount_left->geometry());
    bp_fmt_hheader_rect.last().append(ui->bp_fmt_amount_row->geometry());
    bp_fmt_hheader_rect.last().append(ui->pushButton_fmt_amount_switch->geometry());
    //1
    ftableview_standmodel_sql_none * backlist_model = new ftableview_standmodel_sql_none;
    backlist_model->set_row_column(4, 1);
    backlist_model->model_data_set(0, 0,QObject::tr("表头设置"), Qt::UserRole);
    backlist_model->model_data_set(0, 0,"anyvalue", Qt::UserRole+1);

    backlist_model->model_data_set(1, 0,QObject::tr("账单内容"), Qt::UserRole);
    backlist_model->model_data_set(1, 0,"anyvalue", Qt::UserRole+1);

    backlist_model->model_data_set(2, 0,QObject::tr("表尾内容") + "1", Qt::UserRole);
    backlist_model->model_data_set(2, 0,"anyvalue", Qt::UserRole+1);

    backlist_model->model_data_set(3, 0,QObject::tr("表尾内容") + "2", Qt::UserRole);
    backlist_model->model_data_set(3, 0,"anyvalue", Qt::UserRole+1);

    ftableview_delegate *d = new ftableview_delegate(ui->tableView_list);
    d->keepConnect(ui->tableView_list);
    //    d->setMargin(2);
    d->indexisEmptyDrawGrad = true;
    ui->tableView_list->setProperty("outer_stylesheet", "tableView_type");

    ui->tableView_list->setModel(backlist_model);
    ui->tableView_list->setItemDelegate(d);
    ui->tableView_list->setHeaderResizeMode(Qt::Horizontal, QHeaderView::Stretch);
    ui->tableView_list->verticalHeader()->setDefaultSectionSize(50);
    //3
    ui->tableView_list->setCurrentIndex(backlist_model->index(0, 0));
    toupdateSection(ui->tableView_list->currentIndex());

    scan_all_init_value("cey_sys_parameter", "vch_parameter", "vch_value", "bp_fmt_");
    //
    connect(ui->tableView_list,SIGNAL(clicked(QModelIndex)),this,SLOT(toupdateSection(QModelIndex)));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_getpath, SIGNAL(clicked()),this,SLOT(togetlogo()));
    connect(ui->pushButton_bill_export,SIGNAL(clicked()),this,SLOT(tobillexport()));
    connect(ui->pushButton_bill_import,SIGNAL(clicked()),this,SLOT(tobillimport()));
    connect(ui->pushButton_rb_input,SIGNAL(clicked()),this,SLOT(to_rb_input()));

    connect(ui->pushButton_hide_ExSet, SIGNAL(clicked()), this, SLOT(toHideExSet()));
    //    connect(ui->bp_fmt_serial_no, SIGNAL(timeout()), this, SLOT(toShowExSet()));
    //    connect(ui->bp_fmt_table, SIGNAL(timeout()), this, SLOT(toShowExSet()));
    //    connect(ui->bp_fmt_operid, SIGNAL(timeout()), this, SLOT(toShowExSet()));

    connect(ui->pushButton_fmt_dishname_switch, SIGNAL(clicked()), this, SLOT(to_content_hheader_order()));
    connect(ui->pushButton_fmt_num_switch, SIGNAL(clicked()), this, SLOT(to_content_hheader_order()));
    connect(ui->pushButton_fmt_unit_switch, SIGNAL(clicked()), this, SLOT(to_content_hheader_order()));
    connect(ui->pushButton_fmt_price_switch, SIGNAL(clicked()), this, SLOT(to_content_hheader_order()));
    connect(ui->pushButton_fmt_total_switch, SIGNAL(clicked()), this, SLOT(to_content_hheader_order()));
    connect(ui->pushButton_fmt_disdetail_switch, SIGNAL(clicked()), this, SLOT(to_content_hheader_order()));
    connect(ui->pushButton_fmt_amount_switch, SIGNAL(clicked()), this, SLOT(to_content_hheader_order()));
    connect(ui->pushButton_fmt_default, SIGNAL(clicked()), this, SLOT(to_content_hheader_order()));
    connect(ui->pushButton_bill_test, SIGNAL(clicked()), this, SLOT(to_bill_preview()));


    ui->bp_fmt_table_len->addItem(QObject::tr("设计"), "design");ui->bp_fmt_table_len->setStyleSheet("QComboBox {padding: 1px 1px 1px 3px;color:#ff546b;}");
    ui->bp_fmt_serial_no_len->addItem(QObject::tr("设计"), "design");ui->bp_fmt_serial_no_len->setStyleSheet("QComboBox {padding: 1px 1px 1px 3px;color:#ff546b;}");
    ui->bp_fmt_operid_len->addItem(QObject::tr("设计"), "design");ui->bp_fmt_operid_len->setStyleSheet("QComboBox {padding: 1px 1px 1px 3px;color:#ff546b;}");
    connect(ui->bp_fmt_table_len, SIGNAL(currentIndexChanged(int)), this, SLOT(toShowExSet()));
    connect(ui->bp_fmt_serial_no_len,  SIGNAL(currentIndexChanged(int)), this, SLOT(toShowExSet()));
    connect(ui->bp_fmt_operid_len, SIGNAL(currentIndexChanged(int)), this, SLOT(toShowExSet()));
    lds::hideWidget(ui->bp_fmt_hheader_order);
    //长按可修改
    //    ui->bp_fmt_serial_no->enableLongPress(true);
    //    ui->bp_fmt_table->enableLongPress(true);
    //    ui->bp_fmt_operid->enableLongPress(true);
    //ini
    ui->bp_fmt_serial_no->setText(ui->bp_fmt_serial_no_desc->text());
    ui->bp_fmt_table->setText(ui->bp_fmt_table_fmts->currentText());
    ui->bp_fmt_operid->setText(ui->bp_fmt_operid_fmts->currentText());
    //bp_fmt_serial_no_desc作为备用
    ui->gridLayout->removeWidget(ui->bp_fmt_serial_no_desc);
    ui->gridLayout->removeWidget(ui->bp_fmt_table_fmts);
    ui->gridLayout->removeWidget(ui->bp_fmt_operid_fmts);
    ui->gridLayout->removeWidget(ui->pushButton_hide_ExSet);
    //备用不显示
    lds::hideWidget(ui->bp_fmt_serial_no_desc);
    lds::hideWidget(ui->bp_fmt_table_fmts);
    lds::hideWidget(ui->bp_fmt_operid_fmts);
    lds::hideWidget(ui->pushButton_hide_ExSet);
    //
    billpreview_paperwidth = 48;

    //Ownership of the menu is not transferred to the tool button.
    ui->pushButton_get_print_prepare_ins->setMenu(new QMenu);
    ui->pushButton_get_print_prepare_ins->menu()->addAction(QObject::tr("小票在80mm纸中居中显示"), this, SLOT(to_print_prepare_ins_58receipt_center_in_80paper()));
}

w_sys_manage_outer_bill_preview::~w_sys_manage_outer_bill_preview()
{
    if(ui)delete ui;
}

void w_sys_manage_outer_bill_preview::if_bill_empty_import_58_def()
{
    QString filepath =  "userdata/settings/print/receipt/"+lds::get_soft_curent_language_first()+"_58.txt";
    lds_query_hddpos  query;
    query.execSelect("select vch_parameter FROM cey_sys_parameter where vch_parameter = 'bp_fmt_title_second'; ");
    if(!query.next()) {
        import(filepath);
    }
}

void w_sys_manage_outer_bill_preview::import_bp_fmt_m_default()
{
    lds_query_hddpos  query;
    QMap<QString, QString> map;
    map.insert("bp_fmt_m_adress", "1");
    map.insert("bp_fmt_m_amount", "1");
    map.insert("bp_fmt_m_name", "1");
    map.insert("bp_fmt_m_no", "1");
    map.insert("bp_fmt_m_point", "1");
    for(QMap<QString, QString>::iterator k = map.begin(); k != map.end(); k ++) {
        query.execSelect(QString("select vch_parameter FROM cey_sys_parameter where vch_parameter = '%1' ; ").arg(k.key()));
        query.next();
        if(query.recordValue("vch_parameter").isNull())
            query.execInsertDuplicateUpdate("cey_sys_parameter", "vch_parameter", qrtVariantPairList() << qrtVariantPair("vch_parameter", k.key()) << qrtVariantPair("vch_value", k.value()) );
    }
}

bool w_sys_manage_outer_bill_preview::billexport(QWidget *parent)
{
    QString filepath = getFilePath::getSaveFileName(parent, QObject::tr("导出文件"),
                                                    "userdata/settings/print/receipt/"
                                                    + n_func::f_get_sysdatetime().toString("yyMMddhhmmss")+"."+"txt",
                                                    QString(), 0, QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty()) {
        QSettings fileconf(filepath, QSettings::IniFormat);
        for(int row = 0, rowcount = tablemodel->rowCount(); row < rowcount; row ++) {
            fileconf.setValue("w_sys_manage_outer_bill_preview/"+tablemodel->model_data(row, "vch_parameter").toString() ,
                              tablemodel->model_data(row, "vch_value")
                              );
        }
        return true;
    }
    return false;
}

bool w_sys_manage_outer_bill_preview::billimport(QWidget *parent)
{
    //1
    QString filepath = getFilePath::getOpenFileName(parent, QObject::tr("导入文件"), "userdata/settings/print/receipt");

    if(!filepath.isEmpty()) {
        QSettings fileconf(filepath, QSettings::IniFormat);
        fileconf.beginGroup("w_sys_manage_outer_bill_preview");
        QStringList keys = fileconf.allKeys();
        for(int row = 0, rowcount = tablemodel->rowCount(); row < rowcount; row++) {
            QString subkey = tablemodel->model_data(row, "vch_parameter").toString();
            if(keys.contains(subkey)) {
                tablemodel->model_data_set(row, "vch_value", fileconf.value(subkey));
            }
        }
        return true;
    }
    return false;
}

bool w_sys_manage_outer_bill_preview::import(const QString &filepath)
{
    lds_query_hddpos  query;
    if(!filepath.isEmpty()) {
        QSettings fileconf(filepath, QSettings::IniFormat);
        fileconf.beginGroup("w_sys_manage_outer_bill_preview");
        QStringList keys = fileconf.allKeys();
        foreach(const QString &k, keys) {
            if(!query.execInsertDuplicateUpdate("cey_sys_parameter",
                                                QStringList() << "vch_parameter",
                                                qrtVariantPairList()
                                                << qrtVariantPair("vch_parameter", k)
                                                << qrtVariantPair("vch_value", fileconf.value(k)))) {
                return  false;
            }
        }
    }
    if(filepath.isEmpty()) {
        qDebug("filepath is empty");
    }
    return true;
}

void w_sys_manage_outer_bill_preview::took()
{
    if(saveData()) {
        this->accept();
    }
}

void w_sys_manage_outer_bill_preview::tocancel()
{
    if(tablemodel->model_is_dirty()) {
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

void w_sys_manage_outer_bill_preview::toupdateSection(const QModelIndex &index)
{
    ui->stackedWidget->setCurrentIndex(index.row());
}

//
void w_sys_manage_outer_bill_preview::togetlogo()
{
    QString filepath = getFilePath::getOpenFileName(this, QObject::tr("导入文件"),ui->bp_fmt_title_logo->text());
    if(filepath.isEmpty())
        return;
    ui->bp_fmt_title_logo->setText(filepath);
    ui->bp_fmt_title_logo->setFocus();
}

void w_sys_manage_outer_bill_preview::tobillexport()
{
    if(billexport(this)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("导出成功"));
    }
}

void w_sys_manage_outer_bill_preview::tobillimport()
{
    if(billimport(this)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("导入成功"));
        mappedSetCurrentIndex(1);
    }
}

void w_sys_manage_outer_bill_preview::to_rb_input()
{
    //获取路径
    QString filepath = getFilePath::getOpenFileName(this, QObject::tr("导入文件"));
    if(!filepath.isEmpty()) {
        QFile file(filepath);
        if(file.open(QFile::ReadOnly)) {
            ui->bp_fmt_end_rc->setText(file.readLine());
            ui->bp_fmt_end_rc->setFocus();
            file.close();
            return;
        }
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, file.errorString());
        return;
    }
}

void w_sys_manage_outer_bill_preview::toHideExSet()
{
    //餐桌
    if(lds::widgetIsShow(ui->pushButton_hide_ExSet)
            && ui->pushButton_hide_ExSet->pos() == ui->bp_fmt_table_len->pos()) {
        lds::hideWidget(ui->bp_fmt_table_fmts);
        lds::hideWidget(ui->pushButton_hide_ExSet);
        ui->bp_fmt_table->setText(ifEmpty(ui->bp_fmt_table_fmts->currentText(), ui->bp_fmt_table_fmts->itemText(0)));
    }
    //流失号
    if(lds::widgetIsShow(ui->pushButton_hide_ExSet)
            && ui->pushButton_hide_ExSet->pos() == ui->bp_fmt_serial_no_len->pos()) {
        lds::hideWidget(ui->bp_fmt_serial_no_desc);
        lds::hideWidget(ui->pushButton_hide_ExSet);
        ui->bp_fmt_serial_no->setText(ifEmpty(ui->bp_fmt_serial_no_desc->text(), QObject::tr("流水号")));
    }
    //操作员
    if(lds::widgetIsShow(ui->pushButton_hide_ExSet)
            && ui->pushButton_hide_ExSet->pos() == ui->bp_fmt_operid_len->pos()) {
        lds::hideWidget(ui->bp_fmt_operid_fmts);
        lds::hideWidget(ui->pushButton_hide_ExSet);
        ui->bp_fmt_operid->setText(ifEmpty(ui->bp_fmt_operid_fmts->currentText(), ui->bp_fmt_operid_fmts->itemText(0)));
    }
}

void w_sys_manage_outer_bill_preview::toShowExSet()
{
    //若是非手动修改combox，不触发本函数
    if(this->focusWidget() != ui->bp_fmt_table_len
            && this->focusWidget() != ui->bp_fmt_serial_no_len
            && this->focusWidget() != ui->bp_fmt_operid_len)
        return;
    //已经有设置中的空间会报存并关闭
    toHideExSet();

    ui->pushButton_hide_ExSet->raise();
    QVariant old_value;
    lds_ComboBox *key_name = 0;
    //餐桌
    if(this->sender() == ui->bp_fmt_table_len && ui->bp_fmt_table_len->curusrdata().toString() == "design" ) {
        key_name = ui->bp_fmt_table_len;
        old_value = model_data(key_name->objectName());
        lds::showWidget(ui->bp_fmt_table_fmts);
        lds::showWidget(ui->pushButton_hide_ExSet);
        ui->bp_fmt_table_fmts->setGeometry(ui->bp_fmt_table->geometry());
        ui->pushButton_hide_ExSet->setGeometry(ui->bp_fmt_table_len->geometry());
        ui->bp_fmt_table_fmts->raise();
        ui->bp_fmt_table_fmts->setFocus();
    }
    //流失号
    if(this->sender() == ui->bp_fmt_serial_no_len && ui->bp_fmt_serial_no_len->curusrdata().toString() == "design") {
        key_name = ui->bp_fmt_serial_no_len;
        old_value = model_data(key_name->objectName());
        lds::showWidget(ui->bp_fmt_serial_no_desc);
        lds::showWidget(ui->pushButton_hide_ExSet);
        ui->bp_fmt_serial_no_desc->setGeometry(ui->bp_fmt_serial_no->geometry());
        ui->pushButton_hide_ExSet->setGeometry(ui->bp_fmt_serial_no_len->geometry());
        ui->bp_fmt_serial_no_desc->raise();
        ui->bp_fmt_serial_no_desc->setFocus();
    }
    //操作员
    if(this->sender() == ui->bp_fmt_operid_len && ui->bp_fmt_operid_len->curusrdata().toString() == "design") {
        key_name = ui->bp_fmt_operid_len;
        old_value = model_data(key_name->objectName());
        lds::showWidget(ui->bp_fmt_operid_fmts);
        lds::showWidget(ui->pushButton_hide_ExSet);
        ui->bp_fmt_operid_fmts->setGeometry(ui->bp_fmt_operid->geometry());
        ui->pushButton_hide_ExSet->setGeometry(ui->bp_fmt_operid_len->geometry());
        ui->bp_fmt_operid_fmts->raise();
        ui->bp_fmt_operid_fmts->setFocus();
    }

    if(key_name != 0) {
        model_data_set(key_name->objectName(), old_value);
        key_name->setcomusrdata(old_value);
    }
}

void w_sys_manage_outer_bill_preview::to_print_prepare_ins_58receipt_center_in_80paper()
{
    //设置左边距 nLnH
    //1D4C3000
    //设置打印区域宽度 nLnH
    //1D57e001
    //48 32

    //32 X 12 = 384 = 128 + 256 = 80L01H
    //32的纸在48的纸上，居中显示:1D4C60001D578001
    ui->bp_fmt_print_prepare_ins->setText(
                "1D4C30001D57e001"
                );
    ui->bp_fmt_print_prepare_ins->setFocus();
}

void w_sys_manage_outer_bill_preview::tobilltest()
{
    tablemodel->select();
}

void w_sys_manage_outer_bill_preview::to_content_hheader_order()
{
    if(ui->pushButton_fmt_default == this->sender()) {
        bp_fmt_hheader_order_setText("dishname,num,unit,price,total,disdetail,amount");
        bp_fmt_hheader_order_updateGeometry();
        return;
    }

    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        QString p2 = b->objectName();
        p2.chop(7);//_switch
        p2 = p2.mid(QString("pushButton_fmt_").length());

        QStringList hheader_order_list = ui->bp_fmt_hheader_order->text().split(",");
        int k2 = hheader_order_list.indexOf(p2);

        hheader_order_list.insert(k2, hheader_order_list.takeAt(qMax(k2 - 1, 0)));
        bp_fmt_hheader_order_setText(hheader_order_list.join(","));
        bp_fmt_hheader_order_updateGeometry();
    }
}

void w_sys_manage_outer_bill_preview::to_billpreview_connect()
{
}

void w_sys_manage_outer_bill_preview::to_bill_preview()
{
    public_printby_ch_billno_over_view *dialog = new public_printby_ch_billno_over_view(this);
    dialog->setMinimumHeight(560);
    dialog->setPaperWidth(billpreview_paperwidth);
    dialog->initData(tablemodel);
    transparentCenterDialog d(dialog, false, true);
    connect(dialog, SIGNAL(signal_resize()), &d, SLOT(toUpdatePos()));
    d.exec();
    billpreview_paperwidth = dialog->curPaperWidth();
}

bool w_sys_manage_outer_bill_preview::insert_sql_objectname(const QString &objectname)
{
    lds_query_hddpos  query;
    query.execSelect(QString("SELECT vch_parameter from cey_sys_parameter where vch_parameter = '%1' ").arg(objectname));
    if(!query.next()) {
        QString vch_value;
        if(objectname == ui->bp_fmt_serial_no_desc->objectName()) {
            vch_value = ui->bp_fmt_serial_no->text();//默认
        }
        if(objectname == ui->bp_fmt_operid_fmts->objectName()) {
            vch_value = ui->bp_fmt_operid_fmts->itemData(0).toString();//默认
        }
        if(objectname == ui->bp_fmt_table_fmts->objectName()) {
            vch_value = ui->bp_fmt_table_fmts->itemData(0).toString();//默认
        }
        if(objectname == ui->bp_fmt_hheader_order->objectName()) {
            vch_value = "dishname,num,unit,price,total,disdetail,amount";//默认
        }
        query.execInsert("cey_sys_parameter",qrtVariantPairList()
                         << qrtVariantPair("vch_parameter", objectname)
                         << qrtVariantPair("vch_value", vch_value));
    }
    return true;
}

void w_sys_manage_outer_bill_preview::mappedSetCurrentIndex(int index)
{
    lds_model_mapwidget_vertical::mappedSetCurrentIndex(index);
    bp_fmt_hheader_order_updateGeometry();
}

bool w_sys_manage_outer_bill_preview::saveData()
{
    QString errstring;
    lds_query_hddpos::tran_saction();
    if(commitData(&errstring)) {
        lds_query_hddpos::com_mit();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        return true;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, errstring);
    return false;
}

void w_sys_manage_outer_bill_preview::pushButton_fmt_setText(const QString &text)
{
    QList<QPushButton *> btns;
    btns << ui->pushButton_fmt_dishname_switch;
    btns << ui->pushButton_fmt_num_switch;
    btns << ui->pushButton_fmt_unit_switch;
    btns << ui->pushButton_fmt_price_switch;
    btns << ui->pushButton_fmt_total_switch;
    btns << ui->pushButton_fmt_disdetail_switch;
    btns << ui->pushButton_fmt_amount_switch;

    foreach(QPushButton *b, btns) {
        b->setText(text);
    }
}

QPushButton *w_sys_manage_outer_bill_preview::pushButton_fmt_index(const QString &text)
{
    QList<QPushButton *> btns;
    btns << ui->pushButton_fmt_dishname_switch;
    btns << ui->pushButton_fmt_num_switch;
    btns << ui->pushButton_fmt_unit_switch;
    btns << ui->pushButton_fmt_price_switch;
    btns << ui->pushButton_fmt_total_switch;
    btns << ui->pushButton_fmt_disdetail_switch;
    btns << ui->pushButton_fmt_amount_switch;

    foreach(QPushButton *b, btns) {
        if(b->text() == text)
            return b;
    }
    return 0;
}

void w_sys_manage_outer_bill_preview::bp_fmt_hheader_move(const QString &key, int index)
{
    QWidget *b = ui->stackedWidget->widget(1)->findChild<QWidget*>(QString("bp_fmt_%1").arg(key));
    QWidget *b_len = ui->stackedWidget->widget(1)->findChild<QWidget*>(QString("bp_fmt_%1_len").arg(key));
    QWidget *b_left = ui->stackedWidget->widget(1)->findChild<QWidget*>(QString("bp_fmt_%1_left").arg(key));
    QWidget *b_row = ui->stackedWidget->widget(1)->findChild<QWidget*>(QString("bp_fmt_%1_row").arg(key));
    QWidget *b_switch = ui->stackedWidget->widget(1)->findChild<QWidget*>(QString("pushButton_fmt_%1_switch").arg(key));

    if(b == 0
            || b_len == 0
            || b_left == 0
            || b_row == 0
            || b_switch == 0) {
        return;
    }

    switch(index) {
    case 0:
        b->setGeometry(bp_fmt_hheader_rect[index][0]);//QRect(2, 201, 130, 25));
        b_len->setGeometry(bp_fmt_hheader_rect[index][1]);//(146, 200, 100, 27));
        b_left->setGeometry(bp_fmt_hheader_rect[index][2]);//(298, 200, 100, 27));
        b_row->setGeometry(bp_fmt_hheader_rect[index][3]);//(450, 200, 100, 27));
        b_switch->setGeometry(bp_fmt_hheader_rect[index][4]);//(560, 200, 31, 27));
        break;
    case 1:
        b->setGeometry(bp_fmt_hheader_rect[index][0]);//(2, 231, 130, 25));
        b_len->setGeometry(bp_fmt_hheader_rect[index][1]);//(146, 231, 100, 27));
        b_left->setGeometry(bp_fmt_hheader_rect[index][2]);//(298, 231, 100, 27));
        b_row->setGeometry(bp_fmt_hheader_rect[index][3]);//(450, 231, 100, 27));
        b_switch->setGeometry(bp_fmt_hheader_rect[index][4]);//(560, 230, 31, 27));
        break;
    case 2:
        b->setGeometry(bp_fmt_hheader_rect[index][0]);//(2, 261, 130, 25));
        b_len->setGeometry(bp_fmt_hheader_rect[index][1]);//(146, 260, 100, 27));
        b_left->setGeometry(bp_fmt_hheader_rect[index][2]);//(298, 260, 100, 27));
        b_row->setGeometry(bp_fmt_hheader_rect[index][3]);//(450, 260, 100, 27));
        b_switch->setGeometry(bp_fmt_hheader_rect[index][4]);//(560, 260, 31, 27));
        break;
    case 3:
        b->setGeometry(bp_fmt_hheader_rect[index][0]);//(2, 291, 130, 25));
        b_len->setGeometry(bp_fmt_hheader_rect[index][1]);//(146, 290, 100, 27));
        b_left->setGeometry(bp_fmt_hheader_rect[index][2]);//(298, 290, 100, 27));
        b_row->setGeometry(bp_fmt_hheader_rect[index][3]);//(450, 290, 100, 27));
        b_switch->setGeometry(bp_fmt_hheader_rect[index][4]);//(560, 290, 31, 27));

        break;
    case 4:
        b->setGeometry(bp_fmt_hheader_rect[index][0]);//(2, 321, 130, 25));
        b_len->setGeometry(bp_fmt_hheader_rect[index][1]);//(146, 320, 100, 27));
        b_left->setGeometry(bp_fmt_hheader_rect[index][2]);//(298, 320, 100, 27));
        b_row->setGeometry(bp_fmt_hheader_rect[index][3]);//(450, 320, 100, 27));
        b_switch->setGeometry(bp_fmt_hheader_rect[index][4]);//(560, 320, 31, 27));
        break;

    case 5:
        b->setGeometry(bp_fmt_hheader_rect[index][0]);//(2, 351, 130, 25));
        b_len->setGeometry(bp_fmt_hheader_rect[index][1]);//(146, 350, 100, 27));
        b_left->setGeometry(bp_fmt_hheader_rect[index][2]);//(298, 350, 100, 27));
        b_row->setGeometry(bp_fmt_hheader_rect[index][3]);//(450, 350, 100, 27));
        b_switch->setGeometry(bp_fmt_hheader_rect[index][4]);//(560, 350, 31, 27));
        break;
    case 6:

        b->setGeometry(bp_fmt_hheader_rect[index][0]);//(2, 383, 130, 25));
        b_len->setGeometry(bp_fmt_hheader_rect[index][1]);//(146, 382, 100, 27));
        b_left->setGeometry(bp_fmt_hheader_rect[index][2]);//(298, 382, 100, 27));
        b_row->setGeometry(bp_fmt_hheader_rect[index][3]);//(450, 382, 100, 27));
        b_switch->setGeometry(bp_fmt_hheader_rect[index][4]);//(560, 380, 31, 27));
        break;
    }
}

void w_sys_manage_outer_bill_preview::bp_fmt_hheader_order_updateGeometry()
{
    QStringList def_orders_list = ui->bp_fmt_hheader_order->text().split(",");
    for(int k = 0; k < def_orders_list.count(); k++) {
        bp_fmt_hheader_move(def_orders_list[k], k);
    }
}

void w_sys_manage_outer_bill_preview::bp_fmt_hheader_order_setText(const QString &bp_fmt_hheader_order)
{
    tablemodel->model_data_set(mappedSection(ui->bp_fmt_hheader_order), "vch_value", bp_fmt_hheader_order);
    ui->bp_fmt_hheader_order->setText(bp_fmt_hheader_order);
}

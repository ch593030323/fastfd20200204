#include "w_bt_dish_table_tableset2.h"
#include "ui_w_bt_dish_table_tableset2.h"
#include "lds_query_hddpos.h"
#include "w_bt_dish.h"
#include <QPainter>
#include <QStylePainter>
#include "w_bt_dish_table_tableset2_set.h"
#include "transparentcenterdialog.h"
#include "lds_messagebox.h"
#include "n_func.h"
#include "lds_model_sqltablemodel.h"
#include "backheader.h"
#include <QTimer>
#include <QSqlRecord>

w_bt_dish_table_tableset2::w_bt_dish_table_tableset2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_bt_dish_table_tableset2)
{
    ui->setupUi(this);

    bool pay_fixed_service = n_func::f_get_sysparm("pay_fixed_service", "0") == "1";
    tablemodel = new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_table_type");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->save_set_header("ch_typeno", QObject::tr("编号"));
    tablemodel->save_set_header("vch_typename", QObject::tr("桌类名称"), 1);
    tablemodel->save_set_header("ch_service_mode", QObject::tr("服务费模式"), pay_fixed_service ? 0 : 1);
    tablemodel->save_set_header("ch_low_mode", QObject::tr("低消方式"), 1);
    tablemodel->enable_vheaderdata_dirty_translate();

    ui->tableView->setModel(tablemodel);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setItemDelegate(new restraurantDelegate(this));
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setColumnWidth(tablemodel->fieldIndex("ch_typeno"), 90);
    ui->tableView->setColumnWidth(tablemodel->fieldIndex("vch_typename"), 120);
    ui->tableView->setColumnWidth(tablemodel->fieldIndex("ch_service_mode"), 320);
    ui->tableView->setColumnWidth(tablemodel->fieldIndex("ch_low_mode"), 179);

    connect(ui->pushButton_new, SIGNAL(clicked()), this, SLOT(tonew()));
    connect(ui->pushButton_del, SIGNAL(clicked()), this, SLOT(todel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(toServiceLowMode(QModelIndex)));

    QTimer::singleShot(100, this, SLOT(torefresh()));
}

w_bt_dish_table_tableset2::~w_bt_dish_table_tableset2()
{
    delete ui;
}

void w_bt_dish_table_tableset2::took()
{
    if(saveData())
        this->accept();
}

void w_bt_dish_table_tableset2::toexit()
{
    if(tablemodel->model_is_dirty()) {
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

void w_bt_dish_table_tableset2::todel()
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
        lds_query_hddpos::tran_saction();
        if(false == query.execDelete("cey_bt_table_type", QString("ch_typeno = '%1' ").arg(ch_typeno))) {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
            return;
        }
        lds_query_hddpos::com_mit();
        tablemodel->removeRow(row);
    }
}

void w_bt_dish_table_tableset2::tonew()
{
    bool pay_fixed_service = n_func::f_get_sysparm("pay_fixed_service", "0") == "1";
    int ch_service_mode = 0;
    double int_rate = 0;
    if(pay_fixed_service) {
        ch_service_mode = 4;
        int_rate = n_func::f_get_sysparm("pay_fixed_service_value", 0).toDouble();
    }
    //
    lds_query_hddpos  query;
    query.execSelect(QString("select ifnull(max(ch_typeno), 0) + 1 from cey_bt_table_type "));
    query.next();
    QString ch_typeno = query.recordValue(0).toString().rightJustified(2, '0');

    int row = tablemodel->rowCount();
    tablemodel->insertRow(row);
    tablemodel->model_data_set(row, "ch_typeno", ch_typeno);
    tablemodel->model_data_set(row, "ch_service_mode", ch_service_mode);
    tablemodel->model_data_set(row, "int_rate", int_rate);
    tablemodel->model_data_set(row, "num_ration", 0);
    tablemodel->model_data_set(row, "int_unit_minute", 0);
    tablemodel->model_data_set(row, "num_unit_amount", 0);

    tablemodel->model_data_set(row, "ch_low_mode", 1);//默认整桌低消
    tablemodel->model_data_set(row, "num_lowcost", 0);

    ui->tableView->setCurrentIndex(tablemodel->model_index(row, "vch_typename"));
    ui->tableView->update(tablemodel->model_index(row, "ch_service_mode"));
    ui->tableView->update(tablemodel->model_index(row, "ch_low_mode"));
}

void w_bt_dish_table_tableset2::toServiceLowMode(const QModelIndex &index)
{
    QMap<QString, QVariant> map;
    int row = index.row();
    int column = index.column();
    if(tablemodel->flags(index) & Qt::ItemIsEditable) {

    } else {
        return;
    }
    if(tablemodel->fieldIndex("ch_low_mode") == column) {
        map.insert("ch_low_mode", tablemodel->model_data(row, "ch_low_mode"));
        map.insert("num_lowcost", tablemodel->model_data(row, "num_lowcost"));
    } else if(tablemodel->fieldIndex("ch_service_mode") == column) {
        map.insert("ch_service_mode", tablemodel->model_data(row, "ch_service_mode"));
        map.insert("int_rate", tablemodel->model_data(row, "int_rate"));
        map.insert("num_ration", tablemodel->model_data(row, "num_ration"));
        map.insert("int_unit_minute", tablemodel->model_data(row, "int_unit_minute"));
        map.insert("num_unit_amount", tablemodel->model_data(row, "num_unit_amount"));
    } else {
        return;
    }
    w_bt_dish_table_tableset2_set dialog(map, this);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        for(QMap<QString, QVariant>::iterator k = dialog.map.begin(); k != dialog.map.end(); k ++) {
            tablemodel->model_data_set(row, k.key(), k.value());
        }
        ui->tableView->update(index);
    }
}

void w_bt_dish_table_tableset2::torefresh()
{
    tablemodel->select();
    ui->tableView->restore(0);
}

bool w_bt_dish_table_tableset2::saveData()
{
    for(int row = 0; row < tablemodel->rowCount(); row ++) {
        if(tablemodel->opera(row) == lds_model_sqltablemodel::OpDelete)
            continue;
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
    }

    QString errstring;
    lds_query_hddpos::tran_saction();
    if(!tablemodel->trySubmitAll(&errstring)) {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return false;
    }
    lds_query_hddpos::com_mit();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
    return true;
}

void w_bt_dish_table_tableset2::restraurantDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const lds_model_sqltablemodel *tablemodel = qobject_cast<const lds_model_sqltablemodel * >(index.model());
    if(tablemodel->fieldIndex("ch_service_mode") == index.column()
            || tablemodel->fieldIndex("ch_low_mode") == index.column()
            ) {
        if(tablemodel->flags(index) & Qt::ItemIsEditable) {
            drawButton(painter, option, index);
        } else {
            drawText(painter, option, index);
        }
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QWidget *w_bt_dish_table_tableset2::restraurantDelegate::createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const lds_model_sqltablemodel *tablemodel = qobject_cast<const lds_model_sqltablemodel * >(index.model());
    if(tablemodel->fieldIndex("ch_service_mode") == index.column()
            || tablemodel->fieldIndex("ch_low_mode") == index.column()
            ) {
        return 0;
    }
    return  QStyledItemDelegate::createEditor(aParent, option, index);
}

void w_bt_dish_table_tableset2::restraurantDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    const lds_model_sqltablemodel *tablemodel = qobject_cast<const lds_model_sqltablemodel * >(index.model());
    if(tablemodel->fieldIndex("ch_service_mode") == index.column()
            || tablemodel->fieldIndex("ch_low_mode") == index.column()
            ) {
        return;
    }
    return  QStyledItemDelegate::setModelData(editor, model, index);
}

void w_bt_dish_table_tableset2::restraurantDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const lds_model_sqltablemodel *tablemodel = qobject_cast<const lds_model_sqltablemodel * >(index.model());
    if(tablemodel->fieldIndex("ch_service_mode") == index.column()
            || tablemodel->fieldIndex("ch_low_mode") == index.column()
            ) {
        return;
    }
    return  QStyledItemDelegate::setEditorData(editor, index);
}

QString w_bt_dish_table_tableset2::restraurantDelegate::curText(const QModelIndex &index) const
{
    const lds_model_sqltablemodel *tablemodel = qobject_cast<const lds_model_sqltablemodel * >(index.model());
    int row = index.row();
    int column = index.column();
    QString text;

    if(column == tablemodel->fieldIndex("ch_service_mode")) {
        QString ch_service_mode = tablemodel->model_data(row, "ch_service_mode").toString();
        QString int_rate = tablemodel->model_data(row, "int_rate").toString();
        QString num_ration = tablemodel->model_data(row, "num_ration").toString();
        QString int_unit_minute = tablemodel->model_data(row, "int_unit_minute").toString();
        QString num_unit_amount = tablemodel->model_data(row, "num_unit_amount").toString();
        text = backheader::modelValue(&w_bt_dish::get_MODEL_DISH_SERVICE_MODE_CUT(), ch_service_mode);
        if(ch_service_mode == "3") //餐桌定额
            text += ":" + num_ration;
        if(ch_service_mode == "4") //商品消费服务率
            text += ":" + int_rate + "%";
        if(ch_service_mode == "1") //按时间收费
            text += ":" + int_unit_minute + QObject::tr("分钟") + " " + QObject::tr("支付") + num_unit_amount;
    }
    if(column == tablemodel->fieldIndex("ch_low_mode")) {
        QString ch_low_mode = tablemodel->model_data(row, "ch_low_mode").toString();
        QString num_lowcost = tablemodel->model_data(row, "num_lowcost").toString();
        text = backheader::modelValue(&w_bt_dish::get_MODEL_DISH_LOW_MODE(), ch_low_mode) + ":" + num_lowcost;
    }
    return text;
}

void w_bt_dish_table_tableset2::restraurantDelegate::drawButton(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton *editor = new QStyleOptionButton();
    editor->rect = option.rect;
    editor->state = option.state;
    editor->text = "";
    QApplication::style()->drawControl(QStyle::CE_PushButton, editor, painter);

    QString painter_text = curText(index);
    QFontMetrics fontm(painter->font());
    QStringList list = painter_text.split(":");
    int xoff = (option.rect.width() - fontm.width(painter_text)) / 2 + option.rect.x();
    for(int k = 0; k < list.count(); k ++) {
        QString text = list[k];
        if(k % 2 == 0) {
            if(list.count() % 2 == 0)
                text += ":";
            painter->setPen(QColor("black"));
        } else {
            painter->setPen(QColor("#ff546b"));
        }
        painter->drawText(QRect(xoff, option.rect.y(), fontm.width(text), option.rect.height()), Qt::AlignVCenter | Qt::AlignLeft, text);
        xoff += fontm.width(text);
    }
}

void w_bt_dish_table_tableset2::restraurantDelegate::drawText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt;
    opt = option;
    initStyleOption(&opt, index);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    const QWidget *widget = option.widget;
#else
    const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
    const QWidget *widget = v3? v3->widget : 0;
#endif
    QStyle *style = widget? widget->style() : QApplication::style();
    opt.text = "";
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);


    QString painter_text = curText(index);
    QFontMetrics fontm(painter->font());
    QStringList list = painter_text.split(":");
    int xoff = (option.rect.width() - fontm.width(painter_text)) / 2 + option.rect.x();
    for(int k = 0; k < list.count(); k ++) {
        QString text = list[k];
        if(k % 2 == 0) {
            if(list.count() % 2 == 0)
                text += ":";
            painter->setPen(QColor("black"));
        } else {
            painter->setPen(QColor("#ff546b"));
        }
        if(opt.state & QStyle::State_Selected) {
            painter->setPen(QColor("white"));
        }
        painter->drawText(QRect(xoff, option.rect.y(), fontm.width(text), option.rect.height()), Qt::AlignVCenter | Qt::AlignLeft, text);
        xoff += fontm.width(text);
    }
}

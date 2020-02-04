#include "w_bt_dish_table_tableset2_set.h"
#include "ui_w_bt_dish_table_tableset2_set.h"
#include "w_bt_dish.h"
#include "lds_messagebox.h"

w_bt_dish_table_tableset2_set::w_bt_dish_table_tableset2_set(const QMap<QString, QVariant> &map, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_bt_dish_table_tableset2_set)
{
    ui->setupUi(this);
    this->setWindowTitle(MESSAGE_TITLE_VOID);
    this->map = map;
    ui->comboBox_ch_low_mode->addItemsByModel(&w_bt_dish::get_MODEL_DISH_LOW_MODE());
    ui->comboBox_ch_service_mode->addItemsByModel(&w_bt_dish::get_MODEL_DISH_SERVICE_MODE_CUT());
    if(map.find("ch_low_mode") != map.end()) {
        showRow(ui->comboBox_ch_low_mode, true);
        showRow(ui->comboBox_ch_service_mode, false);
        ui->comboBox_ch_low_mode->setcomusrdata(map.value("ch_low_mode"));
        toUpdateLowMode();
    }
    if(map.find("ch_service_mode") != map.end()) {
        showRow(ui->comboBox_ch_low_mode, false);
        showRow(ui->comboBox_ch_service_mode, true);
        ui->comboBox_ch_service_mode->setcomusrdata(map.value("ch_service_mode"));
        toUpdateServiceMode();
    }
    for(QMap<QString, QVariant>::const_iterator k = map.begin(); k != map.end(); k ++) {
        setWidgetValue(k.key(), k.value());
    }

    connect(ui->comboBox_ch_low_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(toUpdateLowMode()));
    connect(ui->comboBox_ch_service_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(toUpdateServiceMode()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
}

w_bt_dish_table_tableset2_set::~w_bt_dish_table_tableset2_set()
{
    delete ui;
}

void w_bt_dish_table_tableset2_set::toUpdateLowMode()
{
    if(ui->comboBox_ch_low_mode->curusrdata().toString() == "1") {//餐桌低消

    }
    if(ui->comboBox_ch_low_mode->curusrdata().toString() == "2") {//人均低消

    }
    showRow(ui->comboBox_ch_service_mode, true);
    showRow(ui->doubleSpinBox_num_lowcost, true);

    showRow(ui->comboBox_ch_service_mode, false);
    showRow(ui->doubleSpinBox_int_rate, false);
    showRow(ui->doubleSpinBox_int_unit_minute, false);
    showRow(ui->doubleSpinBox_num_ration, false);
    showRow(ui->doubleSpinBox_num_unit_amount, false);
}

void w_bt_dish_table_tableset2_set::toUpdateServiceMode()
{
    showRow(ui->comboBox_ch_service_mode, false);
    showRow(ui->doubleSpinBox_num_lowcost, false);
    showRow(ui->comboBox_ch_service_mode, true);
    if(ui->comboBox_ch_service_mode->curusrdata().toString() == "0") {//不处理
        showRow(ui->doubleSpinBox_int_rate, false);
        showRow(ui->doubleSpinBox_int_unit_minute, false);
        showRow(ui->doubleSpinBox_num_ration, false);
        showRow(ui->doubleSpinBox_num_unit_amount, false);
    }
    if(ui->comboBox_ch_service_mode->curusrdata().toString() == "1") {//按时间收费
        showRow(ui->doubleSpinBox_int_rate, false);
        showRow(ui->doubleSpinBox_int_unit_minute, true);
        showRow(ui->doubleSpinBox_num_ration, false);
        showRow(ui->doubleSpinBox_num_unit_amount, true);
    }
    if(ui->comboBox_ch_service_mode->curusrdata().toString() == "3") {//餐桌定额
        showRow(ui->doubleSpinBox_int_rate, false);
        showRow(ui->doubleSpinBox_int_unit_minute, false);
        showRow(ui->doubleSpinBox_num_ration, true);
        showRow(ui->doubleSpinBox_num_unit_amount, false);
    }
    if(ui->comboBox_ch_service_mode->curusrdata().toString() == "4") {//商品消费服务率
        showRow(ui->doubleSpinBox_int_rate, true);
        showRow(ui->doubleSpinBox_int_unit_minute, false);
        showRow(ui->doubleSpinBox_num_ration, false);
        showRow(ui->doubleSpinBox_num_unit_amount, false);
    }
}

void w_bt_dish_table_tableset2_set::took()
{
    if(lds::widgetIsShow(ui->doubleSpinBox_int_unit_minute)
            && ui->doubleSpinBox_int_unit_minute->value() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("收费单位不能为0"));
        return;
    }

    if(lds::widgetIsShow(ui->comboBox_ch_service_mode)) map.insert("ch_service_mode", ui->comboBox_ch_service_mode->curusrdata());
    if(lds::widgetIsShow(ui->doubleSpinBox_int_rate)) map.insert("int_rate", ui->doubleSpinBox_int_rate->value());
    if(lds::widgetIsShow(ui->doubleSpinBox_int_unit_minute)) map.insert("int_unit_minute", ui->doubleSpinBox_int_unit_minute->value());
    if(lds::widgetIsShow(ui->doubleSpinBox_num_ration)) map.insert("num_ration", ui->doubleSpinBox_num_ration->value());
    if(lds::widgetIsShow(ui->doubleSpinBox_num_unit_amount)) map.insert("num_unit_amount", ui->doubleSpinBox_num_unit_amount->value());

    if(lds::widgetIsShow(ui->comboBox_ch_low_mode)) map.insert("ch_low_mode", ui->comboBox_ch_low_mode->curusrdata());
    if(lds::widgetIsShow(ui->doubleSpinBox_num_lowcost)) map.insert("num_lowcost", ui->doubleSpinBox_num_lowcost->value());
    this->accept();
}

void w_bt_dish_table_tableset2_set::toexit()
{
    this->reject();
}

void w_bt_dish_table_tableset2_set::showRow(const QWidget *widget, bool show)
{
    for(int row = 0; row < ui->gridLayout->rowCount(); row ++) {
        for(int column = 0; column < ui->gridLayout->columnCount(); column ++) {
            QWidget *w = ui->gridLayout->itemAtPosition(row, column)->widget();
            if(w == widget) {
                for(column = 0; column < ui->gridLayout->columnCount(); column ++) {
                    QWidget *w = ui->gridLayout->itemAtPosition(row, column)->widget();
                    if(w)
                        lds::showWidget(w, show);
                }
                return;
            }
        }
    }
}

void w_bt_dish_table_tableset2_set::setWidgetValue(const QString &key, const QVariant &value)
{
    foreach(QObject *o, this->children()) {
        if(o->objectName().contains(key)) {
            lds_doublespinbox *d = qobject_cast<lds_doublespinbox *>(o);
            if(d)
                d->setValue(value.toDouble());
        }
    }
}

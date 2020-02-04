#include "w_i_inventory_provider_new.h"
#include "ui_w_i_inventory_provider_new.h"
#include "lds_model_sqltablemodel.h"
#include "lds_query_hddpos.h"
#include "lds_messagebox.h"

void w_i_inventory_provider_new::initControlMap(const lds_model_sqltablemodel *tablemodel, Ui_w_i_inventory_provider_new *ui, controlViewMap *control_map)
{
    lds_model_sqltablemodel_d::fillData(ui->comboBox_ch_typeno, tablemodel->tablename_kvmap_d("ch_typeno"));

    control_map->insert("ch_providerNo", ui->lineEdit_ch_providerNo, "text");
    control_map->insert("vch_providerName", ui->lineEdit_vch_providerName, "text");
    control_map->insert("vch_tax_number", ui->lineEdit_vch_tax_number, "text");
    control_map->insert("ch_typeno", ui->comboBox_ch_typeno, "comusrdata");
    control_map->insert("vch_linkman", ui->lineEdit_vch_linkman, "text");
    control_map->insert("vch_tel", ui->lineEdit_vch_tel, "text");

    control_map->insert("vch_email", ui->lineEdit_vch_email, "text");
    control_map->insert("vch_adress", ui->lineEdit_vch_adress, "text");
    control_map->insert("vch_voucherno", ui->lineEdit_vch_voucherno, "text");
    control_map->insert("vch_memo", ui->lineEdit_vch_memo, "text");
    control_map->insert("int_tax", ui->doubleSpinBox_int_tax, "value");

    control_map->setKey("ch_providerNo");
}

bool w_i_inventory_provider_new::check(controlViewMap *control_map, QString &errstring)
{
    if(control_map->value("ch_providerNo").toString().trimmed().count() == 0) {
        errstring = QObject::tr("编号不能为空");
        return false;
    }
    if(control_map->value("vch_providerName").toString().trimmed().count() == 0) {
        errstring = QObject::tr("名称不能为空");
        return false;
    }
    if(control_map->value("vch_tax_number").toString().trimmed().count() == 0) {
        errstring = QObject::tr("税号不能为空");
        return false;
    }
    return true;
}

w_i_inventory_provider_new::w_i_inventory_provider_new(const lds_model_sqltablemodel *tablemodel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_i_inventory_provider_new)
{
    ui->setupUi(this);
    initControlMap(tablemodel, ui, &control_map);
    this->setWindowTitle(tr("新增"));
    ui->lineEdit_ch_providerNo->setText(lds_query_hddpos::selectValue(" select ifnull(max(ch_providerNo), 0) + 1 from cey_st_provider").toString().rightJustified(4, '0'));

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
}

w_i_inventory_provider_new::~w_i_inventory_provider_new()
{
    delete ui;
}

void w_i_inventory_provider_new::took()
{
    QString errstring;
    if(lds_query_hddpos::selectHasNext(QString(" select  ch_providerNo from cey_st_provider where ch_providerNo = '%1' ").arg(control_map.value("ch_providerNo").toString().trimmed()))) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("编号有重复"));
        return;
    }
     if(false == w_i_inventory_provider_new::check(&control_map, errstring)) {
         lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
         return;
     }

    lds_query_hddpos query;
     lds_query_hddpos::tran_saction();
     if(query.execInsert("cey_st_provider", control_map.insertBindValues())) {
         lds_query_hddpos::com_mit();
         lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("保存成功"));
         this->accept();
         return;
     }
     lds_query_hddpos::roll_back();
     lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
    this->accept();
}

void w_i_inventory_provider_new::tocancel()
{
    this->reject();
}

w_i_inventory_provider_modify::w_i_inventory_provider_modify(const lds_model_sqltablemodel *tablemodel, int row, QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_i_inventory_provider_new)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("修改"));
    w_i_inventory_provider_new::initControlMap(tablemodel, ui, &control_map);
    control_map.setValue(tablemodel, row);
    ui->lineEdit_ch_providerNo->setEnabled(false);
    //
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
}

w_i_inventory_provider_modify::~w_i_inventory_provider_modify()
{
    delete ui;
}

void w_i_inventory_provider_modify::took()
{
    QString errstring;
    if(false == w_i_inventory_provider_new::check(&control_map, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    lds_query_hddpos query;
    lds_query_hddpos::tran_saction();
    if(query.execUpdate("cey_st_provider", control_map.updateBindValues(), control_map.updateWhere())) {
        lds_query_hddpos::com_mit();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, tr("保存成功"));
        this->accept();
        return;
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
}

void w_i_inventory_provider_modify::tocancel()
{
    this->reject();
}

void controlViewMap::insert(const QString &fieldName, QObject *obj, const QByteArray &propertyName)
{
    obj_map.insert(fieldName, (objData) {obj, propertyName});
}

QVariant controlViewMap::value(const QString &fieldName)
{
    objData d = obj_map.value(fieldName);
    if(d.obj != 0)
        return d.obj->property(d.propertyNam);
    return QVariant();
}

void controlViewMap::setValue(const QString &fieldName, const QVariant &value)
{
    objData d = obj_map.value(fieldName);
    if(d.obj != 0)
        d.obj->setProperty(d.propertyNam, value);
}

void controlViewMap::setValue(const lds_model_sqltablemodel *m, int row)
{
    for(QMap<QString, objData>::iterator k = obj_map.begin(); k != obj_map.end(); k ++) {
        setValue(k.key(), m->model_data(row, k.key()));
    }
}

void controlViewMap::setKey(const QString &fieldName)
{
    key_fieldName = fieldName;
}

qrtVariantPairList controlViewMap::updateBindValues()
{
    qrtVariantPairList r;
    for(QMap<QString, objData>::iterator k = obj_map.begin(); k != obj_map.end(); k ++) {
        if(k.key() != key_fieldName)
            r << qrtVariantPair(k.key(), value(k.key()).toString().trimmed());
    }
    return r;
}

QString controlViewMap::updateWhere()
{
    return key_fieldName + QString(" = '%1' ").arg(value(key_fieldName).toString().trimmed());
}

qrtVariantPairList controlViewMap::insertBindValues()
{
    qrtVariantPairList r;
    for(QMap<QString, objData>::iterator k = obj_map.begin(); k != obj_map.end(); k ++) {
        r << qrtVariantPair(k.key(), value(k.key()).toString().trimmed());
    }
    return r;
}

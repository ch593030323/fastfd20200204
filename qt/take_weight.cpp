#include "take_weight.h"
#include "w_bt_dish.h"
#include "lds_messagebox.h"
#include "public_sql.h"
#include <QTimeLine>
#include "lds.h"
#include <QPainter>
#include "fexpandmain_model_sqltablemodel_data.h"
#include "ui_take_weight.h"


take_weight::take_weight(const QString &ch_dishno, float dishprice, float dishnum, int int_discount, QWidget *parent)
    :    QDialog(parent)
{
    initData();
    refresh(ch_dishno, dishprice, dishnum, int_discount);
}

take_weight::~take_weight()
{
    delete ui;
}

bool take_weight::check(QWidget *parent, const QString &ch_suitflag, const QString &ch_presentflag)
{
    if(ch_suitflag == "Y") {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("子商品不能单独修改数量"));
        return false;
    }
    if(ch_presentflag == "Y")
    {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("已赠送的点单记录,数量不可改"));
        return false;
    }
    return true;
}

void take_weight::retranslateView()
{
    ui->retranslateUi(this);
}

void take_weight::setAppendbtn(bool visiabled)
{
    ui->pushButton_app->setVisible(visiabled);
}

void take_weight::refresh(const QString &ch_dishno, float dishprice, float dishnum, int int_discount)
{
    this->ch_dishno = ch_dishno;
    lds_query_hddpos  query;
    query.execSelect(QString("select vch_dishname, ch_unitno, ch_count, num_count_weight  from cey_bt_dish where ch_dishno = '%1' ").arg(ch_dishno));
    query.next();
    ui->lineEdit_dishname->setText(query.recordValue("vch_dishname").toString());
    QString _ch_unitno = query.recordValue("ch_unitno").toString();
    num_count_weight = query.recordValue("num_count_weight").toDouble();
    ch_count = query.recordValue("ch_count").toString();

    query.execSelect(QString("select vch_unitname from cey_bt_unit where ch_unitno = '%1' ").arg(_ch_unitno));
    query.next();
    QString _unit = query.recordValue("vch_unitname").toString();

    _curdishnum = dishnum;
    //label_unit
    ui->label_unit->setText(_unit);
    int unitkey = 0;
    for(int row  = 0, rowcount =  w_bt_dish::get_MODEL_UNIT().rowCount(); row < rowcount; row++) {
        if(w_bt_dish::get_MODEL_UNIT().item(row)->text() == _unit) {
            unitkey = row;
            break;
        }
    }
    updateweighttype(unitkey);

    //
    ui->lineEdit_dishprice->setValue(dishprice);
    _weight = dishnum;
    ui->spinBox_int_discount->setValue(int_discount);
    if(public_sql::discount_off_mode()) {
        ui->spinBox_int_discount->enableValueMaxMinus();
    }
    lds::showWidget( ui->spinBox_int_discount, int_discount != 100);
    lds::showWidget( ui->label_int_discount, int_discount != 100);

    ui->checkBox->setChecked(lds::sysconf->value("take_weight/autook", false).toBool());
    if(lds::sysconf->value("take_weight/autook", false).toBool()) {
        tline->start();
    }
}

void take_weight::toappend()
{
    //在原来的基础上追加
    _weight=ui->lineEdit_getweight->text().toFloat()+_curdishnum;
    this->accept();
}

void take_weight::took()
{
    _weight=ui->lineEdit_getweight->text().toFloat();
    this->accept();
}

void take_weight::toexit()
{
    this->reject();
}

void take_weight::toqupi()
{
    countprice.qupi();
}

void take_weight::tozhiling()
{
    countprice.zhiling();
}

void take_weight::quipistate(bool f)
{
    if(f) {
        ui->label_qupi->setStyleSheet("background:green;");
    } else {
        ui->label_qupi->setStyleSheet("background:lightgray;");
    }
}

void take_weight::lingweistate(bool f)
{
    if(f) {
        ui->label_zhiling->setStyleSheet("background:green;");
    } else {
        ui->label_zhiling->setStyleSheet("background:lightgray;");
    }
}

void take_weight::updategetweight(const QString &value)
{
    QString little_text;
    if(ch_count == "Y") {
        if(num_count_weight != 0) {
            qreal v = value.toDouble() / num_count_weight;
            ui->lineEdit_getweight->setText(QString::number(qRound(v)));//
            little_text = QString::number(v, 'f', 3) + " = " + value + "KG /" + QString::number(num_count_weight, 'f', 3) + "KG" ;
        }
    } else {
        qreal v = value.toDouble()*kg2otherrate;
        ui->lineEdit_getweight->setText(QString::number(v, 'f', 3));
        little_text = QString::number(v, 'f', 3) + " = " + value + "KG * " + QString::number(kg2otherrate);
    }
    //
    ui->lineEdit_getweight->setLittleText(little_text, QColor("grey"));
}

void take_weight::updategetweight(double value)
{
    QString little_text;
    if(ch_count == "Y") {
        if(num_count_weight != 0) {
            value = value * 1000;
            qreal v = value / num_count_weight;
            ui->lineEdit_getweight->setText(QString::number(qRound(v)));//
            little_text = QString::number(v, 'f', 3) + " = " + QString::number(value, 'f', 3) + "G /" + QString::number(num_count_weight, 'f', 3) + "G" ;
        }
    } else {
        qreal v = value * kg2otherrate;
        ui->lineEdit_getweight->setText(QString::number(v, 'f', 3));
        little_text = QString::number(v, 'f', 3) + " = " + QString::number(value, 'f', 3) + "KG * " + QString::number(kg2otherrate);
    }
    //
    ui->lineEdit_getweight->setLittleText(little_text, QColor("grey"));
}

void take_weight::updateweighttype(int key)
{
    switch(key) {
    case 1://500克
        kg2otherrate = 2;
        break;
    case 2://50克
        kg2otherrate = 20;
        break;
    case 3://克
        kg2otherrate = 1000;
        break;
    case 4://两
        kg2otherrate = 20;
        break;
    case 0://千克
    default:
        kg2otherrate = 1;
        break;
    }
}

void take_weight::updatetotal(const QString &value)
{
    ui->lineEdit_dishtotal->setValue(
                value.toFloat()*
                ui->lineEdit_dishprice->value()*
                ui->spinBox_int_discount->value()/100.0);
}

void take_weight::updateAutook(bool f)
{
    lds::sysconf->setValue("take_weight/autook", f);
}

void take_weight::toAutoOk(int step)
{
    if(weight_old != ui->lineEdit_getweight->text()) {
        if(ui->checkBox->isChecked()) {
            tline->stop();//重新开始
            tline->start();
        }
        weight_old = ui->lineEdit_getweight->text();
    }
    ui->pushButton_ok->setText(QString("%1(%2)").arg(QObject::tr("确认")).arg(step/2));
    if(step == 0) {
        ui->pushButton_ok->click();
    }
}

void take_weight::initData()
{
    ui = new Ui_take_weight;
    ui->setupUi(this);
    ui->lineEdit_getweight->disableSelectAllWhenGetFocus();
    kg2otherrate = 1;
    ui->pushButton_ok->setText(QObject::tr("确认"));


    //2
    ui->lineEdit_type->setText(lds::sysconf->value("system_setting/dianzichengxinghao").toString());
    if(ui->lineEdit_type->text().isEmpty()) {
        ui->lineEdit_type->setText(QObject::tr("没有类型"));
    }

    connect(&countprice,SIGNAL(weighComming(QString)),this,SLOT(updategetweight(QString)));
    connect(&countprice,SIGNAL(weighComming(double)),this,SLOT(updategetweight(double)));
    connect(&countprice,SIGNAL(qupiComming(bool)),this,SLOT(quipistate(bool)));
    connect(&countprice,SIGNAL(lingweiComming(bool)),this,SLOT(lingweistate(bool)));
    connect(ui->lineEdit_getweight,SIGNAL(textChanged(QString)),this,SLOT(updatetotal(QString)));
    countprice.scaleOpen(
                lds::sysconf->value("system_setting/dianzichengxinghao").toString(),
                lds::sysconf->value("system_setting/dianzichengduankou").toString(),
                lds::sysconf->value("system_setting/dianzicheng_baudrate").toInt()
                );


    connect(ui->pushButton_app, SIGNAL(clicked()),this,SLOT(toappend()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_qupi,SIGNAL(clicked()),this,SLOT(toqupi()));
    connect(ui->pushButton_zhiling,SIGNAL(clicked()),this,SLOT(tozhiling()));

    //4
    int timeout = lds::sysconf->value("system_setting/getweigthtimeout", "3").toInt();
    timeout = qMax(1,timeout);
    ui->checkBox->setText(QString("%1(%2)").arg(ui->checkBox->text()).arg(timeout));
    connect(ui->checkBox,SIGNAL(toggled(bool)),this,SLOT(updateAutook(bool)));
    tline = new QTimeLine(timeout*1000, this);
    tline->setFrameRange(0, timeout*2);
    tline->setDirection(QTimeLine::Backward);
    connect(tline,SIGNAL(frameChanged(int)),this,SLOT(toAutoOk(int)));
    if(lds::sysconf->value("take_weight/autook", false).toBool()) {
        tline->start();
    }
}

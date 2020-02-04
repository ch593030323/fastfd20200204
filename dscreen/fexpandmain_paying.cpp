#include "fexpandmain_paying.h"
#include "ui_fexpandmain_paying.h"
#include "cjson_help.h"
#include "cJSON.h"
#include "lds.h"

fexpandmain_paying::fexpandmain_paying(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fexpandmain_paying)
{
    ui->setupUi(this);
    updateStyle("");
}

fexpandmain_paying::~fexpandmain_paying()
{
    delete ui;
}

void fexpandmain_paying::setMasterValue(const QString &json)
{
    cJSON_help j(json.toLocal8Bit());
    setMasterValue(cJSON_help::getcJSONvalue(j, "no").toString(), cJSON_help::getcJSONvalue(j, "name").toString(), cJSON_help::getcJSONvalue(j, "value").toString());
}

void fexpandmain_paying::setMasterValue(const QString &no, const QString &label, const QString &text)
{
    foreach(QObject *o, this->children()) {
        if(o->objectName() == "lineEdit_" + no) {
            qobject_cast<lds_label_text *>(o)->setLabelText(label, text);
        }
    }
}

void fexpandmain_paying::appendDetailValue(const QString &no, const QString &label, const QString &text)
{

}

void fexpandmain_paying::updateStyle(const QString &skin_name)
{
    foreach(QObject *o, this->children()) {
        lds_label_text *l = qobject_cast<lds_label_text *>(o);
        if(l) {
            if(skin_name.count() > 0)
                l->setBgColor(l->skinBgColor(skin_name));
            l->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
            l->setTxtAlignment(Qt::AlignRight | Qt::AlignBottom);
        }
    }

//    ui->lineEdit_qty->setBgColor(QColor("#ff546b"));
//    ui->lineEdit_realreceive->setBgColor(QColor("#ff546b"));
//    ui->lineEdit_moneypay->setBgColor(QColor("green"));
//    ui->lineEdit_change->setBgColor(QColor("#ff546b"));
    ui->lineEdit_change->setTxtFontSize(25);
    ui->lineEdit_realreceive->setTxtFontSize(25);
//    ui->lineEdit_consum->setBgColor(QColor("#ff546b"));
}

void fexpandmain_paying::setAllControl2Zero()
{
    foreach(QObject *o, this->children()) {
        lds_label_text *l = qobject_cast<lds_label_text *>(o);
        if(l) {
            int decimal = l->text().split(".").value(1).count();
            l->setText(lds::septNumber(0, decimal));
        }
    }
}

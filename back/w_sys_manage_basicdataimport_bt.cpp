#include "w_sys_manage_basicdataimport_bt.h"
#include "ui_w_sys_manage_basicdataimport_bt.h"
#include "lds.h"
#include "lds_query_hddpos.h"
#include <QFile>
#include "lds_messagebox.h"
#include "getfilepath.h"
#include "getchinesespell.h"
#include "w_sys_manage_basicdataimport.h"
#include "public_sql.h"
#include "w_sys_manage_basicdataimport_preview.h"
#include <QTableView>

w_sys_manage_basicdataimport_bt::w_sys_manage_basicdataimport_bt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_basicdataimport_bt)
{
    ui->setupUi(this);

    ui->label_code->setText(htmlCannotEmpty(QObject::tr("编号")));
    ui->label_name->setText(htmlCannotEmpty(QObject::tr("名称")));
    ui->label_type->setText(htmlCannotEmpty(QObject::tr("大类编号")));
    ui->label_subtype->setText(htmlCannotEmpty(QObject::tr("小类编号")));
    ui->label_price->setText(htmlCannotEmpty(QObject::tr("价格")));

    ui->label_unit->setText(htmlCannotEmpty(QObject::tr("单位编号")));
    ui->label_discount->setText(QObject::tr("折扣"));
    ui->label_barcode->setText(QObject::tr("条码"));
    ui->label_name_2->setText(QObject::tr("名称") + "2");

    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_import, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_preview, SIGNAL(clicked()), this, SLOT(topreview()));
    connect(ui->checkBox_3, SIGNAL(toggled(bool)), ui->spinBox_type, SLOT(setVisible(bool)));
    connect(ui->checkBox_unit, SIGNAL(toggled(bool)), ui->spinBox_unit, SLOT(setVisible(bool)));

    ui->checkBox_unit->setChecked(false);
    ui->checkBox_3->setChecked(false);
}

w_sys_manage_basicdataimport_bt::~w_sys_manage_basicdataimport_bt()
{
    delete ui;
}

void w_sys_manage_basicdataimport_bt::took()
{
    lds_query_hddpos  query;
    lds_query_hddpos::tran_saction();
    QString errstring;
    if(0 == lds_messagebox::warning(this, _TEXT_SLOT(this), QObject::tr("该操作将会先清空所有的商品信息, 是否继续"), QObject::tr("继续"), QObject::tr("取消"))) {
        if(query.execDelete("cey_bt_dish")) {
            if(query.execDelete("cey_bt_dish_type")) {
                if(query.execDelete("cey_bt_dish_subtype")) {
                    if(query.execDelete("cey_bt_unit")) {
                        QFile file(ui->lineEdit_path->text());
                        QString splitfilter = ui->comboBox->currentText();
                        if(!file.open(QFile::ReadOnly)) {
                            errstring = file.errorString();
                        } else {
                            if(ui->checkBox->isChecked()) {//第一行跳过
                                w_sys_manage_basicdataimport::FILE_READ_LINE_CODEC(file, ui->comboBox_codec->currentText().toLocal8Bit());
                            }
                            bool exec_flag = true;
                            while(!file.atEnd()) {
                                QString line = w_sys_manage_basicdataimport::FILE_READ_LINE_CODEC(file, ui->comboBox_codec->currentText().toLocal8Bit());
                                line = line.trimmed();
                                QStringList lines = line.split(splitfilter);
                                qrtVariantPairList datalist;
                                datalist << qrtVariantPair("ch_dishno", lines.value(ui->spinBox_no->value()));
                                datalist << qrtVariantPair("ch_groupno", lines.value(ui->spinBox_no->value()));
                                datalist << qrtVariantPair("vch_dishname", lines.value(ui->spinBox_name->value()));
                                datalist << qrtVariantPair("vch_spell", getChineseSpell::ChineseSpell(lines.value(ui->spinBox_name->value())).toUpper());

                                datalist << qrtVariantPair("ch_dish_typeno", (ui->spinBox_type->isHidden() ? "11" : lines.value(ui->spinBox_type->value())));
                                datalist << qrtVariantPair("ch_sub_typeno", lines.value(ui->spinBox_subtype->value()));
                                datalist << qrtVariantPair("num_price", lines.value(ui->spinBox_price->value()));
                                datalist << qrtVariantPair("ch_unitno", (ui->spinBox_unit->isHidden() ? "11" : lines.value(ui->spinBox_unit->value())));
                                datalist << qrtVariantPair("ch_stopflag", "N");
                                if(ui->spinBox_discount->value() >= 0) datalist << qrtVariantPair("ch_discount", lines.value(ui->spinBox_discount->value()));
                                if(ui->spinBox_barcode->value() >= 0) datalist << qrtVariantPair("vch_barcode", lines.value(ui->spinBox_barcode->value()));
                                if(ui->spinBox_name_2->value() >= 0) datalist << qrtVariantPair("vch_dishname_2", lines.value(ui->spinBox_name_2->value()));

                                if(query.execInsert("cey_bt_dish", datalist)) {
                                    QString ch_dish_typeno = qrtVariant::pairListValue(datalist, "ch_dish_typeno").toString();
                                    if(lds_query_hddpos::selectHasNext(QString(" select ch_dish_typeno from cey_bt_dish_type where ch_dish_typeno= '%1' ").arg(ch_dish_typeno))
                                            || query.execInsert("cey_bt_dish_type", qrtVariantPairList()
                                                                << qrtVariantPair("ch_dish_typeno", ch_dish_typeno)
                                                                << qrtVariantPair("vch_dish_typename", "大类" + ch_dish_typeno))
                                            ) {

                                        QString ch_sub_typeno = qrtVariant::pairListValue(datalist, "ch_sub_typeno").toString();
                                        if(lds_query_hddpos::selectHasNext(QString(" select ch_sub_typeno from cey_bt_dish_subtype where ch_sub_typeno= '%1' ").arg(ch_sub_typeno))
                                                || query.execInsert("cey_bt_dish_subtype", qrtVariantPairList()
                                                                    << qrtVariantPair("ch_sub_typeno", ch_sub_typeno)
                                                                    << qrtVariantPair("vch_sub_typename", "小类" + ch_sub_typeno)
                                                                    << qrtVariantPair("ch_dish_typeno", ch_dish_typeno)
                                                                    )
                                                ) {
                                            QString ch_unitno = qrtVariant::pairListValue(datalist, "ch_unitno").toString();
                                            if(lds_query_hddpos::selectHasNext(QString(" select ch_unitno from cey_bt_unit where ch_unitno= '%1' ").arg(ch_unitno))
                                                    || query.execInsert("cey_bt_unit", qrtVariantPairList()
                                                                        << qrtVariantPair("ch_unitno", ch_unitno)
                                                                        << qrtVariantPair("vch_unitname", "单位" + ch_unitno))
                                                    ) {
                                                continue;//成功
                                            }
                                        }
                                    }
                                }
                                exec_flag = false;
                                break;//失败
                            }//while
                            if(exec_flag) {
                                lds_query_hddpos::com_mit();
                                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring + query.recordError());
}

void w_sys_manage_basicdataimport_bt::tocancel()
{
    this->reject();
}

void w_sys_manage_basicdataimport_bt::on_toolButton_clicked()
{
    QString filepath = getFilePath::getOpenFileName(this, QObject::tr("导入文件"),ui->lineEdit_path->text());
    if(!filepath.isEmpty()) {
        ui->lineEdit_path->setText(filepath);
    }
}

void w_sys_manage_basicdataimport_bt::topreview()
{
    w_sys_manage_basicdataimport_preview dialog(ui->lineEdit_path->text(), ui->comboBox_codec->currentText(), this);
    transparentCenterDialog(&dialog).exec();
    ui->comboBox_codec->setcomtext(dialog.curCodec());
}

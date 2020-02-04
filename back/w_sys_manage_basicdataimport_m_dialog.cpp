#include "w_sys_manage_basicdataimport_m_dialog.h"
#include "ui_w_sys_manage_basicdataimport_m_dialog.h"
#include "lds_messagebox.h"
#include <QFile>
#include <QFileDialog>
#include "lds_query_hddpos.h"
#include <QtDebug>
#include "w_sys_manage_basicdataimport.h"
#include "n_func.h"
#include "lds.h"
#include "lds.h"
#include "getfilepath.h"
#include "public_sql.h"
#include "w_sys_manage_basicdataimport_preview.h"

w_sys_manage_basicdataimport_m_Dialog::w_sys_manage_basicdataimport_m_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_basicdataimport_m_Dialog)
{
    ui->setupUi(this);
    ui->label->setText(htmlCannotEmpty(ui->label->text()));
    ui->label_2->setText(htmlCannotEmpty(ui->label_2->text()));
    ui->label_3->setText(htmlCannotEmpty(ui->label_3->text()));
    QDateTime sqldt = n_func::f_get_sysdatetime();

    on_checkBox_2_toggled(ui->checkBox_2->isChecked());
    on_checkBox_3_toggled(ui->checkBox_3->isChecked());
    ui->dateTimeEdit->setDateTime(sqldt.addYears(5));
    connect(ui->pushButton_preview, SIGNAL(clicked()) , this, SLOT(topreview()));
}

w_sys_manage_basicdataimport_m_Dialog::~w_sys_manage_basicdataimport_m_Dialog()
{
    delete ui;
}

void w_sys_manage_basicdataimport_m_Dialog::on_toolButton_clicked()
{
    QString filepath = getFilePath::getOpenFileName(this, QObject::tr("导入文件"),ui->lineEdit_path->text());
    if(!filepath.isEmpty()) {
        ui->lineEdit_path->setText(filepath);
    }
}

void w_sys_manage_basicdataimport_m_Dialog::on_checkBox_2_toggled(bool checked)
{
    ui->lineEdit_date->setVisible(checked);
    ui->label_10->setVisible(checked);
    ui->dateTimeEdit->setVisible(!checked);

}

//    << "t_m_gift"
//    << "t_m_curamount"
//    << "t_m_member"

//    << "t_m_member_type"
//    << "t_m_parameter"
//    t_m_deposit;
//    t_m_pay;
//    t_m_point;
//    t_m_point_gift;
void w_sys_manage_basicdataimport_m_Dialog::on_pushButton_import_clicked()
{
    QString errstring;
    lds_messagebox_loading_show loading(this, "", "LOADING");
    QDateTime sqldt = n_func::f_get_sysdatetime();
    lds_query_hddpos  query;
    QString splitfilter = ui->comboBox->currentText();

    loading.show_delay();
    //check data
    if(!checkNoRepeat(ui->lineEdit_path->text(), ui->comboBox_codec->currentText(), splitfilter, ui->lineEdit_no->text().toInt(), true, errstring)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
    // import begin
    QFile file(ui->lineEdit_path->text());
    if(!file.open(QFile::ReadOnly)) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, file.errorString());
        return;
    }
    if(1 == lds_messagebox::warning(this, _TEXT_SLOT(this), QObject::tr("该操作将会先清空所有的会员信息, 是否继续"), QObject::tr("继续"), QObject::tr("取消"))) {
        return;
    }
    if(ui->checkBox->isChecked()) {//第一行跳过
        w_sys_manage_basicdataimport::FILE_READ_LINE_CODEC(file, ui->comboBox_codec->currentText().toLocal8Bit());
    }
    lds_query_hddpos::tran_saction();
    if(!query.execDelete(QStringList() << "t_m_gift" << "t_m_curamount" <<"t_m_member" <<"t_m_member_type" <<"t_m_deposit" <<"t_m_pay" <<"t_m_point"  <<"t_m_point_gift")) {
        lds_query_hddpos::roll_back();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, query.errorString());
        return;
    }
    if(!query.execInsert("t_m_member_type",
                         "ch_typeno, vch_typename, ch_attribute, ch_favourable, ch_pricekind, int_discount, ch_projectno, ch_calc, ch_calc_type, int_calc_unit, ch_area_flag, int_backcash_rate, int_card_baseamount",
                         "01,"+QObject::tr("默认类别")+",2,2,1,100,,Y,0,1,0,0,0",
                         ",")) {
        lds_query_hddpos::roll_back();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, query.errorString());
        return;
    }
    //while
    while(!file.atEnd()) {
        QString line = w_sys_manage_basicdataimport::FILE_READ_LINE_CODEC(file, ui->comboBox_codec->currentText().toLocal8Bit());
        line = line.trimmed();
        if(line.count() == 0)
            continue;
        QStringList lines = line.split(splitfilter);
        //                                                        QString s = "57700006	遗失卡	100%	719	708	-37	0	2014/5/19	10	2016/12/12	xxx	12555";
        QString ch_typeno =  "01";//
        if(ui->lineEdit_ch_typeno2->isVisible()) {
            if(ui->lineEdit_ch_typeno2->text().toInt() >= 0) {
                ch_typeno = lines.value(ui->lineEdit_ch_typeno2->text().toInt());
            }
        }
        if(!query.execInsert("t_m_member", qrtVariantPairList()
                             << qrtVariantPair("vch_memberno", lines.value(ui->lineEdit_no->text().toInt()))
                             << qrtVariantPair("vch_member", lines.value(ui->lineEdit_name->text().toInt()))
                             << qrtVariantPair("ch_typeno", ch_typeno)
                             << qrtVariantPair("vch_id", lines.value(ui->lineEdit_no->text().toInt()))
                             << qrtVariantPair("dt_limit",
                                               ui->lineEdit_date->isVisible()?//导入会员有效期
                                               lines.value(ui->lineEdit_date->text().toInt()):
                                               ui->dateTimeEdit->dateTimeStr())

                             << qrtVariantPair("ch_state", 1)
                             << qrtVariantPair("num_limit", 0)
                             << qrtVariantPair("ch_cardflag", "Y")
                             << qrtVariantPair("vch_cardno", lines.value(ui->lineEdit_no->text().toInt()))
                             << qrtVariantPair("int_basedata", 0)

                             << qrtVariantPair("dt_sendtime", sqldt)
                             << qrtVariantPair("vch_operid", lds::gs_operid)
                             << qrtVariantPair("dt_operdate", sqldt)
                             << qrtVariantPair("dt_birthday", lines.value(ui->lineEdit_birthday->text().toInt(), "2000-01-01"))
                             << qrtVariantPair("vch_handtel", lines.value(ui->lineEdit_handtel->text().toInt()).trimmed())

                             << qrtVariantPair("vch_address", lines.value(ui->lineEdit_address->text().toInt())))) {
            lds_query_hddpos::roll_back();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, query.errorString());
            return;
        }
        //需要导入会员类型

        query.execSelect(QString("select count(0) from t_m_member_type where ch_typeno = '%1' ").arg(ch_typeno));
        query.next();
        if(query.recordValue(0).toLongLong() == 0) {
            if(!query.execInsert("t_m_member_type",
                                 "ch_typeno, vch_typename, ch_attribute, ch_favourable, ch_pricekind, int_discount, ch_projectno, ch_calc, ch_calc_type, int_calc_unit, ch_area_flag, int_backcash_rate, int_card_baseamount",
                                 QString("%1,"+QObject::tr("默认类别")+"%1,2,2,1,100,,Y,0,1,0,0,0")
                                 .arg(ch_typeno),
                                 ",")) {
                lds_query_hddpos::roll_back();
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, query.errorString());
                return;
            }
        }
        if(!query.execInsert("t_m_curamount", qrtVariantPairList()
                             << qrtVariantPair("vch_memberno", lines.value(ui->lineEdit_no->text().toInt()))
                             << qrtVariantPair("num_amount", lines.value(ui->lineEdit_num->text().toInt()).toFloat())
                             << qrtVariantPair("num_point", lines.value(ui->lineEdit_point->text().toInt()).toFloat())
                             << qrtVariantPair("vch_operid", lds::gs_operid)
                             << qrtVariantPair("dt_operdate", sqldt))) {
            lds_query_hddpos::roll_back();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, query.errorString());
            return;
        }
        if(!query.execInsert("t_m_deposit", qrtVariantPairList()
                             << qrtVariantPair("vch_memberno", lines.value(ui->lineEdit_no->text().toInt()))
                             << qrtVariantPair("num_deposit", lines.value(ui->lineEdit_num->text().toInt()).toFloat())
                             << qrtVariantPair("num_realamount", lines.value(ui->lineEdit_num->text().toInt()).toFloat())
                             << qrtVariantPair("ch_deposit_mode", 1)
                             << qrtVariantPair("ch_pay_mode", 1)

                             << qrtVariantPair("vch_operid", lds::gs_operid)
                             << qrtVariantPair("dt_operdate", sqldt)
                             << qrtVariantPair("vch_memo", QObject::tr("导入充值"))
                             << qrtVariantPair("remain_amt", lines.value(ui->lineEdit_num->text().toInt()).toFloat()))) {
            lds_query_hddpos::roll_back();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, query.errorString());
            return;
        }
        if(!query.execInsert("t_m_point", qrtVariantPairList()
                             << qrtVariantPair("vch_memberno", lines.value(ui->lineEdit_no->text().toInt()))
                             << qrtVariantPair("ch_type", 3)
                             << qrtVariantPair("ch_payno", "P00000000001")
                             << qrtVariantPair("num_point", lines.value(ui->lineEdit_point->text().toInt()).toFloat())
                             << qrtVariantPair("vch_operID", lds::gs_operid)

                             << qrtVariantPair("dt_operdate", sqldt))) {
            lds_query_hddpos::roll_back();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, query.errorString());
            return;
        }
    }//~while
    lds_query_hddpos::com_mit();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("导入成功"));
}

void w_sys_manage_basicdataimport_m_Dialog::on_checkBox_3_toggled(bool checked)
{

    ui->lineEdit_ch_typeno2->setVisible(checked);
    ui->label_22->setVisible(checked);
    ui->lineEdit_ch_typeno->setVisible(!checked);
}

void w_sys_manage_basicdataimport_m_Dialog::topreview()
{
    w_sys_manage_basicdataimport_preview dialog(ui->lineEdit_path->text(), ui->comboBox_codec->currentText(), this);
    transparentCenterDialog(&dialog).exec();
    ui->comboBox_codec->setcomtext(dialog.curCodec());
}

bool w_sys_manage_basicdataimport_m_Dialog::checkNoRepeat(const QString &filepath, const QString &codec, const QString &split, int column, bool skip_first_row, QString &errstring)
{
    QString msg;
    QMap<QString, int> no_num_map;
    QFile file(filepath);
    if(!file.open(QFile::ReadOnly)) {
        errstring = file.errorString();
        return false;
    }
    if(skip_first_row)
        file.readLine();
    while(!file.atEnd()) {
        QString line = w_sys_manage_basicdataimport::FILE_READ_LINE_CODEC(file, codec.toLocal8Bit());
        line = line.trimmed();
        if(line.count() == 0)
            continue;
        QStringList lines = line.split(split);
        QString vch_memberno = lines.value(column);
        no_num_map.insert(vch_memberno, no_num_map.value(vch_memberno, 0) + 1);
    }
    file.close();
    msg = "";
    for(QMap<QString, int>::iterator k = no_num_map.begin(); k != no_num_map.end(); k ++) {
        if(k.value() > 1) {
            msg += k.key() + ",";
        }
    }
    if(msg.count() > 0) {
        errstring = QObject::tr("编号有重复") + "\n" + msg;
        return false;
    }
    return true;
}

#include "w_sys_manage_basicdataexport.h"
//基础数据导出
#include "ui_w_sys_manage_basicdataimport_dialog.h"
#include "lds_query_hddpos.h"
#include <QSqlRecord>
#include <QSqlError>
#include "lds_messagebox.h"
#include "public_sql.h"
#include <QFileDialog>
#include <QCloseEvent>
#include "lds.h"
#include "getfilepath.h"
#include "lds.h"
#include "w_sys_manage_sqlcopy.h"


static void SQL_TABLE_EXPORT(w_sys_manage_basicdataexport *receiver, const QString &path, const QStringList &tablenames)
{
    QString errtext;
    QString table_column_name;
    QString table_column_value;
    QString tablename;
    qlonglong columncount;
    qlonglong rowcount;
    QFile file;
    QDir dir(path);
    QString dotPercentEncodeStr = QByteArray(",").toPercentEncoding();

    lds_query_hddpos  query;

    if(!dir.exists()) {
        dir.mkdir(path);
    }
    for(int table_c=0,table_count=tablenames.count();table_c<table_count;table_c++) {//0
        errtext.clear();

        tablename=tablenames[table_c];
        file.setFileName(path+"/"+tablename+".csv");
        receiver->export_info_progress(QString("%1[%2/%3]    start export...")
                                       .arg(tablename)
                                       .arg(table_c+1)
                                       .arg(table_count));

        switch(0) {
        case 0:
            if(!file.open(QFile::WriteOnly)) {
                errtext= file.errorString();
                break;
            }
        case 1:
            if(!query.execSelect("select * from "+tablename, &errtext)) {
                break;
            }
            columncount=query.recordCount();
            for(int c=0; c < columncount; c++) {
                table_column_name = query.record().fieldName(c);
                file.write(table_column_name.toLocal8Bit());
                //是否到结尾
                if(c==columncount-1) {
                    file.write("\n");
                } else {
                    file.write(",");
                }
                receiver->export_info_progress(
                            QString("%1[%2/%3]    table_column_name start export...")
                            .arg(tablename)
                            .arg(table_c+1)
                            .arg(table_count)
                            );
            }
        case 2:
            if(!query.execSelect("select count(0) from "+tablename, &errtext)) {
                break;
            }
            query.next();
            rowcount=query.recordValue(0).toLongLong();
        case 3:
            if(!query.execSelect("select * from "+tablename, &errtext)) {
                break;
            }
            for(int r=0;r<rowcount;r++) {
                query.next();
                for(int c=0; c < columncount; c++) {
                    table_column_value = query.recordValue(c).toString();
                    table_column_value.remove(QChar::Null);
                    table_column_value.replace(",", dotPercentEncodeStr);
                    //{
                    file.write(table_column_value.toLocal8Bit());
                    //是否到结尾
                    if(c==columncount-1) {
                        file.write("\n");
                    } else {
                        file.write(",");
                    }
                }
                receiver->export_info_progress(
                            QString("%1[%2/%3]    table_column_value start export...")
                            .arg(tablename)
                            .arg(table_c+1)
                            .arg(table_count),
                            r + 1,
                            rowcount
                            );
            }
        case 4:
            break;

        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        }
        file.close();
        receiver->export_info_progress(
                    QString("%1[%2/%3]   %4")
                    .arg(tablename)
                    .arg(table_c+1)
                    .arg(table_count)
                    .arg(errtext.isEmpty()?QString("success"):errtext));
        if(errtext.isEmpty()) {
        } else {
            lds_messagebox::information(receiver, MESSAGE_TITLE_VOID, errtext);
            break;
        }
    }//0

    if(errtext.isEmpty())
        lds_messagebox::information(receiver, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
}

w_sys_manage_basicdataexport::w_sys_manage_basicdataexport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_sys_manage_basicdataimport_Dialog)
{
    ui->setupUi(this);
    lds::hideWidget(ui->comboBox_codec);
    lds::hideWidget(ui->label_codec);

    //! toolbar
    ui->widget->setup(QStringList() <<QObject::tr("卸载U盘")<< QObject::tr("确定") << QObject::tr("退出"), Qt::AlignRight);
    connect(ui->toolButton,SIGNAL(clicked()),this,SLOT(togetfilepath()));
    connect(ui->widget->index_widget(QObject::tr("卸载U盘")),SIGNAL(clicked()),this,SLOT(toumount_upan()));
    connect(ui->widget->index_widget(QObject::tr("确定")),SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));

    //! after
    ui->comboBox_path->addItem(lds::localdataPath);
    ui->comboBox_path->addItems(lds::getUsbPathList());
    ui->comboBox_path->setEditText(lds::sysconf->value("w_sys_manage_basicdataimport/path", public_sql::sql_table_export_dir).toString());

    _confname="w_sys_manage_basicdataexport/path";
    _tablenames
            << "cey_bt_unit"
            << "cey_bt_dish"
            << "cey_bt_dish_subtype"
            << "cey_bt_dish_type"
            << "cey_bt_dish_suit"

            << "cey_bt_suit_change"
            << "cey_bt_table"
            << "cey_bt_table_type"
            << "cey_bt_table_area"
            << "cey_bt_cp_dish"

            << "cey_bt_cp_memo"
            << "cey_bt_cp_type"
            << "cey_sys_company"
            << "cey_bt_dish_warn"
            << "cey_bt_paymode"

            << "cey_bt_kitchen_plan"
            << "cey_bt_kitchen_print"
            << "cey_sys_parameter"

            << "t_m_parameter"
            << "t_m_gift"
            << "t_m_curamount"
            << "t_m_member"
            << "t_m_member_type"
               ;
    reset_export_confname("w_sys_manage_basicdataexport");
    if(lds::get_soft_curent_language_first() == _EN_) {
        ui->label->hide();
    }
}

void w_sys_manage_basicdataexport::reset_export_tablenames(const QStringList &tablenames)
{
    _tablenames=tablenames;
}

void w_sys_manage_basicdataexport::reset_export_confname(const QString &confname)
{
    _confname=confname;

    ui->comboBox_path->setEditText(lds::sysconf->value(_confname, public_sql::sql_table_export_dir).toString());

}

void w_sys_manage_basicdataexport::togetfilepath()
{
    QString filepath = getFilePath::getExistingDirectory(this, QObject::tr("导出文件夹"),ui->comboBox_path->currentText(),QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty()) {
        ui->comboBox_path->setEditText( filepath);

        lds::sysconf->setValue(_confname, ui->comboBox_path->currentText());
    }
}

void w_sys_manage_basicdataexport::toumount_upan()
{
    w_sys_manage_sqlcopy::unmount_u_pan(this);
}

void w_sys_manage_basicdataexport::tosave()
{
    //开始写入
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("该操作会覆盖目录下的同名文件，是否继续"),QObject::tr("确认"),QObject::tr("取消"))) {
        SQL_TABLE_EXPORT(this, ui->comboBox_path->currentText(), _tablenames);
    }
}

void w_sys_manage_basicdataexport::toexit()
{
    this->reject();
}

void w_sys_manage_basicdataexport::export_info_progress(const QString &info, int step, int max)
{
    if(max != -1) {
        if(ui->progressBar->maximum() != max) {
            ui->progressBar->setMaximum(max);
        }
    }
    if(step != -1) {
        ui->progressBar->setValue(step);
    }
    ui->lineEdit_sub_going->setText(info);
    ui->lineEdit_sub_going->repaint();
}

void w_sys_manage_basicdataexport::closeEvent(QCloseEvent *e)
{
    e->ignore();
}

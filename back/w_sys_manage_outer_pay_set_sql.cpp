#include "w_sys_manage_outer_pay_set_sql.h"
#include "ui_w_sys_manage_outer_pay_set_sql.h"
#include "lds_query_hddpos.h"
#include <QSqlError>
#include <QElapsedTimer>
#include "getfilepath.h"
#include "public_sql.h"

w_sys_manage_outer_pay_set_sql::w_sys_manage_outer_pay_set_sql(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_outer_pay_set_sql)
{
    ui->setupUi(this);

    //获取路径
    ui->comboBox_sql_path->clear();
    foreach(const QFileInfo &info, QDir("userdata/fastfd_localdata/sql_backup_dir").entryInfoList(QDir::Files)) {
        ui->comboBox_sql_path->addItem(info.fileName(), info.filePath());
    }
    ui->comboBox->addItem(QObject::tr("数据库重新建立"), "drop database hddpos;create database hddpos;");
    ui->comboBox->setCurrentIndex(-1);
    //
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this,SLOT(updateEdit(int)));
}

w_sys_manage_outer_pay_set_sql::~w_sys_manage_outer_pay_set_sql()
{
    delete ui;
}

void w_sys_manage_outer_pay_set_sql::took()
{
    QElapsedTimer etimer;
    etimer.start();
    lds_query_hddpos  query;
    if(query.execDirect(ui->textEdit->toPlainText())) {
        ui->textEdit_2->append("=================\n"+QObject::tr("操作成功") + "  elapsed:"+ QString::number(etimer.elapsed()) + "ms");
        while(query.next()) {
            int count = query.recordCount();
            QString str;
            for(int k = 0; k < count; k ++) {
                str += query.recordValue(k).toString()+"\t";
            }
            ui->textEdit_2->append(str);
        }
    } else {
        ui->textEdit_2->append("=================\n"+query.recordError());
    }
}

void w_sys_manage_outer_pay_set_sql::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_pay_set_sql::updateEdit(int index)
{
    ui->textEdit->setText(ui->comboBox->itemData(index).toString());
}

void w_sys_manage_outer_pay_set_sql::on_pushButton_sql_run_clicked()
{
    QString filepath = ui->comboBox_sql_path->curusrdata().toString();
    lds_messagebox_loading_show loading(this, "" ,"DB RESTORE...");
    loading.show();
    public_sql::MySQL_restore(filepath, false);
    ui->textEdit_2->append(public_sql::MySQL_info);
}

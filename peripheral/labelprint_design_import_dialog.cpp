#include "labelprint_design_import_dialog.h"
#include "ui_labelprint_design_import_dialog.h"
#include "commonheader.h"
#include <QDir>
#include "uinormal_message.h"

labelprint_design_import_dialog::labelprint_design_import_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_labelprint_design_import_dialog)
{
    ui->setupUi(this);
    setWindowTitle(QObject::tr("表单导入"));
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_del,SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
}

void labelprint_design_import_dialog::updatecom(const QString &value)
{
    QString import_dir;
    if(value.isEmpty()) {
        import_dir=_dir;
    } else {
        import_dir=_dir=value;
    }

    QDir dir(import_dir);
    if(!dir.exists()) {
        dir.mkpath(dir.path());
    }
    QFileInfoList fileinfos=dir.entryInfoList();
    QStringList filenames;
    foreach(QFileInfo info, fileinfos) {
        if(info.fileName() == "."
                ||info.fileName() == "..")continue;
        filenames.append(info.fileName());
    }

    ui->comboBox->clear();
    ui->comboBox->addItems(filenames);
}

QString labelprint_design_import_dialog::getfilename() const
{
    return _fname;
}

void labelprint_design_import_dialog::tocancel()
{
    this->reject();
}

void labelprint_design_import_dialog::todel()
{
    if(1==uinormal_message::warning(this, QObject::tr("表单导入"),QObject::tr("确定删除"),QObject::tr("确定"),QObject::tr("取消"))) return;
    QFile f(_dir+"/"+ui->comboBox->currentText());
    if(f.remove()) {
        updatecom();
    } else {
        uinormal_message::warning(this, QObject::tr("表单导入"),f.errorString());
    }
}

void labelprint_design_import_dialog::took()
{
    _fname=_dir+"/"+ui->comboBox->currentText();
    this->accept();
}

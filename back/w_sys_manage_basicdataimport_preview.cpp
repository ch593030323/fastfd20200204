#include "w_sys_manage_basicdataimport_preview.h"
#include "ui_w_sys_manage_basicdataimport_preview.h"
#include "w_sys_manage_basicdataimport.h"
#include <QFile>

w_sys_manage_basicdataimport_preview::w_sys_manage_basicdataimport_preview(const QString &filepath, const QString &def_codec, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_basicdataimport_preview)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("预览"));
    this->filepath = filepath;
    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    ui->comboBox_codec->setCurrentIndex(ui->comboBox_codec->findText(def_codec));
    ui->comboBox->addItem("10", 10);
    ui->comboBox->addItem("100", 100);
    ui->comboBox->addItem("1000", 1000);
    ui->comboBox->addItem(QString::number(file_row_count(filepath)), -1);

    connect(ui->comboBox_codec, SIGNAL(currentIndexChanged(int)), this, SLOT(toFillModel()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(toFillModel()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));

    toFillModel();
}

w_sys_manage_basicdataimport_preview::~w_sys_manage_basicdataimport_preview()
{
    delete ui;
}

QString w_sys_manage_basicdataimport_preview::curCodec()
{
    return ui->comboBox_codec->currentText();
}

void w_sys_manage_basicdataimport_preview::toFillModel()
{
    int max_row = ui->comboBox->curusrdata().toInt();

    model->removeRows(0, model->rowCount());
    model->removeColumns(0, model->columnCount());
    QFile file(filepath);
    file.open(QFile::ReadOnly);
    QString line;
    while(!file.atEnd()) {
        line = w_sys_manage_basicdataimport::FILE_READ_LINE_CODEC(file, ui->comboBox_codec->currentText().toLocal8Bit());
        QList<QStandardItem*> items;
        foreach(const QString &item, line.trimmed().split(","))
            items << new QStandardItem(item);
        model->appendRow(items);

        if(max_row != -1 && max_row <= model->rowCount())
            break;
    }

    QStringList labels;
    for(int column = 0; column < model->columnCount(); column ++) {
        labels << QString::number(column);
    }
    model->setHorizontalHeaderLabels(labels);
    ui->tableView->resizeColumnsToContents();
}

int w_sys_manage_basicdataimport_preview::file_row_count(const QString &filepath, bool skip_empt_row)
{
    int row_count = 0;
    QFile file(filepath);
    file.open(QFile::ReadOnly);
    while(!file.atEnd()) {
        QString line = file.readLine().trimmed();
        if(skip_empt_row && line.count() == 0)
            continue;
        row_count ++;
    }
    return row_count;
}

void w_sys_manage_basicdataimport_preview::took()
{
    this->accept();
}

void w_sys_manage_basicdataimport_preview::tocancel()
{
    this->reject();
}

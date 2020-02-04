#include "w_rpt_report_dataexport.h"
#include "ui_w_rpt_report_dataexport.h"
#include <QStandardItemModel>
#include <QFile>
#include <QtDebug>
#include "lds_messagebox.h"
#include <QFileDialog>
#include "public_sql.h"
#include "lds.h"
#include "getfilepath.h"
#include "lds.h"
#include "lds_query_hddpos.h"
#include "w_sys_manage_sqlcopy.h"
#include "w_rpt_report_frame.h"
#include <QSqlRecord>
#include "lds_tableview_header.h"
#include "bmp_info.h"
#include "printer_pos_serial.h"
#include "lds_model_sqlstandardmodel.h"
#include "lds_query_variant.h"
#include "w_rpt_report_dataexport_jpg.h"

static QString CELL_CHECK_DATA(QString d) {
    d=d.replace(",", "");//去掉逗号
    QString tmpd;
    foreach(const QChar &c, d) {//去掉null
        if(!c.isNull()) {
            tmpd += c;
        }
    }
    return tmpd;
}

static void SQL_REPORT_EXPORT_BY_TABLE(QString &write_byte, w_rpt_report_dataExport *receiver, lds_tableView *content_tableview,lds_tableView *tail_tableview,  bool headexport) {

    QList<int> columnList = content_tableview->horizontalShowedHeaderList();
    //导出表头
    if(headexport) {
        foreach(int c, columnList) {
            write_byte += CELL_CHECK_DATA(content_tableview->hh_paint_transfer(c))+",";
        }
        write_byte.chop(1);
        write_byte += "\n";
        receiver->export_info_progress(QObject::tr("导出表头"), 1, 1);
    }

    //导出内容
    for(int r = 0, rcount = content_tableview->verticalHeader()->count(); r < rcount; r++) {
        foreach(int c, columnList) {
            write_byte += CELL_CHECK_DATA(content_tableview->dg_paint_transfer(r, c))+",";
        }
        write_byte.chop(1);
        write_byte += "\n";
        receiver->export_info_progress(QObject::tr("导出表单"), r + 1, rcount);
    }
    //合计
    if(tail_tableview) {
        columnList = tail_tableview->rePecifyHeaderList;
        if(tail_tableview->rePecifyHeaderList.count() == 0) {
            for(int k = 0; k < tail_tableview->horizontalHeader()->count(); k ++) {
                columnList << k;
            }
        }
        int rcount = tail_tableview->verticalHeader()->count();
        for(int r = 0; r < rcount; r ++) {
            foreach(int c, columnList) {
                write_byte += CELL_CHECK_DATA(tail_tableview->dg_paint_transfer(r, c))+",";
            }
            write_byte.chop(1);
            write_byte += "\n";
            receiver->export_info_progress(QObject::tr("导出表尾"), r, rcount);
        }
    }
    write_byte.chop(1);//remove
    receiver->export_info_progress(QObject::tr("导出结束"), 1, 1);
}

static void SQL_REPORT_EXPORT_BY_SQL(QString &write_byte, w_rpt_report_dataExport *receiver, lds_tableView *content_tableview,  const QString &sql,  bool headexport)
{
    lds_query_hddpos  query;
    query.execSelect("select count(0) from ("+sql+")t");
    query.next();
    int rowcount =query.recordValue(0).toLongLong();
    int going = 0;
    query.execSelect(sql);
    while(query.next()) {
        int ccount = query.recordCount();
        //表头
        if(headexport) {
            for(int c = 0; c < ccount; c ++) {
                write_byte += CELL_CHECK_DATA(content_tableview->hh_paint_transfer(c))+",";
            }
            write_byte.chop(1);
            write_byte += "\n";
            headexport = false;
            receiver->export_info_progress(QObject::tr("导出表头"), 1, 1);
        }

        //内容
        going ++;
        for(int c = 0; c < ccount; c ++) {
            //data, headerdata, tableview, column
            write_byte += CELL_CHECK_DATA(content_tableview->dg_paint_transfer(query.recordValue(c), QString::number(c))) + ",";
        }
        write_byte.chop(1);
        write_byte += "\n";
        receiver->export_info_progress(QObject::tr("导出表单"), going, rowcount);
    }
    write_byte.chop(1);//remove
    receiver->export_info_progress(QObject::tr("导出结束"), 1, 1);
}

static void SQL_REPORT_EXPORT_BY_MASTER(QString &write_byte, w_rpt_report_dataExport *receiver, const QString &head_sql, lds_tableView *content_tableview,lds_tableView *tail_tableview,  bool headexport) {

    const int columnSpan = content_tableview->horizontalShowedHeaderList().count() / 2;
    QString nullFill;
    for(int k = 0; k < columnSpan - 1; k ++) {
        nullFill += ",";
    }
    lds_query_hddpos  query;
    query.execSelect(head_sql);
    query.next();
    int qcount = query.record().count();
    for(int c = 0; c < qcount; c ++) {
        if(columnSpan >= 3) {
            write_byte += lds::reportModelMapValue(query.record().fieldName(c)) + ":" + qrtVariant::toString(query.record().value(c)) + nullFill + ",";
            c ++;
        }
        if(c < qcount)
            write_byte += lds::reportModelMapValue(query.record().fieldName(c)) + ":" + qrtVariant::toString(query.record().value(c)) + "\n";
    }
    if(false == write_byte.endsWith("\n"))
        write_byte += "\n";
    //write_byte2
    QString write_byte2;
    SQL_REPORT_EXPORT_BY_TABLE(write_byte2, receiver, content_tableview, tail_tableview, headexport);
    //
    write_byte += write_byte2;
}


w_rpt_report_dataExport::~w_rpt_report_dataExport()
{
    delete ui;
}

void w_rpt_report_dataExport::defFilepath(const QString &tabtext)
{
    ui->comboBox_path->addItem(QString() + "userdata/fastfd_localdata/sql_table_export_dir" + "/" + tabtext + ".csv");
    foreach(const QString &k, lds::getUsbPathList()) {
        ui->comboBox_path->addItem(k + "/" + tabtext + ".csv");
    }
}

void w_rpt_report_dataExport::export_info_progress(const QString &info, int v, int total)
{
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(v);
    ui->textEdit->append(info+QString("%1/%2").arg(v).arg(total));
    this->repaint();
}

void w_rpt_report_dataExport::togetfilepath()
{
    QString filepath = getFilePath::getSaveFileName(this, QObject::tr("导出文件"),ui->comboBox_path->currentText(),QString(),0,QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty()) {
        ui->comboBox_path->setEditText(filepath);
    }
}

void w_rpt_report_dataExport::getExportData(QString &write_byte)
{
    if(ExportTypeTABLE == export_type)
        SQL_REPORT_EXPORT_BY_TABLE(write_byte, this, content_tableview, tail_tableview, ui->checkBox_exportheader->isChecked());
    if(ExportTypeSQL == export_type)
        SQL_REPORT_EXPORT_BY_SQL(write_byte, this, content_tableview, content_sql, ui->checkBox_exportheader->isChecked());
    if(ExportTypeMASTER == export_type)
        SQL_REPORT_EXPORT_BY_MASTER(write_byte, this, head_sql, content_tableview, tail_tableview, ui->checkBox_exportheader->isChecked());
}

QFont w_rpt_report_dataExport::fontPixelSize(QFont font, int pixelSize)
{
    font.setPixelSize(pixelSize);
    return font;
}

void w_rpt_report_dataExport::getExportImage(const QString &title, QString write_byte, QPixmap &pixmap, int pixmap_width, bool hasGrid)
{
    bool ok;
    QTableView table;
    table.setLayoutDirection(Qt::LeftToRight);
    table.verticalHeader()->setDefaultSectionSize(45);
    table.verticalHeader()->hide();
    table.horizontalHeader()->hide();
    table.setItemDelegate(new ItemDelegateForArabic(&table));
    QFont font = table.font();
    font.setPixelSize(25);
    table.setFont(font);
    QStandardItemModel model;
    table.setModel(&model);
    //fill model
    model.appendRow(new QStandardItem(title));
    model.item(0, 0)->setTextAlignment(Qt::AlignCenter);
    if(write_byte.count() > 0)
        if(write_byte.mid(write_byte.count() - 1, 1) == "\n") {
            write_byte.chop(1);
        }
    foreach(const QString& line, write_byte.split("\n")) {
        QList<QStandardItem*>items;
        foreach(const QString &item, line.split(",")) {
            item.toDouble(&ok);
            items << new QStandardItem(item);
            if(ok) {
                items.last()->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
            }
        }
        model.appendRow(items);
    }
    //take null item
    QMap<int, QList<QStandardItem*> > itemrect;
    for(int row = model.rowCount() - 1; row >= 0; row --) {
        for(int column = 0; column < model.columnCount(); column ++) {
            if(model.item(row, column) == 0) {
                itemrect.insert(row, model.takeRow(row));
                break;
            }
        }
    }
    //
    for(QMap<int, QList<QStandardItem*> >::iterator k = itemrect.begin(); k != itemrect.end(); k ++) {
        QList<QStandardItem*> &items = k.value();
        for(int column = 0; column < items.count(); column ++) {
            if(items[column] != 0 && items[column]->text() == "") {
                delete items[column];
                items[column] = 0;
            }
        }
    }
    //resize columns
    table.resizeColumnsToContents();
    //insert null item
    for(QMap<int, QList<QStandardItem*> >::iterator k = itemrect.begin(); k != itemrect.end(); k ++) {
        model.insertRow(k.key(), k.value());
    }
    //span
    for(int row = 0; row < model.rowCount(); row ++) {
        for(int column = 1, begin = 0; column < model.columnCount(); column ++) {
            if(model.item(row, column) == 0) {
                table.setSpan(row, begin, 1, column - begin + 1);
            } else {
                begin = column;
            }
        }
    }
    //resize table
    int tablew = 0, tableh = 0;
    for(int column = 0; column < model.columnCount(); column ++) {
        tablew += table.columnWidth(column);
    }
    for(int row = 0; row < model.rowCount(); row ++) {
        tableh += table.rowHeight(row);
    }
    int off = pixmap_width - tablew;
    if(off > 0) {
        table.setColumnWidth(model.columnCount() - 1, table.columnWidth(model.columnCount() - 1) + off);
        tablew += off;
    }
    table.resize(tablew + model.columnCount(), tableh + model.rowCount());
    //
    table.setStyleSheet("border:0px;background:white;color:black;");
    table.setShowGrid(false);
    for(int row = 0; row < model.rowCount(); row ++) {
        for(int column = 0; column < model.columnCount(); column ++) {
            QLabel *b = new QLabel;
            QString border;
            if(hasGrid) {
                border = "border-bottom:1px solid black;border-right:1px solid black;";
                if(row == 0 && column == 0) {
                    border += "border-top:1px solid black;border-left:1px solid black;";
                } else if(row == 0) {
                    border += "border-top:1px solid black;";
                } else if(column == 0) {
                    border += "border-left:1px solid black;";
                }
            } else {
                border = "border-bottom:1px solid black;";
                if(row == 0 && column == 0) {
                    border += "border-top:1px solid black;border-left:1px solid black;";
                } else if(column == model.columnCount() - 1) {
                    border += "border-right:1px solid black;";
                } else if(row == 0) {
                    border += "border-top:1px solid black;";
                } else if(column == 0) {
                    border += "border-left:1px solid black;";
                }
            }

            b->setStyleSheet("QLabel{background:transparent;border:0px;" + border + "}");
            table.setIndexWidget(table.model()->index(row, column), b);
        }
    }
    //get pixmap
    pixmap  = QPixmap::grabWidget(&table);

#ifdef QT_DEBUG
    pixmap.save("report_" + title + ".jpg");
    //    qDebug() << table.layoutDirection();
    qDebug() << pixmap.width();
    QDialog dialog;
    table.setParent(&dialog);
    QHBoxLayout layout;
    layout.addWidget(&table);
    dialog.setLayout(&layout);
    dialog.resize(table.size());
    dialog.exec();
    table.setParent(0);
#endif
    if(off < 0)
        pixmap = pixmap.scaledToWidth(pixmap_width, Qt::SmoothTransformation);

    qDebug() << pixmap.width();
}

void w_rpt_report_dataExport::initData()
{
    //数据初始化
    ui->progressBar->setValue(0);
    ui->comboBox_path->setEditText(lds::sysconf->value("w_rpt_report_dataExport/path").toString());
    ui->checkBox_checkdata->setChecked(lds::sysconf->value("w_rpt_report_dataExport/checkdata", false).toBool());
    ui->checkBox_exportheader->setChecked(lds::sysconf->value("w_rpt_report_dataExport/exportheader",true).toBool());

    connect(ui->pushButton_exit,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_export,SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->pushButton_export_jpg,SIGNAL(clicked()),this,SLOT(toexport_JPG()));
    connect(ui->toolButton_getfilepath,SIGNAL(clicked()),this,SLOT(togetfilepath()));
    connect(ui->pushButton_umount_upan, SIGNAL(clicked()),this,SLOT(toumount_upan()));

    lds::hideWidget(ui->checkBox_checkdata);
    if(ExportTypeMASTER != export_type) {
        lds::hideWidget(ui->pushButton_export_jpg, true);
    }
}

w_rpt_report_dataExport::w_rpt_report_dataExport(lds_tableView *content_tableview, const QString &content_sql, lds_tableView *tail_tableview, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::w_rpt_report_dataExport)
{
    ui->setupUi(this);
    export_type = ExportTypeSQL;
    this->content_sql = content_sql;
    this->content_tableview = content_tableview;
    this->tail_tableview = tail_tableview;
    initData();
}

w_rpt_report_dataExport::w_rpt_report_dataExport(lds_tableView *content_tableview, lds_tableView *tail_tableview, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_rpt_report_dataExport)
{
    ui->setupUi(this);
    export_type = ExportTypeTABLE;
    this->content_tableview = content_tableview;
    this->tail_tableview = tail_tableview;
    initData();
}

w_rpt_report_dataExport::w_rpt_report_dataExport(const QString &head_sql, lds_tableView *content_tableview, lds_tableView *tail_tableview, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_rpt_report_dataExport)
{
    ui->setupUi(this);
    export_type = ExportTypeMASTER;
    this->head_sql = head_sql;
    this->content_tableview = content_tableview;
    this->tail_tableview = tail_tableview;
    initData();
}

void w_rpt_report_dataExport::toexit()
{
    //    lds::sysconf->setValue("w_rpt_report_dataExport/path", ui->comboBox_path->text());
    lds::sysconf->setValue("w_rpt_report_dataExport/checkdata", ui->checkBox_checkdata->isChecked());
    lds::sysconf->setValue("w_rpt_report_dataExport/exportheader", ui->checkBox_exportheader->isChecked());
    this->reject();
}

void w_rpt_report_dataExport::toexport()
{
    //开始写入
    if(0==lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("该操作会覆盖目录下的同名文件，是否继续"),QObject::tr("确认"),QObject::tr("取消"))) {
        ui->textEdit->clear();
        QString write_byte;
        getExportData(write_byte);
        lds::fileWriteByte(ui->comboBox_path->currentText(), write_byte.toLocal8Bit());
    }

#ifdef QT_DEBUG
    QDialog dialog(this);
    dialog.setWindowTitle("DEBUG_DIALOG");
    QTextEdit text(&dialog);
    text.setGeometry(10, 10, 800, 500);
    dialog.resize(820, 520);

    QFile file(ui->comboBox_path->currentText());
    file.open(QFile::ReadOnly);
    text.setText(file.readAll());

    dialog.exec();
#endif
}

void w_rpt_report_dataExport::toexport_JPG()
{
    QString write_byte;
    QPixmap pixmap;
    getExportData(write_byte);
    getExportImage(this->windowTitle(), write_byte, pixmap, PRINTER_A4_WIDTH_PIXELS);
    //
    QFileInfo info(ui->comboBox_path->currentText());
    QString filepath_JPG = info.path() + "/" + info.baseName() + ".JPG";
    w_rpt_report_dataExport_JPG dialog(filepath_JPG, this);
    dialog.setLabelPixmap(pixmap);
    transparentCenterDialog(&dialog).exec();
}

void w_rpt_report_dataExport::toumount_upan()
{
    w_sys_manage_sqlcopy::unmount_u_pan(this);
}


w_rpt_report_dataExport::ItemDelegateForArabic::ItemDelegateForArabic(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void w_rpt_report_dataExport::ItemDelegateForArabic::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt;
    opt = option;
    initStyleOption(&opt, index);
    painter->setLayoutDirection(opt.direction);
    painter->setFont(opt.font);
    public_sql::drawTextForArabic(painter, opt.rect, opt.displayAlignment, index.data().toString());
}

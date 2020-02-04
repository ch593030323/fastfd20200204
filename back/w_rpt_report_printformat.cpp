#include "w_rpt_report_printformat.h"
#include "ui_w_rpt_report_printformat.h"
#include <QStandardItemModel>
#include "lds_tableview_delegate_check.h"
#include "lds_messagebox.h"
#include "lds.h"
#include "w_rpt_report_frame.h"
#include "lds.h"
#include <QSqlRecord>
#include <QTextEdit>
#include "w_rpt_report_frame.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QCloseEvent>

//w_rpt_report_printFormat::w_rpt_report_printFormat(lds_tableView *content_tableview, lds_tableView *tail_tableview, const QString &title, const QString &operid, const QString &opertime, const QString &_conf_path, QWidget *parent)
//    :QDialog(parent)
//{
//    print_type_index = 1;
//    initData(content_tableview, tail_tableview, title, operid, opertime, _conf_path);

//    ui->label_dt_begin->hide();ui->lineEdit_dt_begin->hide();
//    ui->label_dt_end->hide();ui->lineEdit_dt_end->hide();
//}

w_rpt_report_printFormat::w_rpt_report_printFormat(showMode mode, const QString &title, const QString &operid, const QString &opertime, const QString &_conf_path, const QString &dt_from, const QString &dt_to,  lds_tableView *content_tableview, lds_tableView *tail_tableview, QWidget *parent)
    :QDialog(parent)
{
    print_type_index = 2;
    initData(mode, content_tableview, tail_tableview, title, operid, opertime, _conf_path);
    ui->label_dt_begin->setHidden(dt_from.isEmpty());ui->lineEdit_dt_begin->setText(dt_from);
    ui->label_dt_end->setHidden(dt_to.isEmpty());ui->lineEdit_dt_end->setText(dt_to);
}

w_rpt_report_printFormat::w_rpt_report_printFormat(showMode mode, const QString &title, const QString &operid, const QString &opertime, const QString &_conf_path, const QString &dt_from, const QString &dt_to,   lds_tableView *content_tableview, const QString &sql, lds_tableView *tail_tableview, QWidget *parent)
    :QDialog(parent)
{
    print_type_index = 3;
    initData(mode, content_tableview, tail_tableview, title, operid, opertime, _conf_path);
    this->sql = sql;
    ui->label_dt_begin->setHidden(dt_from.isEmpty());ui->lineEdit_dt_begin->setText(dt_from);
    ui->label_dt_end->setHidden(dt_to.isEmpty());ui->lineEdit_dt_end->setText(dt_to);
}

w_rpt_report_printFormat::w_rpt_report_printFormat(showMode mode, const QString &title, const QString &operid, const QString &opertime, const QString &_conf_path, const QString &dt_from, const QString &dt_to, lds_tableview_sum *tableview_sum, QWidget *parent)
    :QDialog(parent)
{
    print_type_index = 4;
    initData(mode, tableview_sum->tableView, tableview_sum->lineView, title, operid, opertime, _conf_path);
    ui->label_dt_begin->setHidden(dt_from.isEmpty());ui->lineEdit_dt_begin->setText(dt_from);
    ui->label_dt_end->setHidden(dt_to.isEmpty());ui->lineEdit_dt_end->setText(dt_to);
}

w_rpt_report_printFormat::~w_rpt_report_printFormat()
{
    delete ui;
}

void w_rpt_report_printFormat::pop(QWidget *basewidget, QWidget *parent)
{
    QAbstractItemModel *model = content_tableview->model();
    if(model == 0
            || model->columnCount() == 0) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("没有打印数据"));
        return ;
    }
    //
    this->setWindowTitle(MESSAGE_TITLE_VOID);
    lds_roundeddialog_rect_pop d(this, parent, Qt::Dialog | Qt::FramelessWindowHint);
    d.updatePos(basewidget);
    connect(this, SIGNAL(signal_printmore()), &d, SLOT(toUpdatePos()));
    d.exec();
}

void w_rpt_report_printFormat::keyPressEvent(QKeyEvent *e)
{
    QDialog::keyPressEvent(e);
}

void w_rpt_report_printFormat::showQuickMode()
{
    this->setFixedSize(500, 70);
    ui->widget->move(0, 0);
}

void w_rpt_report_printFormat::showMoreMode()
{
    this->setFixedSize(500, 480);
    ui->widget->move(0, -60);
}

void w_rpt_report_printFormat::modelappRow(const QString &checked, const QString &header, int width, int line)
{
    standmodel->appendRow(
                QList<QStandardItem*>()
                << new QStandardItem(checked)
                << new QStandardItem(header)
                << new QStandardItem(QString::number(width))
                << new QStandardItem(QString::number(line))
                );
    QStandardItem *item;
    item = standmodel->item(standmodel->rowCount() - 1, 2); item->setFlags(item->flags() | Qt::ItemIsEditable);
    item = standmodel->item(standmodel->rowCount() - 1, 3); item->setFlags(item->flags() | Qt::ItemIsEditable);
}

void w_rpt_report_printFormat::initData(showMode mode, lds_tableView *content_tableview, lds_tableView *tail_tableview, const QString &title, const QString &operid, const QString &opertime, const QString &_conf_path)
{
    ui  = new Ui::w_rpt_report_printFormat;
    ui->setupUi(this);
    if(mode == show_model_more)
        showMoreMode();
    else
        showQuickMode();
    //    ui->checkBox_printspacing_2->hide();
    //    ui->checkBox_printspacing_2->setChecked(false);

    print_format_conf = 0;
    //
    ui->comboBox->addItem(QObject::tr("58打印机"),  "userdata/settings/print/report/" + lds::get_soft_curent_language_first() + "/58_" + _conf_path);
    ui->comboBox->addItem(QObject::tr("80打印机"),  "userdata/settings/print/report/" + lds::get_soft_curent_language_first() + "/80_" + _conf_path);
    //
    standmodel = new QStandardItemModel(this);
    standmodel->setHorizontalHeaderLabels(QStringList() << QObject::tr("选中") << QObject::tr("打印项")<< QObject::tr("宽度") << QObject::tr("行"));
    ui->tableView->setModel(standmodel);
    ui->tableView->setItemDelegateForColumn(1, new lds_tableview_delegate_report(this));
    ui->tableView->setColumnWidth(1, 150);
    ui->tableView->setItemDelegateForColumn(3, new lds_model_sqltablemodel_delegate_com(this, ldsVariantMap("1", 1, "2", 2, "3", 3)));

    lds_tableview_delegate_check *delegate=new lds_tableview_delegate_check(
                QVector<int>()<<0,
                ui->tableView
                );
    delegate->setClickable();
    ui->tableView->setItemDelegateForColumn(0, delegate);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    //
    ui->lineEdit_operid->setReadOnly(true);
    ui->lineEdit_opertime->setReadOnly(true);
    //    ui->lineEdit_title->setReadOnly(true);
    ui->lineEdit_dt_begin->setReadOnly(true);
    ui->lineEdit_dt_end->setReadOnly(true);

    //
    this->content_tableview = content_tableview;
    this->tail_tableview = tail_tableview;
    ui->checkBox_printtotal->setDisabled(tail_tableview == 0);
    for(int c = 0, ccount = content_tableview->horizontalHeader()->count(); c < ccount; c++) {
        modelappRow("N", content_tableview->hh_paint_transfer(c), 5, 0);
    }
    //
    ui->lineEdit_operid->setText(operid);
    ui->lineEdit_opertime->setText(opertime);
    ui->lineEdit_title->setText(title);
    conf_path = _conf_path;

    toPrintFormatSaveAndLoad(0);

    connect(ui->pushButton_exit,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_print,SIGNAL(clicked()),this,SLOT(toReportPrint()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(toPrintFormatSaveAndLoad(int)));

    connect(ui->pushButton_printmore, SIGNAL(clicked()), this, SLOT(toquickprintmore()));
    connect(ui->pushButton_print58, SIGNAL(clicked()), this, SLOT(toprint58()));
    connect(ui->pushButton_print80, SIGNAL(clicked()), this, SLOT(toprint80()));
}

void w_rpt_report_printFormat::toPrintFormatSaveAndLoad(int index)
{
    toPrintFormatSave();

    if(print_format_conf != 0) {
        delete print_format_conf;
        print_format_conf = 0;
    }

    print_format_conf = new QSettings(ui->comboBox->itemData(index).toString(), QSettings::IniFormat, this);

    ui->checkBox_printtitle->setChecked(print_format_conf->value("conf/printtitle", true).toBool());
    ui->checkBox_printtotal->setChecked(print_format_conf->value("conf/printtail", true).toBool());
    ui->checkBox_printspacing->setChecked(print_format_conf->value("conf/printspacing", false).toBool());
    for(int row = 0; row < standmodel->rowCount(); row ++) {
        standmodel->item(row, 0)->setText(print_format_conf->value("conf/row_" + QString::number(row) + "_check", "N").toString());
        standmodel->item(row, 2)->setText(print_format_conf->value("conf/row_" + QString::number(row) + "_width",  "5").toString());
        standmodel->item(row, 3)->setText(print_format_conf->value("conf/row_" + QString::number(row) + "_line",     "1").toString());
    }
}

void w_rpt_report_printFormat::toPrintFormatSave()
{
    if(print_format_conf == 0) return;

    print_format_conf->setValue("conf/printtitle", ui->checkBox_printtitle->isChecked());
    print_format_conf->setValue("conf/printtail", ui->checkBox_printtotal->isChecked());
    print_format_conf->setValue("conf/printspacing", ui->checkBox_printspacing->isChecked());
    for(int row = 0; row < standmodel->rowCount(); row ++) {
        print_format_conf->setValue("conf/row_" + QString::number(row) + "_check",  standmodel->item(row, 0)->text());
        print_format_conf->setValue("conf/row_" + QString::number(row) + "_width",  standmodel->item(row, 2)->text());
        print_format_conf->setValue("conf/row_" + QString::number(row) + "_line",     standmodel->item(row, 3)->text());
    }
}

void w_rpt_report_printFormat::toquickprintmore()
{
    showMoreMode();
    emit signal_printmore();
}

void w_rpt_report_printFormat::toprint58()
{
    ui->comboBox->setCurrentIndex(print_size_58);
    toReportPrint();
}

void w_rpt_report_printFormat::toprint80()
{
    ui->comboBox->setCurrentIndex(print_size_80);
    toReportPrint();
}

void w_rpt_report_printFormat::toReportPrint()
{
    if(backheader::print_def_path_is_tty()) {
        toReportPrintIns();
    } else {
        toReportPrintImg();
    }
}

void w_rpt_report_printFormat::toReportPrintImg()
{
    QString write_byte;
    lds_query_hddpos query;


    write_byte += ui->label_operid->text() + ui->lineEdit_operid->text()+"\n";
    write_byte += ui->label_opertime->text() + ui->lineEdit_opertime->text()+"\n";


    if(ui->lineEdit_dt_begin->isVisible()) write_byte += ui->label_dt_begin->text() + ui->lineEdit_dt_begin->text() + "\n";
    if(ui->label_dt_end->isVisible()) write_byte += ui->label_dt_end->text() + ui->lineEdit_dt_end->text() + "\n";

    //printFormat
    QVector<QList<printFormat> > printline(3);
    for(int row = 0; row < standmodel->rowCount(); row ++) {
        printFormat line;
        line.width = standmodel->item(row, 2)->text().toInt();
        line.index = row;
        int sqr = standmodel->item(row, 3)->text().toInt();
        if(standmodel->item(row, 0)->text() == "Y") {
            if(sqr == 1) printline[0].append(line);
            if(sqr == 2) printline[1].append(line);
            if(sqr == 3) printline[2].append(line);
        }
    }
    //表头
    if(ui->checkBox_printtitle->isChecked()) {
        foreach(const QList<printFormat> &pf, printline) {
            foreach(const printFormat &p, pf) {
                write_byte += content_tableview->hh_paint_transfer(p.index).replace(",", "") + ",";
            }
        }
        write_byte.chop(1);
        write_byte += "\n";
    }
    //~表头

    //内容
    int printcount =5 + 1;
    switch(print_type_index) {
    case 2://打印内容、汇总、
    case 4://打印内容、汇总、
    {
        for(int row = 0, rowcount = content_tableview->verticalHeader()->count(); row < rowcount ;row++) {
            printcount-- ;
            if(ui->checkBox_printspacing_2->isChecked() && printcount <= 0) break;

            foreach(const QList<printFormat> &pf, printline) {
                foreach(const printFormat &p, pf) {
                    //
                    QString blank  = " ";
                    if(false == query.next()) {
                        blank = "";
                    }
                    //
                    write_byte += content_tableview->dg_paint_transfer(row, p.index).replace(",", "") + ",";
                }
            }
            write_byte.chop(1);
            write_byte += "\n";
            if(ui->checkBox_printspacing->isChecked()) {
                write_byte += "\n";
            }
        }
    }
        break;
    case 3://打印分页内容、汇总、
    {
        lds_query_hddpos  query;
        query.execSelect(sql);
        while(query.next()) {
            printcount-- ;
            if(ui->checkBox_printspacing_2->isChecked() && printcount <= 0) break;

            for(int index = 0; index < 2; index ++) {
                foreach(const printFormat &p, printline[index]) {
                    //
                    QString blank  = " ";
                    if(false == query.next()) {
                        blank = "";
                    }
                    query.previous();
                    //
                    write_byte += content_tableview->dg_paint_transfer(query.recordValue(p.index), QString::number(p.index)).replace(",", "") + ",";
                }
            }
            write_byte.chop(1);
            write_byte += "\n";
            if(ui->checkBox_printspacing->isChecked()) {
                write_byte += "\n";
            }
        }
    }
        break;
    default:
        break;
    }
    //～内容
    //合计
    //tail_data_pair_list
    QList<QPair<QString, QString> > tail_data_pair_list;
    if(print_type_index == 2//两个view，
            || print_type_index ==3) {//两个view，一个sql
        for(int row = 0; row < tail_tableview->verticalHeader()->count(); row ++) {
            QPair<QString, QString> data_pair;
            for(int column = 0; column < tail_tableview->horizontalHeader()->count(); column ++) {
                if(column % 2 == 0) {
                    data_pair.first = tail_tableview->dg_paint_transfer(row, column);
                }
                if(column % 2 == 1) {
                    data_pair.second = tail_tableview->dg_paint_transfer(row, column);
                    if(data_pair.second.length() > 0)
                        tail_data_pair_list.append(data_pair);
                }
            }
        }
    }
    if(print_type_index == 4) {//tableview_sum
        QPair<QString, QString> data_pair;
        for(int column = 0; column < tail_tableview->horizontalHeader()->count(); column ++) {
            if(column == 0) continue;//第一列是合计不计入

            QString d = tail_tableview->dg_paint_transfer(0, column);
            if(d.length() > 0) {
                data_pair.first = tail_tableview->hh_paint_transfer(column);
                data_pair.second = tail_tableview->dg_paint_transfer(0, column);
                tail_data_pair_list.append(data_pair);
            }
        }
    }
    //print
    if(ui->checkBox_printtotal->isChecked()) {
        int print_column_count = 1;//打印2列
        if(lds::get_soft_curent_language_first() == _CN_) {
            print_column_count = 2;//打印4列
        }
        int print_count = 0;
        for(int k = 0; k < tail_data_pair_list.count(); k++) {
            print_count = print_count % print_column_count;
            write_byte += tail_data_pair_list[k].first.replace(",", "") + "," +tail_data_pair_list[k].second.replace(",", "") + "\n";
            print_count ++;
        }
    }
#ifdef QT_DEBUG
    lds::fileWriteByte(QString("report_") + ui->lineEdit_title->text() + ".txt", write_byte.toLocal8Bit());
#endif
    QString errstring;
    if(false == backheader::reportPrint(printer, ui->lineEdit_title->text(), write_byte, errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        return;
    }
}

void w_rpt_report_printFormat::toReportPrintIns()
{
    int int_paperwidth = 32;
    lds_query_hddpos  query;

    query.execSelect(QString("SELECT int_paperwidth FROM cey_bt_kitchen_print where vch_printip = '%1' ")
                     .arg(backheader::print_def_path()));
    query.next();
    if(query.recordValue(0).toInt() != 0) {
        int_paperwidth = query.recordValue(0).toInt();
    }

    blockIns printins;
    printins.append(backheader::printer.drawText("a00,s11", ui->lineEdit_title->text() +"\n","a10,s00"));
    backheader::backheader_printer_insert_company(printins, "a10");


    printins.append(backheader::printer.drawText("", ui->label_operid->text()+ui->lineEdit_operid->text()+"\n"));
    printins.append(backheader::printer.drawText("", ui->label_opertime->text()+ui->lineEdit_opertime->text()+"\n"));

    if(ui->lineEdit_dt_begin->isVisible()) printins.append(backheader::printer.drawText("", ui->label_dt_begin->text() +  ui->lineEdit_dt_begin->text()+"\n"));
    if(ui->label_dt_end->isVisible()) printins.append(backheader::printer.drawText("", ui->label_dt_end->text() +  ui->lineEdit_dt_end->text()+"\n"));

    //printFormat
    QVector<QList<printFormat> > printline(3);
    for(int row = 0; row < standmodel->rowCount(); row ++) {
        printFormat line;
        line.width = standmodel->item(row, 2)->text().toInt();
        line.index = row;
        int sqr = standmodel->item(row, 3)->text().toInt();
        if(standmodel->item(row, 0)->text() == "Y") {
            if(sqr == 1) printline[0].append(line);
            if(sqr == 2) printline[1].append(line);
            if(sqr == 3) printline[2].append(line);
        }
    }
    //表头
    if(ui->checkBox_printtitle->isChecked()) {
        printins.append(backheader::printer.drawText("-", ""));

        foreach(const QList<printFormat> &pf, printline) {
            foreach(const printFormat &p, pf) {
                printins.append(backheader::printer.drawText("",
                                                             content_tableview->hh_paint_transfer(p.index),
                                                             p.width));
            }
            if(pf.count() > 0)
                printins.append(backheader::printer.drawText("", "\n"));
        }
        printins.append(backheader::printer.drawText("-", ""));
    }
    //~表头

    //内容
    int printcount =5 + 1;
    switch(print_type_index) {
    case 2://打印内容、汇总、
    case 4://打印内容、汇总、
    {
        for(int row = 0, rowcount = content_tableview->verticalHeader()->count(); row < rowcount ;row++) {
            printcount-- ;
            if(ui->checkBox_printspacing_2->isChecked() && printcount <= 0) break;


            foreach(const QList<printFormat> &pf, printline) {
                foreach(const printFormat &p, pf) {
                    //
                    QString blank  = " ";
                    if(false == query.next()) {
                        blank = "";
                    }
                    //
                    printins.append(backheader::printer.drawText("", content_tableview->dg_paint_transfer(row, p.index), p.width));
                }
                if(pf.count() > 0)
                    printins.append(backheader::printer.drawText("", "\n"));
            }
            if(ui->checkBox_printspacing->isChecked()) {
                printins.append(backheader::printer.drawText("", "\n"));
            }
        }
    }
        break;
    case 3://打印分页内容、汇总、
    {
        lds_query_hddpos  query;
        query.execSelect(sql);
        while(query.next()) {
            printcount-- ;
            if(ui->checkBox_printspacing_2->isChecked() && printcount <= 0) break;

            for(int index = 0; index < 2; index ++) {
                foreach(const printFormat &p, printline[index]) {
                    //
                    QString blank  = " ";
                    if(false == query.next()) {
                        blank = "";
                    }
                    query.previous();
                    //
                    printins.append(backheader::printer.drawText("", content_tableview->dg_paint_transfer(query.recordValue(p.index), QString::number(p.index)), p.width));
                }
                if(printline[index].count() > 0)
                    printins.append(backheader::printer.drawText("", "\n"));
            }
            if(ui->checkBox_printspacing->isChecked()) {
                printins.append(backheader::printer.drawText("", "\n"));
            }
        }
    }
        break;
    default:
        break;
    }
    //～内容
    //合计
    //tail_data_pair_list
    QList<QPair<QString, QString> > tail_data_pair_list;
    if(print_type_index == 2//两个view，
            || print_type_index ==3) {//两个view，一个sql
        for(int row = 0; row < tail_tableview->verticalHeader()->count(); row ++) {
            QPair<QString, QString> data_pair;
            for(int column = 0; column < tail_tableview->horizontalHeader()->count(); column ++) {
                if(column % 2 == 0) {
                    data_pair.first = tail_tableview->dg_paint_transfer(row, column);
                }
                if(column % 2 == 1) {
                    data_pair.second = tail_tableview->dg_paint_transfer(row, column);
                    if(data_pair.second.length() > 0)
                        tail_data_pair_list.append(data_pair);
                }
            }
        }
    }
    if(print_type_index == 4) {//tableview_sum
        QPair<QString, QString> data_pair;
        for(int column = 0; column < tail_tableview->horizontalHeader()->count(); column ++) {
            if(column == 0) continue;//第一列是合计不计入

            QString d = tail_tableview->dg_paint_transfer(0, column);
            if(d.length() > 0) {
                data_pair.first = tail_tableview->hh_paint_transfer(column);
                data_pair.second = tail_tableview->dg_paint_transfer(0, column);
                tail_data_pair_list.append(data_pair);
            }
        }
    }
    //print
    if(ui->checkBox_printtotal->isChecked()) {
        int print_column_count = 1;//打印2列
        if(lds::get_soft_curent_language_first() == _CN_) {
            print_column_count = 2;//打印4列
        }
        int print_column_with = int_paperwidth / print_column_count;

        printins.append(backheader::printer.drawText("-", ""));
        int print_count = 0;
        for(int k = 0; k < tail_data_pair_list.count(); k++) {
            print_count = print_count % print_column_count;

            int div = print_column_with - lds::size_g_of_u(tail_data_pair_list[k].first + tail_data_pair_list[k].second);
            printins.append(backheader::printer.drawText("", tail_data_pair_list[k].first));
            printins.append(backheader::printer.drawText("",QString(div, ' ')));
            printins.append(backheader::printer.drawText("", tail_data_pair_list[k].second));
            if(print_count == print_column_count - 1)
                printins.append(backheader::printer.drawText("", "\n"));

            print_count ++;
        }
    }
    printins.append(backheader::printer.drawText("1B6405 1D5601", ""));//切刀
    backheader::reslove_write(printins);
}

void w_rpt_report_printFormat::toexit()
{
    toPrintFormatSaveAndLoad(ui->comboBox->currentIndex());
    this->reject();
}

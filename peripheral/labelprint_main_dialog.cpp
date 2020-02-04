#include "labelprint_main_dialog.h"
#include "labelprint_set_dialog.h"
#include "labelprint_set_tagbarcode_dialog.h"
#include "labelprint_design_dialog.h"
#include <QtDebug>
#include "commonheader.h"
#include <QSqlDatabase>
#include "labelprint_goodinfo_add_dialog.h"
#include "ui_labelprint_main_dialog.h"
#include "uinormal_message.h"
#include <QSettings>
#include "lds_query_db.h"
#include "printer_pos_net.h"
#include "printer_pos_serial.h"
#include "printer_pos_vidpid.h"
#include "printer_label_postek.h"
#include "printer_label_argox_pplb.h"
#include "printer_label_argox_ppla.h"
#include "printer_label_tsc_ttp_244_plus.h"
#include "printer_label_gainscha.h"
#include "printer_label_gainscha_1224t.h"
#include "printer_label_tsc_hprt.h"
#include "lds.h"
#include "printer_label.h"
#include "ui_labelprint_goodinfo_add_dialog.h"

labelprint_main_dialog::labelprint_main_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_labelprint_main_dialog)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("标签打印机"));
    is_label_mode = false;
    setdialog=static_cast<labelprint_set_dialog*>(ui->stackedWidget->widget(1));
    gooddialog=static_cast<labelprint_goodinfo_dialog*>(ui->stackedWidget->widget(2));
    designdialog=static_cast<labelprint_design_dialog*>(ui->stackedWidget->widget(0));

    setdialog->st_dialog=designdialog->setdialog;
    designdialog->setting_printer = setdialog;

    designdialog->design->printmodel=printmodel=gooddialog->printmodel;

    connect(ui->pushButton_exit,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->radioButton_overview,SIGNAL(clicked()),this,SLOT(toswitch()));
    connect(ui->radioButton_set,SIGNAL(clicked()),this,SLOT(toswitch()));
    connect(ui->radioButton_goodinfo,SIGNAL(clicked()),this,SLOT(toswitch()));
    //    connect(ui->pushButton_print,SIGNAL(clicked()),designdialog,SLOT(toprint()));
    connect(gooddialog,SIGNAL(signal_print()),designdialog,SLOT(toprint()));
    connect(gooddialog,SIGNAL(signal_add()),this,SLOT(togoodadd()));

    this->setMaximumSize(800, 560);
    ui->radioButton_goodinfo->click();
    //此处影藏，新的按钮将会移到商品添加界面
    //    ui->pushButton_print->hide();
}

labelprint_main_dialog::~labelprint_main_dialog()
{
    delete ui;
}

void labelprint_main_dialog::inputfinished(bool label)
{
    Q_UNUSED(label)

    QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE", "labelprint_main_dialog");
    db.setDatabaseName(lds::localdataPath +"/labelprint_main_dialog.db");
    if(db.open()) {
        lds_query_db query(db);
        query.execCreate("create table if not exists labelprint_main_dialog_table ("
                         "int_id int primary key,"
                         "vch_billno char(20),"
                         "vch_ip char(20), "
                         "vch_content char(2048), "
                         "dt_date datetime NULL,"
                         "vch_type char(20)"
                         ")"
                         );
    }
    for(int c = 0, ccount = printmodel->columnCount(); c < ccount; c++)
        gooddialog->printmodel_defh_List << printmodel->headerData(c, Qt::Horizontal).toString();

    for(int index = 0, count = gooddialog->printmodel_selfh_List.count(); index < count; index ++) {
        int c = printmodel->columnCount();
        printmodel->setHorizontalHeaderItem(c, new QStandardItem(gooddialog->printmodel_selfh_List.value(index)));
        for(int row = 0, rowcount = printmodel->rowCount(); row  < rowcount; row++) {
            printmodel->setItem(row, c, new QStandardItem(gooddialog->printmodel_selfh_conttent_List.value(index)));
        }
    }

    gooddialog->isfinishinput=true;
}

void labelprint_main_dialog::toprint(QWidget *parent)
{
    designdialog->designprint(parent, false, false);
}

void labelprint_main_dialog::set_label_enabled()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->radioButton_set->click();
    is_label_mode = true;
}

void labelprint_main_dialog::goodsadd(const QList<QStringList> &itemslist)
{
    /** temslist
    1 2 3 4 5 6
    1 2 3 4 5 6
    1 2 3 4 5 6
*/
    foreach(QStringList items, itemslist) {
        QList<QStandardItem*> items0;
        for(int c=0,ccount=items.count();c<ccount;c++) {
            items0.append(new QStandardItem(items[c]));
        }
        printmodel->appendRow(items0);
    }
}

void labelprint_main_dialog::togoodadd()
{
    ///标签打印机导入的表头是什么，生成的itemlist的一行对应的就是什么
    ///重载步骤：
    /// 1调用其他窗口，生成对应的itemlist，
    /// 2调用goodsadd(itemslist);
    //1
    QList<QStringList> itemslist;
    labelprint_goodinfo_add_dialog dialog(this);
    dialog.ui->tableView->setModel(gooddialog->inputallgoods_model);

    if(QDialog::Accepted==dialog.exec()) {
        QModelIndexList indexlist=dialog.ui->tableView->selectionModel()->selectedRows();

        foreach(QModelIndex index, indexlist) {
            QStringList items;
            for(int c=0,ccount=gooddialog->inputallgoods_model->columnCount();c<ccount;c++) {
                items.append(gooddialog->inputallgoods_model->item(index.row(), c)->text());
            }
            itemslist.append(items);
        }
    }
    //2
    goodsadd(itemslist);
}

void labelprint_main_dialog::toexit()
{
    tosave();
    setdialog->tosave();
    designdialog->setdialog->tosave();
    designdialog->tosave();
    gooddialog->tosave();
    this->reject();
}

void labelprint_main_dialog::togoodremove()
{

}

void labelprint_main_dialog::tooverview()
{
    labelprint_design_dialog dialog(this);
}

void labelprint_main_dialog::toset()
{
    labelprint_set_dialog dialog(this);
}

void labelprint_main_dialog::toswitch()
{
    QRadioButton *b=qobject_cast<QRadioButton*>(sender());
    if(b==ui->radioButton_goodinfo) {
        ui->stackedWidget->setCurrentIndex(2);
    }
    if(b==ui->radioButton_overview) {
        ui->stackedWidget->setCurrentIndex(0);
        designdialog->setcurrentIndex(0);
    }
    if(b==ui->radioButton_set) {
        ui->stackedWidget->setCurrentIndex(1);
    }

    setdialog->tosave();
}

void labelprint_main_dialog::tosave()//保存自定义的内容
{
    QStandardItemModel *goodmodel = gooddialog->printmodel;
    int index = gooddialog->printmodel_defh_List.count();
    gooddialog->printmodel_selfh_conttent_List.clear();

    for(int count = goodmodel->columnCount(); index < count; index ++) {
        gooddialog->printmodel_selfh_conttent_List.append(goodmodel->index(0, index).data().toString());
    }

    lds::sysconf->setValue("labelprint_goodinfo_dialog/printmodel_selfh_conttent_List", gooddialog->printmodel_selfh_conttent_List);
    lds::sysconf->setValue("labelprint_goodinfo_dialog/printmodel_selfh_List", gooddialog->printmodel_selfh_List);
}


labelprint_main_dialog_object::labelprint_main_dialog_object(QObject *parent)
    : QObject(parent)
{
    printer_label::set_line_char_height(lds::sysconf->value("label_print/line_char_height", -1).toInt());
}

bool labelprint_main_dialog_object::printLabel(QStandardItemModel *model, bool istest, QString *errstring)
{
    SSRPairMap ssrmap;
    if(!getPrintMapFromDef(ssrmap, errstring)) {
        return false;
    }
    return printLabel(model, ssrmap, istest, errstring);
}

bool labelprint_main_dialog_object::printLabel(QStandardItemModel *model, const SSRPairMap &ssrmap, bool istest, QString *errstring)
{
    return printLabel(model, S_MainWindow::Map2Json(ssrmap), istest, errstring);
}

bool labelprint_main_dialog_object::printLabel(QStandardItemModel *model, const QString &print_json, bool istest, QString *errstring)
{

    //! 执行打印
    //!==================20
    bool istag = lds::sysconf->value(QString("label_print/istag"),true).toBool();
    float _gap = lds::sysconf->value(QString("label_print/gap"), "0").toFloat();
    float _height = lds::sysconf->value(QString("label_print/height"), "320").toFloat();
    float _width = lds::sysconf->value(QString("label_print/width"),"720").toFloat();
    int column = lds::sysconf->value(QString("label_print/column"),"2").toInt();
    int print_count = /*lds::sysconf->value(QString("label_print/printcount"), "1").toInt()*/1;
    QString printpathtype = lds::sysconf->value(QString("label_print/printpathtype"), "USB").toString();
    QString path = lds::sysconf->value(QString("label_print/printpath"), "/dev/usb/lp0").toString();
    QString baudrate = lds::sysconf->value(QString("label_print/baudrate"),"9600").toString();

    //!==================21
    if(model->rowCount() <= 0) {
        if(errstring) * errstring = QObject::tr("没有需要打印的数据");
        return false;
    }

    QString print_darkness = 0;
    //! 获取打印接口类型； 并且打开打印机
    //!================10
    Printer_POS_NET netprinter;
    Printer_POS_SERIAL serialprinter;
    Printer_POS_VIDPID vidpidprinter;
    Printer_POS *printer=0;
#ifdef Q_OS_UNIX
    if(path.contains("tty")) {
        serialprinter.psettings.BaudRate=BaudRateType(CommonHeader::creat_baudrates().key(baudrate));
        printer=&serialprinter;
    } else
#elif defined (Q_OS_WIN)
    if(path.contains("com")||path.contains("COM")) {
        serialprinter.psettings.BaudRate=BaudRateType(CommonHeader::creat_baudrates().key(baudrate));
        printer=&serialprinter;
    } else
#endif
        if(path.contains("usb")) {
            serialprinter.setType(Printer_POS_SERIAL::USB);
            printer=&serialprinter;
        } else if(path.contains("&")) {
            printer=&vidpidprinter;
        } else if(path.count(".")==3) {
            printer=&netprinter;
        }
    //!================11
    if(printer==0) {
        if(errstring) * errstring = "no printer is selected";
        return false;
    }
    if(!printer->tryOpen(path)) {
        if(errstring) * errstring = QObject::tr("端口打开失败");
        return false;
    }

    float width = valueTranslate(_width,"dots","200DPI");
    float gap = valueTranslate(_gap,"dots","200DPI");
    float height = valueTranslate(_height,"dots","200DPI");

    int piece_width = istag ? 1 : qMax(float(1.0), width / column);

    QString printtype = lds::sysconf->value(QString("label_print/printtype"), "TSC TTP-244 PLUS").toString();
    switch(CommonHeader::creat_printtypes().indexOf(printtype)) {
    case 0://POSTEK Q8/200
    {
        printer_label_postek printer0;
        QVector<QVector<QString> > vvs;
        for(int rr = 0, rr_cout = model->rowCount(),actual_row = 0; rr < rr_cout; rr++) {
            for(int actual_r = 0, actual_count = model->data(model->index(rr, 0)).toInt()
                ; actual_r < actual_count; actual_r++, actual_row++) {
                int cc=actual_row%column;
                int rrr=actual_row/column;
                if(cc==0)vvs.append(QVector<QString>(column));
                vvs[rrr][cc]= print_piece_POSTEK(model, print_json, printer0, rr,piece_width * (actual_row % column));
            }
            if(istest) break;//若是测试则打印一次后，直接跳出
        }

        foreach(QVector<QString> vs, vvs) {
            QString rowins;
            foreach(QString s, vs) {
                rowins+=s;
            }
            printer->write(printer0.toPrint(rowins, print_count,height, gap), lds::get_soft_curent_language_first());
        }
    }
        break;
    case 1://TSC TTP-244 PLUS"
    {
        width = valueTranslate(_width,"mm","200DPI");
        gap = valueTranslate(_gap,"mm","200DPI");
        height = valueTranslate(_height,"mm","200DPI");

        printer_label_tsc_ttp_244_plus printer0;
        QVector<QVector<QString> > vvs;
        for(int rr = 0, rr_cout = model->rowCount(),actual_row = 0; rr < rr_cout; rr++) {
            for(int actual_r = 0, actual_count = model->item(rr, 0)->text().toInt()
                ; actual_r < actual_count; actual_r++, actual_row++) {
                int cc=actual_row%column;
                int rrr=actual_row/column;
                if(cc==0)vvs.append(QVector<QString>(column));
                vvs[rrr][cc]= print_piece_TSC(model, print_json, printer0, rr,piece_width * (actual_row % column));

            }
            if(istest) break;//若是测试则打印一次后，直接跳出
        }
        foreach(QVector<QString> vs, vvs) {
            QString rowins;
            foreach(QString s, vs) {
                rowins+=s;
            }
#ifdef QT_DEBUG
            qDebug() << printer0.toPrint(rowins, print_count, height, gap, width), lds::get_soft_curent_language_first();
#endif
            printer->write(printer0.toPrint(rowins, print_count, height, gap, width), lds::get_soft_curent_language_first());
        }
    }
        break;
    case 9://佳博GP-1224T
    {
        width = valueTranslate(_width,"mm","200DPI");
        gap = valueTranslate(_gap,"mm","200DPI");
        height = valueTranslate(_height,"mm","200DPI");

        printer_label_tsc_ttp_244_plus printer0;
        QVector<QVector<QString> > vvs;
        for(int rr = 0, rr_cout = model->rowCount(),actual_row = 0; rr < rr_cout; rr++) {
            for(int actual_r = 0, actual_count = model->item(rr, 0)->text().toInt()
                ; actual_r < actual_count; actual_r++, actual_row++) {
                int cc=actual_row%column;
                int rrr=actual_row/column;
                if(cc==0)vvs.append(QVector<QString>(column));
                vvs[rrr][cc]= print_piece_TSC(model, print_json, printer0, rr,piece_width * (actual_row % column));
            }
            if(istest) break;//若是测试则打印一次后，直接跳出
        }
        foreach(QVector<QString> vs, vvs) {
            QString rowins;
            foreach(QString s, vs) {
                rowins+=s;
            }
            printer->write(printer0.toPrint(rowins, print_count, height, gap, width), lds::get_soft_curent_language_first());
        }
    }
        break;
    case 10://      汉印(HPRT)
    {
        width = valueTranslate(_width,"mm","200DPI");
        gap = valueTranslate(_gap,"mm","200DPI");
        height = valueTranslate(_height,"mm","200DPI");

        printer_label_tsc_HPRT printer0;
        QVector<QVector<QString> > vvs;
        for(int rr = 0, rr_cout = model->rowCount(),actual_row = 0; rr < rr_cout; rr++) {
            for(int actual_r = 0, actual_count = model->item(rr, 0)->text().toInt()
                ; actual_r < actual_count; actual_r++, actual_row++) {
                int cc=actual_row%column;
                int rrr=actual_row/column;
                if(cc==0)vvs.append(QVector<QString>(column));
                vvs[rrr][cc]= print_piece_TSC(model, print_json, printer0, rr,piece_width * (actual_row % column));
            }
            if(istest) break;//若是测试则打印一次后，直接跳出
        }
        foreach(QVector<QString> vs, vvs) {
            QString rowins;
            foreach(QString s, vs) {
                rowins+=s;
            }
            printer->write(printer0.toPrint(rowins, print_count, height, gap, width), lds::get_soft_curent_language_first());
        }
    }
        break;
    case 2://SC2100(商超专用)
    {
        /*由于flash大小有限，需要外置sd卡才可以打印中文，这儿暂时不做*/
        printer_label_argox_PPLA printer0;
        QVector<QVector<QString> > vvs;
        for(int rr = 0, rr_cout = model->rowCount(),actual_row = 0; rr < rr_cout; rr++) {
            for(int actual_r = 0, actual_count = model->item(rr, 0)->text().toInt()
                ; actual_r < actual_count; actual_r++, actual_row++) {
                int cc=actual_row%column;
                int rrr=actual_row/column;
                if(cc==0)vvs.append(QVector<QString>(column));
                vvs[rrr][cc]= print_piece_ARGOX_PPLA(model, print_json, printer0, rr,piece_width * (actual_row % column));
            }
            if(istest) break;//若是测试则打印一次后，直接跳出
        }
        QString ins;
        foreach(QVector<QString> vs, vvs) {
            QString rowins;
            foreach(QString s, vs) {
                rowins+=s;
            }
            printer->write(printer0.toPrint(rowins, print_count), lds::get_soft_curent_language_first());
        }
    }
        break;
    case 8://GODEX G500-U
        print_darkness = lds::sysconf->value("label_print/print_darkness", 8).toString();
    case 3://ARGOX CP-2140
    {
        if(print_darkness.isEmpty())
            print_darkness = "8";
        printer_label_argox_PPLB printer0;
        QVector<QVector<QString> > vvs;
        for(int rr = 0, rr_cout = model->rowCount(),actual_row = 0; rr < rr_cout; rr++) {
            for(int actual_r = 0, actual_count = model->item(rr, 0)->text().toInt()
                ; actual_r < actual_count; actual_r++, actual_row++) {
                int cc=actual_row%column;
                int rrr=actual_row/column;
                if(cc==0)vvs.append(QVector<QString>(column));
                vvs[rrr][cc]= print_piece_ARGOX_PPLB (model, print_json, printer0, rr,piece_width * (actual_row % column));
            }
            if(istest) break;//若是测试则打印一次后，直接跳出
        }
        QString ins;
        foreach(QVector<QString> vs, vvs) {
            QString rowins;
            foreach(QString s, vs) {
                rowins+=s;
            }
            printer->write(printer0.toPrint(rowins, print_count, print_darkness.toInt()), lds::get_soft_curent_language_first());
        }
    }
        break;
    case 4://佳博GP-3120TN
    case 5://佳博GP-3150TIN
    case 6://佳博GP-2120TF
    case 7://佳博GP-9034T
    {
        width = valueTranslate(_width,"mm","200DPI");
        gap = valueTranslate(_gap,"mm","200DPI");
        height = valueTranslate(_height,"mm","200DPI");

        printer_label_gainscha printer0;
        QVector<QVector<QString> > vvs;

        for(int rr = 0, rr_cout = model->rowCount(),actual_row = 0; rr < rr_cout; rr++) {
            for(int actual_r = 0, actual_count = model->item(rr, 0)->text().toInt()
                ; actual_r < actual_count; actual_r++, actual_row++) {
                int cc=actual_row%column;
                int rrr=actual_row/column;
                if(cc==0)vvs.append(QVector<QString>(column));
                vvs[rrr][cc]= print_piece_GAINSCHA (model, print_json, printer0, rr,piece_width * (actual_row % column));
            }
            if(istest) break;//若是测试则打印一次后，直接跳出
        }

        foreach(QVector<QString> vs, vvs) {
            QString rowins;
            foreach(QString s, vs) {
                rowins+=s;
            }
            printer->write(printer0.toPrint(rowins, print_count,height, gap,width), lds::get_soft_curent_language_first());
        }
    }
        break;
    default :break;
    }
    printer->close();
    return true;
}

//bool labelprint_main_dialog_object::printLabel(QStandardItemModel *model, const SSRPairMap &map, bool istest, QString *errstring)
//{

//}

QString labelprint_main_dialog_object::print_piece_POSTEK(QStandardItemModel *model, const QString &print_json, printer_label_postek &printer, int rr, int start_x)
{
    prit_piece_data d;
    if(!d.select(model, print_json, &printer, rr))
        return "";
    for(int k = 0; k < d.path.array_count("{items"); k ++) {
        switch(d.type(k)) {
        case Shape::Rectangle:
            d.ins += printer.drawText(
                        d.textCut(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  d.font
                        ,  printer_label_postek::ROTATE_0
                        ,  d.xscale(k)
                        ,  d.yscale(k)
                        );
            break;
        case Shape::TwoBarCode:
            d.ins += printer.drawTwoCode(
                        d.text(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  "QR"
                        ,  0
                        ,  0
                        ,  printer_label_postek::ROTATE_0);
            break;
        case Shape::OneBarCode:
            d.ins += printer.drawOneCode(
                        d.text(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  d.onebartype
                        ,  d.height(k)

                        ,  printer_label_postek::ROTATE_0
                        ,  d.wide_width
                        ,  d.narrow_width
                        );//ONEBARCODE
            break;
        default:break;
        }
    }

    return d.ins;
}

QString labelprint_main_dialog_object::print_piece_TSC(QStandardItemModel *model, const QString &print_json, printer_label_tsc_ttp_244_plus &printer, int rr, int start_x)
{
    prit_piece_data d;
    if(!d.select(model, print_json, &printer, rr))
        return "";
    for(int k = 0; k < d.path.array_count("{items"); k ++) {
        switch(d.type(k)) {
        case Shape::Rectangle:
            d.ins += printer.drawText(
                        d.textCut(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  d.font

                        ,  d.xyscale(k)
                        );
            break;
        case Shape::TwoBarCode:
            d.ins += printer.drawTwoCode(
                        d.text(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)

                        );
            break;
        case Shape::OneBarCode:
            d.ins += printer.drawOneCode(
                        d.text(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  d.onebartype
                        ,  d.height(k)
                        ,  printer_label_tsc_ttp_244_plus::ROTATE_0
                        ,  d.wide_width
                        ,  d.narrow_width
                        );//ONEBARCODE
            break;
        default:break;
        }
    }
    return d.ins;
}

QString labelprint_main_dialog_object::print_piece_ARGOX_PPLA(QStandardItemModel *model, const QString &print_json, printer_label_argox_PPLA &printer, int rr, int start_x)
{
    prit_piece_data d;
    if(!d.select(model, print_json, &printer, rr))
        return "";
    for(int k = 0; k < d.path.array_count("{items"); k ++) {
        switch(d.type(k)) {
        case Shape::Rectangle:
            d.ins += printer.drawText(
                        d.textCut(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  d.font

                        ,  printer_label_argox_PPLA::ROTATE_0
                        ,  d.xscale(k)
                        ,  d.yscale(k)
                        );
            break;
        case Shape::TwoBarCode:
            d.ins += printer.drawTwoCode(
                        d.text(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  "Q"
                        );
            break;
        case Shape::OneBarCode:
            d.ins += printer.drawOneCode(
                        d.text(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  d.onebartype
                        ,  d.height(k)
                        ,  printer_label_argox_PPLA::ROTATE_0
                        ,  d.wide_width
                        ,  d.narrow_width
                        );//ONEBARCODE
            break;
        default:break;
        }
    }

    return d.ins;
}

QString labelprint_main_dialog_object::print_piece_ARGOX_PPLB(QStandardItemModel *model, const QString &print_json, printer_label_argox_PPLB &printer, int rr, int start_x)
{
    prit_piece_data d;
    if(!d.select(model, print_json, &printer, rr))
        return "";
    for(int k = 0; k < d.path.array_count("{items"); k ++) {
        switch(d.type(k)) {
        case Shape::Rectangle:
            d.ins += printer.drawText(
                        d.textCut(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  d.font

                        ,  (printer_label_argox_PPLB::ROTATE)d.rotate(k)//printer_label_argox_PPLB::ROTATE_0
                        ,  d.xscale(k)
                        ,  d.yscale(k)
                        );
            break;
        case Shape::TwoBarCode:
            d.ins += printer.drawTwoCode(
                        d.text(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  "Q"
                        );
            break;
        case Shape::OneBarCode:
            d.ins += printer.drawOneCode(
                        d.text(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  d.onebartype
                        ,  d.height(k)
                        ,  (printer_label_argox_PPLB::ROTATE)d.rotate(k)//printer_label_argox_PPLB::ROTATE_0
                        ,  d.wide_width
                        ,  d.narrow_width
                        );//ONEBARCODE
            break;
        default:break;
        }
    }
    return d.ins;
}

QString labelprint_main_dialog_object::print_piece_GAINSCHA(QStandardItemModel *model, const QString &print_json, printer_label_gainscha &printer, int rr, int start_x)
{
    prit_piece_data d;
    if(!d.select(model, print_json, &printer, rr))
        return "";
    for(int k = 0; k < d.path.array_count("{items"); k ++) {
        switch(d.type(k)) {
        case Shape::Rectangle:
            d.ins += printer.drawText(
                        d.textCut(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  d.font

                        ,  d.xyscale(k)
                        );
            break;
        case Shape::TwoBarCode:
            d.ins += printer.drawTwoCode(
                        d.text(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)

                        );
            break;
        case Shape::OneBarCode:
            d.ins += printer.drawOneCode(
                        d.text(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  d.onebartype
                        ,  d.height(k)
                        ,  printer_label_gainscha::ROTATE_0
                        ,  d.wide_width
                        ,  d.narrow_width
                        );//ONEBARCODE
            break;
        default:break;
        }
    }
    qDebug() << __LINE__ << d.ins;
    return d.ins;
}

QString labelprint_main_dialog_object::print_piece_HPRT(QStandardItemModel *model, const QString &print_json, printer_label_tsc_HPRT &printer, int rr, int start_x)
{
    prit_piece_data d;
    if(!d.select(model, print_json, &printer, rr))
        return "";
    for(int k = 0; k < d.path.array_count("{items"); k ++) {
        switch(d.type(k)) {
        case Shape::Rectangle:
            d.ins += printer.drawText(
                        d.textCut(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  d.font

                        ,  d.xyscale(k)
                        );
            break;
        case Shape::TwoBarCode:
            d.ins += printer.drawTwoCode(
                        d.text(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)

                        );
            break;
        case Shape::OneBarCode:
            d.ins += printer.drawOneCode(
                        d.text(k)
                        ,  d.x(k) + start_x
                        ,  d.y(k)
                        ,  d.onebartype
                        ,  d.height(k)
                        ,  printer_label_tsc_HPRT::ROTATE_0
                        ,  d.wide_width
                        ,  d.narrow_width
                        );//ONEBARCODE
            break;
        default:break;
        }
    }
    return d.ins;
}

float labelprint_main_dialog_object::valueTranslate(float value, const QString &unit, const QString &rate)
{

    QString danwei = lds::sysconf->value("label_print/danwei", "dots").toString();
    QString fengbianlv = lds::sysconf->value("label_print/fengbianlv", "200DPI").toString();
    if(danwei.isEmpty() || fengbianlv.isEmpty()) return 0;

    float f1 = 1.0;
    if(danwei == "mm") { //to [dots 200dpi]
        f1 = 8.0;
    } else  if(danwei == "dots" && fengbianlv == "200DPI") {
        f1 = 1.0;
    } else  if(danwei == "dots" && fengbianlv == "300DPI") {
        f1 = 2.0 / 3.0;
    }

    float f2 = 1.0;
    if(unit == "mm") {//[dots 200dpi] to [unit rate]
        f2 = 1.0 / 8.0;
    } else if(unit == "dots" && rate == "200DPI") {
        f2 = 1.0;
    } else if(unit == "dots" && rate == "300DPI") {
        f2 = 3.0 / 2.0;
    }

    return value * f1 * f2;
}

SSRPairMap labelprint_main_dialog_object::ssrmapTranslate(const SSRPairMap &printMap, const QString &unit, const QString &rate, labelprint_design_dialog::DIRECT origin)
{
    SSRPairMap map = /*design->*/printMap;
    QStringList names = map.keys();
    QSize refsize(lds::sysconf->value("label_print/width", "720").toInt(),
                  lds::sysconf->value("label_print/height", "320").toInt());
    //根据配置文件获取 单位和分辨率；若是空，则返回空； 若和默认的一致，则返回原始
    QString danwei = lds::sysconf->value("label_print/danwei", "dots").toString();
    QString fengbianlv = lds::sysconf->value("label_print/fengbianlv", "200DPI").toString();

    if(!refsize.isValid()) return SSRPairMap();
    if(danwei.isEmpty() || fengbianlv.isEmpty()) return SSRPairMap();

    if(danwei == "mm") {//to dots 200dpi
        refsize *= 8;
    } else if(danwei == "dots" && fengbianlv == "200DPI") {
        refsize *= 1.0;
    } else if(danwei == "dots" && fengbianlv == "300DPI") {
        refsize *= 2.0 / 3.0;
    }

    double factor = 1.0;//[dots 200dpi] to [unit rate]
    if(unit == "mm" && rate == "200DPI") {
        factor = 1.0 / 8.0;
    } else if(unit == "mm" && rate == "300DPI") {
        factor = 1.0 / 8.0;
    } else if(unit == "dots" && rate == "200DPI") {
        factor = 1.0;
    } else if(unit == "dots" && rate == "300DPI") {
        factor = 3.0 / 2.0;
    }

    foreach(QString name, names) {
        QRect rect(map[name].value(1).toRect());
        rectDiv(rect, factor, origin, refsize);
        map[name][1] = rect;
    }
    return map;
}

QByteArray labelprint_main_dialog_object::ssrmapTranslate(const QString &print_json, const QString &unit, const QString &rate, labelprint_design_dialog::DIRECT origin)
{
    cJSON_path p(print_json.toLocal8Bit());
    if(p.isNull())
        return "";
    if(p.path_value("{rect_convert").toInt() == 0)
        return p.toByteArray(true);
    QSize refsize(lds::sysconf->value("label_print/width", "720").toInt(),
                  lds::sysconf->value("label_print/height", "320").toInt());
    //根据配置文件获取 单位和分辨率；若是空，则返回空； 若和默认的一致，则返回原始
    QString danwei = lds::sysconf->value("label_print/danwei", "dots").toString();
    QString fengbianlv = lds::sysconf->value("label_print/fengbianlv", "200DPI").toString();

    if(!refsize.isValid()) return QByteArray();
    if(danwei.isEmpty() || fengbianlv.isEmpty()) return QByteArray();

    if(danwei == "mm") {//to dots 200dpi
        refsize *= 8;
    } else if(danwei == "dots" && fengbianlv == "200DPI") {
        refsize *= 1.0;
    } else if(danwei == "dots" && fengbianlv == "300DPI") {
        refsize *= 2.0 / 3.0;
    }

    double factor = 1.0;//[dots 200dpi] to [unit rate]
    if(unit == "mm" && rate == "200DPI") {
        factor = 1.0 / 8.0;
    } else if(unit == "mm" && rate == "300DPI") {
        factor = 1.0 / 8.0;
    } else if(unit == "dots" && rate == "200DPI") {
        factor = 1.0;
    } else if(unit == "dots" && rate == "300DPI") {
        factor = 3.0 / 2.0;
    }

    for(int k = 0; k < p.array_count("{items"); k ++) {
        QRect rect(p.array_value("{items[%1{x", k).toInt(),
                   p.array_value("{items[%1{y", k).toInt(),
                   p.array_value("{items[%1{width", k).toInt(),
                   p.array_value("{items[%1{height", k).toInt()
                   );
        rectDiv(rect, factor, origin, refsize);
        p.replaceItemValue(QString("{items[%1").arg(k), "x", rect.x());
        p.replaceItemValue(QString("{items[%1").arg(k), "y", rect.y());
        p.replaceItemValue(QString("{items[%1").arg(k), "width", rect.width());
        p.replaceItemValue(QString("{items[%1").arg(k), "height", rect.height());
    }
    return p.toByteArray(true);
}

void labelprint_main_dialog_object::rectDiv(QRect &rect, double factor, labelprint_design_dialog::DIRECT origin, const QSize &p1)
{
    int x = rect.x() * factor;
    int y = rect.y() * factor;
    int w = rect.width() *factor;
    int h = rect.height() * factor;
    QSize p = p1 * factor;

    switch(origin) {
    case labelprint_design_dialog::LEFTTOP:
        x = p.width() - x;
        y = p.height() - y;
        break;
    case labelprint_design_dialog::RIGHTBOTTOM:

        break;
    }
    rect.setRect(x, y, w, h);
}

QSize labelprint_main_dialog_object::getsizeby14(const QString &size_str)
{
    int v = size_str.toInt();
    return QSize( qMax(1, v/10), qMax(1, v%10));
}

bool labelprint_main_dialog_object::getPrintMapFromDef(SSRPairMap &map, QString *errstring)
{
    return getPrintMap((lds::localdataPath +"/def.txt"), map, errstring);
}

bool labelprint_main_dialog_object::getPrintMap(const QString &filepath, SSRPairMap &map, QString *errstring)
{
    SSRPairMap ssrmap;
    QFile file(filepath);
    if(!file.open(QFile::ReadOnly)) {
        if(errstring) *errstring = file.errorString();
        map = ssrmap;
        return false;
    }

    QByteArray ba = file.readAll();
    QDataStream in(&ba, QIODevice::ReadOnly);    // read the data serialized from the file
    while(!in.atEnd()) {
        QString name;
        QVariantList d;
        in >> name >> d;
        ssrmap.insert(name, d);
    }
    file.close();

    map = ssrmap;
    return true;
}

bool labelprint_main_dialog_object::savePrintMapToDef(const SSRPairMap &map, QString *errstring)
{
    return savePrintMap(lds::localdataPath +"/def.txt",  map, errstring);
}

bool labelprint_main_dialog_object::savePrintMap(const QString &filepath, const SSRPairMap &map, QString *errstring)
{
    SSRPairMap ssrmap= map;
    QByteArray ba;
    QDataStream in(&ba, QIODevice::WriteOnly);    // read the data serialized from the file
    QStringList names = ssrmap.keys();
    foreach(QString name, names) {
        in << name << ssrmap.value(name);
    }

    QFile file(filepath);
    if(!file.open(QFile::WriteOnly)) {
        if(errstring) *errstring = file.errorString();
        return false;
    }
    file.write(ba);
    file.close();
    return true;
}

QString labelprint_main_dialog_object::textCutN(bool istext, const QString &key, QString text)
{
    text = (istext ? (key + ":") : "") + text;
    int row_count = 0;
    const int max_chars = lds::sysconf->value("label_print/line_max_chars", -1).toInt();
    int k = max_chars;
    if(max_chars <= 0 || printer_label::get_line_char_height() <= 0)
        return text;
    while(k < text.length()) {
        text.insert(k, "\n");
        k += max_chars;
        k += 1;
        row_count ++;
        if(row_count >= 3)
            break;
    }
    return text;
}


bool labelprint_main_dialog_object::prit_piece_data::select(QStandardItemModel *model, const QString &print_json, printer_label *printer, int rr)
{
    if(rr >= model->rowCount())
        return false;//当前行数超过总行数
    //获取 打印机设置属性 ； 根据属性将ssrmap的内容转换成需要打格式:[dots 、200dpi]
    path.parse(ssrmapTranslate(print_json, "dots", "200DPI", labelprint_design_dialog::LEFTTOP));
    if(path.isNull())
        return false;
    wide_width = lds::sysconf->value(QString("label_print/wide_width"),"2").toInt();
    narrow_width = lds::sysconf->value(QString("label_print/narrow_width"),"2").toInt();
    onebartype_key = lds::sysconf->value("label_print/onebartype").toString();
    font_key = lds::sysconf->value("label_print/font").toString();
    onebartype = printer->print_bar_code_map().value(onebartype_key);
    font = printer->print_text_map().value(font_key, font_key);
    for(int c = 0, cout = model->columnCount() ; c < cout; c++) {
        ssmap.insert(model->horizontalHeaderItem(c)->text(), model->data(model->index(rr, c)).toString());
    }

    qDebug() << __LINE__ << path.toByteArray(true);
    return true;
}

QString labelprint_main_dialog_object::prit_piece_data::textCut(int k)
{
    QString key = path.array_value("{items[%1{name", k).toString();
    QString text = ssmap.value(key);
    bool istext = path.array_value("{items[%1{istext", k).toInt();
    return textCutN(istext, key, text);
}

QString labelprint_main_dialog_object::prit_piece_data::text(int k)
{
    QString key = path.array_value("{items[%1{name", k).toString();
    QString text = ssmap.value(key);
    return text;
}

int labelprint_main_dialog_object::prit_piece_data::x(int k)
{
    return path.array_value("{items[%1{x", k).toInt();
}

int labelprint_main_dialog_object::prit_piece_data::y(int k)
{
    return path.array_value("{items[%1{y", k).toInt();
}

int labelprint_main_dialog_object::prit_piece_data::width(int k)
{
    return path.array_value("{items[%1{width", k).toInt();
}

int labelprint_main_dialog_object::prit_piece_data::height(int k)
{
    return path.array_value("{items[%1{height", k).toInt();
}

int labelprint_main_dialog_object::prit_piece_data::type(int k)
{
    return path.array_value("{items[%1{type", k).toInt();
}

int labelprint_main_dialog_object::prit_piece_data::xscale(int k)
{
    int f = path.array_value("{items[%1{fontsize", k).toInt();
    return qMax(1, f / 10);
}

int labelprint_main_dialog_object::prit_piece_data::yscale(int k)
{
    int f = path.array_value("{items[%1{fontsize", k).toInt();
    return qMax(1, f % 10);
}

QSize labelprint_main_dialog_object::prit_piece_data::xyscale(int k)
{
    return QSize(xscale(k), yscale(k));
}

int labelprint_main_dialog_object::prit_piece_data::rotate(int k)
{
    return path.array_value("{items[%1{rotate", k).toInt();
}

#include "labelprint_set_dialog.h"
#include <QSettings>
#include <QMessageBox>
#include "commonheader.h"
#include "printer_label_postek.h"
#include "printer_label_argox_pplb.h"
#include "printer_label_argox_ppla.h"
#include "printer_label_tsc_ttp_244_plus.h"
#include "printer_label_gainscha.h"
#include "printer_pos_serial.h"
#include "printer_0.h"
#include "uinormal_message.h"
#include "printer_pos_net.h"
#include "labelprint_set_tagbarcode_dialog.h"
#include "labelprint_set_vidpid_dialog.h"
#include "printer_pos_vidpid.h"
#include "printer_label_gainscha_1224t.h"
#include "printer_label_tsc_hprt.h"
#include "lds.h"
#include "labelprint_main_dialog.h"
#include "uinormal_message.h"
#include "ui_labelprint_set_dialog.h"

labelprint_set_dialog::labelprint_set_dialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui_labelprint_set_dialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setWindowTitle(QObject::tr("标签打印设置"));
    //标签打印机 labelprint_
    //标签打印机 labelprint_
    ui->comboBox_labelprint_printtype->addItems(CommonHeader::creat_printtypes());
    ui->comboBox_labelprint_printpathtype->addItems(CommonHeader::creat_printpathtypes());
    ui->comboBox_labelprint_baudrate->addItems(CommonHeader::creat_baudrates().values());

    update_combobox(ui->comboBox_labelprint_baudrate, lds::sysconf->value("label_print/baudrate", "9600").toString());
    update_combobox(ui->comboBox_labelprint_printtype,lds::sysconf->value("label_print/printtype", "TSC TTP-244 PLUS").toString());
    update_labelprint_onebartype(ui->comboBox_labelprint_printtype->currentText());
    update_combobox(ui->comboBox_labelprint_font, lds::sysconf->value("label_print/font", "FONT001").toString());
    ui->spinBox_line_max_chars->setValue(lds::sysconf->value("label_print/line_max_chars", -1).toInt());
    ui->spinBox_line_char_height->setValue(lds::sysconf->value("label_print/line_char_height", -1).toInt());
    update_combobox(ui->comboBox_labelprint_onebartype,lds::sysconf->value("label_print/onebartype", "Code 128 AUTO").toString());
    update_combobox(ui->comboBox_labelprint_printpathtype,lds::sysconf->value("label_print/printpathtype", "USB").toString());
    update_labelprint_path(ui->comboBox_labelprint_printpathtype->currentText());
    update_combobox(ui->comboBox_labelprint_printpath,lds::sysconf->value("label_print/printpath", "/dev/usb/lp0").toString());
    ui->spinBox_wide->setValue(lds::sysconf->value("label_print/wide_width", "2").toInt());
    ui->spinBox_narrow->setValue(lds::sysconf->value("label_print/narrow_width", "2").toInt());
    ui->spinBox_darkness->setValue(lds::sysconf->value("label_print/print_darkness", 8).toInt());

    connect(ui->comboBox_labelprint_printpathtype,SIGNAL(currentIndexChanged(QString)),this,SLOT(update_labelprint_path(QString)));
    connect(ui->comboBox_labelprint_printtype,SIGNAL(currentIndexChanged(QString)),this,SLOT(update_labelprint_onebartype(QString)));
    connect(ui->pushButton_labelprint_test,SIGNAL(clicked()),this,SLOT(test_labelprint()));
    connect(ui->pushButton_labelprint_vidpid,SIGNAL(clicked()),this,SLOT(toportbyvidpid()));
}

labelprint_set_dialog::~labelprint_set_dialog()
{
    delete ui;
}

void labelprint_set_dialog::tosave()
{
    //标价签 打印机
    lds::sysconf->setValue("label_print/baudrate", ui->comboBox_labelprint_baudrate->currentText());
    lds::sysconf->setValue("label_print/printtype", ui->comboBox_labelprint_printtype->currentText());
    lds::sysconf->setValue("label_print/font", ui->comboBox_labelprint_font->currentText());
    lds::sysconf->setValue("label_print/line_max_chars", ui->spinBox_line_max_chars->value());
    lds::sysconf->setValue("label_print/line_char_height", ui->spinBox_line_char_height->value());
    lds::sysconf->setValue("label_print/onebartype", ui->comboBox_labelprint_onebartype->currentText());
    lds::sysconf->setValue("label_print/printpathtype", ui->comboBox_labelprint_printpathtype->currentText());

    lds::sysconf->setValue("label_print/printpath", ui->comboBox_labelprint_printpath->currentText());

    lds::sysconf->setValue("label_print/wide_width", ui->spinBox_wide->value());

    lds::sysconf->setValue("label_print/narrow_width", ui->spinBox_narrow->value());

    lds::sysconf->setValue("label_print/print_darkness", ui->spinBox_darkness->value());
}

void labelprint_set_dialog::toexit()
{
}

void labelprint_set_dialog::test_labelprint()
{
    tosave();
    QString errstring;
    QStandardItemModel printmodel;
    printmodel.setHorizontalHeaderLabels(QStringList()
                                         << QObject::tr("打印次数")<< QObject::tr("单号")<< QObject::tr("价格")
                                         << "dishname" <<  "dishcode" <<QObject::tr("时间") << QObject::tr("做法") << QObject::tr("店名")
                                         << QObject::tr("数量")
                                         );
    printmodel.appendRow(QList<QStandardItem*>()
                         <<new QStandardItem("1")
                         <<new QStandardItem("")
                         <<new QStandardItem("")

                         <<new QStandardItem(ui->lineEdit_labelprint_test_text->text())
                         <<new QStandardItem(ui->lineEdit_labelprint_test_barcode->text())
                         <<new QStandardItem("")
                         <<new QStandardItem("")
                         <<new QStandardItem("")

                         <<new QStandardItem("")
                         );
    SSRPairMap ssrmap;
    ssrmap.insert("dishname", QVariantList() << 0 << QRect(20, 20, 200, 40) << "11" << false << 0);
    ssrmap.insert("dishcode",  QVariantList() << 4 << QRect(20, 20, 200, 50) << "" << false << 0);
    cJSON_path p(S_MainWindow::Map2Json(ssrmap).toLocal8Bit());
    p.replaceItemValue("", "rect_convert", 0);
    labelprint_main_dialog_object d;
    if(!d.printLabel(&printmodel, p.toByteArray(true), false, &errstring)) {
        uinormal_message::warning(this, "", errstring);
    }

//    QString baudrate = ui->comboBox_labelprint_baudrate->currentText();
//    QString printcount = "1";
//    QString printtype = ui->comboBox_labelprint_printtype->currentText();
//    QString font_key = ui->comboBox_labelprint_font->currentText();

//    QString onebartype_key = ui->comboBox_labelprint_onebartype->currentText();
//    QString printpathtype = ui->comboBox_labelprint_printpathtype->currentText();
//    QString path = ui->comboBox_labelprint_printpath->currentText();

//    int wide_width=ui->spinBox_wide->value();
//    int narrow_width=ui->spinBox_narrow->value();
//    //! 获取打印接口类型； 并且打开打印机
//    //!================10
//    Printer_POS_NET netprinter;
//    Printer_POS_SERIAL serialprinter;
//    Printer_POS_VIDPID vidpidprinter;
//    Printer_POS *printer=0;

//#ifdef Q_OS_UNIX
//    if(path.contains("tty")) {
//        serialprinter.psettings.BaudRate=BaudRateType(CommonHeader::creat_baudrates().key(baudrate));
//        printer=&serialprinter;
//    } else
//#elif defined (Q_OS_WIN)
//    if(path.contains("com")||path.contains("COM")) {
//        serialprinter.psettings.BaudRate=BaudRateType(CommonHeader::creat_baudrates().key(baudrate));
//        printer=&serialprinter;
//    } else
//#endif
//        if(path.contains("usb")) {
//            serialprinter.setType(Printer_POS_SERIAL::USB);
//            printer=&serialprinter;
//        } else if(path.contains("&")) {
//            printer=&vidpidprinter;
//        } else if(path.count(".")==3) {
//            printer=&netprinter;
//        }
//    //!================11
//    if(printer==0) {
//        uinormal_message::warning(this, QObject::tr("标签打印机"), "no printer is selected");
//        return;
//    }
//    if(!printer->tryOpen(path)) {
//        uinormal_message::warning(this, QObject::tr("标签打印机"), printer->lastError());
//        return;
//    }


//    //! 开始执行
//    QString test_text = ui->lineEdit_labelprint_test_text->text();
//    QString test_barcode = ui->lineEdit_labelprint_test_barcode->text();
//    switch(CommonHeader::creat_printtypes().indexOf(printtype)) {
//    case 0://! POSTEK Q8/200 测试通过 字体随意 串口成功9600
//    {
//        printer_label_postek printer0;
//        QString onebartype = printer0.print_bar_code_map().value(onebartype_key);
//        QString font = printer0.print_text_map().value(font_key);
//        if(font.isEmpty()) font = font_key;//若是没有在原字体里找到，则是新定义字体

//        QString ins;
//        ins += printer0.drawText(test_text, 20, 20,font,printer_label_postek::ROTATE_0);
//        ins += printer0.drawOneCode(test_barcode,20,100,onebartype,20, printer_label_postek::ROTATE_0, wide_width, narrow_width);
//        printer->writedirect(lds::u2local(printer0.toPrint(ins, printcount.toInt(),248,40)));
//    }
//        break;
//    case 1://! TSC TTP-244 PLUS 测试通过 字体FONT001 串口成功9600  提示： toprint的高度不能为0，会认为时无限大
//    {
//        printer_label_tsc_ttp_244_plus printer0;
//        QString onebartype = printer0.print_bar_code_map().value(onebartype_key);
//        QString font = printer0.print_text_map().value(font_key);
//        if(font.isEmpty()) font = font_key;//若是没有在原字体里找到，则是新定义字体

//        QString ins;
//        ins += printer0.drawText(test_text, 20, 20,font);
//        ins += printer0.drawOneCode(test_barcode,20,100,onebartype,41, printer_label_tsc_ttp_244_plus::ROTATE_0, wide_width, narrow_width);

//        printer->writedirect(lds::u2local(printer0.toPrint(ins, printcount.toInt(),30,0,40)));
//    }
//        break;
//    case 9://佳博GP-1224T
//    {
//        printer_label_gainscha_1224t printer0;
//        QString onebartype = printer0.print_bar_code_map().value(onebartype_key);
//        QString font = printer0.print_text_map().value(font_key);
//        if(font.isEmpty()) font = font_key;//若是没有在原字体里找到，则是新定义字体

//        QString ins;
//        ins += printer0.drawText(test_text, 20, 20,font);
//        ins += printer0.drawOneCode(test_barcode,20,100,onebartype,41, printer_label_gainscha_1224t::ROTATE_0, wide_width, narrow_width);

//        printer->writedirect(lds::u2local(printer0.toPrint(ins, printcount.toInt(),30,0,40)));
//    }
//        break;
//    case 10://汉印(HPRT)
//    {
//        printer_label_tsc_HPRT printer0;
//        QString onebartype = printer0.print_bar_code_map().value(onebartype_key);
//        QString font = printer0.print_text_map().value(font_key);
//        if(font.isEmpty()) font = font_key;//若是没有在原字体里找到，则是新定义字体

//        QString ins;
//        ins += printer0.drawText(test_text, 20, 20,font);
//        ins += printer0.drawOneCode(test_barcode,20,100,onebartype,41, printer_label_gainscha_1224t::ROTATE_0, wide_width, narrow_width);

//        printer->writedirect(lds::u2local(printer0.toPrint(ins, printcount.toInt(),30,0,40)));
//    }
//        break;
//    case 2://! SC2100(商超专用)测试通过 无串口
//    {
//        /*由于flash大小有限，需要外置sd卡才可以打印中文，这儿暂时不做*/
//        printer_label_argox_PPLA printer0;
//        QString onebartype = printer0.print_bar_code_map().value(onebartype_key);
//        QString font = printer0.print_text_map().value(font_key);
//        if(font.isEmpty()) font = font_key;//若是没有在原字体里找到，则是新定义字体

//        QString ins;
//        ins += printer0.drawText(test_text, 20, 20,font);
//        ins += printer0.drawOneCode(test_barcode,20,100,onebartype,41, printer_label_argox_PPLA::ROTATE_0, wide_width, narrow_width);

//        printer->writedirect(lds::u2local(printer0.toPrint(ins, printcount.toInt())));
//    }
//        break;
//    case 3://! ARGOX CP-2140 字体966 串口打印失败 9600 无效
//    case 8://GODEX G500-U
//    {
//        printer_label_argox_PPLB printer0;
//        QString onebartype =printer0.print_bar_code_map().value(onebartype_key);
//        QString font = printer0.print_text_map().value(font_key);
//        if(font.isEmpty()) font = font_key;//若是没有在原字体里找到，则是新定义字体

//        QString ins;
//        ins += printer0.drawText(test_text, 20, 20,font);
//        ins += printer0.drawOneCode(test_barcode,20,100,onebartype,41, printer_label_argox_PPLB::ROTATE_0, wide_width, narrow_width);

//        if(ui->spinBox_darkness->isHidden()) {
//            printer->writedirect(lds::u2local(printer0.toPrint(ins, printcount.toInt())));
//        } else {
//            printer->writedirect(lds::u2local(printer0.toPrint(ins, printcount.toInt(), ui->spinBox_darkness->value())));
//        }

//    }
//        break;
//    case 4://!  佳博GP-3120TN 测试通过 字体FONT001 串口成功9600  提示： toprint的高度不能为0，会认为时无限大
//    case 5:// 佳博GP-3150TIN
//    case 6://佳博GP-2120TF
//    case 7://佳博GP-9034T
//    {
//        printer_label_gainscha printer0;
//        QString onebartype = printer0.print_bar_code_map().value(onebartype_key);
//        QString font = printer0.print_text_map().value(font_key);
//        if(font.isEmpty()) font = font_key;//若是没有在原字体里找到，则是新定义字体

//        QString ins;
//        QString height="30";
//        ins += printer0.drawText(test_text, 20, 20,font);
//        ins += printer0.drawOneCode(test_barcode,20,100,onebartype);
//        ins += printer0.drawOneCode(test_barcode,20,100,onebartype,41, printer_label_gainscha::ROTATE_0, wide_width, narrow_width);

//        ins = printer0.toPrint(ins, printcount.toInt(),height.toFloat(),0,40);
//        printer->writedirect(lds::u2local(ins));
//    }

//        break;
//    }

//    printer->close();
}

void labelprint_set_dialog::update_labelprint_path(const QString &pathtype)
{
    ui->comboBox_labelprint_baudrate->setEnabled(false);
    switch(CommonHeader::creat_printpathtypes().indexOf(pathtype)) {//swtich
    case 0: //usb
        ui->comboBox_labelprint_printpath->clear();
        ui->comboBox_labelprint_printpath->addItems(printer_0::f_scan_usbPort());
        break;
    case 1: //串口
        ui->comboBox_labelprint_printpath->clear();
        ui->comboBox_labelprint_printpath->addItems(printer_0::f_scan_ttyPort());
        ui->comboBox_labelprint_baudrate->setEnabled(true);
        break;
    case 2: //网口
    {
        QAbstractItemModel *model=ui->comboBox_labelprint_printpathtype->model();
        if(model->rowCount()>=3
                && qvariant_cast<QColor>(model->data(model->index(2, 0), Qt::TextColorRole))==QColor("red")) {
            uinormal_message::warning(this, QObject::tr("标签打印机"),  QObject::tr("不支持网口打印"));
            ui->comboBox_labelprint_printpathtype->setCurrentIndex(1);//串口
        }
    }
    }//!swtich
}
void labelprint_set_dialog::update_labelprint_onebartype(const QString &printtype)
{
    //目前只有科诚的可以设置
    ui->label_darkness->hide();
    ui->spinBox_darkness->hide();

    update_combobox_textcolor(ui->comboBox_labelprint_printpathtype, QColor("red"));
    switch(CommonHeader::creat_printtypes().indexOf(printtype)) {
    case 0://POSTEK Q8/200
        ui->comboBox_labelprint_onebartype->clear();
        ui->comboBox_labelprint_onebartype->addItems(printer_label_postek().print_bar_code_map().keys());
        ui->comboBox_labelprint_font->clear();
        ui->comboBox_labelprint_font->addItems(printer_label_postek().print_text_map().keys());
        break;
    case 1://TSC TTP-244 PLUS"
        ui->comboBox_labelprint_onebartype->clear();
        ui->comboBox_labelprint_onebartype->addItems(printer_label_tsc_ttp_244_plus().print_bar_code_map().keys());
        ui->comboBox_labelprint_font->clear();
        ui->comboBox_labelprint_font->addItems(printer_label_tsc_ttp_244_plus().print_text_map().keys());
        break;
    case 9://佳博GP-1224T
    {
        ui->comboBox_labelprint_onebartype->clear();
        ui->comboBox_labelprint_onebartype->addItems(printer_label_gainscha_1224t().print_bar_code_map().keys());
        ui->comboBox_labelprint_font->clear();

        ui->comboBox_labelprint_font->addItems(printer_label_gainscha_1224t().print_text_map().keys());
    }
        break;

    case 10://汉印(HPRT)
    {
        ui->comboBox_labelprint_onebartype->clear();
        ui->comboBox_labelprint_onebartype->addItems(printer_label_tsc_HPRT().print_bar_code_map().keys());
        ui->comboBox_labelprint_font->clear();

        ui->comboBox_labelprint_font->addItems(printer_label_tsc_HPRT().print_text_map().keys());
    }
        break;
    case 2://SC2100(商超专用)
        ui->comboBox_labelprint_onebartype->clear();
        ui->comboBox_labelprint_onebartype->addItems(printer_label_argox_PPLA().print_bar_code_map().keys());
        ui->comboBox_labelprint_font->clear();
        ui->comboBox_labelprint_font->addItems(printer_label_argox_PPLA().print_text_map().keys());
        break;
    case 8://GODEX G500-U
        //目前只有科诚的可以设置
        ui->label_darkness->show();
        ui->spinBox_darkness->show();
    case 3://ARGOX CP-2140
        ui->comboBox_labelprint_onebartype->clear();
        ui->comboBox_labelprint_onebartype->addItems(printer_label_argox_PPLB().print_bar_code_map().keys());
        ui->comboBox_labelprint_font->clear();
        ui->comboBox_labelprint_font->addItems(printer_label_argox_PPLB().print_text_map().keys());
        break;
    case 5://佳博GP-3150TIN可以用网口打印
        update_combobox_textcolor(ui->comboBox_labelprint_printpathtype, QColor("green"));
    case 4://佳博GP-3120TN
    case 6://佳博GP-2120TF
    case 7://佳博GP-9034T
        ui->comboBox_labelprint_onebartype->clear();
        ui->comboBox_labelprint_onebartype->addItems(printer_label_gainscha().print_bar_code_map().keys());
        ui->comboBox_labelprint_font->clear();
        ui->comboBox_labelprint_font->addItems(printer_label_gainscha().print_text_map().keys());
        break;
    }
    update_labelprint_path(ui->comboBox_labelprint_printpathtype->currentText());
}

void labelprint_set_dialog::update_combobox_textcolor(QComboBox *com, QColor color)
{
    QAbstractItemModel *model=com->model();
    if(model->rowCount()>=3)model->setData(model->index(2,0), color, Qt::TextColorRole);
}

void labelprint_set_dialog::toportbyvidpid()
{
    labelprint_set_vidpid_dialog dialog(this);
    dialog.setWindowTitle(QObject::tr("提示"));
    if(QDialog::Accepted == dialog.exec()) {
        ui->comboBox_labelprint_printpath->setEditText(dialog.ret_value);
    }
}

void labelprint_set_dialog::update_combobox(QComboBox *com, const QString &index)
{
    if(com->isEditable()) {
        com->setEditText(index);
    } else {
        int sqr;
        if((sqr = com->findText(index)) == -1)    com->setCurrentIndex(0);
        else    com->setCurrentIndex(sqr);
    }
}

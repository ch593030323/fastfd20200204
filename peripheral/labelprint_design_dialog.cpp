#include "labelprint_design_dialog.h"
#include "ui_labelprint_design_dialog.h"
#include "ui_labelprint_design_save_dialog.h"
#include "commonheader.h"
#include <QDir>
#include<QMessageBox>
#include "ui_labelprint_design_import_dialog.h"
#include "commonheader.h"
#include<QSettings>

#include "printer_label_postek.h"
#include "printer_label_argox_pplb.h"
#include "printer_label_argox_ppla.h"
#include "printer_label_tsc_ttp_244_plus.h"
#include "printer_label_gainscha.h"
#include "printer_label_gainscha_1224t.h"
#include "printer_pos_serial.h"
#include "uinormal_message.h"
#include "printer_pos_net.h"
#include "s_mainwindow.h"
#include "labelprint_design_import_dialog.h"
#include "printer_pos_vidpid.h"
#include <QTimer>
#include <QTableView>
#include "labelprint_set_dialog.h"
#include "labelprint_main_dialog.h"
#include "lds.h"


labelprint_design_dialog::labelprint_design_dialog(QWidget *parent) :
    QWidget(parent),
    setting_printer(0)
{

    ui = new Ui::labelprint_design_dialog;
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setWindowTitle(QObject::tr("设计"));
    design=static_cast<S_MainWindow*>(ui->widget);// stackedWidget->widget(0));
    setdialog=static_cast<labelprint_set_tagbarcode_dialog*>(ui->stackedWidget->widget(1));

    ui->stackedWidget->setCurrentIndex(0);
    ui->pushButton_switch->setText("-->" + QObject::tr("预览设置"));

    connect(ui->pushButton_switch,SIGNAL(clicked()),this,SLOT(toswitch()));
    connect(ui->pushButton_defbill,SIGNAL(clicked()),this,SLOT(todefbill()));
    connect(ui->pushButton_importbill,SIGNAL(clicked()),this,SLOT(toimportbill()));
    connect(ui->pushButton_savebill,SIGNAL(clicked()),this,SLOT(tosavebill()));
    connect(ui->pushButton_test,SIGNAL(clicked()),this,SLOT(totestprint()));
    toload();

    ui->pushButton_switch->click();
}

labelprint_design_dialog::~labelprint_design_dialog()
{
    delete ui;
}

void labelprint_design_dialog::tosave()
{
    QString errstring;
    if(!labelprint_main_dialog_object::savePrintMap(lds::localdataPath +"/def.txt", design->printMap(), &errstring)) {
        uinormal_message::warning(this, QObject::tr("提示"),  errstring);
        return;
    }
}

void labelprint_design_dialog::toload()
{
    SSRPairMap ssrmap;
    QString errstring;
    if(!labelprint_main_dialog_object::getPrintMap(lds::localdataPath +"/def.txt", ssrmap, &errstring)) {
        uinormal_message::warning(this, QObject::tr("提示"),  errstring);
        return;
    }
    design->initDocument(ssrmap);
}

void labelprint_design_dialog::designprint(QWidget *parent, bool istest, bool pop)
{
    if(setting_printer)setting_printer->tosave();
    designdialog_save();//设计预览的保存和条码宽度间隔的保存
    //
    QString errstring;
    labelprint_main_dialog_object print_help;
    if(!print_help.printLabel(design->printmodel, design->printMap(), istest, &errstring)) {
        if(pop)uinormal_message::warning(parent, QObject::tr("标签打印机"), errstring);
        return;
    }
    if(pop)uinormal_message::warning(parent, QObject::tr("标签打印机"),QObject::tr("打印成功"));

}

void labelprint_design_dialog::setcurrentIndex(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

void labelprint_design_dialog::toswitch()
{
    switch(ui->stackedWidget->currentIndex()) {
    case 0:{
        ui->stackedWidget->setCurrentIndex(1);
        ui->pushButton_switch->setText("<--" + QObject::tr("预览"));
    }
        break;
    case 1:
        ui->stackedWidget->setCurrentIndex(0);
        ui->pushButton_switch->setText("-->" + QObject::tr("预览设置"));

        //需要[保存]然后重新[导出]
        designdialog_save();
        break;
    }
}

void labelprint_design_dialog::todefbill()
{
    SSRPairMap restore_ssrmap;
    restore_ssrmap.insert(QObject::tr("品名"),QVariantList() << (int)Shape::Rectangle << QRect(600,300,200,30) << QString("1"));//
    restore_ssrmap.insert(QObject::tr("条码"),QVariantList() << (int)Shape::OneBarCode << QRect(600,250,200,30) << QString("1"));
    restore_ssrmap.insert(QObject::tr("价格"),QVariantList() << (int)Shape::Rectangle << QRect(600,200,200,30) << QString("1"));
    design->initDocument(restore_ssrmap);
}

void labelprint_design_dialog::toimportbill()
{
    labelprint_design_import_dialog dialog(this);
    dialog.updatecom(lds::localdataPath + "/labelprinterform");

    //! ========================
    if(QDialog::Accepted != dialog.exec()) return;

    if(dialog.getfilename().isEmpty()) {
        uinormal_message::warning(this, QObject::tr("标签打印机"), QObject::tr("没有找到文件"));
        return;
    }
    QString errstring;
    SSRPairMap ssrmap;
    if(!labelprint_main_dialog_object::getPrintMap(dialog.getfilename(), ssrmap, &errstring)) {
        uinormal_message::warning(this, QObject::tr("标签打印机"), errstring);
        return;
    }
    design->initDocument(ssrmap);
}

void labelprint_design_dialog::tosavebill()
{
    Ui_labelprint_design_save_dialog sui;
    QDialog dialog(this);
    dialog.setWindowTitle(QObject::tr("保存表单"));
    sui.setupUi(&dialog);
    dialog.setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    //! ========================
    if(QDialog::Accepted!=dialog.exec()) return;

    if(sui.lineEdit_filename->text().isEmpty()) {
        uinormal_message::warning(this, QObject::tr("标签打印机"), QObject::tr("文件名为空"));
        return;
    }

    QString path = lds::localdataPath + "/labelprinterform";
    QDir().mkpath(path);

    QString formname=path+"/"+sui.lineEdit_filename->text() + ".txt";
    QStringList filenams;
    foreach(const QFileInfo &info, QDir(path).entryInfoList(QDir::Files)) {
        filenams.append(info.fileName());
    }

    if( (filenams.contains(sui.lineEdit_filename->text()) && uinormal_message::warning(this, QObject::tr("标签打印机"), QObject::tr("有同名文件,是否覆盖?"), QObject::tr("确定"), QObject::tr("取消"))==0)
            || !filenams.contains(sui.lineEdit_filename->text())
            ) {
        QString errstring;
        if(!labelprint_main_dialog_object::savePrintMap(formname, design->printMap(), &errstring)) {
            uinormal_message::warning(this, QObject::tr("标签打印机"), errstring);
            return;
        }
        uinormal_message::warning(this, QObject::tr("标签打印机"), QObject::tr("保存成功"));
    }
}

void labelprint_design_dialog::totestprint()
{
    toprint(true);
}

void labelprint_design_dialog::toprint(bool istest, bool pop)
{
    designprint(this, istest, pop);
}

QString labelprint_design_dialog::print_piece_POSTEK(printer_label_postek &printer, int rr, int start_x)
{
    return labelprint_main_dialog_object::print_piece_POSTEK(design->printmodel, design->printMapJson(), printer, rr, start_x);
}

QString labelprint_design_dialog::print_piece_TSC(printer_label_tsc_ttp_244_plus &printer, int rr, int start_x)
{
    return labelprint_main_dialog_object::print_piece_TSC(design->printmodel, design->printMapJson(), printer, rr, start_x);
}

QString labelprint_design_dialog::print_piece_ARGOX_PPLA(printer_label_argox_PPLA &printer, int rr, int start_x)
{
    return labelprint_main_dialog_object::print_piece_ARGOX_PPLA(design->printmodel, design->printMapJson(), printer, rr, start_x);
}

QString labelprint_design_dialog::print_piece_ARGOX_PPLB(printer_label_argox_PPLB &printer, int rr, int start_x)
{
    return labelprint_main_dialog_object::print_piece_ARGOX_PPLB(design->printmodel, design->printMapJson(), printer, rr, start_x);
}

QString labelprint_design_dialog::print_piece_GAINSCHA(printer_label_gainscha &printer, int rr, int start_x)
{
    return labelprint_main_dialog_object::print_piece_GAINSCHA(design->printmodel, design->printMapJson(), printer, rr, start_x);
}

QSize labelprint_design_dialog::getsizeby14(const QString &size_str)
{
    return labelprint_main_dialog_object::getsizeby14(size_str);
}

float labelprint_design_dialog::valueTranslate(float value, const QString &unit, const QString &rate)
{
    return labelprint_main_dialog_object::valueTranslate(value, unit, rate);
}

//SSRPairMap labelprint_design_dialog::ssrmapTranslate(const QString &unit, const QString &rate, DIRECT origin)
//{
//    return labelprint_main_dialog_object::ssrmapTranslate(design->printMap(), unit, rate, origin);
//}

void labelprint_design_dialog::rectDiv(QRect &rect, double factor, DIRECT origin, const QSize &p1)
{
    return labelprint_main_dialog_object::rectDiv(rect, factor, origin, p1);
}

void labelprint_design_dialog::designdialog_save()
{
    setdialog->tosave();
    design->initDocument(design->printMap());
}

void labelprint_design_dialog::on_pushButton_importbill_clicked()
{

}

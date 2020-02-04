#include "w_bt_dish_kitchenprinter_printerset.h"
#include "ui_w_bt_dish_kitchenprinter_printerset.h"
#include "commonheader.h"
#include "printer_0.h"
#include "printer_normal_t2us.h"
#include "lds_messagebox.h"
#include "lds_query_hddpos.h"
#include "backheader.h"
#include "lds_pushbuttonlock.h"
#include "lds_tableview.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "printer_pos_vidpid.h"
#include "public_printby_ch_billno.h"
#include "n_func.h"
#include "frontviewdialog_virtual.h"
#include "w_bt_dish_kitchenprinter_printerset_port.h"
#include "w_bt_dish_kitchenprinter_printermanager_tool.h"
#include "bmp_info.h"

w_bt_dish_Kitchenprinter_printerSet0::w_bt_dish_Kitchenprinter_printerSet0(QWidget *parent)
    : QDialog(parent)
{
    ui = new Ui_w_bt_dish_Kitchenprinter_printerSet;
    ui->setupUi(this);
    ui->label_tip->setText(QObject::tr("厨打时属于该区域的打印机才会厨打") + ";" + QObject::tr("图片打印时建议使用lp0等端口"));
    ch_kitchen_view = "N";
    //1
    {
        ui->comboBox_paperwidth->addItem("80mm", 48);
        ui->comboBox_paperwidth->addItem("58mm", 32);
    }
    //2
    ui->comboBox_printtype->addItemsByMap(Printer_Normal::get_normal_print_map_ref());
    ui->comboBox_instype->addItemsByMap(Printer_Normal::get_normal_ins_map_ref());
    print_help = new w_bt_dish_kitchenprinter_printerset_port(this);
    print_help->addControl(ui->frame);

    connect(ui->comboBox_printtype,SIGNAL(currentIndexChanged(int)),this,SLOT(toprinttype(int)));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_exit,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_test,SIGNAL(clicked()),this,SLOT(totest()));
    connect(ui->comboBox_ch_areano, SIGNAL(currentIndexChanged(int)),this,SLOT(toSaaSCheck(int)));
    connect(ui->pushButton_tool, SIGNAL(clicked()),this,SLOT(totool()));

    ui->lineEdit_no->setEnabled(false);
    lds::hideWidget(ui->pushButton_tool);
}

w_bt_dish_Kitchenprinter_printerSet0::~w_bt_dish_Kitchenprinter_printerSet0()
{
    delete ui;
}

void w_bt_dish_Kitchenprinter_printerSet0::initData(const QString &no, const QString &name, const QString &ch_kitchen_view, const QString &printtype, int instype, const QString &port, int paperwidth, const QString &ch_areano, int xinye_beep)
{
    this->ch_kitchen_view = ch_kitchen_view;
    setPrinterport(port);
    if(!no.isEmpty())ui->lineEdit_no->setText(no);
    if(!name.isEmpty())ui->lineEdit_name->setText(name);
    ui->comboBox_printtype->setCurrentIndex(ui->comboBox_printtype->findData(printtype));
    if(ui->comboBox_printtype->currentIndex() == -1) {
        ui->comboBox_printtype->setCurrentIndex(0);
    }
    ui->comboBox_instype->setCurrentIndex(ui->comboBox_instype->findData(instype));
    if(paperwidth != 0) {//exist
        ui->comboBox_paperwidth->setEditText(QString::number(paperwidth));//def
        for(int row = 0, rowcount = ui->comboBox_paperwidth->count(); row < rowcount ; row++) {//find
            if(ui->comboBox_paperwidth->itemData(row).toInt() == paperwidth) {
                ui->comboBox_paperwidth->setCurrentIndex(row);
                break;
            }
        }
    }

    ui->comboBox_ch_areano->setCurrentIndex(ui->comboBox_ch_areano->findData(ch_areano));
    ui->spinBox_XINYE_beep->setValue(xinye_beep);

    tochkitchenview(ch_kitchen_view == "Y");
    if(ch_kitchen_view == "Y" )
        print_help->onlyNetPort();
}

QString w_bt_dish_Kitchenprinter_printerSet0::getno()
{
    return ui->lineEdit_no->text();
}

QString w_bt_dish_Kitchenprinter_printerSet0::getname()
{
    return ui->lineEdit_name->text();
}

QString w_bt_dish_Kitchenprinter_printerSet0::getprinttype()
{
    return ui->comboBox_printtype->itemData(ui->comboBox_printtype->currentIndex()).toString();
}

int w_bt_dish_Kitchenprinter_printerSet0::getinstype()
{
    return ui->comboBox_instype->itemData(ui->comboBox_instype->currentIndex()).toInt();
}

QString w_bt_dish_Kitchenprinter_printerSet0::getport()
{
    return port;
}

int w_bt_dish_Kitchenprinter_printerSet0::getpaperwidth()
{
    if(ui->comboBox_paperwidth->findText(ui->comboBox_paperwidth->currentText()) < 0) {//not find
        return ui->comboBox_paperwidth->currentText().toInt();
    }
    //find
    return ui->comboBox_paperwidth->itemData(ui->comboBox_paperwidth->currentIndex()).toInt();
}

QString w_bt_dish_Kitchenprinter_printerSet0::get_ch_areano()
{
    return ui->comboBox_ch_areano->itemData(ui->comboBox_ch_areano->currentIndex()).toString();
}

int w_bt_dish_Kitchenprinter_printerSet0::get_xinye_beep()
{
    return ui->spinBox_XINYE_beep->value();
}

QString w_bt_dish_Kitchenprinter_printerSet0::get_ch_kitchen_view()
{
    return ch_kitchen_view;
}

QString w_bt_dish_Kitchenprinter_printerSet0::printerport()
{
    return print_help->printerport();
}

void w_bt_dish_Kitchenprinter_printerSet0::setPrinterport(const QString &port)
{
    print_help->setPrinterport(port);
}

void w_bt_dish_Kitchenprinter_printerSet0::toprinttype(int index)
{
    QString print_class_name = ui->comboBox_printtype->itemData(index).toString();
    print_help->refreshPort();
    print_help->set_com_usb_enable(true);

    ui->label_XINYE_beep->hide();
    ui->spinBox_XINYE_beep->hide();
    ui->label_XINYE_desc->hide();
    if("Printer_Normal_T2US" == print_class_name) {
        print_help->setPrinterport(Printer_Normal_T2US().vidpid);
        print_help->set_com_usb_enable(false);
    }
    if("Printer_Normal_XINYE" == print_class_name) {
        ui->label_XINYE_beep->show();
        ui->spinBox_XINYE_beep->show();
        ui->label_XINYE_desc->show();
    }
}

void w_bt_dish_Kitchenprinter_printerSet0::took()
{
    port =printerport();
    if(trysave(getno(), getname(), getprinttype(),getinstype(),getport())) {
        this->accept();
    }
}

bool w_bt_dish_Kitchenprinter_printerSet0::trysave(const QString &int_id, const QString &vch_printname, const QString & vch_printtype, int vch_instype, const QString &vch_printip)
{
    if(int_id.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("打印机编号不能为空"));
        return false;
    }
    if(vch_printname.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("打印机名字不能为空"));
        return false;
    }
    if(vch_printtype.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("打印机类型为空"));
        return false;
    }
    if(vch_instype < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("打印类型为空"));
        return false;
    }
    if(vch_printip.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("打印端口为空"));
        return false;
    }
    return true;
}

void w_bt_dish_Kitchenprinter_printerSet0::toexit()
{
    this->reject();
}

void w_bt_dish_Kitchenprinter_printerSet0::totest()
{
    lds_pushbuttonlock b(qobject_cast<QWidget *>(this->sender()));
    b.toLock();
    backheader::syn_dataprin_test_byresolveself(this, ch_kitchen_view, printerport(), getinstype(), getprinttype(),getpaperwidth(), get_xinye_beep());
}

void w_bt_dish_Kitchenprinter_printerSet0::tonextpage()
{

}

void w_bt_dish_Kitchenprinter_printerSet0::touppage()
{

}

void w_bt_dish_Kitchenprinter_printerSet0::tochkitchenview(bool checked)
{
    lds::hideWidget(ui->label_3, checked);// && lds::widgetIsShow(ui->label_3));
    lds::hideWidget(ui->comboBox_printtype, checked);// && lds::widgetIsShow(ui->comboBox_printtype));
    lds::hideWidget(ui->label_5, checked);// && lds::widgetIsShow(ui->label_5));
    lds::hideWidget(ui->comboBox_instype, checked);// && lds::widgetIsShow(ui->comboBox_instype));
    lds::hideWidget(ui->label_7, checked);// && lds::widgetIsShow(ui->label_7));
    lds::hideWidget(ui->comboBox_paperwidth, checked);// && lds::widgetIsShow(ui->comboBox_paperwidth));
    lds::hideWidget(ui->label_XINYE_beep, checked);// && lds::widgetIsShow(ui->label_XINYE_beep));
    lds::hideWidget(ui->spinBox_XINYE_beep, checked);// && lds::widgetIsShow(ui->spinBox_XINYE_beep));
    lds::hideWidget(ui->label_XINYE_desc, checked);// && lds::widgetIsShow(ui->label_XINYE_desc));
    if(false == checked) {
        toprinttype(ui->comboBox_printtype->currentIndex());
    }
}

void w_bt_dish_Kitchenprinter_printerSet0::totool()
{
    w_bt_dish_Kitchenprinter_printermanager_tool dialog(printerport(), this);

    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {

    }
}

void w_bt_dish_Kitchenprinter_printerSet0::toSaaSCheck(int index)
{
}




w_bt_dish_Kitchenprinter_printerSet::w_bt_dish_Kitchenprinter_printerSet(QWidget *parent) :
    w_bt_dish_Kitchenprinter_printerSet0(parent)
{
    ui->comboBox_ch_areano->addItem("", "");
    ui->comboBox_ch_areano->addItemsBySql(" SELECT ch_areaNo, vch_areaname FROM cey_bt_table_area ");
}

void w_bt_dish_Kitchenprinter_printerSet::toSaaSCheck(int index)
{
    if(!ui->comboBox_ch_areano->curusrdata().toString().isEmpty()) {
        if(!saas_pay_detail::SaaSFuncCheck(this, "0509"/*区域厨打*/)) {
            ui->comboBox_ch_areano->setcomusrdata("");
            return;
        }
    }
}

bool w_bt_dish_Kitchenprinter_printerSet::trysave(const QString &int_id, const QString &vch_printname, const QString &vch_printtype, int vch_instype, const QString &vch_printip)
{
    if(false == w_bt_dish_Kitchenprinter_printerSet0::trysave(int_id, vch_printname, vch_printtype, vch_instype, vch_printip)) {
        return false;
    }
    lds_query_hddpos  query;
    query.execSelect(QString("select count(0) from cey_bt_kitchen_print where vch_printname = '%1' and int_id <> '%2' ")
                     .arg(vch_printname)
                     .arg(int_id));
    query.next();
    if(query.recordValue(0).toLongLong() > 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("打印机名称有重复"));
        return false;
    }
    query.execSelect(QString("select * from cey_bt_kitchen_print where vch_printip = '%1'  and int_id <> '%2' ")
                     .arg(vch_printip)
                     .arg(int_id));
    if(query.next()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  QObject::tr("端口被占用") + ":" +query.recordValue("vch_printname").toString());
        return false;
    }
    return true;
}
/////////////////////////////////////
QMutex w_bt_dish_Kitchenprinter_printer_thread_event::mutex;
w_bt_dish_Kitchenprinter_printer_thread::w_bt_dish_Kitchenprinter_printer_thread(QObject *parent)
    : lds_thread("结账打印", parent)
{
}

void w_bt_dish_Kitchenprinter_printer_thread::emitsignalTrigger(const QString &bp_ch_billno, const QString &bp_languageKey, const QStringList &label_insert_int_flowid_list, int pay_print_mode)
{
    //后台打印
    w_bt_dish_Kitchenprinter_printer_thread_event::mutex.lock();
    emit signalTrigger(bp_ch_billno, bp_languageKey, label_insert_int_flowid_list, pay_print_mode);
}

void w_bt_dish_Kitchenprinter_printer_thread::emitsignalTrigger(const QString &bp_ch_billno, const QString &bp_languageKey, const QStringList &label_insert_int_flowid_list, int pay_print_mode, const QImage &img)
{
    //后台打印
    w_bt_dish_Kitchenprinter_printer_thread_event::mutex.lock();
    emit signalTrigger(bp_ch_billno, bp_languageKey, label_insert_int_flowid_list, pay_print_mode, img);
}

void w_bt_dish_Kitchenprinter_printer_thread::run()
{
    w_bt_dish_Kitchenprinter_printer_thread_event event;
    connect(this, SIGNAL(signalTrigger(QString,QString,QStringList, int)), &event, SLOT(toprintBill(QString,QString,QStringList, int)));
    connect(this, SIGNAL(signalTrigger(QString,QString,QStringList, int,QImage)), &event, SLOT(toprintBill(QString,QString,QStringList, int,QImage)));
    this->exec();
}


w_bt_dish_Kitchenprinter_printer_thread_event::w_bt_dish_Kitchenprinter_printer_thread_event(QObject *parent)
    : lds_thread_event(parent)
{

}

void w_bt_dish_Kitchenprinter_printer_thread_event::toprintBill(const QString &bp_ch_billno, const QString &bp_languageKey, const QStringList & label_insert_int_flowid_list, int pay_print_mode)
{
    public_printby_ch_billno printer(bp_ch_billno, bp_languageKey);
    printer.toPayPrint(printer.defpath(), n_func::f_get_sysparm("bp_count", "1").toInt(), label_insert_int_flowid_list, (public_printby_ch_billno::payPrintMode)pay_print_mode);
    w_bt_dish_Kitchenprinter_printer_thread_event::mutex.unlock();
}

void w_bt_dish_Kitchenprinter_printer_thread_event::toprintBill(const QString &bp_ch_billno, const QString &bp_languageKey, const QStringList & label_insert_int_flowid_list, int pay_print_mode, const QImage &image)
{
    public_printby_ch_billno::payPrint(bp_ch_billno, bp_languageKey, label_insert_int_flowid_list, pay_print_mode, image);
    w_bt_dish_Kitchenprinter_printer_thread_event::mutex.unlock();
}

void w_bt_dish_Kitchenprinter_printer_thread_event::toExec()
{
    //do nothing
}

////////////////////////////////////////
w_bt_dish_Kitchenprinter_display_thread::w_bt_dish_Kitchenprinter_display_thread(QObject *parent)
    : lds_thread("客显线程", parent)
{

}

void w_bt_dish_Kitchenprinter_display_thread::emitsignalTrigger(const QByteArray &json)
{
    emit signalTrigger(json);
}

void w_bt_dish_Kitchenprinter_display_thread::emitsignalTrigger(int sqr, double value, const QString &name)
{
    cJSON_path path(cJSON_Array);
    cJSON_AddItemToArray(path.path_root(), cJSON_help::mapTocJSON(ldsVariantMap("sqr", sqr, "value", value, "name", name)));
    emitsignalTrigger(path.toByteArray());
}

void w_bt_dish_Kitchenprinter_display_thread::emitsignalTrigger(int sqr, double value)
{
    emitsignalTrigger(sqr, value, "");
}

void w_bt_dish_Kitchenprinter_display_thread::run()
{
    w_bt_dish_Kitchenprinter_display_thread_event event;
    connect(this, SIGNAL(signalTrigger(QByteArray)), &event, SLOT(toDisplay(QByteArray)));
    this->exec();
}

w_bt_dish_Kitchenprinter_display_thread_event::w_bt_dish_Kitchenprinter_display_thread_event(QObject *parent)
    : lds_thread_event(parent)
{

}

void w_bt_dish_Kitchenprinter_display_thread_event::toDisplay(const QByteArray &json)
{
    QString led_path = lds::sysconf->value("system_setting/kexianduankou").toString();
    if(led_path.isEmpty())
        return;
    if(public_sql::display_led.getType() != lds::sysconf->value("system_setting/kexianleixing").toString()) {
        public_sql::display_led.setType(lds::sysconf->value("system_setting/kexianleixing").toString());
    }

    QString vch_dishname;
    QMap<int, double> map;
    cJSON_path path(json);
    for(int k = 0; k < path.array_count(""); k ++) {
        map.insert(path.array_value("[%1{sqr", k).toInt(), path.array_value("[%1{value", k).toDouble());
        vch_dishname = path.array_value("[%1{name", k).toString();
    }
    public_sql::display_led.writeCom(map,
                                     led_path,
                                     vch_dishname);
}

void w_bt_dish_Kitchenprinter_display_thread_event::toExec()
{
    //do nothing
}

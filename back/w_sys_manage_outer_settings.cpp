#include "w_sys_manage_outer_settings.h"
#include "ui_w_sys_manage_outer_settings.h"
#include "ftableview_delegate.h"
#include "ftableview_standmodel_sql_none.h"
#include "w_sys_manage_outer_bill_preview.h"
#include "lds_messagebox.h"
#include "lds.h"
#include "commonheader.h"
#include "backheader.h"
#include "n_func.h"
#include "w_bt_dish_kitchenprinter.h"
#include "printer_0.h"
#include "clientdisplay.h"
#include "lds.h"
#include "w_sys_manage_wirelessordermonitoring.h"
#include "lds_tableview_delegate_com.h"
#include "w_sys_manage_outer_settings_label.h"
#include "w_bt_dish_base.h"
#include "n_func.h"
#include "w_scr_dish_barscale.h"
#include "system_setting_mobile_max_connect.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_bt_dish_kitchen_dish.h"
#include "qextserialenumerator.h"
#include "public_printby_ch_billno.h"
#include <QScrollBar>
#include "getfilepath.h"
#include <QFontDatabase>
#include "w_sys_manage_outer_pay_set.h"
#include "colordialog.h"
#include "public_showimages.h"
#include <QTimer>
#include "public_logindialog.h"
#include "printer_pos_serial.h"
#include <QtCore/qmath.h>
#include "lds_dialog_input_double.h"
#include "ui_lds_dialog_input_dialog_double.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "w_sys_manage_outer_bill_txt_set.h"
#include "w_bt_dish_kitchenprinter_printerset_port.h"
#include "printer_normal.h"
#include "w_sys_manage_sendemail.h"
#include "w_bt_dish_kitchen_view.h"
#include "qt_retail_scale_scpos_keyboard_thread.h"
#include <QButtonGroup>
#include "w_sys_manage_outer_settings_label_win.h"


bool w_sys_manage_outer_settings::supportDesktopSize()
{
    QStringList list;
    list << "1920x1080" <<"1366x768"<< "1024x768" << "800x600" << "480x800";
    QString cur_size = QString("%1x%2")
            .arg(qApp->desktop()->size().width())
            .arg(qApp->desktop()->size().height());

    return list.contains(cur_size);
}

void w_sys_manage_outer_settings::setSystemSize(QWidget *parent)
{
    QStringList list;
    list << "1920x1080" <<"1366x768"<< "1024x768" << "800x600" << "480x800";
    QString cur_size = QString("%1x%2")
            .arg(qApp->desktop()->size().width())
            .arg(qApp->desktop()->size().height());
    list.prepend(cur_size);


    lds_dialog_input dialog(parent);
    dialog.showView(lds_dialog_input::viewShowCom | lds_dialog_input::viewShowKey);
    dialog.ui->label_com->setText(QObject::tr("主屏尺寸"));
    dialog.ui->comboBox->setEditable(true);
    dialog.ui->comboBox->addItems(list);
    dialog.setWindowTitle(QObject::tr("软件无法适配当前分辨率"));
    lds::MAIN_WINDOW_SIZE.setOther(qApp->desktop()->size());
    while(QDialog::Accepted == lds_roundeddialog_rect_align_600x400(&dialog).exec()) {
        QString presize = dialog.ui->comboBox->currentText().trimmed();
        QStringList presizes = presize.split("x");
        if(presizes.count() != 2
                || presizes[0].toInt() <= 800
                || presizes[1].toInt() <= 600) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("格式错误"));
            continue;
        }
        lds::sysconf->setValue("system_setting/screen_primary_size", presize);
        break;
    }
}

w_sys_manage_outer_settings::w_sys_manage_outer_settings(QWidget *parent, int fisrtIndex) :
    public_Dialog(parent),
    ui(new Ui::w_sys_manage_outer_settings)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    wx_model = new QStandardItemModel(this);
    wx_model_2 = new QStandardItemModel(this);
    read_tel_timer = new QTimer(this);
    countprice = 0;
    frame_label_path_select_help = new w_bt_dish_kitchenprinter_printerset_port(this);
    frame_label_path_select_help->addControl(ui->frame_label_path_select);
    //1

    backlist_model = new ftableview_standmodel_sql_none;

    ftableview_delegate *d = new ftableview_delegate(ui->tableView_list);
    d->keepConnect(ui->tableView_list);
    //    d->setMargin(2);
    d->indexisEmptyDrawGrad = true;
    ui->tableView_list->setProperty("outer_stylesheet", "tableView_type");

    ui->tableView_list->setModel(backlist_model);
    ui->tableView_list->setItemDelegate(d);
    ui->tableView_list->setHeaderResizeMode(Qt::Horizontal, QHeaderView::Stretch);

    ui->tableView_list->verticalHeader()->setDefaultSectionSize(50);

    //
    tablemodel_dish_subtype = new lds_model_sqltablemodel(this);
    tablemodel_dish_subtype->setTable("cey_bt_dish_subtype");
    tablemodel_dish_subtype->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    ui->tableView_dish_subtype->setModel(tablemodel_dish_subtype);

    retranslateView();

    //connect
    connect(ui->tableView_list,SIGNAL(clicked(QModelIndex)),this,SLOT(toupdateSection(QModelIndex)));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    //3
    connect(ui->comboBox_vch_language, SIGNAL(currentIndexChanged(int)), this,SLOT(toSaaSCheck(int)));
    connect(ui->checkBox_pay_select_language, SIGNAL(clicked(bool)), this,SLOT(toSaaSCheck(bool)));
    connect(ui->pushButton_color_picker, SIGNAL(clicked()), this, SLOT(tocolorPicker()));
    connect(ui->pushButton_label_printer_win, SIGNAL(clicked()), this, SLOT(to_label_printer_win()));

    ui->stackedWidget->removeWidget(ui->stackedWidget->widget(0));
    ui->tableView_list->setCurrentIndex(backlist_model->index(fisrtIndex, 0));

    toupdateSection(ui->tableView_list->currentIndex());

#ifdef QT_NO_DEBUG
    lds::hideWidget(ui->lineEdit_drawer_char, true);
#endif

    lds::hideWidget(ui->pushButton_label_printer_win);

    //
    int label_top_off= 0;
#if defined(Q_OS_WIN) || defined(QT_DEBUG)
    lds::showWidget(ui->pushButton_label_printer_win);
    lds::hideWidget(ui->frame_label_path_select);
    lds::hideWidget(ui->pushButton_label_print);
    lds::hideWidget(ui->pushButton_label_test);

    label_top_off =ui->pushButton_label_printer_win->y() - 10;
    ui->pushButton_label_printer_win->move(ui->pushButton_label_printer_win->x(), ui->pushButton_label_printer_win->y() - label_top_off);
    ui->tableView_dish_subtype->move(ui->tableView_dish_subtype->x(), ui->tableView_dish_subtype->y() - label_top_off);
    ui->frame_labelprinter_buttons->move(ui->frame_labelprinter_buttons->x(), ui->frame_labelprinter_buttons->y() - label_top_off);
    ui->label_typeview->move(ui->label_typeview->x(), ui->label_typeview->y() - label_top_off);
#endif
}

w_sys_manage_outer_settings::~w_sys_manage_outer_settings()
{
    Printer_POS_SERIAL::SERIAL_PORT_MUTEX_COM_MAP_CLOSE_ALL();
    delete ui;
}

void w_sys_manage_outer_settings::retranslateView()
{

#ifdef QT_SCPOS_SUPPORT
    backlist_model->set_row_column(6, 1);

    backlist_model->model_data_set(0, 0,QObject::tr("结账打印机"),Qt::UserRole);
    backlist_model->model_data_set(0, 0,model_normal_print,Qt::UserRole + 1);

    backlist_model->model_data_set(1, 0,QObject::tr("厨房打印机"),Qt::UserRole);
    backlist_model->model_data_set(1, 0,model_kitchen_print,Qt::UserRole + 1);

    backlist_model->model_data_set(2, 0,QObject::tr("标签打印机"),Qt::UserRole);
    backlist_model->model_data_set(2, 0,model_label_print,Qt::UserRole + 1);

    backlist_model->model_data_set(3, 0,QObject::tr("客显及副屏"),Qt::UserRole);
    backlist_model->model_data_set(3, 0,model_screen_print,Qt::UserRole + 1);

    backlist_model->model_data_set(4, 0,QObject::tr("前台计价秤"),Qt::UserRole);
    backlist_model->model_data_set(4, 0,model_price_scale,Qt::UserRole + 1);

    backlist_model->model_data_set(5, 0,QObject::tr("后台条码秤"),Qt::UserRole);
    backlist_model->model_data_set(5, 0,model_bar_scale,Qt::UserRole + 1);
#else
    backlist_model->set_row_column(9, 1);

    backlist_model->model_data_set(0, 0,QObject::tr("结账打印机"),Qt::UserRole);
    backlist_model->model_data_set(0, 0,model_normal_print,Qt::UserRole + 1);

    backlist_model->model_data_set(1, 0,QObject::tr("厨房打印机"),Qt::UserRole);
    backlist_model->model_data_set(1, 0,model_kitchen_print,Qt::UserRole + 1);

    backlist_model->model_data_set(2, 0,QObject::tr("标签打印机"),Qt::UserRole);
    backlist_model->model_data_set(2, 0,model_label_print,Qt::UserRole + 1);

    backlist_model->model_data_set(3, 0,QObject::tr("客显及副屏"),Qt::UserRole);
    backlist_model->model_data_set(3, 0,model_screen_print,Qt::UserRole + 1);

    backlist_model->model_data_set(4, 0,QObject::tr("前台计价秤"),Qt::UserRole);
    backlist_model->model_data_set(4, 0,model_price_scale,Qt::UserRole + 1);

    backlist_model->model_data_set(5, 0,QObject::tr("后台条码秤"),Qt::UserRole);
    backlist_model->model_data_set(5, 0,model_bar_scale,Qt::UserRole + 1);
    backlist_model->model_data_set(6, 0,QObject::tr("无线点菜机"),Qt::UserRole);
    backlist_model->model_data_set(6, 0,model_wireless_machine,Qt::UserRole + 1);

    backlist_model->model_data_set(7, 0,QObject::tr("邮箱"),Qt::UserRole);
    backlist_model->model_data_set(7, 0,model_email,Qt::UserRole + 1);

    backlist_model->model_data_set(8, 0,QObject::tr("平板/厨显/电话盒"),Qt::UserRole);
    backlist_model->model_data_set(8, 0,model_tablet,Qt::UserRole + 1);
#endif

    tablemodel_dish_subtype->save_set_header("ch_sub_typeno", QObject::tr("小类编号"));
    tablemodel_dish_subtype->save_set_header("vch_sub_typename", QObject::tr("小类名称"));
    tablemodel_dish_subtype->save_set_header("ch_labelprint_flag", QObject::tr("标签打印"), true);
    ui->tableView_dish_subtype->rePecifyHeader(tablemodel_dish_subtype->get_save_set_header_saver_column_list());
    ui->tableView_dish_subtype->setColumnWidth(tablemodel_dish_subtype->fieldIndex("ch_sub_typeno"), 100);
    ui->tableView_dish_subtype->setColumnWidth(tablemodel_dish_subtype->fieldIndex("vch_sub_typename"), 290);
    ui->tableView_dish_subtype->setColumnWidth(tablemodel_dish_subtype->fieldIndex("ch_labelprint_flag"), 100);
    ui->tableView_dish_subtype->horizontalHeader()->setStretchLastSection(true);

    wx_model->setHorizontalHeaderLabels(QStringList()<<QObject::tr("基站号")<<QObject::tr("端口号"));
    wx_model_2->setHorizontalHeaderLabels(QStringList()<<QObject::tr("基站号")<<QObject::tr("点菜机号")<<QObject::tr("操作员"));


    comIndexSaveClear(ui->comboBox_print_formate_def);
    foreach(const QFileInfo &info, QDir("userdata/settings/print/receipt").entryInfoList()) {
        if(info.baseName() == ""
                || info.baseName() == "."
                || info.baseName() == ".."
                ) {
            continue;
        }

        if(saas_pay_detail::isSaaS()) {
            if(false == info.baseName().contains("saas", Qt::CaseInsensitive)) {
                continue;
            }
        }
        ui->comboBox_print_formate_def->addItem(info.fileName(), info.path());
    }


    comIndexSaveClear(ui->comboBox_fuping_mode);
    ui->comboBox_fuping_mode->addItem(QObject::tr("不启用"), -1);
    ui->comboBox_fuping_mode->addItem(QObject::tr("普通副屏"), 0);
    ui->comboBox_fuping_mode->addItem(QObject::tr("普通副屏") + "+", 4);
    ui->comboBox_fuping_mode->addItem(QObject::tr("4.3吋客显"), 1);
    ui->comboBox_fuping_mode->addItem(QObject::tr("双屏异显"), 2);
    ui->comboBox_fuping_mode->addItem(QObject::tr("称重副屏"), 3);

    ui->comboBox_screen_type->addItem(QObject::tr("副屏尺寸"));
    ui->comboBox_screen_type->addItem(QObject::tr("主屏尺寸"));

    ui->comboBox_fuping_size->addItems(QStringList()<<QObject::tr("随主屏") << "1920x1080" <<"1366x768"<< "1024x768" << "800x600" << "480x800" );
    ui->comboBox_screen_primary_size->addItems(QStringList() << QObject::tr("自动获取") << "1920x1080" <<"1366x768"<< "1024x768" << "800x600" << "480x800" );


    comIndexSaveClear(ui->comboBox_fuping_direct);
    ui->comboBox_fuping_direct->addItems(QStringList() << QObject::tr("水平") << QObject::tr("垂直"));

    comIndexSaveClear(ui->comboBox_vch_language);
    w_sys_manage_outer_pay_set::addLanguagesWithDefault(ui->comboBox_vch_language);

    comIndexSaveClear(ui->comboBox_vch_dishname_No);
    ui->comboBox_vch_dishname_No->addItem(QObject::tr("名称") + "1", "vch_dishname");
    ui->comboBox_vch_dishname_No->addItem(QObject::tr("名称") + "2", "vch_dishname_2");
    ui->comboBox_vch_dishname_No->addItem(QObject::tr("名称") + "1," + QObject::tr("名称") + "2", "vch_dishname,vch_dishname_2");

    comIndexRestoreAll();

}

void w_sys_manage_outer_settings::toupdateSection(const QModelIndex &index)
{
    int sqr = index.data(Qt::UserRole + 1).toInt();
    ui->stackedWidget->setCurrentIndex(sqr);
    saveData(sqr);
}

void w_sys_manage_outer_settings::took()
{
    //点菜机打印标签保留
    lds_model_sqltablemodel *tablemodel = static_cast<lds_model_sqltablemodel*>(ui->tableView_dish_subtype->model());
    if(tablemodel) {
        lds_query_hddpos::tran_saction();
        tablemodel->trySubmitAll();
        lds_query_hddpos::com_mit();
    }
    //
    foreach(int index, index_set) {
        if(!saveData(index, true)) {
            return;
        }
    }

    //
    if(lds::sysconf->value("system_setting/kexianduankou").toString().length() > 0
            ||lds::sysconf->value("system_setting/dianzichengduankou").toString().length() > 0) {
        if(lds::sysconf->value("system_setting/kexianduankou").toString() == lds::sysconf->value("system_setting/dianzichengduankou").toString()) {
            if(0 == lds_messagebox::warning(0, MESSAGE_TITLE_VOID, QObject::tr("设置冲突") + "\n" +
                                            QObject::tr("前台计价秤") + ":" + lds::sysconf->value("system_setting/kexianduankou").toString() + "\n"+
                                            QObject::tr("客显") + ":" + lds::sysconf->value("system_setting/dianzichengduankou").toString(),
                                            QObject::tr("返回"), QObject::tr("退出"))) {
                return;
            }
        }
    }

    this->accept();
}

void w_sys_manage_outer_settings::tocancel()
{
    this->reject();
}

void w_sys_manage_outer_settings::tobillset()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0506"/*结账打印格式*/)) {
        return;
    }

    w_sys_manage_outer_bill_preview dialog(this);
    lds::setwflagFrameless(&dialog);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    dialog.setGeometry(0, 0, lds::MAIN_WINDOW_SIZE.width(), lds::MAIN_WINDOW_SIZE.height());
    dialog.exec();
}

void w_sys_manage_outer_settings::tobillset_txt()
{
    w_sys_manage_outer_bill_txt_set dialog(this);
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_outer_settings::todrawerdef()
{
    ui->lineEdit_drawer_hex->setText("1B 70 00 28 28");
}

void w_sys_manage_outer_settings::todrawertest()
{
    if(ui->checkBox_drawer->isChecked()) {
        QSharedPointer<Printer_POS> printer = Printer_POS::get_pos_printer_by_path(ui->comboBox_printer_name->comusrdata().toString());
        if(printer.isNull()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "PRINTER_IS_NULL");
            return;
        }
        if(false == printer->tryOpen(ui->comboBox_printer_name->comusrdata().toString())) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, printer->lastError());
            return;
        }
        printer->writeHex(ui->lineEdit_drawer_hex->text().toLocal8Bit());
#ifdef QT_DEBUG
        printer->writedirect(ui->lineEdit_drawer_char->text().toLocal8Bit());
#endif
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("开钱箱未勾选"));
    }

    /*
    if(false == ui->checkBox_drawer->isChecked()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("开钱箱未勾选"));
        return;
    }
    QString errstring;
    blockIns ins;
    ins += pieceIns(ui->lineEdit_drawer_hex->text().toLocal8Bit(), "");
    QString printerpath = ui->comboBox_printer_name->comusrdata().toString();
    if(false == Printer_Normal::toprint(printerpath, ins, 1, &errstring)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,  errstring);
        return;
    }
*/
}

void w_sys_manage_outer_settings::toprintermanager()
{
    QString port = ui->comboBox_printer_name->comusrdata().toString();
    w_bt_dish_Kitchenprinter_printermanager dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();

    ui->comboBox_printer_name->clear();
    //2
    lds_query_hddpos  query;
    query.execSelect("select vch_printip, vch_printname FROM cey_bt_kitchen_print;");
    while(query.next()) {
        ui->comboBox_printer_name->addItem(query.recordValue("vch_printname").toString(), query.recordValue("vch_printip"));
    }
    //3
    ui->comboBox_printer_name->setcomusrdata(port);
}

void w_sys_manage_outer_settings::tokexiantest()
{
    static int sqr=-0;
    QString showtext;
    QString path = ui->comboBox_kexian_port->currentText();
    if(path.length() > 0) {//88888888
        showtext = QString(8, path.right(1).at(0));
    } else {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有端口被选择"));
        return;
    }
    public_sql::display_led.setType(ui->comboBox_kexian_type->currentText());
    if(sqr%5==0) {
        if(!public_sql::display_led.startShowinfo(ui->lineEdit_kexian_runinfo->text(), path)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,public_sql::display_led.lastError());
        }
    } else {
        if(!public_sql::display_led.writeCom(sqr%5, showtext.toInt() / 100.0, path)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,public_sql::display_led.lastError());
        }
    }
    sqr ++;
}

void w_sys_manage_outer_settings::toshow_kexianruninfo(int index)
{
    ui->label_17->setVisible(index == 1);
    ui->lineEdit_kexian_runinfo->setVisible(index == 1);
}

void w_sys_manage_outer_settings::tofupingreset()
{
    QFile file("userdata/save.txt");
    if(file.exists()) {
        if(file.remove()) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("重置成功"));
            return;
        }
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, file.errorString());
    }
}

void w_sys_manage_outer_settings::tofuping_adv()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0504"/*广告只能读取云端*/)) {
        return;
    }

    QString beforepath = lds::sysconf->value("w_sys_manage_outer_settings/tofuping_adv", lds::localdataPath + "/image_double_screen_dir").toString();
    QString filepath = getFilePath::getExistingDirectory(this, QObject::tr("选择文件夹"),
                                                         beforepath.isEmpty()?"/mnt":beforepath,
                                                         QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty()) {
        lds::sysconf->setValue("w_sys_manage_outer_settings/tofuping_adv", filepath);
        public_showImages dialog(filepath, this);
        if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
            //copy
            if(dialog.selected_image_path_list().isEmpty()) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有图片被选中"));
                return;
            }
            //mkpath
            QDir().mkpath(lds::localdataPath +"/image_double_screen_dir");
            foreach(const QString &image_path, dialog.selected_image_path_list()) {
                QFile file(image_path);
                QString destfile = lds::localdataPath +"/image_double_screen_dir" +"/"+ QFileInfo(image_path).fileName();
                if(false == file.copy(destfile)) {
                    lds_messagebox::information(this, MESSAGE_TITLE_VOID, file.errorString());
                    return;
                }
            }
            //error
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        }
    }
}

void w_sys_manage_outer_settings::tofuping_dish()
{
    QString str = ui->lineEdit_fuping_dish_dir->text();
    QString filepath = getFilePath::getExistingDirectory(this, QObject::tr("选择文件夹"),str.isEmpty()?"/mnt":str,QFileDialog::ShowDirsOnly);
    if(!filepath.isEmpty())
        ui->lineEdit_fuping_dish_dir->setText(filepath);
}

void w_sys_manage_outer_settings::tojijia_test()
{
    if(countprice == 0) {
        countprice = new ElectronicCountPrice_resolve(this);
        connect(countprice,SIGNAL(weighComming(QString)), ui->lineEdit_jijia_readback, SLOT(setText(QString)));
    }

    QString errstring;
    if(false == countprice->scaleOpen(
                ui->comboBox_jijia_type->currentText(),
                ui->comboBox_jijia_port->currentText(),
                ui->comboBox_jijia_boud->currentText().toInt(),
                &errstring
                )) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    }
}

void w_sys_manage_outer_settings::tojijia_CAL()
{
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(QObject::tr("密码") + ":");
    inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
    inputdialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
        return;
    }
    if(inputdialog.ui->lineEdit->text() != n_func::f_get_godpassword()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
        return;
    }

    //
    lds_dialog_input_double dialog(this);
    dialog.ui->label->setText(QObject::tr("重量") + "(kG)" + ":");
    dialog.ui->lineEdit->setRange(0, 9999);
    if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
        return;
    }
    //
    int kg = dialog.ui->lineEdit->value();
    //
    QByteArray cal_value = QByteArray("CAL") + QByteArray::number(kg).rightJustified(4, '0', true);
    QByteArray readall;
    int check = 0;
    for(int k = 0; k < cal_value.count(); k++) {
        check += cal_value[k];
    }
    cal_value +=  char(check & 0xff);
    qDebug("0x%x", check & 0xff);
    //

    delete countprice;
    countprice = 0;
    //
    //!等待
    lds_messagebox_loading_show loading(this, "", "WAITING...");
    loading.show_delay();
    //!~等待
    {
        QEventLoop eloop;
        Printer_POS_SERIAL printer;
        int try_cur = 0, try_count = 10;
        if(printer.tryOpen(QString("%1,%2")
                           .arg(ui->comboBox_jijia_port->currentText())
                           .arg(ui->comboBox_jijia_boud->currentText()))) {
            printer.writedirect("<" + cal_value  +">");
            for(;try_cur < try_count && !(readall += printer.readAll()).contains("<ERR"); try_cur ++) {
                QTimer::singleShot(1000, &eloop, SLOT(quit()));
                eloop.exec();
            }
            if(try_cur >= try_count) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败") + "TIME OUT");
            } else {
                int k0 = readall.indexOf("<ERR");
                readall = readall.mid(k0);
                if("1" == readall.mid(4, 1) ) {
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败") + readall);
                } else {
                    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功") + readall);
                }
            }
        }
    }
}

void w_sys_manage_outer_settings::tojijia_type_check(int checked)
{
    lds::showWidget(ui->pushButton_jijia_CAL, false);
    switch(checked) {
    case 14://acs-jc 191
        lds::showWidget(ui->pushButton_jijia_CAL, true);
        break;
    }
}

void w_sys_manage_outer_settings::towxsavestart(bool f)
{
    lds::sysconf->setValue("w_sys_manage_wirelessordermonitoring/start", f);
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
}

void w_sys_manage_outer_settings::towxdel()
{
    wx_model->removeRow(ui->tableView_wx_jizhan->currentIndex().row());

}

void w_sys_manage_outer_settings::towxdel2()
{
    wx_model_2->removeRow(ui->tableView_wx_diancaiji->currentIndex().row());

}

void w_sys_manage_outer_settings::towxnew()
{
    wx_model->insertRow(wx_model->rowCount(), QList<QStandardItem*>()
                        << new QStandardItem
                        << new QStandardItem);
}

void w_sys_manage_outer_settings::towxnew2()
{
    wx_model_2->insertRow(wx_model_2->rowCount(), QList<QStandardItem*>()
                          << new QStandardItem
                          << new QStandardItem
                          << new QStandardItem
                          );
}

void w_sys_manage_outer_settings::towxinit()
{
    w_sys_manage_wirelessordermonitoring::initdata(this, wx_dir);
}

void w_sys_manage_outer_settings::towxstartjz()
{
    w_sys_manage_wirelessordermonitoring::startjz(this);
}

void w_sys_manage_outer_settings::towxclose()
{
    w_sys_manage_wirelessordermonitoring::closejz();
}

void w_sys_manage_outer_settings::towxsave()
{
    if(wxsavejizhan()) {
        if(wxsavediancaiji()) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
            return;
        }
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));

}

void w_sys_manage_outer_settings::tolabel_print()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0507"/*标签格式*/)) {
        return;
    }
    w_sys_manage_outer_settings_label dialog(this);
    bool islabel = false;
    //奶茶模式的表头是不可以改的
    switch(lds_messagebox::question(this,MESSAGE_TITLE_VOID, QObject::tr("标签打印机"), QObject::tr("导入基础资料"), QObject::tr("进入奶茶模式"), QObject::tr("退出"))) {
    case 0://导入基础资料
        dialog.printmodel->setHorizontalHeaderLabels(QStringList()
                                                     << QObject::tr("打印次数")
                                                     << QObject::tr("品名")
                                                     << QObject::tr("品码")
                                                     << QObject::tr("价格")
                                                     << QObject::tr("生产日期")
                                                     << QObject::tr("保质期")
                                                     << QObject::tr("原料")
                                                     );
        //        query.execSelect("select * from cey_bt_dish where ch_stopflag='N' ");
        //        while(query.next()) {
        //            dialog.printmodel->appendRow(QList<QStandardItem*>()
        //                                         <<new QStandardItem("1")
        //                                         <<new QStandardItem(query.recordValue("vch_dishname").toString())
        //                                         <<new QStandardItem(query.recordValue("ch_dishno").toString())
        //                                         <<new QStandardItem(QString().sprintf("%0.2f", query.recordValue("num_price").toFloat()))
        //                                         <<new QStandardItem(sqldt.toString("yyyy-MM-dd"))
        //                                         <<new QStandardItem("")
        //                                         <<new QStandardItem("")
        //                                         );
        //        }
        break;
    case 1://进入奶茶模式
        dialog.printmodel->setHorizontalHeaderLabels(QStringList()
                                                     << QObject::tr("打印次数")
                                                     //                                                 << QObject::tr("抬头")
                                                     << QObject::tr("单号")

                                                     << QObject::tr("价格")
                                                     //                                                 << QObject::tr("电话")
                                                     //                                                 << QObject::tr("地址")

                                                     << QObject::tr("品名")
                                                     << QObject::tr("品码")
                                                     << QObject::tr("时间")
                                                     << QObject::tr("做法")
                                                     << QObject::tr("店名")
                                                     << QObject::tr("数量")
                                                     );
        dialog.printmodel->appendRow(QList<QStandardItem*>()
                                     <<new QStandardItem("1")
                                     //                                 <<new QStandardItem(QObject::tr("抬头"))
                                     <<new QStandardItem(QObject::tr("单号"))
                                     <<new QStandardItem(QObject::tr("价格"))
                                     //                                 <<new QStandardItem(QObject::tr("电话"))
                                     //                                 <<new QStandardItem(QObject::tr("地址"))

                                     <<new QStandardItem(QObject::tr("品名"))
                                     <<new QStandardItem(QObject::tr("品码"))
                                     <<new QStandardItem(QObject::tr("时间"))
                                     <<new QStandardItem(QObject::tr("做法"))
                                     <<new QStandardItem(QObject::tr("店名"))
                                     <<new QStandardItem(QObject::tr("数量"))
                                     );
        dialog.set_label_enabled();
        islabel = true;
        break;
    default://退出或其他
        return;
    }
    dialog.inputfinished(islabel);
    dialog.setWindowTitle(QObject::tr("标签打印机"));
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    transparentCenterDialog(&dialog, false).exec();

}

void w_sys_manage_outer_settings::tolabel_print_naicai_test()
{
    QFile::remove(lds::localdataPath +"/def.txt") ;
    QFile::copy(ui->comboBox_label_paperwidth->itemData(ui->comboBox_label_paperwidth->currentIndex()).toString(), lds::localdataPath +"/def.txt");
    QFile::setPermissions(lds::localdataPath +"/def.txt", QFile::Permission(0x777));

    int  w = ui->comboBox_label_paperwidth->currentText().split("x").value(0).toInt();
    int  h = ui->comboBox_label_paperwidth->currentText().split("x").value(1).toInt();
    lds::sysconf->setValue("label_print/baudrate", 9600);
    lds::sysconf->setValue("label_print/column", 1);
    lds::sysconf->setValue("label_print/danwei", "mm");
    lds::sysconf->setValue("label_print/fengbianlv", "200DPI");
    lds::sysconf->setValue("label_print/font", "TSS24.BF2");
    lds::sysconf->setValue("label_print/line_max_chars", -1);
    lds::sysconf->setValue("label_print/line_char_height", -1);

    lds::sysconf->setValue("label_print/gap=", 0);
    lds::sysconf->setValue("label_print/height", h);
    lds::sysconf->setValue("label_print/width", w);
    lds::sysconf->setValue("label_print/istag", false);
    lds::sysconf->setValue("label_print/narrow_width", 2);

    lds::sysconf->setValue("label_print/wide_width", 2);
    lds::sysconf->setValue("label_print/onebartype", 93);
    lds::sysconf->setValue("label_print/printnormaltype", 0);
    lds::sysconf->setValue("label_print/printtype", "GP-3120TN");
    lds::sysconf->setValue("label_print/printpath", frame_label_path_select_help->printerport());

    public_printby_ch_billno::print_label_test(this);
}

void w_sys_manage_outer_settings::tobar_scale()
{
    w_scr_dish_barscale dialog(this);
    //获取店名
    lds_query_hddpos  query;
    query.execSelect("select vch_company FROM hddpos.cey_sys_company;");
    query.next();
    dialog.set_shop_name(query.recordValue("vch_company").toString());
    // 同步商品名称/商品价格
    dialog.keep(query, "select vch_dishname, num_price, '' from cey_bt_dish where vch_barcode = '%1' ");
    //设置窗口参数
    dialog.setWindowTitle(QObject::tr("条码秤"));
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    transparentCenterDialog(&dialog, false).exec();
}

void w_sys_manage_outer_settings::toimport()
{
    QString printpath = ui->comboBox_printer_name->itemData(ui->comboBox_printer_name->currentIndex()).toString();

    QString path = ui->comboBox_print_formate_def->itemData(ui->comboBox_print_formate_def->currentIndex()).toString();
    QString filename = ui->comboBox_print_formate_def->itemText(ui->comboBox_print_formate_def->currentIndex());
    QString filepath = path +"/"+filename;

    if(filepath.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, "filepath is empty");
        return;
    }
    if(printpath.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("结账打印机未设置"));
        return;
    }
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, ui->comboBox_print_formate_def->currentText(), QObject::tr("确认"), QObject::tr("取消"))) {
        lds_query_hddpos  query;
        lds_query_hddpos::tran_saction();
        if(w_sys_manage_outer_bill_preview::import(filepath)) {
            if(filepath.contains("58"))
                query.execUpdate("cey_bt_kitchen_print", qrtVariantPairList()
                                 << qrtVariantPair("int_paperwidth",32),
                                 QString(" vch_printip = '%1' ").arg(printpath));
            if(filepath.contains("80"))
                query.execUpdate("cey_bt_kitchen_print", qrtVariantPairList()
                                 << qrtVariantPair("int_paperwidth",48),
                                 QString(" vch_printip = '%1' ").arg(printpath));

            lds_query_hddpos::com_mit();
            lds::sysconf->setValue("system_setting/print_formate_def", ui->comboBox_print_formate_def->currentIndex());
            lds::sysconf->setValue("system_setting/printerpath",ui->comboBox_printer_name->comusrdata().toString());

            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return;
        }
        lds_query_hddpos::roll_back();

        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    }
}

void w_sys_manage_outer_settings::tomobile_connect_max_value()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0503"/*手机最大连接数*/)) {
        return;
    }
    system_setting_mobile_max_connect dialog(this);
    dialog.setWindowTitle(QObject::tr("移动端最大连接数"));
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_outer_settings::toSaaSCheck(int index)
{
    if(ui->comboBox_vch_language == this->sender()) {
        if(!ui->comboBox_vch_language->curusrdata().toString().isEmpty()) {
            if(!saas_pay_detail::SaaSFuncCheck(this, "0512"/*多语言*/)) {
                ui->comboBox_vch_language->setcomusrdata("");
            }
        }
    }
}

void w_sys_manage_outer_settings::toSaaSCheck(bool check)
{
    if(!check) return;

    if(ui->checkBox_pay_select_language == this->sender()) {
        if(!saas_pay_detail::SaaSFuncCheck(this, "0512"/*多语言*/)) {
            ui->checkBox_pay_select_language->setChecked(false);
        }
    }
}

bool w_sys_manage_outer_settings::saveData(int index, bool issave)
{
    lds_query_hddpos  query;
    switch(index) {
    case model_normal_print://小票格式
        if(issave) {
            lds::sysconf->setValue("system_setting/printerpath",ui->comboBox_printer_name->comusrdata().toString());
            lds::sysconf->setValue("system_setting/maidanzhangdan",ui->checkBox_pay_print->isChecked()?"1":"0");
            lds::sysconf->setValue("system_setting/pay_select_language",ui->checkBox_pay_select_language->isChecked());
            lds::sysconf->setValue("system_setting/maidan_language",ui->comboBox_vch_language->comusrdata());

            n_func::f_set_sysparm_q(query, "bp_vch_dishname_No", ui->comboBox_vch_dishname_No->comusrdata());
            n_func::f_set_sysparm_q(query, "bp_count", ui->lineEdit_pay_print_count->text());
            n_func::f_set_sysparm_q(query, "bp_togo_count", ui->lineEdit_pay_waimai_count->text());

            //本机落单打印总单
            lds::sysconf->setValue("system_setting/luodanzongdan", ui->checkBox_9->isChecked()?"1":"0");
            lds::sysconf->setValue("system_setting/fukuanshishurushoupai", ui->checkBox_fukuanshishurushoupai->isChecked()?"1":"0");
            //结帐打开钱箱
            lds::sysconf->setValue("system_setting/qianxiang",ui->checkBox_drawer->isChecked());
            lds::sysconf->setValue("system_setting/qianxiangzhiling",ui->lineEdit_drawer_hex->text());
            lds::sysconf->setValue("system_setting/drawer_timeout",ui->comboBox_drawer_timeout->curusrdata());

            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            //结帐打印机
            if(ui->comboBox_printer_name->count() == 0) {
                query.execSelect("select vch_printip, vch_printname FROM cey_bt_kitchen_print;");
                while(query.next()) {
                    ui->comboBox_printer_name->addItem(query.recordValue("vch_printname").toString(), query.recordValue("vch_printip"));
                }
            }
            ui->comboBox_printer_name->setcomusrdata(lds::sysconf->value("system_setting/printerpath", "/dev/usb/lp0").toString());
            ui->checkBox_pay_print->setChecked(lds::sysconf->value("system_setting/maidanzhangdan").toString() == "1");
            ui->checkBox_pay_select_language->setChecked(lds::sysconf->value("system_setting/pay_select_language", false).toBool());
            ui->comboBox_vch_language->setcomusrdata(lds::sysconf->value("system_setting/maidan_language").toString());

            ui->comboBox_vch_dishname_No->setcomusrdata(n_func::f_get_sysparm_q(query, "bp_vch_dishname_No", "vch_dishname"));
            ui->lineEdit_pay_print_count->setText(n_func::f_get_sysparm_q(query,"bp_count"));
            ui->lineEdit_pay_waimai_count->setText(n_func::f_get_sysparm_q(query,"bp_togo_count"));
            //本机落单打印总单
            ui->checkBox_9->setChecked(lds::sysconf->value("system_setting/luodanzongdan").toString() == "1");
            ui->checkBox_fukuanshishurushoupai->setChecked(lds::sysconf->value("system_setting/fukuanshishurushoupai").toString() == "1");

            //结帐打开钱箱
            ui->checkBox_drawer->setChecked(lds::sysconf->value("system_setting/qianxiang").toBool());
            ui->lineEdit_drawer_hex->setText(lds::sysconf->value("system_setting/qianxiangzhiling", "1B 70 00 28 28").toString());

            ui->comboBox_print_formate_def->setCurrentIndex(lds::sysconf->value("system_setting/print_formate_def").toInt());

            ui->comboBox_drawer_timeout->addItem("500ms", 500);
            ui->comboBox_drawer_timeout->addItem("1000ms", 1000);
            ui->comboBox_drawer_timeout->addItem("1500ms", 1500);
            ui->comboBox_drawer_timeout->setcomusrdata(lds::sysconf->value("system_setting/drawer_timeout", 1000).toInt());

            connect(ui->pushButton_import,SIGNAL(clicked()),this,SLOT(toimport()));

            connect(ui->pushButton_bill, SIGNAL(clicked()),this,SLOT(tobillset()));
            connect(ui->pushButton_drawer_def, SIGNAL(clicked()), this,SLOT(todrawerdef()));
            connect(ui->pushButton_drawer_test, SIGNAL(clicked()), this,SLOT(todrawertest()));
            connect(ui->pushButton_printer_manager,SIGNAL(clicked()),this,SLOT(toprintermanager()));
        }
        break;
        ///////////////////////////

    case model_kitchen_print://厨房打印机
    {
        if(issave) {
            if(ui->checkBox_kt_kitchen_print->isChecked())
                n_func::f_set_sysparm("kt_kitchen_type", "kitchen_print");
            if(ui->checkBox_kt_kitchen_view->isChecked())
                n_func::f_set_sysparm("kt_kitchen_type", "kitchen_view");
            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            QButtonGroup* pButtonGroup = new QButtonGroup(this);
            pButtonGroup->addButton(ui->checkBox_kt_kitchen_print);
            pButtonGroup->addButton(ui->checkBox_kt_kitchen_view);

            ui->checkBox_kt_kitchen_print->setChecked(n_func::f_get_sysparm("kt_kitchen_type") == "kitchen_print");
            ui->checkBox_kt_kitchen_view->setChecked(n_func::f_get_sysparm("kt_kitchen_type") == "kitchen_view");
            //            connect(ui->checkBox_kt_kitchen_print, SIGNAL(toggled(bool)), ui->checkBox_kt_kitchen_view, SLOT(setUnchecked(bool)));
            //            connect(ui->checkBox_kt_kitchen_view, SIGNAL(toggled(bool)), ui->checkBox_kt_kitchen_print, SLOT(setUnchecked(bool)));
            connect(ui->pushButton_kt_kitchen_print, SIGNAL(clicked()), this, SLOT(toshowkitchen_print()));
            connect(ui->pushButton_kt_kitchen_view, SIGNAL(clicked()), this, SLOT(toshowkitchen_view()));
        }
    }
        break;
        ////////////////////////////
    case model_label_print://标价签
        if(issave) {
            lds::sysconf->setValue("system_setting/label_print_path",frame_label_path_select_help->printerport());
            lds::sysconf->setValue("system_setting/label_print_type",ui->comboBox_label_print_type->currentIndex());
            lds::sysconf->setValue("system_setting/label_print_paper",ui->comboBox_label_paperwidth->currentIndex());
            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            ui->comboBox_label_paperwidth->addItem("40x30", ":/image/print_format/label_40x30_def.txt");
            ui->comboBox_label_paperwidth->addItem("60x40", ":/image/print_format/label_60x40_def.txt");
            ui->comboBox_label_print_type->addItems(QStringList() << "Gainscha");

            frame_label_path_select_help->setPrinterport(lds::sysconf->value("system_setting/label_print_path").toString());
            ui->comboBox_label_print_type->setCurrentIndex(lds::sysconf->value("system_setting/label_print_type").toInt());
            ui->comboBox_label_paperwidth->setCurrentIndex(lds::sysconf->value("system_setting/label_print_paper").toInt());

            tablemodel_dish_subtype->select();

            ui->tableView_dish_subtype->setEditTriggers(QTableView::AllEditTriggers);
            ui->tableView_dish_subtype->setItemDelegate(new lds_tableview_delegate_check(QVector<int>()<<3,  ui->tableView_dish_subtype, true));

            connect(ui->pushButton_label_print, SIGNAL(clicked()), this, SLOT(tolabel_print()));
            connect(ui->pushButton_label_test, SIGNAL(clicked()), this, SLOT(tolabel_print_naicai_test()));
            connect(ui->pushButton_label_dish_subtype_up, SIGNAL(clicked()), ui->tableView_dish_subtype, SLOT(toPageUp()));
            connect(ui->pushButton_label_dish_subtype_down, SIGNAL(clicked()), ui->tableView_dish_subtype, SLOT(toPageDown()));
            connect(ui->pushButton_label_dish_subtype_checkun, SIGNAL(clicked()), this, SLOT(toLabelDishCheckUn()));
            connect(ui->pushButton_label_dish_subtype_checkall, SIGNAL(clicked()),  this, SLOT(toLabelDishCheckAll()));
        }
        break;
        ///////////////////////////

    case model_screen_print://客显与副屏
        if(issave) {
            lds::sysconf->setValue("system_setting/kexianleixing",ui->comboBox_kexian_type->currentText());
            lds::sysconf->setValue("system_setting/kexianduankou",ui->comboBox_kexian_port->currentText());

            lds::sysconf->setValue("system_setting/kexian_vfd_start",ui->lineEdit_kexian_runinfo->text());

            //副屏
            lds::sysconf->setValue("system_setting/bofanglujing",ui->lineEdit_fuping_dish_dir->text());
            lds::sysconf->setValue("system_setting/qt_show_dish_pic",ui->checkBox_qt_show_dish_pic->isChecked());
            lds::sysconf->setValue("system_setting/bofangg_runningtext",ui->lineEdit_fuping_runtext->text());
            lds::sysconf->setValue("system_setting/bofangg_runningtext_color",ui->lineEdit_fuping_runtext_color->text());
            lds::sysconf->setValue("system_setting/bofangg_runningtext_size",ui->lineEdit_fuping_runtext_font->text());

            lds::sysconf->setValue("system_setting/bofangg_rate",ui->comboBox_fuping_size->currentText());
            lds::sysconf->setValue("system_setting/bofangg_rote",ui->comboBox_fuping_rotate->currentText());

            lds::sysconf->setValue("system_setting/bofang_layout_dir",ui->comboBox_fuping_direct->currentIndex());
            lds::sysconf->setValue("system_setting/fuping_mode",ui->comboBox_fuping_mode->curusrdata());
            lds::sysconf->setValue("system_setting/dd_show_dish_list", ui->checkBox_dd_show_dish_list->isChecked());

            lds::sysconf->setValue("system_setting/screen_primary_size", ui->comboBox_screen_primary_size->currentText());

            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            ui->comboBox_kexian_type->addItems(CommonHeader::creat_clientdisplay_ledtypes());
            ui->comboBox_kexian_type->setCurrentIndex(ui->comboBox_kexian_type->findText(lds::sysconf->value("system_setting/kexianleixing").toString()));
            ui->comboBox_kexian_port->addItems(printer_0::f_scan_ttyPort(true));
            ui->comboBox_kexian_port->setCurrentIndex(ui->comboBox_kexian_port->findText(lds::sysconf->value("system_setting/kexianduankou").toString()));

            ui->lineEdit_kexian_runinfo->setText(lds::sysconf->value("system_setting/kexian_vfd_start").toString());
            connect(ui->pushButton_kexian_test,SIGNAL(clicked()),this,SLOT(tokexiantest()));
            toshow_kexianruninfo(ui->comboBox_kexian_type->currentIndex());
            connect(ui->comboBox_kexian_type, SIGNAL(currentIndexChanged(int)),this, SLOT(toshow_kexianruninfo(int)));

            //副屏
            ui->lineEdit_fuping_dish_dir->setText(lds::sysconf->value("system_setting/bofanglujing").toString());
            ui->checkBox_qt_show_dish_pic->setChecked(lds::sysconf->value("system_setting/qt_show_dish_pic", true).toBool());
            ui->lineEdit_fuping_runtext->setText(lds::sysconf->value("system_setting/bofangg_runningtext").toString());
            ui->lineEdit_fuping_runtext_color->setText(lds::sysconf->value("system_setting/bofangg_runningtext_color", "white").toString());
            QPixmap p(30,30);
            p.fill(QColor(ui->lineEdit_fuping_runtext_color->text()));ui->pushButton_color_picker->setIcon(p);
            ui->lineEdit_fuping_runtext_font->setText(lds::sysconf->value("system_setting/bofangg_runningtext_size", "80").toString());

            setEditText(ui->comboBox_fuping_size, lds::sysconf->value("system_setting/bofangg_rate", QObject::tr("随主屏")).toString());
            ui->comboBox_fuping_mode->setcomusrdata(lds::sysconf->value("system_setting/fuping_mode", -1));
            ui->checkBox_dd_show_dish_list->setChecked(lds::sysconf->value("system_setting/dd_show_dish_list", true).toBool());

            ui->comboBox_fuping_rotate->addItems(QStringList()<<"0"<<"90"<<"180"<<"270");
            setEditText(ui->comboBox_fuping_rotate, QString::number(lds::sysconf->value("system_setting/bofangg_rote", 0).toInt()));

            ui->comboBox_fuping_direct->setCurrentIndex(lds::sysconf->value("system_setting/bofang_layout_dir", 0).toInt());
            setEditText(ui->comboBox_screen_primary_size, lds::sysconf->value("system_setting/screen_primary_size", QObject::tr("自动获取")).toString());
            //            connect(ui->pushButton_fuping_reset, SIGNAL(clicked()),this,SLOT(tofupingreset()));


            connect(ui->toolButton_fuping_adv, SIGNAL(clicked()),this,SLOT(tofuping_adv()));
            connect(ui->toolButton_fuping_dish, SIGNAL(clicked()),this,SLOT(tofuping_dish()));
            connect(ui->comboBox_fuping_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(tofuping_4_3cun(int)));
            connect(ui->comboBox_screen_type, SIGNAL(currentIndexChanged(int)), this, SLOT(tofuping_screen_index(int)));
            //init data
            tofuping_screen_index(ui->comboBox_screen_type->currentIndex());


            lds::hideWidget(ui->label_62); lds::hideWidget(ui->lineEdit_fuping_runtext);
            lds::hideWidget(ui->label_63); lds::hideWidget(ui->lineEdit_fuping_runtext_color);lds::hideWidget(ui->pushButton_color_picker);
            lds::hideWidget(ui->label_66); lds::hideWidget(ui->lineEdit_fuping_runtext_font);lds::hideWidget(ui->label_65);
        }
        break;
        ///////////////////////////
    case model_price_scale://计价秤
        if(issave) {
            lds::sysconf->setValue("system_setting/dianzichengxinghao",ui->comboBox_jijia_type->currentText());
            lds::sysconf->setValue("system_setting/dianzichengduankou",ui->comboBox_jijia_port->currentText());
            lds::sysconf->setValue("system_setting/dianzicheng_baudrate",ui->comboBox_jijia_boud->currentText());

            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            ui->comboBox_jijia_type->addItems(ElectronicCountPrice_resolve::creat_pricecount_scaletypes());
            ui->comboBox_jijia_type->setCurrentIndex(ui->comboBox_jijia_type->findText(lds::sysconf->value("system_setting/dianzichengxinghao").toString()));

            ui->comboBox_jijia_port->addItems(printer_0::f_scan_ttyPort());
            ui->comboBox_jijia_port->setCurrentIndex(ui->comboBox_jijia_port->findText(lds::sysconf->value("system_setting/dianzichengduankou").toString()));

            ui->comboBox_jijia_boud->addItems(Printer_POS_SERIAL::creat_baudrate_list());
            ui->comboBox_jijia_boud->setCurrentIndex(ui->comboBox_jijia_boud->findText(lds::sysconf->value("system_setting/dianzicheng_baudrate").toString()));

            tojijia_type_check(ui->comboBox_jijia_type->currentIndex());

            connect(ui->pushButton_jijia_test, SIGNAL(clicked()),this,SLOT(tojijia_test()));
            connect(ui->comboBox_jijia_type, SIGNAL(currentIndexChanged(int)), this,SLOT(tojijia_type_check(int)));
            connect(ui->pushButton_jijia_CAL, SIGNAL(clicked()), this, SLOT(tojijia_CAL()));
        }
        break;
        ///////////////////////////
    case model_bar_scale://条码秤
        if(issave) {
            return true;
        }
        tobar_scale();
        ui->tableView_list->setCurrentIndex(ui->tableView_list->model()->index(0, 0));
        ui->stackedWidget->setCurrentIndex(0);
        break;
        ///////////////////////////
    case model_wireless_machine://无线点菜机
        if(!saas_pay_detail::SaaSFuncCheck(this, "0502"/*点菜宝*/)) {
            ui->tableView_list->setCurrentIndex(ui->tableView_list->model()->index(0, 0));
            ui->stackedWidget->setCurrentIndex(0);
            return true;
        }
        if(index_set.find(index) == index_set.end()) {
            wx_dir = qApp->applicationDirPath();
            ui->tableView_wx_jizhan->setModel(wx_model);
            ui->tableView_wx_jizhan->setColumnWidth(0, 60);
            ui->tableView_wx_jizhan->horizontalHeader()->setStretchLastSection(true);
            ui->tableView_wx_jizhan->setEditTriggers(QTableView::AllEditTriggers);
            ui->tableView_wx_diancaiji->setModel(wx_model_2);
            ui->tableView_wx_diancaiji->setColumnWidth(0, 60);
            ui->tableView_wx_diancaiji->horizontalHeader()->setStretchLastSection(true);
            ui->tableView_wx_diancaiji->setEditTriggers(QTableView::AllEditTriggers);
            //
            QStandardItemModel *model = new QStandardItemModel(this);
            QList<QStandardItem *> items;
            QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
            foreach(const QextPortInfo &port, ports) {
                items.clear();items << new QStandardItem(port.portName) << new QStandardItem(port.portName);model->appendRow(items);
            }

            lds_tableview_delegate_com *comdelegat = new lds_tableview_delegate_com(model, 1, ui->tableView_wx_jizhan);
            ui->tableView_wx_jizhan->setItemDelegate(comdelegat);

            lds_tableview_delegate_com *delegate_2 = new lds_tableview_delegate_com(wx_model,0, ui->tableView_wx_diancaiji);
            ui->tableView_wx_diancaiji->setItemDelegate(delegate_2);
            delegate_2->setonlyk(true);

            //
            QDir d(wx_dir+"/TXT");
            if(!d.exists()) {
                d.mkdir(d.path());
            }
            wx_init_jizhan();
            wx_init_diancaiji();

            //
            connect(ui->pushButton_wx_save,SIGNAL(clicked()),this,SLOT(towxsave()));
            connect(ui->pushButton_wx_jizhan_del,SIGNAL(clicked()),this,SLOT(towxdel()));
            connect(ui->pushButton_wx_diancaiji_del,SIGNAL(clicked()),this,SLOT(towxdel2()));
            connect(ui->pushButton_wx_jizhan_new,SIGNAL(clicked()),this,SLOT(towxnew()));
            connect(ui->pushButton_wx_diancaiji_new,SIGNAL(clicked()),this,SLOT(towxnew2()));
            connect(ui->pushButton_wx_initdata,SIGNAL(clicked()),this,SLOT(towxinit()));
            connect(ui->pushButton_wx_startjz,SIGNAL(clicked()),this,SLOT(towxstartjz()));
            connect(ui->pushButton_wx_close,SIGNAL(clicked()),this,SLOT(towxclose()));
            ui->checkBox_wx->setChecked(lds::sysconf->value("w_sys_manage_wirelessordermonitoring/start", false).toBool());
            connect(ui->checkBox_wx,SIGNAL(toggled(bool)),this,SLOT(towxsavestart(bool)));

        }
        break;

        /////////
    case model_tablet:
    {
        //平板设置
        if(index_set.find(index) == index_set.end()) {
            connect(ui->pushButton_mobile_connect_max_value, SIGNAL(clicked()),this,SLOT(tomobile_connect_max_value()));
        }
        //电话盒子
        if(issave) {
            lds::sysconf->setValue("system_setting/read_tel_enable",ui->checkBox_read_tel_enable->isChecked());
            return true;
        }

        if(index_set.find(index) == index_set.end()) {
            ui->checkBox_read_tel_enable->setChecked(lds::sysconf->value("system_setting/read_tel_enable", false).toBool());
            connect(ui->pushButton_read_tel_test, SIGNAL(clicked()), this, SLOT(to_read_tel_test()));
            //线程后台没有运行,使用定时器辅助
            //read_tel_enable 控制后台的电话盒是否正在运行
            if(false == lds::sysconf->value("system_setting/read_tel_enable", false).toBool()) {
                connect(read_tel_timer, SIGNAL(timeout()), this, SLOT(to_read_tel_readdata()));
            }
            ui->pushButton_read_tel_test->setText(QObject::tr("开始测试") + "...");
        }
    }
        break;
    case model_email:
    {
        lds::hideWidget(ui->pushButton_email_set);
        w_sys_manage_sendEmail dialog(this);
        transparentCenterDialog(&dialog).exec();
        ui->tableView_list->setCurrentIndex(ui->tableView_list->model()->index(0, 0));
        ui->stackedWidget->setCurrentIndex(0);
        //        if(index_set.find(index) == index_set.end()) {
        //            connect(ui->pushButton_email_set, SIGNAL(clicked()), this, SLOT(toEmailSet()));
        //        }
    }
        break;
    }

    index_set.insert(index);
    return true;
}

void w_sys_manage_outer_settings::setEditText(QComboBox *com, const QString &editText)
{
    int index = com->findText(editText);
    if(index >= 0)
        com->setCurrentIndex(index);
    else
        com->setEditText(editText);
}

bool w_sys_manage_outer_settings::kitchen_plan_check(const QString &kt_kitchen_type)
{
    //cey_sys_parameter/vch_value(vch_parameter == kt_kitchen_type):'kitchen_print','kitchen_view'
    //    QString type = n_func::f_get_sysparm("kt_kitchen_type");
    //    if(kt_kitchen_type != type) {
    //        if(lds_query_hddpos::selectValue(" select count(0) from cey_bt_kitchen_plan").toInt() > 0
    //                && 1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("将会重置厨打方案"), QObject::tr("确定"), QObject::tr("取消"))) {
    //            return false;
    //        }
    n_func::f_set_sysparm("kt_kitchen_type", kt_kitchen_type);
    //        lds_query_hddpos::deleteTable("cey_bt_kitchen_plan", "");
    //    }
    return true;
}

bool w_sys_manage_outer_settings::wxsavejizhan()
{
    QString value;
    QString info;
    QMultiMap<QString,int > map;
    for(int row = 0, rowcount = wx_model->rowCount();row<rowcount;row++) {
        value = wx_model->item(row, 0)->text();
        if(value.length() != 1) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString("[%1]").arg(row)+QObject::tr("基站号只能为1位"));
            return false;
        }
        if(value.toInt() == 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString("[%1]").arg(row)+QObject::tr("基站号必须为数字且不能为0"));
            return false;
        }
        map.insert(value, row);
    }
    ///重复数据
    info = "";
    foreach(const QString &k, map.keys().toSet()) {
        int count = map.count(k);
        if(count > 1) {
            QMapIterator<QString,int > mapitr(map);
            while(mapitr.hasNext()) {
                mapitr.next();
                if(mapitr.key() == k) {
                    info += QString("[%1%2]").arg(mapitr.value()  + 1).arg(QObject::tr("行"));
                    count --;
                }
                if(count <= 0) break;
            }
        }
    }
    if(!info.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("基站号不能重复")+info);
        return false;
    }
    ///~重复数据


    QFile file;
    file.setFileName(wx_dir+_TXT_jz_txt);
    if(!file.open(QFile::WriteOnly)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, file.errorString());
        return false;
    }

    for(int row = 0, rowcount = wx_model->rowCount(); row < rowcount; row++) {
        QByteArray line;
        line += wx_model->data(wx_model->index(row, 0)).toString() + " ";
        line += wx_model->data(wx_model->index(row, 1)).toString() + "\r\n";
        file.write(line);

    }
    file.close();
    return true;
}

bool w_sys_manage_outer_settings::wxsavediancaiji()
{
    QMultiMap<QString,int > map;
    QString info;
    QString value;
    for(int row = 0, rowcount = wx_model_2->rowCount();row<rowcount;row++) {
        value = wx_model_2->item(row, 1)->text();
        if(value.length() != 3) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString("[%1]").arg(row+1)+QObject::tr("点菜机号只能为3位"));
            return false;
        }
        if(value.toInt() == 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString("[%1]").arg(row+1)+QObject::tr("点菜机号必须为数字且不能为0"));
            return false;
        }

        map.insert(value, row);
    }


    ///重复数据
    info = "";
    foreach(const QString &k, map.keys().toSet()) {
        int count = map.count(k);
        if(count > 1) {
            QMapIterator<QString,int > mapitr(map);
            while(mapitr.hasNext()) {
                mapitr.next();
                if(mapitr.key() == k) {
                    info += QString("[%1%2]").arg(mapitr.value()  + 1).arg(QObject::tr("行"));
                    count --;
                }
                if(count <= 0) break;
            }
        }
    }
    if(!info.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("点菜机号不能重复")+info);
        return false;
    }
    ///~重复数据


    QFile file;
    file.setFileName(wx_dir+_TXT_dcj_txt);
    if(!file.open(QFile::WriteOnly)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, file.errorString());
        return false;
    }

    for(int row = 0, rowcount = wx_model_2->rowCount(); row < rowcount; row++) {
        QByteArray line;
        line += wx_model_2->data(wx_model_2->index(row, 1)).toString() + " ";
        line += wx_model_2->data(wx_model_2->index(row, 0)).toString() + "\r\n";
        file.write(line);
    }
    file.close();
    return true;
}

void w_sys_manage_outer_settings::wx_init_jizhan()
{
    QFile file;
    file.setFileName(wx_dir+_TXT_jz_txt);
    if(!file.open(QFile::ReadOnly)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("未找到基站文件,请先[新增基站]"));
        return;
    }

    while(!file.atEnd()) {
        QString readline = file.readLine();
        QStringList line = readline.trimmed().split(" ");
        wx_model->appendRow(QList<QStandardItem*>()
                            << new QStandardItem(line.value(0)) << new QStandardItem(line.value(1)));
    }
    file.close();
}

void w_sys_manage_outer_settings::wx_init_diancaiji()
{
    QFile file;
    file.setFileName(wx_dir+_TXT_dcj_txt);
    if(!file.open(QFile::ReadOnly)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("未找到点菜机文件,请先[新增点菜机]"));
        return;
    }

    while(!file.atEnd()) {
        QString readline = file.readLine();
        QStringList line = readline.trimmed().split(" ");
        wx_model_2->appendRow(QList<QStandardItem*>()
                              << new QStandardItem(line.value(1)) << new QStandardItem(line.value(0)));
    }
    file.close();
}

void w_sys_manage_outer_settings::tocolorPicker()
{
    colordialog dialog(this);
    dialog.setWindowTitle(QObject::tr("选择颜色"));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        ui->lineEdit_fuping_runtext_color->setText(dialog.result_color_name());
        QPixmap p(30,30);p.fill(QColor(ui->lineEdit_fuping_runtext_color->text()));ui->pushButton_color_picker->setIcon(p);
    }
}

void w_sys_manage_outer_settings::to_read_tel_test()
{
    if(read_tel_timer->isActive()) {
        read_tel_timer->stop();
        ui->pushButton_read_tel_test->setText(QObject::tr("停止测试"));
        //这边有后台有运行电话盒子和未运行的两种，但是两种都只要disconnect即可
        disconnect(public_sql::save_login_object(), SIGNAL(signal_telNoComingState(QString)), ui->plainTextEdit_read_tel, SLOT(appendPlainText(QString)));

    } else{
        read_tel_timer->start(public_sql::save_login->runTelBoxTimeout * 1000);
        ui->pushButton_read_tel_test->setText(QObject::tr("开始测试") + "...");
        connect(public_sql::save_login_object(), SIGNAL(signal_telNoComingState(QString)), ui->plainTextEdit_read_tel, SLOT(appendPlainText(QString)));
    }
    ui->plainTextEdit_read_tel->appendPlainText(ui->pushButton_read_tel_test->text());
}

void w_sys_manage_outer_settings::to_read_tel_readdata()
{
    TelBox_thread_event::readTel();
}

void w_sys_manage_outer_settings::toLabelDishCheckUn()
{
    lds_model_sqltablemodel *tablemodel = static_cast<lds_model_sqltablemodel*>(ui->tableView_dish_subtype->model());
    if(tablemodel)
        for(int row = 0; row < tablemodel->rowCount(); row ++)
            tablemodel->model_data_set(row, "ch_labelprint_flag", "N");
}

void w_sys_manage_outer_settings::toLabelDishCheckAll()
{
    lds_model_sqltablemodel *tablemodel = static_cast<lds_model_sqltablemodel*>(ui->tableView_dish_subtype->model());
    if(tablemodel)
        for(int row = 0; row < tablemodel->rowCount(); row ++)
            tablemodel->model_data_set(row, "ch_labelprint_flag", "Y");
}

void w_sys_manage_outer_settings::tofuping_4_3cun(int index)
{
    if(ui->comboBox_fuping_mode->comusrdata(index).toInt() == 1) {//4.3吋客显
        ui->comboBox_fuping_size->setCurrentIndex(0);
        ui->comboBox_fuping_rotate->setCurrentIndex(2);
        ui->comboBox_fuping_direct->setCurrentIndex(0);
    }
}

void w_sys_manage_outer_settings::tofuping_screen_index(int index)
{
    switch(index) {
    case 0://副屏
        ui->comboBox_screen_primary_size->hide();
        ui->comboBox_fuping_size->show();
        break;
    case 1://主屏
        ui->comboBox_screen_primary_size->show();
        ui->comboBox_fuping_size->hide();
        break;
    }
}

void w_sys_manage_outer_settings::toEmailSet()
{
    w_sys_manage_sendEmail dialog(this);
    if(QDialog::Accepted ==  transparentCenterDialog(&dialog).exec()) {

    }
}

void w_sys_manage_outer_settings::toshowkitchen_print()
{
    w_bt_dish_kitchen_dish dialog(this);
    dialog.setWindowTitle(QObject::tr("厨房打印机"));
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.move(0, 0);
    dialog.refresh();
    dialog.exec();
}

void w_sys_manage_outer_settings::toshowkitchen_view()
{
    w_bt_dish_kitchen_view dialog(this);
    dialog.setWindowTitle(QObject::tr("厨房打印机"));
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    dialog.move(0, 0);
    dialog.refresh();
    dialog.exec();
}

void w_sys_manage_outer_settings::to_label_printer_win()
{
    w_sys_manage_outer_settings_label_win dialog(this);
    transparentCenterDialog(&dialog).exec();
}

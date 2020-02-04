#include "qt_xiangshan_main.h"
#include "ui_qt_xiangshan_main.h"
#include "lds.h"
#include "ccb_pay_help.h"
#include "lds_messagebox.h"
#include "qt_xiangshan_pay.h"
#include "transparentcenterdialog.h"
#include "qt_xiangshan.h"
#include <QProcess>
#include <QDir>
#include "qt_xiangshan_setting.h"

qt_XIANGSHAN_main::qt_XIANGSHAN_main(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::qt_XIANGSHAN_main)
{
    ui->setupUi(this);
    lds::setwflagFrameless(this);
#ifdef QT_DEBUG
    resize(1024, 768);
#else
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    resize(lds::FULL_WINDOW_SIZE);
#endif
    pay_info_widget = 0;
    adv_show_cur = 0;//cur
    QDir().mkpath(lds::localdataPath +  "/image_double_screen_dir" );

    model =new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList() << "" << "" << "" << ""<< "");
    //
    int w = this->width();
    int dish_width = w * 2 / 3;
    int dish_width_6 = dish_width / 6;
    int left_width = w - w * 2 / 3;
    ui->widget_scale_led->setGeometry(0, 0, w, 120);

    ui->label_shop_title->setGeometry(0, 120, left_width, 30);
    ui->label_state_title->setGeometry(left_width, 120, dish_width_6, 30);
    ui->label_dishname_title->setGeometry(left_width + dish_width_6, 120, dish_width_6 * 2, 30);
    ui->label_dishnum_title->setGeometry(left_width + dish_width_6 * 3, 120, dish_width_6, 30);
    ui->label_dishprice_title->setGeometry(left_width + dish_width_6 * 4, 120, dish_width_6, 30);
    ui->label_dishtotal_title->setGeometry(left_width + dish_width_6 * 5, 120, w - left_width - dish_width_6 * 5, 30);

    ui->label_shop->setGeometry(0, 120 + 30, left_width, this->height() - 120 - 30 -30);
    ui->lineEdit_ip->setGeometry(0, this->height() - 30, left_width,  30);
    ui->pushButton_shutdown->setGeometry(left_width - 30 - 30, this->height() - 30, 30, 30);
    ui->pushButton_settings->setGeometry(left_width - 30, this->height() - 30, 30, 30);
    ui->tableView_dish->setGeometry(left_width, 120 + 30, dish_width, this->height() - 120 - 30);

    //
    ui->tableView_dish->horizontalHeader()->hide();
    ui->tableView_dish->verticalHeader()->hide();
    ui->tableView_dish->setShowGrid(false);
    ui->tableView_dish->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView_dish->setSelectionMode(QTableView::SingleSelection);
    ui->tableView_dish->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_dish->setAlternatingRowColors(true);
    ui->tableView_dish->setModel(model);
    ui->tableView_dish->setColumnWidth(0, ui->label_state_title->width());
    ui->tableView_dish->setColumnWidth(1, ui->label_dishname_title->width());
    ui->tableView_dish->setColumnWidth(2, ui->label_dishprice_title->width());
    ui->tableView_dish->setColumnWidth(3, ui->label_dishnum_title->width());
    ui->tableView_dish->setColumnWidth(4, ui->label_dishtotal_title->width());
    ui->tableView_dish->setItemDelegate(new itemDelegate(this));
    ui->tableView_dish->verticalHeader()->setDefaultSectionSize(50);
    ui->tableView_dish->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //
    ui->tableView_m->setGeometry(0, 0, this->width(), this->height());
    ui->tableView_m->initScene(lds::localdataPath +  "/image_double_screen_dir");
    lds::hideWidget(ui->tableView_m);

    ui->widget_scale_led->appendCell("num_num", QObject::tr("重量"), "");
    ui->widget_scale_led->appendCell("num_price", QObject::tr("单价"), "");
    ui->widget_scale_led->appendCell("num_total", QObject::tr("金额"), "");
    ui->widget_scale_led->appendCell("num_amount", QObject::tr("合计"), "");

    ui->widget_scale_led->setFONT_DIGIT_Family(lds::FONT_DIGIT_Family);

    //countprice_scale
    foreach(const QHostAddress &host, QNetworkInterface::allAddresses()) {
        if(host.toString().contains("192.168")) {
            ui->lineEdit_ip->setText("IP:"+ host.toString() + "D");
            break;
        }
    }
    //
    ui->label_shop->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    ui->label_shop->setPixmap(QPixmap(lds::localdataPath + "/shop_info").scaledToWidth(ui->label_shop->width(), Qt::SmoothTransformation));


    printer.setBaudRate(lds::sysconf->value("system_setting/dianzicheng_baudrate").toInt());
    printer.tryOpen(lds::sysconf->value("system_setting/dianzichengduankou").toString());

    tcp = new QTcpServer(this);
    tcp->listen(QHostAddress::Any, 9100);
    connect(tcp, SIGNAL(newConnection()), this, SLOT(toTcpNewComing()));
    //
    connect(ui->tableView_m, SIGNAL(signal_mouse_pressed()), this, SLOT(to_adv_show_cur_0()));
    connect(ui->pushButton_settings, SIGNAL(clicked()), this, SLOT(toSetting()));
    connect(ui->pushButton_shutdown, SIGNAL(clicked()), this, SLOT(toShutDown()));
    //
    QTimer::singleShot(100, this, SLOT(toScaleRead()));
}

qt_XIANGSHAN_main::~qt_XIANGSHAN_main()
{
    delete ui;
}

void qt_XIANGSHAN_main::updategetweight(const QString &w)
{
    QString num_num = w.split(".").value(0) + "." + w.split(".").value(1).leftJustified(3, '0', true);
    ui->widget_scale_led->updateCellNumber("num_num", num_num);
}

void qt_XIANGSHAN_main::toScaleRead()
{
    QWidget *w = qApp->activeWindow();
    if(w
            && QString("qt_XIANGSHAN_main") == w->metaObject()->className()) {
    } else {
        //若正在系统的前台操作
        QTimer::singleShot(100, this, SLOT(toScaleRead()));
        return;
    }


    printer.writedirect("<WEI >");
    if(printer.bytesAvailable()) {
        scale_pool += printer.readAll();
        forever {
            int k0 = scale_pool.indexOf('<');
            int k1 = scale_pool.indexOf('>', k0);
            QByteArray ins;
            if(k0 >= 0 && k1 >= 0) {
                ins = scale_pool.mid(k0, k1 - k0 + 1);
                scale_pool.remove(k0, k1 - k0 + 1);
            } else {
                break;
            }

            int index = 1;//<
            QString flag = ins.mid(index, 1);index += 1;//标志
            index += 1;//空格
            //正常称重
            if(flag == "1") {
                QString weight = ins.mid(index, 7);index += 7;//毛重
                index += 1;//空格
                QString tare = ins.mid(index, 7);index += 7;//皮重
                index += 1;//空格
                QString net = ins.mid(index, 7);index += 7;//净重
                index += 1;//空格
                QString flag_weight = ins.mid(index, 1);index += 1;//毛重是否为负
                index += 1;//空格
                QString flag_net = ins.mid(index, 1);index += 1;//净重是否为负
                index += 1;//空格
                QString flag_overload = ins.mid(index, 1);index += 1;//是否超载
                index += 1;//空格
                QString flag_zero = ins.mid(index, 1);index += 1;//是否零位
                index += 1;//空格
                QString flag_tare = ins.mid(index, 1);index += 1;//是否去皮
                index += 1;//空格
                QString flag_stable = ins.mid(index, 1);index += 1;//是否稳定

                ui->widget_scale_led->updateCellNumber("num_num", ledNet(net, flag_net));
                ui->widget_scale_led->updateCellNumber("num_total", QString().sprintf("%.02f", ui->widget_scale_led->cellNumber("num_num").toDouble() * ui->widget_scale_led->cellNumber("num_price").toDouble()));
            }
            //商品及商品名
            if(flag == "2") {
                QString price = ins.mid(index, 8);index += 8;//价格
                index += 1;//空格
                QByteArray nameGBK = ins.mid(index, k1 - index - 1);//名称
                QString name = printer_0::trans_CODEC2UTF8(nameGBK, "GBK");

                double num_price = price.toDouble() / 100;
                price = QString().sprintf("%.02f", num_price);
                if(model->rowCount() > 0
                        && model->item(model->rowCount() - 1, 0)->data(Qt::UserRole + 1).toString() == "N") {
                    updateRow(model->rowCount() - 1,
                              "N",
                              name,
                              ui->widget_scale_led->cellNumber("num_num"),
                              price,
                              QString().sprintf("%.02f", num_price * ui->widget_scale_led->cellNumber("num_num").toDouble())
                              );
                } else {
                    appendRow( "N",
                               name,
                               ui->widget_scale_led->cellNumber("num_num"),
                               price,
                               QString().sprintf("%.02f", num_price * ui->widget_scale_led->cellNumber("num_num").toDouble())
                               );
                }
                ui->widget_scale_led->updateCellNumber("num_price", price);

                printer.writedirect("1");
            }
            //结算金额
            if(flag == "3") {
                QString total = ins.mid(index, 8);//价格
                double num_amount = total.toDouble() / 100;
                QString amount = QString().sprintf("%.02f", num_amount);

                QString PAYMENT = amount;

                bool pay_success = false;
                qt_XIANGSHAN_pay dialog(PAYMENT, this);
                dialog.setWindowTitle(QObject::tr("支付") + ":" +PAYMENT);
                if(dialog.makeUrl()) {
                    QTimer::singleShot(1000, &dialog, SLOT(toCheckPaid()));
                    if(QDialog::Accepted == lds_roundeddialog_rect_align_200x100(&dialog).exec()) {
                        pay_success= true;
                    }
                }
                if(pay_success) {
                    clearAllRows();
                    show_pay_info(QObject::tr("龙支付"), PAYMENT);
                    printer.writedirect("1");
                } else {
                    printer.writedirect("0");
                }
            }

            //清屏
            if(flag == "4") {
                QString clear = ins.mid(index, 3);index += 3;//毛重
                clearAllRows();
            }
            //累计按钮
            if(flag == "5") {
                QString weight = ins.mid(index, 7);index += 7;//毛重
                index += 1;//空格
                QString tare = ins.mid(index, 7);index += 7;//皮重
                index += 1;//空格
                QString net = ins.mid(index, 7);index += 7;//净重
                index += 1;//空格
                QString flag_weight = ins.mid(index, 1);index += 1;//毛重是否为负
                index += 1;//空格
                QString flag_net = ins.mid(index, 1);index += 1;//净重是否为负
                index += 1;//空格
                QString flag_overload = ins.mid(index, 1);index += 1;//是否超载
                index += 1;//空格
                QString flag_zero = ins.mid(index, 1);index += 1;//是否零位
                index += 1;//空格
                QString flag_tare = ins.mid(index, 1);index += 1;//是否去皮
                index += 1;//空格
                QString flag_stable = ins.mid(index, 1);index += 1;//是否稳定
                index += 1;//空格
                QString total = ins.mid(index, 8);index += 8;//总金额
                index += 1;//空格


                int row = model->rowCount() - 1;
                if(row >= 0) {
                    if(model->item(row, 0)->data(Qt::UserRole + 1).toString() == "N") {
                        updateData(row, "state", "Y");
                        updateData(row, "num", ledNet(net, flag_net));
                        updateData(row, "total", ledTotal(total));//抹零到分

                        ui->widget_scale_led->updateCellNumber("num_amount",  QString().sprintf("%.02f", ui->widget_scale_led->cellNumber("num_amount").toDouble() + getData(row, "total").toDouble()));
                    }
                }
            }
            //修改单价
            if(flag == "6") {
                QString price = ins.mid(index, 8);index += 8;//单价
                index += 1;//空格
                QByteArray nameGBK = ins.mid(index, 128);index += 128;//商品
                QString name = printer_0::trans_CODEC2UTF8(nameGBK, "GBK");


                int row = model->rowCount() - 1;
                if(row >= 0) {
                    if(model->item(row, 0)->data(Qt::UserRole + 1).toString() == "N") {
                        updateData(row, "price", ledPrice(price));
                    }
                }
            }
        }
    }

    //
    if(adv_show_save_weight != ui->widget_scale_led->cellNumber("num_num")) {
        adv_show_save_weight = ui->widget_scale_led->cellNumber("num_num");
        adv_show_cur = 0;
    } else {
        adv_show_cur ++;
    }
#ifdef QT_DEBUG
    if(adv_show_cur >= 30) {//30s
#else
    if(adv_show_cur >= 30 * 10) {//30s
#endif
        lds::showWidget(ui->tableView_m);
        ui->tableView_m->raise();
        ui->tableView_m->start();
    } else {
        lds::hideWidget(ui->tableView_m);
        ui->tableView_m->stop();
    }
    //
    QTimer::singleShot(100, this, SLOT(toScaleRead()));
}

void qt_XIANGSHAN_main::toTcpNewComing()
{
    QTcpSocket *t;
    while(0 != (t = tcp->nextPendingConnection())) {
        connect(t, SIGNAL(readyRead()), this, SLOT(toTcpDataComing()));
    }
}

void qt_XIANGSHAN_main::toTcpDataComing()
{
    qt_XIANGSHAN::tcp_request_data r;
    QTcpSocket *tcp = qobject_cast<QTcpSocket *>(this->sender());
    if(tcp) {
        if(socket_file_map.find(tcp) != socket_file_map.end()) {
            FileData &filed = socket_file_map[tcp];
            if(filed.pic_size != 0) {
                filed.pic_byte += tcp->readAll();
                if(filed.pic_byte.size() == filed.pic_size) {
                    toTcpWriteFile(tcp);
                }
            }
            return;
        }

        qt_XIANGSHAN::tcp_request_data d = qt_XIANGSHAN::tcp_request_output(tcp->readAll());
        qt_XIANGSHAN::tcp_request_data_print(d);
        r.type = d.type;

        if(d.type.contains(qt_XIANGSHAN::tcp_request_type_HELLO)) {
            r.state = qt_XIANGSHAN::tcp_request_state_OK;
            tcp->write(qt_XIANGSHAN::tcp_request_input(r));
        }
        if(d.type.contains(qt_XIANGSHAN::tcp_request_type_get_parameter)) {
            r.state = qt_XIANGSHAN::tcp_request_state_OK;

            r.fuping_rotate = lds::sysconf->value("system_setting/bofangg_rote").toByteArray();
            r.fuping_size = lds::sysconf->value("system_setting/bofangg_rate").toByteArray();
            r.fuping_skin = lds::sysconf->value("system_setting/skin_name").toByteArray();

            r.scale_bundrate = lds::sysconf->value("system_setting/dianzicheng_baudrate").toByteArray();
            r.scale_port = lds::sysconf->value("system_setting/dianzichengduankou").toByteArray();

            r.ccb_BRANCHID = lds::sysconf->value("system_setting/ccb_BRANCHID").toByteArray();
            r.ccb_MERCHANTID = lds::sysconf->value("system_setting/ccb_MERCHANTID").toByteArray();
            r.ccb_POSID = lds::sysconf->value("system_setting/ccb_POSID").toByteArray();
            r.ccb_PUB = lds::sysconf->value("system_setting/ccb_PUB").toByteArray();
            r.ccb_QUPWD = lds::sysconf->value("system_setting/ccb_QUPWD").toByteArray();

            tcp->write(qt_XIANGSHAN::tcp_request_input(r));
        }
        if(d.type.contains(qt_XIANGSHAN::tcp_request_type_set_fuping)) {

            lds::sysconf->setValue("system_setting/bofangg_rote", d.fuping_rotate);
            lds::sysconf->setValue("system_setting/bofangg_rate", d.fuping_size);
            lds::sysconf->setValue("system_setting/skin_name", d.fuping_skin);

            r.state = qt_XIANGSHAN::tcp_request_state_OK;
            tcp->write(qt_XIANGSHAN::tcp_request_input(r));
            tcp->waitForBytesWritten();
            reboot();
        }
        if(d.type.contains(qt_XIANGSHAN::tcp_request_type_set_scale)) {

            lds::sysconf->setValue("system_setting/dianzicheng_baudrate", d.scale_bundrate);
            lds::sysconf->setValue("system_setting/dianzichengduankou", d.scale_port);

            printer.close();
            if(!printer.tryOpen(QString().append(d.scale_port))) {
                r.state = qt_XIANGSHAN::tcp_request_state_ERROR;
                r.msg = printer.lastError().toLocal8Bit();
            } else {
                printer.setBaudRate(d.scale_bundrate.toInt());
                r.state = qt_XIANGSHAN::tcp_request_state_OK;
            }
            tcp->write(qt_XIANGSHAN::tcp_request_input(r));
        }
        if(d.type.contains(qt_XIANGSHAN::tcp_request_type_set_ccb)) {
            r.state = qt_XIANGSHAN::tcp_request_state_OK;
            tcp->write(qt_XIANGSHAN::tcp_request_input(r));

            lds::sysconf->setValue("system_setting/ccb_BRANCHID", d.ccb_BRANCHID);
            lds::sysconf->setValue("system_setting/ccb_MERCHANTID", d.ccb_MERCHANTID);
            lds::sysconf->setValue("system_setting/ccb_POSID", d.ccb_POSID);
            lds::sysconf->setValue("system_setting/ccb_PUB", d.ccb_PUB);
            lds::sysconf->setValue("system_setting/ccb_QUPWD", d.ccb_QUPWD);
        }
        if(d.type.contains(qt_XIANGSHAN::tcp_request_type_pic_list)) {
            QByteArray pic_path_list;
            foreach(const QFileInfo &info, QDir(lds::localdataPath +  "/image_double_screen_dir").entryInfoList()) {
                if(info.fileName() == "."
                        || info.fileName() == "..")
                    continue;

                pic_path_list+= info.fileName() + ",";
                pic_path_list+= QByteArray::number(info.size()) + ",";
            }
            r.state = qt_XIANGSHAN::tcp_request_state_OK;
            r.pic_info_list = pic_path_list;
            tcp->write(qt_XIANGSHAN::tcp_request_input(r));
        }

        if(d.type.contains(qt_XIANGSHAN::tcp_request_type_pic_del)) {
            QFile file(lds::localdataPath + "/image_double_screen_dir/" + d.pic_info_del_file_name);
            if(file.remove()) {
                r.state = qt_XIANGSHAN::tcp_request_state_OK;

                ui->tableView_m->initScene(lds::localdataPath +  "/image_double_screen_dir");
            } else {
                r.state = qt_XIANGSHAN::tcp_request_state_ERROR;
                r.msg = file.errorString().toLocal8Bit();
            }
            tcp->write(qt_XIANGSHAN::tcp_request_input(r));

        }

        qt_XIANGSHAN::tcp_request_data_print(d);
        if(d.type.contains(qt_XIANGSHAN::tcp_request_type_pic_transfer)
                || d.type.contains(qt_XIANGSHAN::tcp_request_type_zip_update)
                || d.type.contains(qt_XIANGSHAN::tcp_request_type_pic_shop_info)) {
            FileData filed;
            filed.type = d.type;
            filed.pic_size = d.pic_size;
            filed.pic_byte = d.pic_byte;
            filed.pic_name = d.pic_name;
            socket_file_map.insert(tcp, filed);
            if(filed.pic_byte.size() == filed.pic_size) {
                toTcpWriteFile(tcp);
            }
        }
    }
}

void qt_XIANGSHAN_main::toTcpWriteFile(QTcpSocket *tcp)
{

    if(socket_file_map.find(tcp) == socket_file_map.end()) {
        qDebug() << __FILE__ << __LINE__ << "no filed find";
        return;
    }
    FileData &filed = socket_file_map[tcp];

    qt_XIANGSHAN::tcp_request_data r;
    r.type = filed.type;
    if(qt_XIANGSHAN::tcp_request_type_pic_transfer == filed.type) {
        QImage image;
        if(!image.loadFromData(filed.pic_byte)) {
            r.state = qt_XIANGSHAN::tcp_request_state_ERROR;
            r.msg = "ERROR";
        } else {
            image.save(lds::localdataPath +  "/image_double_screen_dir/" + filed.pic_name);
            r.state = qt_XIANGSHAN::tcp_request_state_OK;

            ui->tableView_m->initScene(lds::localdataPath +  "/image_double_screen_dir");
        }
        tcp->write(qt_XIANGSHAN::tcp_request_input(r));
    }
    if(qt_XIANGSHAN::tcp_request_type_zip_update == filed.type) {
        QFile file("tmp_update_zip");
        r.state = qt_XIANGSHAN::tcp_request_state_ERROR;
        if(file.open(QFile::WriteOnly)) {
            file.write(filed.pic_byte);
            file.close();

            QString targetpath;
#if defined(QT_DEBUG) || defined(Q_OS_WIN)
            targetpath = qApp->applicationDirPath();
#else
            targetpath = "/usr/desk";
#endif
            QProcess p;
            //解压文件 直接重写，密码解压
            p.start(QString("%1 -o -P %2 %3 -d %4")
                    .arg("unzip")
                    .arg("linpos.scpos.com")
                    .arg(file.fileName())
                    .arg(targetpath));
            if(p.waitForFinished()) {
                r.state = qt_XIANGSHAN::tcp_request_state_OK;

                system("chmod +x /usr/desk/install.sh");
                system("/usr/desk/install.sh");

                system("chmod +x /usr/desk/pos/SoftPos");
                system("chmod +x /usr/desk/pos/fastfdd");
            }
        }
        file.remove();
        tcp->write(qt_XIANGSHAN::tcp_request_input(r));
        if(r.state == qt_XIANGSHAN::tcp_request_state_OK) {
            tcp->waitForBytesWritten();
            reboot();
        }
    }
    //
    if(qt_XIANGSHAN::tcp_request_type_pic_shop_info == filed.type) {
        QImage image;
        r.state = qt_XIANGSHAN::tcp_request_state_ERROR;
        r.msg = "ERROR";
        if(!image.loadFromData(filed.pic_byte)) {
        } else {
            foreach(const QFileInfo &info, QDir(lds::localdataPath).entryInfoList()) {
                if(info.fileName() == "."
                        || info.fileName() == "..")
                    continue;
                if(info.baseName() == "shop_info") {
                    QFile::remove(info.filePath());
                }
            }
            if(image.save(lds::localdataPath +  "/shop_info." + QFileInfo(filed.pic_name).suffix())) {
                r.state = qt_XIANGSHAN::tcp_request_state_OK;
                QPixmapCache::clear();
                ui->label_shop->setPixmap(QPixmap(lds::localdataPath + "/shop_info").scaledToWidth(ui->label_shop->width(), Qt::SmoothTransformation));
                adv_show_cur = 0;
            }
        }
        tcp->write(qt_XIANGSHAN::tcp_request_input(r));

    }
    socket_file_map.remove(tcp);
}

void qt_XIANGSHAN_main::toReboot()
{
    reboot();
}

void qt_XIANGSHAN_main::to_adv_show_cur_0()
{
    adv_show_cur = 0;
}

void qt_XIANGSHAN_main::toSetting()
{
    qt_XIANGSHAN_setting dialog(this);
    dialog.get_label_shop_info_size()->setText(QString("%1x%2").arg(ui->label_shop->width()).arg(ui->label_shop->height()));
    if(QDialog::Accepted == lds_roundeddialog_rect_align_200x100(&dialog).exec()) {
        reboot();
    }
}

void qt_XIANGSHAN_main::toShutDown()
{
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确定退出"), QObject::tr("确定"), QObject::tr("取消"))) {
        qApp->quit();
    }
}

QString qt_XIANGSHAN_main::ledNet(const QString &net, const QString &flag_net)
{
    double f = net.toDouble() / 1000;
    if(flag_net == "1")
        f = -1 * f;//负数
    return QString().sprintf("%.03f",f);
}

QString qt_XIANGSHAN_main::ledPrice(const QString &price)
{
    double f = price.toDouble() / 100;
    return QString().sprintf("%.02f",f);
}

QString qt_XIANGSHAN_main::ledTotal(const QString &total)
{
    return ledPrice(total);
}

void qt_XIANGSHAN_main::appendRow(const QString &state, const QString &name, const QString &num, const QString &price, const QString &total)
{
    model->appendRow(QList<QStandardItem* >()
                     << new QStandardItem
                     << new QStandardItem(name)
                     << new QStandardItem(num)
                     << new QStandardItem(price)
                     << new QStandardItem(total)
                     );
    int row = model->rowCount() - 1;

    model->item(row, 2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    model->item(row, 3)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    model->item(row, 4)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    updateData(row, "state", state);
    updateData(row, "name", name);
    updateData(row, "num", num);
    updateData(row, "price", price);
    updateData(row, "total", total);

    ui->tableView_dish->selectRow(row);
    if(pay_info_widget)
        lds::hideWidget(pay_info_widget);
}

void qt_XIANGSHAN_main::updateRow(int row, const QString &state, const QString &name, const QString &num, const QString &price, const QString &total)
{
    model->item(row, 0)->setData(state, Qt::UserRole + 1);
    model->item(row, 0)->setText(state == "N" ? QObject::tr("未确定") : QObject::tr("确定"));
    model->item(row, 1)->setText(name);
    model->item(row, 2)->setText(num);
    model->item(row, 3)->setText(price);
    model->item(row, 4)->setText(total);

}

void qt_XIANGSHAN_main::updateData(int row, const QString key, const QString &value)
{
    if(key == "state") {
        model->item(row, 0)->setText(value == "N" ? QObject::tr("未确定") : QObject::tr("确定"));
        model->item(row, 0)->setData(value, Qt::UserRole + 1);
    }
    if(key == "name") {
        model->item(row, 1)->setText(value);
    }
    if(key == "num") {
        model->item(row, 2)->setText(value);
    }
    if(key == "price") {
        model->item(row, 3)->setText(value);
    }
    if(key == "total") {
        model->item(row, 4)->setText(value);
    }
}

QString qt_XIANGSHAN_main::getData(int row, const QString &key)
{
    if(key == "state") {
        return model->item(row, 0)->data().toString();
    }
    if(key == "name") {
        return model->item(row, 1)->text();
    }
    if(key == "num") {
        return model->item(row, 2)->text();
    }
    if(key == "price") {
        return model->item(row, 3)->text();
    }
    if(key == "total") {
        return model->item(row, 4)->text();
    }
    return "";
}

void qt_XIANGSHAN_main::clearAllRows()
{
    model->removeRows(0, model->rowCount());
    ui->widget_scale_led->updateCellNumber("num_num", "0.000");
    ui->widget_scale_led->updateCellNumber("num_price", "0.00");
    ui->widget_scale_led->updateCellNumber("num_total", "0.00");
    ui->widget_scale_led->updateCellNumber("num_amount", "0.00");
}

void qt_XIANGSHAN_main::show_pay_info(const QString &payName, const QString &payAmount)
{
    if(0 == pay_info_widget) {
        pay_info_widget =new  qt_XIANGSHAN_pay_info(ui->tableView_dish);
    }
    pay_info_widget->upatePay(payName, payAmount);
    pay_info_widget->move(lds::getParentCenterPopup(ui->tableView_dish, pay_info_widget));
    lds::showWidget(pay_info_widget, true);
}

void qt_XIANGSHAN_main::reboot()
{
    //开始
    //#ifdef QT_NO_DEBUG
    //    QString cmd =qApp->applicationDirPath()+"/fastfdd";
    //#else
    //    QString cmd =qApp->applicationDirPath() + "/" + lds::appName();
    //#endif
    QString cmd =qApp->applicationDirPath() + "/" + lds::appName();

#if defined (Q_OS_UNIX)
#ifdef QT_NO_DEBUG
    //    QString param = QString("-display Transformed:Rot%1:LinuxFb:/dev/fb1:1 -qws")
    //            .arg(lds::sysconf->value("system_setting/bofangg_rote", 0).toInt());//为逆时针旋转
    //    cmd += " "+param;
#endif
    cmd += " &";
    system(cmd.toLocal8Bit());
#else
    QProcess *p = new QProcess(public_sql::save_login);
    p->start("\"" + cmd + "\"");
#endif

    qApp->quit();
}


void itemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt;
    opt = option;
    initStyleOption(&opt, index);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    const QWidget *widget = option.widget;
#else
    const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
    const QWidget *widget = v3? v3->widget : 0;
#endif
    QStyle *style = widget? widget->style() : QApplication::style();
    if(opt.state & QStyle::State_Selected)
        painter->setPen(opt.palette.brush(QPalette::HighlightedText).color());
    else
        painter->setPen(opt.palette.brush(QPalette::Text).color());

    opt.text = opt.text + "    ";
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

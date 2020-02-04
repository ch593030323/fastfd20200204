#include "doublewidget.h"
#include "ui_doublewidget.h"
#include <QSettings>
#include <QApplication>
#include <QtDebug>
#include <QFile>
#include <QProcess>
#include <QTimer>
#include <QLabel>
#include <QGraphicsProxyWidget>
#include <QDir>
#include "lds_graphicsview.h"
#include "lds.h"
#include "erweima.h"
#include <QFontDatabase>
#include <QScrollBar>
#include "tableview_item.h"
#include "testwx_help.h"
#include "lds_thread.h"
#include "fexpandmain_paying.h"
#include "ui_fexpandmain_payover.h"
#include "cjson_help.h"

//const QString doublewidget::_dishpath = "/usr/desk";
//const QString doublewidget::_advpath = "/usr/desk";
//const QString doublewidget::_showdish = "1";
//const QString doublewidget::_skinname = "skyblue";
//const DSize doublewidget::_primaryscreensize = QSize(1024, 768);
//const DSize doublewidget::_secondaryscreensize = QSize(1024, 768);

//const QString doublewidget::_type = "0";
//const QString doublewidget::_layout = "0";
//const QString doublewidget::_secondaryscreenx = "0";

void doublewidget::showPayView(QWidget *tableview, QFrame *&pay, const lds::DOUBLE_DATA &d)
{
    if(pay) {delete pay; pay = 0;}
    //1
    Ui_fexpandmain_payover *pui = new Ui_fexpandmain_payover;
    pay = new QFrame(tableview->parentWidget());//样式表无法穿透tableview
    pui->setupUi(pay);
    int width = qMin(tableview->height(), tableview->width())*5/6;
    int height = width * 3 / 4;

    pay->setGeometry(tableview->geometry().x() + (tableview->width() - width) / 2, tableview->geometry().y() + (tableview->height() - height) / 2, width, height);
    //2
    pui->label->setText(d.p_pay_desc);
    pui->label_num_payamount->setText(d.p_pay);
    pui->label_2->setText(d.p_receive_desc);
    pui->label_num_realamount->setText(d.p_receive);
    pui->label_3->setText(d.p_change_desc);
    pui->label_num_change->setText(d.p_change);
    pui->label_desc->setText(d.p_desc);
    lds::hideWidget(pui->frame_explain);

    pay->show();
}

QString doublewidget::parentPID()
{
    return lds::systemValue(__FUNCTION__).toString();
}

QString doublewidget::_dishpath()
{
    return lds::systemValue(__FUNCTION__).toString();
}

QString doublewidget::_advpath()
{
    return lds::systemValue(__FUNCTION__).toString();
}

QString doublewidget::_showdish()
{
    return lds::systemValue(__FUNCTION__).toString();
}

QString doublewidget::_skinname()
{
    return lds::systemValue(__FUNCTION__).toString();
}

QSize doublewidget::_primaryscreensize()
{
    return lds::systemValue(__FUNCTION__).toSize();
}

QSize doublewidget::_secondaryscreensize()
{
    return lds::systemValue(__FUNCTION__).toSize();
}

QString doublewidget::_type()
{
    return lds::systemValue(__FUNCTION__).toString();
}

QString doublewidget::_layout()
{
    return lds::systemValue(__FUNCTION__).toString();
}

QString doublewidget::_secondaryscreenx()
{
    return lds::systemValue(__FUNCTION__).toString();
}

QSize doublewidget::xstringToSize(const QString &sizestr, const QString &defSize)
{
    if(sizestr.contains("x"))
        return QSize(sizestr.split("x").value(0).toInt(), sizestr.split("x").value(1).toInt());
    return QSize(defSize.split("x").value(0).toInt(), defSize.split("x").value(1).toInt());
}

QString doublewidget::sizeToxstring(const QSize &size)
{
    return QString("%1x%2").arg(size.width()).arg(size.height());
}

doublewidget::doublewidget(QSize dsize, QWidget *parent) :
    QDialog(parent),
    ui(new Ui_doublewidget),
    paywidget(0)
{
    ui->setupUi(this);
    lds::setwflagFrameless(this);
    resize(dsize);

    standmodel = new QStandardItemModel(this);
    ui->tableView->setModel(standmodel);

    //1 监听
    udpSocket= new QUdpSocket(this);
    udpSocket->bind(lds::SYS_DOUBLE_SCREEN_PORT);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    //2
    //    expand=new tableview_item(this);
    ui->tableView_m->initScene(_advpath());
    //3
    dishtimeouttimer = new QTimer(this);
    connect(dishtimeouttimer,SIGNAL(timeout()), this, SLOT(adc_timer_out()));
    connect(ui->tableView_m, SIGNAL(signal_pixmap_go()), this, SLOT(toAutoQuit()));
    //4

#ifdef QT_DEBUG
    dishtimeouttimer->setInterval(60*1000);
    ui->tableView_m->showPixmap("C:/Users/cz/Desktop/餐饮window版/fastfd_localdata/image_dish_dir_2/11102.png");
#else
    dishtimeouttimer->setInterval(60*1000);
#endif

    int FONT_ID = QFontDatabase::addApplicationFont(":/LiquidCrystal-Normal.otf");
    if(-1 != FONT_ID) {
        FONT_DIGIT_Family = QFontDatabase::applicationFontFamilies(FONT_ID).first();
    }
}

doublewidget::~doublewidget()
{
    delete ui;
    if(paywidget) delete paywidget;
}

void doublewidget::start()
{
    this->show();
    adc_timer_start();
    adc_timer_out();
}

void doublewidget::toquit()
{
    qApp->quit();
}

void doublewidget::toAutoQuit()
{
#ifdef Q_OS_UNIX
    if(false == QDir("/proc/" +doublewidget::parentPID() ).exists()) {
        qDebug() << "auto quit";
        qApp->quit();
        return;
    }
#endif
}

void doublewidget::total_settext(const QString &text)
{
    cJSON_help j(text.toLocal8Bit());
    ui->label_total->setText(cJSON_help::getcJSONvalue(j, "name").toString() + ":" + cJSON_help::getcJSONvalue(j, "value").toString());
}

void doublewidget::num_settext(const QString &text)
{
    cJSON_help j(text.toLocal8Bit());
    ui->label_num->setText(cJSON_help::getcJSONvalue(j, "name").toString() + ":" + cJSON_help::getcJSONvalue(j, "value").toString());
}

void doublewidget::scale_update(const QString &keys, const QString titles, const QString &suffixs)
{

}

void doublewidget::scale_update_numbers(const QString &numbers)
{

}

bool doublewidget::hlabels_will_update()
{
    return false;
}

void doublewidget::update_pay_request(const lds_dd::DOUBLE_DATA &data)
{
    showPayView(ui->tableView, paywidget, data);
}

void doublewidget::update_skin_request(const lds_dd::DOUBLE_DATA &data)
{
    QFile file(":/fastfd_" + data.skin_name + ".qss");
    if(file.open(QFile::ReadOnly)) {
        qApp->setStyleSheet(file.readAll());
        file.close();
    } else {
        qDebug() << file.errorString();
    }
}

void doublewidget::update_paying_request(const lds_dd::DOUBLE_DATA &data)
{
}

void doublewidget::trytoclose()
{
}

void doublewidget::processPendingDatagrams()
{
    //不显示商品
    // == lds::sysconf->value("system_setting/dd_show_dish_list", true).toBool())  return;

    QString picdishno;
    QString title_num;
    QString title_total;
    QPixmap pixmap_rbcode_begin;
    QPixmap pixmap_rbcode_end;
    bool scale_update_request = false;
    bool paying_request = false;
    int currow = -1;

    while(udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
#ifdef Q_OS_WIN
        lds_thread::msecSleep(5);//window下解决办法
#endif
        lds::DOUBLE_DATA data;
        lds::DOUBLE_DATA_output(datagram, data);

        //开始处理
        if(data.udpKey == lds::udp_close_request) {//关闭请求
            qApp->quit();
            return;
        }

        //不显示商品
        if("0" == _showdish())
            continue;

        if(data.udpKey == lds::udp_insert_request) {
            standmodel_insetdata(data.row, data.items.vch_dishname, data.items.dish_price, data.items.dish_num, data.items.dish_total, data.items.vch_other);
            currow = data.row;
            if(data.items.ch_suitflag != "Y") {//P Y N
                picdishno =data.items.ch_dishno;
            }
            if(paywidget) { delete paywidget; paywidget = 0;  }
        }
        if(data.udpKey == lds::udp_update_request) {
            standmodel_setdata(data.row,  data.items.vch_dishname, data.items.dish_price, data.items.dish_num, data.items.dish_total, data.items.vch_other);
            currow = data.row;
            if(data.items.ch_suitflag != "Y") {//P Y N
                picdishno =data.items.ch_dishno;
            }
        }
        if(data.udpKey == lds::udp_delete_all_request) {
            standmodel->removeRows(0, standmodel->rowCount());
        }
        if(data.udpKey == lds::udp_delete_request) {
            standmodel->removeRows(data.row, data.count);
            currow = data.row;
        }

        if(data.udpKey == lds::udp_rb_code_request_begin) {
            pixmap_rbcode_begin = testwx_help::get_rbcode(data.rb_title,
                                                          data.rb_wechat.toLocal8Bit(), data.rb_title_wechat,
                                                          data.rb_alipay.toLocal8Bit(), data.rb_title_alipay,
                                                          data.rb_ch_paymodeno);
        }
        if(data.udpKey == lds::udp_rb_code_request_end) {

            pixmap_rbcode_end = QPixmap(250, 250);
            QPainter p(&pixmap_rbcode_end);
            p.setPen(QColor("#ff546b"));
            p.fillRect(QRect(0, 0, pixmap_rbcode_end.width(), pixmap_rbcode_end.height()), QColor("white"));
            p.drawText(QRect(0, 0, pixmap_rbcode_end.width(), pixmap_rbcode_end.height()), Qt::AlignCenter, data.rb_title);
        }
        if(data.udpKey == lds::udp_title_request) {
            title_num = data.title_num;
            title_total = data.title_total;
        }

        if(data.udpKey == lds::udp_pay_request) {
            update_pay_request(data);
        }

        if(data.udpKey == lds::udp_m_hlabels_request) {
            if(hlabels_will_update()) {
                standmodel->setHorizontalHeaderLabels(data.m_hlabels.split(","));
            }
            lds::setSystemValue("parentPID", data.parentPID);
        }

        //scale
        if(data.udpKey == lds::udp_scale_update_request) {
            scale_update_numbers(data.scale_numbers);
            scale_update_request = true;
        }
        if(data.udpKey == lds::udp_scale_init_request) {
            scale_update(data.scale_keys, data.scale_titles, data.scale_suffixs);
        }
        //skin_name
        if(data.udpKey == lds::udp_change_skin_request) {
            update_skin_request(data);
        }
        //paying_request
        if(data.udpKey == lds::udp_paying_request) {
            update_paying_request(data);
            paying_request = true;
        }
        //font path
        if(data.udpKey == lds::udp_font_update_request) {
            int FONT_ID = QFontDatabase::addApplicationFont(data.font_path);
            if(-1 != FONT_ID) {
                qApp->setFont(ldsObjFont<QApplication>(qApp, QFontDatabase::applicationFontFamilies(FONT_ID).value(0)));
            }
        }
    }

    if(!title_num.isEmpty())  num_settext(title_num);
    if(!title_total.isEmpty())  total_settext(title_total);

    if(!title_num.isEmpty()
            || !title_total.isEmpty()
            || scale_update_request
            || paying_request) {
        adc_timer_start();
    }
    if(currow >= 0)
        ui->tableView->selectRow(currow);

    if(!picdishno.isEmpty()) {//商品图片
        //        QPixmap m(dishimagepath+"/" + picdishno);
        QPixmap m(_dishpath() + "/" + picdishno);
        if(m.isNull()) {
            ui->tableView_m->start();
        } else {
            ui->tableView_m->showPixmap(m);
        }
    }

    //显示二维码，则不会在启动广告
    if(!pixmap_rbcode_begin.isNull()) {//二维码
        dishtimeouttimer->stop();
        ui->tableView_m->showPixmap(pixmap_rbcode_begin);

        qr_code_adv_hold();
    }

    //显示二维码，则不会在启动广告
    if(!pixmap_rbcode_end.isNull()) {//二维码
        dishtimeouttimer->start();
        ui->tableView_m->showPixmap(pixmap_rbcode_end);
        qr_code_adv_hold();
    }
}


void doublewidget_delegate::keepConnect(fexpandmain_table *tableview)
{
    divColor = &tableview->divColor;
    nameColor = &tableview->nameColor;
    nameSubColor = &tableview->nameSubColor;
    border = &tableview->border;
    defColor = &tableview->defColor;
    itemBgColor = &tableview->itemBgColor;
}

void doublewidget_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 0) {
        const QRect option_rect = option.rect;
        const QStandardItemModel *model = static_cast< const QStandardItemModel *>(index.model());
        int row = index.row();
        QFont font = qApp->font();

        QRect rect1 = QRect(option_rect.x(), option_rect.y(), option_rect.width()/7.0, option_rect.height());
        QRect rect2 = QRect(option_rect.x()+option_rect.width()*1/7.0, option_rect.y()+0, option_rect.width()*4/7.0, option_rect.height()*3/5.0);
        QRect rect3 = QRect(option_rect.x()+option_rect.width()*5/7.0, option_rect.y()+0, option_rect.width()*2/7.0, option_rect.height()*3/5.0);
        QRect rect4 = QRect(option_rect.x()+option_rect.width()*1/7.0, option_rect.y()+option_rect.height()*3/5.0, option_rect.width()*5/7.0, option_rect.height()*2/5.0);

        QStringList list = model->item(row, 0)->text().split("\t");
        QString vch_dishname = list.value(0);
        QString dish_price = list.value(1);
        QString dish_num = list.value(2);
        QString dish_total = list.value(3);
        QString vch_other = list.value(4);

        //开始绘制
        if(option.state & QStyle::State_Selected) {
            painter->fillRect(option_rect, QColor("#ff546b"));
        } else {
            painter->fillRect(option.rect, *itemBgColor);
            painter->fillRect(QRect(option.rect.left(), option.rect.bottom(), option.rect.width(), 1), *divColor);
        }
        //文本
        painter->setPen(*defColor);
        font.setPixelSize(rect1.height() * 2 / 5.0);painter->setFont(font);
        painter->drawText(rect1, int(Qt::AlignCenter), QString::number(row + 1));

        font.setPixelSize(rect2.height() * 2 / 4.0);painter->setFont(font);
        painter->drawText(rect2, int(Qt::AlignLeft|Qt::AlignVCenter), vch_dishname);
        painter->drawText(rect3, int(Qt::AlignRight|Qt::AlignVCenter), dish_total);

        font.setPixelSize(rect4.height() * 2 / 3.0);painter->setFont(font);
        painter->drawText(rect4, int(Qt::AlignLeft | Qt::AlignTop), QString("%1x%2 %3").arg(dish_price).arg(dish_num).arg(vch_other));
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

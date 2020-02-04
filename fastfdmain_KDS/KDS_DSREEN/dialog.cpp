#include "dialog.h"
#include "ui_dialog.h"
#include <QPainter>
#include "lds.h"
#include "cjson_help.h"
#include "kds.h"
#include <QFile>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    lds::setwflagFrameless(this);
    this->setStyleSheet("QDialog{background-color:rgb(18, 26, 34);} QFrame{color:white;font-size:20pt;}");

    //
    model_service = new ftableview_standmodel_sql_none;
    model_service->set_row_column(8, 2);
    model_service->setParent(ui->tableView_service);
    ui->tableView_service->setModel(model_service);
    ui->tableView_service->setItemDelegate(new kds_dcreen_service_delegate(this));
    ui->tableView_service->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView_service->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView_service->horizontalHeader()->hide();
    ui->tableView_service->verticalHeader()->hide();
    ui->tableView_service->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_service->setShowGrid(false);
    ui->tableView_service->setFrameShape(QFrame::NoFrame);
    model_service->setSelectionModel(ui->tableView_service->selectionModel());

    //
    model_cooking = new ftableview_standmodel_sql_none;
    model_cooking->set_row_column(8, 4);
    model_cooking->setParent(ui->tableView_cooking);
    ui->tableView_cooking->setModel(model_cooking);
    ui->tableView_cooking->setItemDelegate(new kds_dcreen_service_delegate(this));
    ui->tableView_cooking->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView_cooking->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView_cooking->horizontalHeader()->hide();
    ui->tableView_cooking->verticalHeader()->hide();
    ui->tableView_cooking->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_cooking->setShowGrid(false);
    ui->tableView_cooking->setFrameShape(QFrame::NoFrame);
    model_cooking->setSelectionModel(ui->tableView_cooking->selectionModel());
    //
    readLabel();
    //1 监听
    udpSocket= new QUdpSocket(this);
    udpSocket->bind(lds::SYS_DOUBLE_SCREEN_PORT);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    //
    datagram2model("{\"cooking\":[\"004\",\"005\",\"008\"],\"service\":[\"005\",\"008\",\"114\"]}");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::processPendingDatagrams()
{
    while(udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
#ifdef Q_OS_WIN
        lds_thread::msecSleep(5);//window下解决办法
#endif
        datagram2model(datagram);
    }
}

void Dialog::datagram2model(const QByteArray &gram)
{
    kds::kdsddData dd;
    kds::kdsdd_datagram_resolve(dd, gram);
    if(dd.type == kds::kdsdd_request_setTitle) {
        readLabel();
    }
    if(dd.type == kds::kdsdd_request_setCooking) {
        model_cooking->setPrepareList(ftableview_standmodel_sql_none::stringlist2datalist(dd.list_cooking));
        model_cooking->toinit();
    }
    if(dd.type == kds::kdsdd_request_setService) {
        model_service->setPrepareList(ftableview_standmodel_sql_none::stringlist2datalist(dd.list_service));
        model_service->toinit();
    }
    if(dd.type == kds::kdsdd_request_setCookingService) {
        model_cooking->setPrepareList(ftableview_standmodel_sql_none::stringlist2datalist(dd.list_cooking));
        model_cooking->toinit();
        model_service->setPrepareList(ftableview_standmodel_sql_none::stringlist2datalist(dd.list_service));
        model_service->toinit();
    }
    if(dd.type == kds::kdsdd_request_close) {
        qApp->quit();
    }
    if(dd.type == kds::kdsdd_request_size) {
        this->resize(kds::xstringToSize(dd.size, qApp->desktop()->availableGeometry().size()));
    }
}

void Dialog::readLabel()
{
    //
    QMap<QString ,QString> map;
    kds::kds_second_screen_read(map);
    ui->label_company_name->setText(map.value("company_name"));
    ui->label_cooking->setText(map.value("cooking"));
    ui->label_information->setText(map.value("information"));
    ui->label_logo->setPixmap(QPixmap(map.value("logo")));
    ui->label_service->setText(map.value("service"));
    ui->label_soft_name->setText(map.value("soft_name"));
}


void Dialog::kds_dcreen_service_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString serial_no = index.data(Qt::UserRole).toString();
    painter->fillRect(option.rect, QColor(18, 26, 34));
    QFont font = painter->font();
    font.setPixelSize(option.rect.height() * 0.6);
    painter->setFont(font);
    painter->drawText(option.rect, Qt::AlignVCenter | Qt::AlignHCenter, serial_no);
}

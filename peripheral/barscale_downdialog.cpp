#include "barscale_downdialog.h"
#include <QSettings>
#include <QFile>
#include <QSettings>
#include <qdebug.h>
#include "commonheader.h"
#include "lds.h"
#include "ui_barscale_downdialog.h"

barscale_downdialog::barscale_downdialog(barscale_main_dialog *parent) :
    QDialog(parent),
    ui(new Ui_barscale_downdialog),
    _parent(parent)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setWindowTitle(QObject::tr("下载商品"));
    model = new QStandardItemModel;
    model->setHorizontalHeaderLabels(QStringList() << QObject::tr("序号") << QObject::tr("地址") << QObject::tr("端口")
                                     << QObject::tr("状态") << QObject::tr("计数") << QObject::tr("进度"));
    ui->tableView->setModel(model);
    ui->tableView->setItemDelegate(new ItemDelegate);

    timer = new QTimer(this);
    timer->setInterval(1);
    connect(timer,SIGNAL(timeout()),SLOT(updateBottomTime()));

}

barscale_downdialog::~barscale_downdialog()
{
    delete ui;
}

void barscale_downdialog::showUpdate()
{
    model->removeRows(0, model->rowCount());
    //读取数据
    //从保存路径读取配置信息
    QByteArray ba=lds::sysconf->value("barscale/netinfo").toByteArray();
    QDataStream out(&ba, QIODevice::ReadOnly);
    while(!out.atEnd()) {
        QList<QStandardItem *> items;
        QString p1, p2, p3, p4;
        out >> p1 >> p2 >> p3 >> p4;

        QStringList ipport = p3.split(":");
        QString ip, port;
        if(ipport.size() == 2) {
            ip = ipport.value(0);
            port = ipport.value(1);
        }
        if(p4 == QObject::tr("不使用")) continue;
        items <<new QStandardItem(p1); //序号
        items <<new QStandardItem(ip); //地址
        items <<new QStandardItem(port); //端口
        items <<new QStandardItem(QObject::tr("未连接")); //状态
        items <<new QStandardItem("0"); //计数
        items <<new QStandardItem("0/1"); //百分比

        model->appendRow(items);
    }
    ui->timeEdit->setTime(QTime(0,0,0));
    this->show();
}

void barscale_downdialog::updateProgressbar(const QString &step, int row)
{
    model->item(row, 5)->setText(step);
}

void barscale_downdialog::updateBottomTime()
{
    time_go++;
    ui->timeEdit->setTime(QTime().addMSecs(time_go));
}

QObject *barscale_downdialog::finishButton()
{
    return ui->pushButton;
}

void barscale_downdialog::updateButtonText(const QString &text)
{
    ui->pushButton->setText(text);
}

void barscale_downdialog::startTimer()
{
    time_go = 0;
    timer->start();
}

void barscale_downdialog::stopTimer()
{
    timer->stop();
}

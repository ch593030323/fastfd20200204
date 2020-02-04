#include "saas_guide_direct.h"
#include "ui_saas_guide_direct.h"
#include <QStateMachine>
#include <QPropertyAnimation>
#include <QSignalTransition>
#include <QScrollBar>
#include <QtDebug>
//#include "back_view_dialog.h"
#include "lds.h"

saas_guide_direct::saas_guide_direct(int ssqr, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::saas_guide_direct)
{
    ui->setupUi(this);

    //tableWidget
    lds::setHeaderResizeMode(ui->tableWidget->horizontalHeader(), QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(43);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QTableWidget::NoSelection);
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    lds::setHeaderResizeMode(ui->tableWidget_2->horizontalHeader(), QHeaderView::Stretch);
    ui->tableWidget_2->verticalHeader()->setDefaultSectionSize(43);
    ui->tableWidget_2->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget_2->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tableWidget_2->setSelectionMode(QTableWidget::NoSelection);
    ui->tableWidget_2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->tableWidget_5->verticalHeader()->setDefaultSectionSize(43);
    ui->tableWidget_5->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget_5->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tableWidget_5->setSelectionMode(QTableWidget::NoSelection);
    ui->tableWidget_5->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //machine
    QStateMachine *machine = new QStateMachine(this);
    s1 = new QState(machine);
    s2 = new QState(machine);
    s3 = new QState(machine);

    s1->assignProperty(ui->frame_2, "pos", ui->frame_2->pos());
    s1->assignProperty(ui->frame_3, "pos", ui->frame_3->pos());
    s1->assignProperty(ui->frame_4, "pos", ui->frame_4->pos());

    s2->assignProperty(ui->frame_2, "pos", ui->frame_3->pos());
    s2->assignProperty(ui->frame_3, "pos", ui->frame_2->pos());
    s2->assignProperty(ui->frame_4, "pos", ui->frame_4->pos());

    s3->assignProperty(ui->frame_2, "pos", ui->frame_4->pos());
    s3->assignProperty(ui->frame_3, "pos", ui->frame_3->pos());
    s3->assignProperty(ui->frame_4, "pos", ui->frame_2->pos());

    //QPropertyAnimation
    QPropertyAnimation *anm2 = new QPropertyAnimation(ui->frame_2, "pos", this);
    anm2->setDuration(500);
    anm2->setEasingCurve(QEasingCurve::Linear);

    QPropertyAnimation *anm3 = new QPropertyAnimation(ui->frame_3, "pos", this);
    anm3->setDuration(500);
    anm3->setEasingCurve(QEasingCurve::Linear);

    QPropertyAnimation *anm4 = new QPropertyAnimation(ui->frame_4, "pos", this);
    anm4->setDuration(500);
    anm4->setEasingCurve(QEasingCurve::Linear);

    //QSignalTransition
    QSignalTransition *trans;
    trans = s1->addTransition(ui->radioButton_2, SIGNAL(clicked()), s2);trans->addAnimation(anm2);trans->addAnimation(anm3);trans->addAnimation(anm4);
    trans = s1->addTransition(ui->radioButton_3, SIGNAL(clicked()), s3);trans->addAnimation(anm2);trans->addAnimation(anm3);trans->addAnimation(anm4);

    trans = s2->addTransition(ui->radioButton, SIGNAL(clicked()), s1);trans->addAnimation(anm2);trans->addAnimation(anm3);trans->addAnimation(anm4);
    trans = s2->addTransition(ui->radioButton_3, SIGNAL(clicked()), s3);trans->addAnimation(anm2);trans->addAnimation(anm3);trans->addAnimation(anm4);

    trans = s3->addTransition(ui->radioButton, SIGNAL(clicked()), s1);trans->addAnimation(anm2);trans->addAnimation(anm3);trans->addAnimation(anm4);
    trans = s3->addTransition(ui->radioButton_2, SIGNAL(clicked()), s2);trans->addAnimation(anm2);trans->addAnimation(anm3);trans->addAnimation(anm4);
    //machine
    switch(ssqr) {
    case 1: machine->setInitialState(s1);ui->radioButton->setChecked(true);break;
    case 2: machine->setInitialState(s2);ui->radioButton_2->setChecked(true);break;
    case 3: machine->setInitialState(s3);ui->radioButton_3->setChecked(true);break;
    default: machine->setInitialState(s1);ui->radioButton->setChecked(true);break;
    }

    machine->start();

    this->resize(550, 410);

    connect(s3, SIGNAL(propertiesAssigned()), this, SLOT(toUpdateS3()));
    connect(ui->pushButton_s3_down, SIGNAL(clicked()), this, SLOT(to_s3_down()));
    connect(ui->pushButton_s3_up, SIGNAL(clicked()), this, SLOT(to_s3_up()));
}

saas_guide_direct::~saas_guide_direct()
{
    delete ui;
}

void saas_guide_direct::toUpdateS3()
{
    if(ui->tableWidget_5->rowCount() > 0) return;

    appItem( QObject::tr("项目"),	QObject::tr("标准版"),QObject::tr("专业版"));
    appItem( QObject::tr("手牌号"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("点单叫号"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("餐桌数量"),	QObject::tr("10台"),QObject::tr("不限制"));
    appItem( QObject::tr("餐桌地图模式"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("本地餐桌预订"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("云会员、本地会员积分"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("云会员、本地会员积分兑换"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("云会员、本地会员储值"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("微会员"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("组合商品"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("库存管理"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("商品原料"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("时价商品"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("临时商品"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("商品沽清"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("商品服务费"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("商品营业员提成"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("优惠券"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("点菜宝"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("手机最大连接数"),QObject::tr("固定0"),QObject::tr("可以修改"));
    appItem( QObject::tr("双屏"),QObject::tr("内容读取云端"),QObject::tr("自定义"));
    appItem( QObject::tr("促销"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("结账打印格式"),QObject::tr("没法修改"),QObject::tr("可以修改"));
    appItem( QObject::tr("标签格式"),QObject::tr("不能修改"),QObject::tr("可以修改"));
    appItem( QObject::tr("厨房打印机数量"),QObject::tr("只能1台厨打"),QObject::tr("不限制"));
    appItem( QObject::tr("区域厨打"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("厨显"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("来电弹屏"),QObject::tr("不支持"),QObject::tr("支持"));
    appItem( QObject::tr("多语言"),QObject::tr("不支持"),QObject::tr("支持"));

    //531
    ui->tableWidget_5->setColumnWidth(0, 270);
    ui->tableWidget_5->setColumnWidth(1, 150);
    ui->tableWidget_5->setColumnWidth(2, 90);
}

void saas_guide_direct::to_s3_down()
{
    ui->tableWidget_5->verticalScrollBar()->setValue(ui->tableWidget_5->verticalScrollBar()->value()+ui->tableWidget_5->verticalScrollBar()->pageStep());
}

void saas_guide_direct::to_s3_up()
{
    ui->tableWidget_5->verticalScrollBar()->setValue(ui->tableWidget_5->verticalScrollBar()->value() - ui->tableWidget_5->verticalScrollBar()->pageStep());
}

void saas_guide_direct::appItem(const QString &p1, const QString &p2, const QString &p3)
{
    int row = ui->tableWidget_5->rowCount();
    ui->tableWidget_5->insertRow(row);
    ui->tableWidget_5->setItem(row, 0,  new QTableWidgetItem(p1));
    ui->tableWidget_5->setItem(row, 1,  new QTableWidgetItem(p2));
    ui->tableWidget_5->setItem(row, 2,  new QTableWidgetItem(p3));
}

#include "saas_guide2.h"
#include "ui_saas_guide2.h"
#include "saas_guide_direct.h"
#include "saas_pay_detail.h"
#include "saas_guide.h"
#include "saas_guide_activateterminal.h"
#include "saas_guide_hard_test.h"
#include "saas_guide_init_soft.h"
#include "saas_guide_pay.h"
#include <QtDebug>
#include <QStateMachine>
#include <QPropertyAnimation>
#include <QSignalTransition>
#include "lds.h"


saas_guide2::saas_guide2(QWidget *w, QWidget *parent) :
    QDialog(parent)
{
    init(w);
}

saas_guide2::~saas_guide2()
{
    delete ui;
}

void saas_guide2::only_show_exit()
{
    b_only_show_exit = true;
}

void saas_guide2::hide_next_btn()
{
    lds::hideWidget(ui->pushButton_next);
}

void saas_guide2::s2Assigned()
{
    if(ui->frame_cur->pos() == ui->frame_right->pos()) {
        QWidget *w_right = ui->frame_right->tabkeWidget();
        QWidget *w_cur = ui->frame_cur->tabkeWidget();

        ui->frame_cur->addWidget(w_right);
        ui->frame_left->addWidget(w_cur);
    }
    if(ui->frame_cur->pos() == ui->frame_left->pos()) {
        QWidget *w_left = ui->frame_left->tabkeWidget();
        QWidget *w_cur = ui->frame_cur->tabkeWidget();

        ui->frame_cur->addWidget(w_left);
        ui->frame_right->addWidget(w_cur);
    }

    //
    ui->frame_cur->move(pos_cur);
    ui->frame_right->move(pos_right);
    ui->frame_left->move(pos_left);

    ui->frame_cur->printWidgets();
    ui->frame_right->printWidgets();
    ui->frame_left->printWidgets();

    ui->pushButton_hardTest->hide();
    ui->pushButton_next->setText(QObject::tr("下一步"));
    if(QString("saas_guide_direct") == ui->frame_cur->curWidget()->metaObject()->className()) {
        ui->pushButton_next->setText(QObject::tr("下一步"));
        ui->pushButton_hardTest->show();
    }

    if(QString("saas_guide_hard_test") == ui->frame_cur->curWidget()->metaObject()->className()) {
        ui->pushButton_next->setText(QObject::tr("返回"));
    }

    if(QString("saas_guide_pay") == ui->frame_cur->curWidget()->metaObject()->className()
            || QString("saas_guide_pay_update") == ui->frame_cur->curWidget()->metaObject()->className()) {
        ui->pushButton_next->setText(QObject::tr("支付"));
    }

    if(QString("saas_guide_init_soft") == ui->frame_cur->curWidget()->metaObject()->className()) {
        ui->pushButton_next->setText(QObject::tr("完成"));
    }

    if(QString("saas_guide_activateTerminal") == ui->frame_cur->curWidget()->metaObject()->className()) {
        ui->pushButton_next->setText(QObject::tr("激活"));
    }
    enableBtn(true);
}

void saas_guide2::toNext()
{
    enableBtn(false);

    httpDataAutoDel httpd;
    if(QString("saas_guide_direct") == ui->frame_cur->curWidget()->metaObject()->className()) {
        httpd = saas_guide::getTerminal();
        if("-101" == httpd.code) {//未激活
            ui->frame_right->addWidget(new saas_guide_activateTerminal);
            emit signal_right();
        } else {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,
                                    QObject::tr("终端号") +":" + lds::terminalCode + "\n" + httpd.message);
            this->accept();
            return ;
        }
    }

    if(QString("saas_guide_activateTerminal") == ui->frame_cur->curWidget()->metaObject()->className()) {
        saas_guide_activateTerminal *d = qobject_cast<saas_guide_activateTerminal *>(ui->frame_cur->curWidget());
        if(d->toTerminalActive())
        {
            if(false == saas_pay_detail::isSaaSAA21())//标准版,将不会进入测试化
            {
                ui->frame_right->addWidget(new saas_guide_init_soft);
                emit signal_right();
                return;
            }
        }
        this->accept();
        return ;
    }

    if(QString("saas_guide_hard_test") == ui->frame_cur->curWidget()->metaObject()->className()) {
        emit signal_right();
    }

    if(QString("saas_guide_init_soft") == ui->frame_cur->curWidget()->metaObject()->className()) {
        saas_guide_init_soft *d = qobject_cast<saas_guide_init_soft *>(ui->frame_cur->curWidget());
        if(false == d->MySQL_isConnected()) {
            enableBtn(true);
            return;
        }
        this->accept();
    }

    if(QString("saas_guide_pay") == ui->frame_cur->curWidget()->metaObject()->className()
            || QString("saas_guide_pay_update") == ui->frame_cur->curWidget()->metaObject()->className()) {
        saas_guide_pay *d = qobject_cast<saas_guide_pay *>(ui->frame_cur->curWidget());
        if(d->pay()) {
            this->accept();
        }
        enableBtn(true);
    }
}

void saas_guide2::toPre()
{
    enableBtn(false);

    if(ui->frame_left->countWidget() > 0)
        emit signal_left();
}

void saas_guide2::toHardTest()
{
    if(QString("saas_guide_hard_test") != ui->frame_cur->curWidget()->metaObject()->className()) {
        ui->frame_left->addWidget(new saas_guide_hard_test);
        toPre();
    }
}

void saas_guide2::toExit()
{
    this->reject();
}

void saas_guide2::init(QWidget *w)
{
    ui = new Ui::saas_guide2;
    ui->setupUi(this);

    ui->frame_cur->addWidget(w);
    pos_cur = ui->frame_cur->pos();
    pos_right = ui->frame_right->pos();
    pos_left = ui->frame_left->pos();
    //machine
    QStateMachine *machine = new QStateMachine(this);
    s1 = new QState(machine);
    s2 = new QState(machine);
    s3 = new QState(machine);

    s1->assignProperty(ui->frame_cur, "pos", ui->frame_cur->pos());

    s2->assignProperty(ui->frame_right, "pos", ui->frame_cur->pos());

    s3->assignProperty(ui->frame_left, "pos", ui->frame_cur->pos());
    //QPropertyAnimation
    QPropertyAnimation *anm2 = new QPropertyAnimation(ui->frame_right, "pos", this);
    anm2->setDuration(300);
    anm2->setEasingCurve(QEasingCurve::Linear);

    QPropertyAnimation *anm3 = new QPropertyAnimation(ui->frame_left, "pos", this);
    anm3->setDuration(300);
    anm3->setEasingCurve(QEasingCurve::Linear);
    //QSignalTransition
    QSignalTransition *trans;
    trans = s1->addTransition(this, SIGNAL(signal_right()), s2);
    trans->addAnimation(anm2);
    trans = s2->addTransition(s2, SIGNAL(propertiesAssigned()), s1);

    trans = s1->addTransition(this, SIGNAL(signal_left()), s3);
    trans->addAnimation(anm3);
    trans = s3->addTransition(s3, SIGNAL(propertiesAssigned()), s1);

    //machine
    machine->setInitialState(s1);
    machine->start();

    //connect
    connect(s1, SIGNAL(propertiesAssigned()), this, SLOT(s2Assigned()));

    connect(ui->pushButton_next, SIGNAL(clicked()), this, SLOT(toNext()));
    connect(ui->pushButton_pre, SIGNAL(clicked()), this, SLOT(toPre()));
    connect(ui->pushButton_hardTest, SIGNAL(clicked()), this, SLOT(toHardTest()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toExit()));

    resize(570, 470);

    ui->pushButton_pre->hide();
    this->setWindowTitle("");
}

void saas_guide2::enableBtn(bool f)
{
    ui->pushButton_hardTest->setEnabled(f);
    ui->pushButton_next->setEnabled(f);
    ui->pushButton_pre->setEnabled(f);
    ui->pushButton_exit->setEnabled(f);

    if(b_only_show_exit) {
        ui->pushButton_exit->show();
        ui->pushButton_hardTest->hide();
        ui->pushButton_next->hide();
        ui->pushButton_pre->hide();
    }
}

#include "pay_helper_main.h"
#include "ui_pay_helper_main.h"
#include "pay_helper_input.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>

pay_helper_main::pay_helper_main(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pay_helper_main)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setFixedSize(100, 100);
    ispressed = false;
}

pay_helper_main::~pay_helper_main()
{
    delete ui;
}

void pay_helper_main::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if(ispressed) {
        painter.drawPixmap(this->rect(), QPixmap(":/rec/pay_press.png"));
    } else {
        painter.drawPixmap(this->rect(), QPixmap(":/rec/pay_release.png"));
    }
}

void pay_helper_main::mousePressEvent(QMouseEvent *event)
{
    QDialog::mousePressEvent(event);
    ispressed = true;
    ismoved = false;

    pressPos = event->globalPos() - pos();
    update();
}

void pay_helper_main::mouseReleaseEvent(QMouseEvent *event)
{
    QDialog::mouseReleaseEvent(event);
    ispressed = false;

    update();
    //
    int rx = this->x();
    int ry = this->y();

    if(this->x() < 0) {
        rx = -this->width() * 0.6;
    }
    if(this->x() + this->width() > qApp->desktop()->width()) {
        rx = qApp->desktop()->width() - this->width() + this->width() * 0.6;
    }
    if(this->y() < 0) {
        ry = -this->height() * 0.6;
    }
    if(this->y() + this->height() > qApp->desktop()->height()) {
        ry = qApp->desktop()->height() - this->height() + this->height() * 0.6;
    }

    if( this->x() != rx
            || this->y() != ry) {
        this->move(rx, ry);
    } else {
        if(false == ismoved
                && this->rect().contains(event->pos())) {
            pay_helper_input dialog(this);
            dialog.exec();
        }
    }
}

void pay_helper_main::mouseMoveEvent(QMouseEvent *event)
{
    QDialog::mouseMoveEvent(event);
    ismoved = true;
    if(ispressed) {
        move(event->globalPos() - pressPos);
    }

}

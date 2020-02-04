#include "doublewidget_normal.h"
#include "ui_doublewidget.h"
#include <QLabel>

doublewidget_normal::doublewidget_normal(QSize dsize, QWidget *parent)
    : doublewidget(dsize, parent)
{
    doublewidget_layout();
}

void doublewidget_normal::doublewidget_layout()
{
    //
    if("1" == _layout() ) {
        setStyleSheet("font-size:30pt");
        int advH = this->height() / 3;
        int titleH = ui->frame_deskinfo->sizeHint().height();

        ui->frame_deskinfo->setGeometry(0, 0, this->width(), titleH);
        ui->tableView->setGeometry(0, titleH, this->width(), this->height() - advH - titleH);
        ui->tableView_m->setGeometry(0, this->height() - advH, this->width(), advH);
    } else {//水平
        setStyleSheet("font-size:16pt");
        int dishW = qMin(1024, this->width()) / 3;
        int titleH = ui->frame_deskinfo->sizeHint().height();

        ui->frame_deskinfo->setGeometry(0, 0, dishW, titleH);
        ui->tableView->setGeometry(0, titleH, dishW, this->height() - titleH);
        ui->tableView_m->setGeometry(dishW, 0, this->width() - dishW, this->height());
    }

    ///
    ui->tableView->setProperty("outer_stylesheet", "fexpandmain_tableview");
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setShowGrid(false);
    doublewidget_delegate *d = new doublewidget_delegate;
    ui->tableView->setItemDelegate(d);
    d->keepConnect(ui->tableView);

    start();
}

void doublewidget_normal::adc_timer_start()
{
    lds::showWidget(ui->frame_deskinfo, true);
    lds::showWidget(ui->tableView, true);
    lds::showWidget(ui->tableView_m, true);

    if("1" == _layout()) {//垂直
        int advH = this->height() /3;
        ui->tableView_m->setGeometry(0, this->height() - advH, this->width(), advH);
    } else {//水平
        int dishW = qMin(1024, this->width()) / 3;
        ui->tableView_m->setGeometry(dishW, 0, this->width() - dishW, this->height());
    }
    //
    dishtimeouttimer->start();
}

void doublewidget_normal::adc_timer_out()
{
    lds::showWidget(ui->frame_deskinfo, false);
    lds::showWidget(ui->tableView, false);
    lds::showWidget(ui->tableView_m, true);
    ui->tableView_m->raise();
    //
    ui->tableView_m->setGeometry(0, 0, this->width(), this->height());
    //
    ui->tableView_m->start();
}

void doublewidget_normal::standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4)
{
    standmodel->item(row, 0)->setText(p0 + "\t" + p1 + "\t" + p2 + "\t" + p3 + "\t" + p4);
}

void doublewidget_normal::standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4)
{
    standmodel->insertRow(row, QList<QStandardItem *> ()
                          << new QStandardItem
                          );
    standmodel_setdata(row, p0, p1, p2, p3, p4);
}

void doublewidget_normal::qr_code_adv_hold()
{
    lds::showWidget(ui->frame_deskinfo, true);
    lds::showWidget(ui->tableView, true);
    lds::showWidget(ui->tableView_m, true);
}

#include "doublewidget_scale.h"
#include "ui_doublewidget.h"
#include "tableview_item.h"
#include "public_scale_led.h"
#include <QTableWidget>

doublewidget_scale::doublewidget_scale(QSize dsize, QWidget *parent)
    : doublewidget(dsize, parent)
{
    doublewidget_layout();
}

void doublewidget_scale::doublewidget_layout()
{
    widget_scale_led = new public_scale_led(this);
    widget_scale_led->setFONT_DIGIT_Family(FONT_DIGIT_Family);
    setStyleSheet("font-size:16pt");
    int dishW = qMin(1024, this->width()) / 3;
    int titleH = ui->frame_deskinfo->sizeHint().height();
    int led_h = 100;
    if(this->width() == 800) {
        led_h = 86;
    }

    widget_scale_led->setGeometry(0, 0, this->width(), led_h);
    ui->frame_deskinfo->setGeometry(0, led_h, dishW, titleH);
    ui->tableView->setGeometry(0, led_h + titleH, dishW, this->height() - titleH - led_h);
    ui->tableView_m->setGeometry(dishW, led_h, this->width() - dishW, this->height() - led_h);

    ///
    ui->tableView->setProperty("outer_stylesheet", "fexpandmain_tableview");
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setStyleSheet("border:0;background:transparent;selection-background-color: transparent;");
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setShowGrid(false);
    doublewidget_delegate *d = new doublewidget_delegate;
    ui->tableView->setItemDelegate(d);
    d->keepConnect(ui->tableView);

    start();
}

void doublewidget_scale::adc_timer_start()
{
    lds::showWidget(ui->frame_deskinfo, true);
    lds::showWidget(ui->tableView, true);
    lds::showWidget(ui->tableView_m, true);

    int dishW = qMin(1024, this->width()) / 3;
    int led_h = 100;
    if(this->width() == 800) {
        led_h = 86;
    }
    ui->tableView_m->setGeometry(dishW, led_h, this->width() - dishW, this->height() - led_h);
    //
    dishtimeouttimer->start();
}

void doublewidget_scale::adc_timer_out()
{
    lds::showWidget(ui->frame_deskinfo, false);
    lds::showWidget(ui->tableView, false);
    lds::showWidget(ui->tableView_m, true);
    ui->tableView_m->raise();
    //
    ui->tableView_m->setGeometry(0, 0, this->width(), this->height());
    ui->tableView_m->start();
}

void doublewidget_scale::standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4)
{
    standmodel->item(row, 0)->setText(p0 + "\t" + p1 + "\t" + p2 + "\t" + p3 + "\t" + p4);
}

void doublewidget_scale::standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4)
{
    standmodel->insertRow(row, QList<QStandardItem *> ()
                          << new QStandardItem
                          );
    standmodel_setdata(row, p0, p1, p2, p3, p4);
}

void doublewidget_scale::qr_code_adv_hold()
{
    lds::showWidget(ui->frame_deskinfo, true);
    lds::showWidget(ui->tableView, true);
    lds::showWidget(ui->tableView_m, true);
}

void doublewidget_scale::scale_update(const QString &keys, const QString titles, const QString &suffixs)
{
    QStringList keylist = keys.split(",");
    QStringList titlelist = titles.split(",");
    QStringList suffixlist = suffixs.split(",");

    for(int k = 0; k < keylist.count(); k++) {
        widget_scale_led->appendCell(keylist.value(k), titlelist.value(k), suffixlist.value(k));
    }
}

void doublewidget_scale::scale_update_numbers(const QString &numbers)
{
    QStringList number_list = numbers.split(",");
    for(int k = 0; k < number_list.count(); k++) {
        widget_scale_led->updateCellNumber(k, number_list[k]);
    }
}

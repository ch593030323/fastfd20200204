#include "doublewidget_1024x600.h"
#include "ui_doublewidget.h"
#include "tableview_item.h"

doublewidget_1024x600::doublewidget_1024x600(QSize dsize, QWidget *parent)
    : doublewidget(dsize, parent)
{
    blank = "    ";
    doublewidget_layout();
}

void doublewidget_1024x600::doublewidget_layout()
{
    QString fontpt = "40";
    QString totalpt = "50";
    QString vheaderh = "50";
    ui->frame_deskinfo->setStyleSheet("border:0px;");
    ui->frame_deskinfo->layout()->setSpacing(0);
    ui->frame_deskinfo->layout()->setMargin(0);

    //
    setStyleSheet(
                "font-size:" +fontpt+ "pt; background-color:black; color:white;"
                );
    ui->tableView->setStyleSheet(
                "QTableView::item{"
                "border-bottom:2px solid  white;"
                "}"
                "QTableView::item:selected{"
                "color: white;"
                "}"
                "QHeaderView::section{ "
                "min-height:"+vheaderh+"px;"
                "font-size:"+fontpt+"pt;"
                "border:0px; background-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0, y2:1, stop:0 #2b2b2b, stop:1 black);"
                "border-bottom:2px solid #ff546b;"
                "}");
    int titleH = (this->height()-vheaderh.toInt() ) / 5;
    ui->tableView->verticalHeader()->setDefaultSectionSize(titleH);
    ui->tableView->setGeometry(0, 0, this->width(), this->height() - titleH);
    ui->frame_deskinfo->setGeometry(0, this->height() - titleH, this->width(), titleH);
    ui->tableView_m->setGeometry(0, 0, this->width(), this->height());
    //
    lds::showWidget(ui->frame_deskinfo, true);
    lds::showWidget(ui->tableView, true);
    lds::showWidget(ui->tableView_m, false);

    ui->label_num->setStyleSheet("font-size:"+totalpt+"pt;");
    ui->label_total->setStyleSheet("font-size:"+totalpt+"pt;");
    //

    standmodel->setHorizontalHeaderLabels(QStringList() << "" << "" << "" << "");//QObject::tr("名称") << QObject::tr("价格") << QObject::tr("数量") << QObject::tr("金额"));
    ui->tableView->setColumnWidth(0, 400);
    int w1 = (this->width() - 400) / 3;
    ui->tableView->setColumnWidth(1, w1);
    ui->tableView->setColumnWidth(2, w1);
    ui->tableView->setColumnWidth(3, w1);
    ui->tableView->horizontalHeader()->show();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setShowGrid(false);
    ui->tableView->setFocusPolicy(Qt::NoFocus);

    ui->label_total->setAlignment(Qt::AlignRight);
    ui->label_num->setAlignment(Qt::AlignRight);


    standmodel_insetdata(0, "hello1", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello2", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello3", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello4", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello5", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello6", "12.00", "11.00", "23.00", "");
    num_settext(QObject::tr("数量100.12"));
    total_settext(QObject::tr("总计100.12"));

    //开始广告，并显示
    this->start();
}

void doublewidget_1024x600::adc_timer_start()
{
    lds::showWidget(ui->frame_deskinfo, true);
    lds::showWidget(ui->tableView, true);
    lds::showWidget(ui->tableView_m, false);
    //
    dishtimeouttimer->start();
}

void doublewidget_1024x600::adc_timer_out()
{
    qr_code_adv_hold();
    ui->tableView_m->start();
}

void doublewidget_1024x600::standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4)
{
    standmodel->item(row, 0)->setText(QString::number(row+1) + "." +p0);
    standmodel->item(row, 1)->setText(p1+blank);//单价
    standmodel->item(row, 2)->setText(p2+blank);//数量
    standmodel->item(row, 3)->setText(p3+blank);//金额
    //    standmodel->item(row, 4)->setText(p4);

    standmodel->item(row, 1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    standmodel->item(row, 2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    standmodel->item(row, 3)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

void doublewidget_1024x600::standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4)
{
    standmodel->insertRow(row, QList<QStandardItem *> ()
                          << new QStandardItem
                          << new QStandardItem
                          << new QStandardItem
                          << new QStandardItem
                          );
    standmodel_setdata(row, p0, p1, p2, p3, p4);
}

void doublewidget_1024x600::total_settext(const QString &text)
{
    doublewidget::total_settext(text);
    ui->label_total->setText(ui->label_total->text() + blank);
}

void doublewidget_1024x600::num_settext(const QString &text)
{
    doublewidget::num_settext(text);
    ui->label_num->setText(ui->label_num->text() + blank);
}

void doublewidget_1024x600::qr_code_adv_hold()
{
    lds::showWidget(ui->frame_deskinfo, false);
    lds::showWidget(ui->tableView, false);
    lds::showWidget(ui->tableView_m, true);
    ui->tableView_m->raise();
}


bool doublewidget_1024x600::hlabels_will_update()
{
    return true;
}

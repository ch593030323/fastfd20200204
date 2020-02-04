#include "doublewidget_normal_paying.h"
#include "ui_doublewidget.h"
#include "cjson_help.h"
#include "lds_label_text.h"
#include "fexpandmain_paying.h"
#include <QPushButton>
#include <QPainter>

doublewidget_normal_paying::doublewidget_normal_paying(QSize dsize, QWidget *parent)
    : doublewidget(dsize, parent)
{
    ispaying = false;
    blank = " ";
    leftspace = dsize.width() * 2 / 3;
    rightspace = dsize.width() - leftspace;
    doublewidget_layout();

}

void doublewidget_normal_paying::doublewidget_layout()
{
    setStyleSheet("font-size:15pt");
    standmodel->setHorizontalHeaderLabels(QStringList() << "" << "");//QObject::tr("名称") << QObject::tr("价格") << QObject::tr("数量") << QObject::tr("金额"));

    standmodel_insetdata(0, "hello1", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello2", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello3", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello4", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello5", "12.00", "11.00", "23.00", "");
    standmodel_insetdata(0, "hello6", "12.00", "11.00", "23.00", "");

    ui->tableView->setProperty("outer_stylesheet", "fexpandmain_tableview");
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->verticalHeader()->setDefaultSectionSize(40);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setShowGrid(false);
    ui->tableView->hideColumn(1);
    doublewidget_normal_paying_delegate *d = new doublewidget_normal_paying_delegate;
    ui->tableView->setItemDelegate(d);
    d->keepConnect(ui->tableView);

    paying_view = new fexpandmain_paying(this);
    pushbutton_bottom = new QPushButton(this);
    pushbutton_bottom->setProperty("outer_stylesheet", "pushbutton_bottom");
    pushbutton_bottom->setFocusPolicy(Qt::NoFocus);
    pushbutton_bottom->setText("Thank You Please Come Again");
    //开始广告，并显示
    this->start();
}

void doublewidget_normal_paying::adc_timer_start()
{
    lds::showWidget(ui->frame_deskinfo, false);
    lds::showWidget(ui->tableView, true);
    lds::showWidget(ui->tableView_m, true);
    lds::showWidget(paying_view, true);
    lds::showWidget(pushbutton_bottom, true);


    ui->tableView_m->setGeometry(0, 0, leftspace, this->height() - 30);
    ui->tableView->setGeometry(leftspace, 0, rightspace, this->height() - 60 * 4 - 30 - 0);
    paying_view->setGeometry(leftspace, ui->tableView->geometry().bottom(), rightspace,  60 * 4);
    pushbutton_bottom->setGeometry(0, this->height() - 30, this->width(), 30);
    //
    dishtimeouttimer->start();
}

void doublewidget_normal_paying::adc_timer_out()
{
    qr_code_adv_hold();
    ui->tableView_m->start();
}

void doublewidget_normal_paying::standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4)
{
    standmodel->item(row, 0)->setText(p0);
    standmodel->item(row, 1)->setText(p3);//金额
    ui->tableView->update(standmodel->index(row, 0));

}

void doublewidget_normal_paying::standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4)
{
    standmodel->insertRow(row, QList<QStandardItem *> ()
                          << new QStandardItem
                          << new QStandardItem
                          );
    standmodel_setdata(row, p0, p1, p2, p3, p4);
}

void doublewidget_normal_paying::qr_code_adv_hold()
{
    lds::showWidget(ui->frame_deskinfo, false);
    lds::showWidget(ui->tableView, false);
    lds::showWidget(ui->tableView_m, true);
    lds::showWidget(paying_view, false);
    lds::showWidget(pushbutton_bottom, false);

    ui->tableView_m->setGeometry(0, 0, this->width(), this->height());
    ui->tableView_m->raise();
}

void doublewidget_normal_paying::total_settext(const QString &text)
{
    cJSON_help j(text.toLocal8Bit());
    if(cJSON_help::getcJSONvalue(j, "no").toString() == "order")
        paying_view->setMasterValue("consum", cJSON_help::getcJSONvalue(j, "name").toString(), cJSON_help::getcJSONvalue(j, "value").toString());
}

void doublewidget_normal_paying::num_settext(const QString &text)
{
    cJSON_help j(text.toLocal8Bit());
    paying_view->setMasterValue("qty", cJSON_help::getcJSONvalue(j, "name").toString(), cJSON_help::getcJSONvalue(j, "value").toString());
}

void doublewidget_normal_paying::update_pay_request(const lds_dd::DOUBLE_DATA &data)
{
    paying_view->setAllControl2Zero();
}

void doublewidget_normal_paying::update_skin_request(const lds_dd::DOUBLE_DATA &data)
{
    doublewidget::update_skin_request(data);
    paying_view->updateStyle(data.skin_name);
}

void doublewidget_normal_paying::update_paying_request(const lds_dd::DOUBLE_DATA &data)
{
    cJSON_help j(data.paying_master.toLocal8Bit());
    paying_view->setMasterValue(cJSON_help::getcJSONvalue(j, "no").toString(), cJSON_help::getcJSONvalue(j, "name").toString(), cJSON_help::getcJSONvalue(j, "value").toString());
}

void doublewidget_normal_paying_delegate::keepConnect(fexpandmain_table *tableview)
{
    divColor = &tableview->divColor;
    nameColor = &tableview->nameColor;
    nameSubColor = &tableview->nameSubColor;
    border = &tableview->border;
    defColor = &tableview->defColor;
    itemBgColor = &tableview->itemBgColor;
}

void doublewidget_normal_paying_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int row = index.row();
    int column = index.column();
    if(column == 0) {
        QStyleOptionViewItemV4 opt;
        opt = option;
        initStyleOption(&opt, index);
        const QStandardItemModel *model = static_cast< const QStandardItemModel *>(index.model());
        const QRect option_rect = opt.rect;

        //开始绘制
        if(opt.state & QStyle::State_Selected) {
            painter->fillRect(option_rect, QColor("#ff546b"));
        } else {
            painter->fillRect(option.rect, *itemBgColor);
            painter->fillRect(QRect(option.rect.left(), option.rect.bottom(), option.rect.width(), 1), *divColor);
            if(column == 0) {
                painter->fillRect(QRect(option.rect.left(), option.rect.top(), 1, option.rect.height()), *divColor);
            }
        }
        //文本
        QFontMetrics fm(painter->font());
        QString sqr = QString::number(row + 1) + ".";
        int sqr_width = fm.width("99.");
        QString dishname = model->item(row, 0)->text();
        QString dishtotal = model->item(row, 1)->text();
        painter->setPen(*defColor);
        painter->drawText(QRect(option_rect.x(), option_rect.y(), sqr_width, option_rect.height()), Qt::AlignRight | Qt::AlignVCenter, sqr);
        painter->drawText(option_rect.adjusted(sqr_width, 0, -fm.width(dishtotal), 0), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, dishname);
        painter->drawText(option_rect.adjusted(0, 0, -lds_label_text::borderPix, 0), Qt::AlignRight | Qt::AlignVCenter, dishtotal);
        return;
    }
    QStyledItemDelegate::paint(painter, option, index);
}

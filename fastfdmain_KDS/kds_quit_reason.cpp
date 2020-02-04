#include "kds_quit_reason.h"
#include "ui_kds_quit_reason.h"
#include <QPainter>
#include "kds.h"
#include "lds.h"

kds_quit_reason::kds_quit_reason(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kds_quit_reason)
{
    ui->setupUi(this);
    this->setWindowTitle(QObject::tr("退货原因"));
    //
    model = new ftableview_standmodel_sql_none;
    model->set_row_column(3, 3);
    model->setParent(ui->tableView);
    ui->tableView->setModel(model);
    ui->tableView->setItemDelegate(new kds_quit_reason_delegate(this));
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    model->setSelectionModel(ui->tableView->selectionModel());

    ftableview_standmodel_sql_none::noneData nodata;
    QList<ftableview_standmodel_sql_none::noneData> data_list;
    QStringList reasons = QStringList() << QObject::tr("已经制作") << QObject::tr("其他原因");
    for(int k = 0; k < reasons.count(); k ++) {
        nodata.init(QString::number(k));
        nodata.append("reason", reasons[k]);
        data_list << nodata;
    }
    model->setPrepareList(data_list);
    model->toinit();
    //
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(totableclick(QModelIndex)));
}

kds_quit_reason::~kds_quit_reason()
{
    delete ui;
}

QString kds_quit_reason::reason()
{
    return ui->lineEdit->text();
}

void kds_quit_reason::took()
{
    this->accept();
}

void kds_quit_reason::tocancel()
{
    this->reject();
}

void kds_quit_reason::totableclick(const QModelIndex &index)
{
    if(index.data(Qt::UserRole + 1).toInt() == -1) {
        model->togeneratePre();
    } else if(index.data(Qt::UserRole + 1).toInt() == -2) {
        model->togenerateNext();
    } else {
        QString reason = ftableview_standmodel_sql_none::value(index, "reason").toString();
        ui->lineEdit->setText(reason);
    }
}


void kds_quit_reason::kds_quit_reason_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString key = ftableview_standmodel_sql_none::keyvalue(index);
    QString reason = ftableview_standmodel_sql_none::value(index, "reason").toString();

    QRect rect = option.rect;
    painter->setPen(QColor("white"));
    painter->fillRect(rect, QColor(18, 26, 34));
    rect = lds::indexRect(rect, index, 5);
    painter->fillRect(rect, QColor(37, 48, 65));
    if(option.state & QStyle::State_Selected) {
        painter->fillRect(rect, QColor(kds::color_prepared_warn));
    }
    //
    if(key == "-1") {
        QPixmap p(":/image/widgets_pic/arrow_white_up.png");
        p = p.scaledToHeight(rect.height() * 0.7);
        painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, p.size(), option.rect), p);
    } else if(key == "-2") {
        QPixmap p(":/image/widgets_pic/arrow_white_down.png");
        p = p.scaledToHeight(rect.height() * 0.7);
        painter->drawPixmap(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, p.size(), option.rect), p);
    } else if(key.count() > 0) {
        painter->drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, reason);
    }
}

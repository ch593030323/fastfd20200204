#include "lds_tableview.h"
#include <QItemSelectionModel>
#include <QDebug>
#include <QHeaderView>
#include <QScrollBar>
#include <QMouseEvent>
#include <QPainter>
#include "lds_tableview_delegate_virtual.h"
#include "lds_tableview_header.h"
#include "lds.h"
#include <QTimer>

lds_tableView::lds_tableView(QWidget *parent) :
    lds_tableview_virtual(parent)
{
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);

    stack_row.push(-1);
    stack_row.push(-1);

    this->setAlternatingRowColors(true);

    QFont f = horizontalHeader()->font();
    f.setPointSize(12);
    horizontalHeader()->setFont(f);

    this->setItemDelegate(new lds_tableview_delegate_virtual(this));

    //longpress
    do_longpress = new lds_record_do_longpress(this);
    connect(do_longpress, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

lds_tableView::~lds_tableView()
{
    qDebug() << __FUNCTION__ ;
}

//!可拖动---------------end
void lds_tableView::restore(int defrow)
{
    int row = stack_row.value(0);
    if(row == -1) {
        row = stack_row.value(1);
    }
    this->selectRow(row);
    if(this->currentIndex().row() < 0 && defrow >= 0) {
        this->selectRow(defrow);
    }
}

void lds_tableView::select_beforerow()
{
    this->selectRow(stack_row.last());
}

void lds_tableView::toselectrow(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)
    emit selectChanged(current.row());
    emit selectChanged(current);
    int row = current.row();
    stack_row.push_front(row);
    stack_row.pop_back();
}

void lds_tableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
    connect(this->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(toselectrow(QModelIndex, QModelIndex)));
}

QString lds_tableView::dg_paint_transfer(const QVariant &dd, const QString &column)
{
    int c = column.toInt();
    lds_tableview_delegate_virtual *d = this->cur_item_delegate(c);

    return d->paint_transfer(dd, this->model()->headerData(c, Qt::Horizontal).toString());
}

QString lds_tableView::dg_paint_transfer(int row, int column)
{
    return dg_paint_transfer(this->model()->data(this->model()->index(row, column)), QString::number(column));
}

QString lds_tableView::hh_paint_transfer(int logicalIndex)
{
    lds_tableview_header *h = qobject_cast<lds_tableview_header *>(horizontalHeader());
    QString d;
    if(h == 0) {
        d = this->model()->headerData(logicalIndex, Qt::Horizontal).toString();
    } else {
        d = h->paint_transfer(logicalIndex);
    }
    return d;
}

lds_tableview_delegate_virtual *lds_tableView::cur_item_delegate(int column)
{
    lds_tableview_delegate_virtual *d = 0;
    if(0 == d)
        d = qobject_cast<lds_tableview_delegate_virtual *>(this->itemDelegateForColumn(column));
    if(0 == d)
        d = qobject_cast<lds_tableview_delegate_virtual *>(this->itemDelegate());
    return d;
}

void lds_tableView::setTransferHheader()
{
    QList<int> hide_column_list;
    bool hhide = false;
    for(int k = 0; k < this->horizontalHeader()->count(); k ++) {
        if(this->horizontalHeader()->isSectionHidden(k)) {
            hide_column_list << k;
        }
    }
    hhide = this->horizontalHeader()->isHidden();
    //
    lds_tableview_header *h = new lds_tableview_header(Qt::Horizontal, this);
    this->setHorizontalHeader(h);
    this->horizontalHeader()->setHidden(hhide);
    QFont f = h->font();
    f.setPointSize(12);
    h->setFont(f);
    //
    for(int k = hide_column_list.count() - 1; k >= 0; k --) {
        h->hideSection(hide_column_list[k]);
    }
}

int lds_tableView::maxDecimals(int column)
{
    lds_tableview_delegate_virtual *d = cur_item_delegate(column);
    QAbstractItemModel *m = this->model();
    return maxDecimals(d, m, column);
}

int lds_tableView::maxDecimals(lds_tableview_delegate_virtual *d, QAbstractItemModel *m, int column)
{
    int retd = 0;
    if(d != 0 && m != 0) {
        for(int row = 0; row < m->rowCount(); row++) {
            QVariant value = m->data(m->index(row, column));
            if(lds::isNumber(value)) {
                retd = qMax(retd, d->paint_transfer(value, m->headerData(column, Qt::Horizontal).toString()).split(".").value(1).length());
            }
        }
    }
    return retd;
}

int lds_tableView::maxDecimalFontWidth(QFontMetrics &fontM, int decimals)
{
    if(0 == decimals)
        return 0;

    return fontM.width(".") + fontM.width(QString(decimals, '0'));
}

void lds_tableView::resize_decimal(int column)
{
    lds_tableview_delegate_virtual *d = cur_item_delegate(column);
    QAbstractItemModel *m = this->model();
    if(0 != d) {
        d->resize_decimal(column, maxDecimals(d, m, column));
    }
}

void lds_tableView::resize_decimal_evencolumn()
{
    QAbstractItemModel *m = this->model();
    if(0 != m) {
        for(int c = 0; c < m->columnCount(); c++) {
            if(c % 2 == 1) {
                resize_decimal(c);
            }
        }
    }
}

void lds_tableView::resizeColumnsToContents(bool scan_all_hhead)
{
    resizeColumnsToContents2(scan_all_hhead);
}

void lds_tableView::resizeColumnsToContents2(bool scan_all_hhead)
{
    lds_tableview_delegate_virtual::delay_tableview_resize_column_to_content(this, rePecifyHeaderList, scan_all_hhead);
}

void lds_tableView::mousePressEvent(QMouseEvent *event)
{
    QTableView::mousePressEvent(event);
    //
    QModelIndex index = indexAt(event->pos());
    if(do_double_clicked.press_event_do_double_clicked(index)) {
        emit double_clicked(index);
        return;
    }
}

void lds_tableView::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit double_clicked(indexAt(event->pos()));
    QTableView::mouseDoubleClickEvent(event);
}

void lds_tableView::resizeColumnsToContentsDelay(int mesc)
{
    QTimer::singleShot(mesc, this, SLOT(resizeColumnsToContents2()));
}

void lds_tableView::resize_decimal_enable()
{
    lds_tableview_delegate_virtual *d = qobject_cast<lds_tableview_delegate_virtual *>(this->itemDelegate());
    d->resize_decimal_enable();
}

void lds_tableView::clearSelection()
{
    QTableView::clearSelection();
    this->setCurrentIndex(QModelIndex());
}

int lds_tableView::gonext()
{
    this->selectRow(stack_row.value(0) + 1);
    return this->currentIndex().row();
}

int lds_tableView::goprevious()
{
    this->selectRow(stack_row.value(0) - 1);
    return this->currentIndex().row();
}

void lds_tableView::resizeColumnsToContentsOrigin()
{
    QTableView::resizeColumnsToContents();
}

#include "lds_tableview_timeout.h"
#include <QMovie>
#include <QLabel>
#include <QEventLoop>
#include <QtDebug>
#include <QMouseEvent>
#include <QTimer>
#include "lds.h"

lds_tableview_timeout::lds_tableview_timeout(QWidget *parent)
    : lds_tableview_virtual(parent)
{
    is_release_clear_selection = false;
    do_longpress = new lds_record_do_longpress(this);
    connect(do_longpress, SIGNAL(timeout(QModelIndex)), this, SIGNAL(timeout(QModelIndex)));
}

void lds_tableview_timeout::enable_release_clear_selection(bool enable)
{
    is_release_clear_selection = enable;
}

void lds_tableview_timeout::enableLongPress(bool enable)
{
    do_longpress->setEnable(enable);
}

void lds_tableview_timeout::mouseMoveEvent(QMouseEvent *event)
{
    //取消移动效果
    //    QTableView::mouseMoveEvent(event);
}

void lds_tableview_timeout::mouseReleaseEvent(QMouseEvent *event)
{
    QModelIndex index  = indexAt(event->pos());

    ///一列release后，清除当前列的选区
    if(pressIndex.isValid()
            && is_release_clear_selection) {
        this->selectionModel()->select(pressIndex, QItemSelectionModel::Deselect);
    }
    ///~一列release后，清除当前列的选区

    if(do_longpress->isprepared_stop()) {
        lds_tableview_virtual::mouseReleaseEvent(event);
    }
end:
    emit released(index);
}

void lds_tableview_timeout::mousePressEvent(QMouseEvent *event)
{
    QTableView::mousePressEvent(event);
    pressIndex = this->indexAt(event->pos());
    //
    if(do_double_clicked.press_event_do_double_clicked(pressIndex)) {
        emit double_clicked(pressIndex);
        return;
    }
    //
    do_longpress->start(event->pos());
}

void lds_tableview_timeout::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit double_clicked(indexAt(event->pos()));
    QTableView::mouseDoubleClickEvent(event);
}


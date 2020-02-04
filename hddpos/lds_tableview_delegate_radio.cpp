#include "lds_tableview_delegate_radio.h"
#include <QPainter>

lds_tableview_delegate_radio::lds_tableview_delegate_radio(QObject *parent)
    : QItemDelegate(parent)
{

}

lds_tableview_delegate_radio::~lds_tableview_delegate_radio()
{

}

void lds_tableview_delegate_radio::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    QPixmap p;
    if(option.state & QStyle::State_Selected) {
        p = QPixmap(":/image/widgets_pic/radio_on.png");
    } else {
        p = QPixmap(":/image/widgets_pic/radio_off.png");
    }
    p = p.scaledToHeight(20, Qt::SmoothTransformation);
    painter->fillRect(option.rect, QColor("lightgrey"));
    painter->drawPixmap(QStyle::alignedRect(option.direction, Qt::AlignCenter, p.size(), option.rect), p);
}

QWidget *lds_tableview_delegate_radio::createEditor(QWidget */*aParent*/, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    return 0;
}

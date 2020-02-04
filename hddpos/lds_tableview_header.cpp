#include "lds_tableview_header.h"
#include <QPainter>
#include <QStyleOptionHeader>
#include "lds.h"


lds_tableview_header::lds_tableview_header(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{

}

QSize lds_tableview_header::sectionSizeFromContents(int logicalIndex) const
{
    QFontMetrics fm(this->font());
    return QSize(fm.width(paint_transfer(logicalIndex)), fm.height() + 4);
}

void lds_tableview_header::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    // get the state of the section
    QStyleOptionHeader opt;
    initStyleOption(&opt);
    QStyle::State state = QStyle::State_None;
    if (isEnabled())
        state |= QStyle::State_Enabled;
    if (window()->isActiveWindow())
        state |= QStyle::State_Active;

    // setup the style options structure
    opt.rect = rect;
    opt.section = logicalIndex;
    opt.state |= state;
    opt.textAlignment = Qt::AlignCenter;
    opt.iconAlignment = Qt::AlignVCenter;
    opt.text = paint_transfer(logicalIndex);
    QPointF oldBO = painter->brushOrigin();
    // draw the section
    style()->drawControl(QStyle::CE_Header, &opt, painter, this);

    painter->setBrushOrigin(oldBO);
}

QString lds_tableview_header::paint_transfer(int logicalIndex) const
{
    QString hkey = this->model()->headerData(logicalIndex, this->orientation()).toString();
    QString hvalue = lds::reportModelMapValue(hkey.toLower(), hkey);
    return hvalue;
}

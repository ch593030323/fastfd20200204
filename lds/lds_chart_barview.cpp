#include "lds_chart_barview.h"
#include <QScrollBar>
#include <QMouseEvent>
#include <QPainter>


lds_chart_barview::lds_chart_barview(QWidget *parent)
    : QAbstractItemView(parent)
{
    horizontalScrollBar()->setRange(0, 0);
    verticalScrollBar()->setRange(0, 0);
    horizontalScrollBar()->setValue(0);
    verticalScrollBar()->setValue(0);

    margin = 8;
    totalSize = 500;
    pieSize = totalSize - 2 * margin;
    validItems = 0;
    totalValue = 0.0;
    rubberBand = 0;

    bar_itemvaluemax = 0.0;

}

QRect lds_chart_barview::visualRect(const QModelIndex &index) const
{
    QRect rect = itemRect(index);
    if (!rect.isValid())
        return rect;

    return QRect(rect.left() - horizontalScrollBar()->value(),
                 rect.top() - verticalScrollBar()->value(),
                 rect.width(), rect.height());
}

void lds_chart_barview::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    QRect area = viewport()->rect();
    QRect rect = visualRect(index);

    if (rect.left() < area.left()) {
        horizontalScrollBar()->setValue(
                    horizontalScrollBar()->value() + rect.left() - area.left());
    } else if (rect.right() > area.right()) {
        horizontalScrollBar()->setValue(
                    horizontalScrollBar()->value() + qMin(
                        rect.right() - area.right(), rect.left() - area.left()));
    }

    if (rect.top() < area.top()) {
        verticalScrollBar()->setValue(
                    verticalScrollBar()->value() + rect.top() - area.top());
    } else if (rect.bottom() > area.bottom()) {
        verticalScrollBar()->setValue(
                    verticalScrollBar()->value() + qMin(
                        rect.bottom() - area.bottom(), rect.top() - area.top()));
    }

    update();
}

QModelIndex lds_chart_barview::indexAt(const QPoint &point) const
{
    if (validItems == 0)
        return QModelIndex();

    int wx = point.x() + horizontalScrollBar()->value();
    int wy = point.y() + verticalScrollBar()->value();

    if(QRect(
                margin-horizontalScrollBar()->value(),
                margin-verticalScrollBar()->value(),
                pieSize, pieSize).contains(point)) {
        QPoint indexpoint = QPoint(wx, wy);
        indexpoint = QPoint(indexpoint.x()-margin, (pieSize/2+margin-indexpoint.y())*-1);

        QMapIterator<int, QRect> m_int_rect(bar_index_rects_0);
        while(m_int_rect.hasNext()) {
            m_int_rect.next();
            if(m_int_rect.value().contains(indexpoint)) {
//                return model()->index(m_int_rect.key(), 1, rootIndex());
            }
        }
    } else {
        QMapIterator<int, QRect> m_int_rect(bar_index_rects_1);
        while(m_int_rect.hasNext()) {
            m_int_rect.next();
            if(m_int_rect.value().contains(point)) {
//                return model()->index(m_int_rect.key(), 0, rootIndex());
            }
        }
    }
    return model()->index(5,  0,  rootIndex());
    return QModelIndex();
}

void lds_chart_barview::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    QAbstractItemView::dataChanged(topLeft, bottomRight);

    validItems = 0;
    totalValue = 0.0;

    for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {

        QModelIndex index = model()->index(row, 1, rootIndex());
        double value = model()->data(index).toDouble();

        if (qAbs(value) != 0.0) {
            totalValue += value;
            validItems++;
        }
    }
    update_baritemvaluemax();
    viewport()->update();
}

void lds_chart_barview::rowsInserted(const QModelIndex &parent, int start, int end)
{
    for (int row = start; row <= end; ++row) {
        QModelIndex index = model()->index(row, 1, rootIndex());
        double value = model()->data(index).toDouble();

        if (qAbs(value) != 0.0) {
            totalValue += value;
            ++validItems;
        }
    }

    QAbstractItemView::rowsInserted(parent, start, end);
    update_baritemvaluemax();
}

void lds_chart_barview::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    for (int row = start; row <= end; ++row) {
        QModelIndex index = model()->index(row, 1, rootIndex());
        double value = model()->data(index).toDouble();
        if (qAbs(value) != 0.0) {
            totalValue -= value;
            --validItems;
        }
    }

    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
    update_baritemvaluemax();
}

bool lds_chart_barview::edit(const QModelIndex &index, QAbstractItemView::EditTrigger trigger, QEvent *event)
{
    if (index.column() == 0)
        return QAbstractItemView::edit(index, trigger, event);
    else
        return false;
}

QModelIndex lds_chart_barview::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    QModelIndex current = currentIndex();

    switch (cursorAction) {
    case MoveLeft:
    case MoveUp:
        if (current.row() > 0)
            current = model()->index(current.row() - 1, current.column(),
                                     rootIndex());
        else
            current = model()->index(0, current.column(), rootIndex());
        break;
    case MoveRight:
    case MoveDown:
        if (current.row() < rows(current) - 1)
            current = model()->index(current.row() + 1, current.column(),
                                     rootIndex());
        else
            current = model()->index(rows(current) - 1, current.column(),
                                     rootIndex());
        break;
    default:
        break;
    }

    viewport()->update();
    return current;
}

int lds_chart_barview::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

int lds_chart_barview::verticalOffset() const
{
    return verticalScrollBar()->value();
}

bool lds_chart_barview::isIndexHidden(const QModelIndex &index) const
{
    return false;
}

void lds_chart_barview::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    // Use content widget coordinates because we will use the itemRegion()
    // function to check for intersections.

    QRect contentsRect = rect.translated(
                horizontalScrollBar()->value(),
                verticalScrollBar()->value()).normalized();

    int rows = model()->rowCount(rootIndex());
    int columns = model()->columnCount(rootIndex());
    QModelIndexList indexes;

    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            QModelIndex index = model()->index(row, column, rootIndex());
            QRegion region = itemRegion(index);
            if (region.intersects(contentsRect))
                indexes.append(index);
        }
    }

    if (indexes.size() > 0) {
        int firstRow = indexes[0].row();
        int lastRow = indexes[0].row();
        int firstColumn = indexes[0].column();
        int lastColumn = indexes[0].column();

        for (int i = 1; i < indexes.size(); ++i) {
            firstRow = qMin(firstRow, indexes[i].row());
            lastRow = qMax(lastRow, indexes[i].row());
            firstColumn = qMin(firstColumn, indexes[i].column());
            lastColumn = qMax(lastColumn, indexes[i].column());
        }

        QItemSelection selection(
                    model()->index(firstRow, firstColumn, rootIndex()),
                    model()->index(lastRow, lastColumn, rootIndex()));
        selectionModel()->select(selection, command);
    } else {
        QModelIndex noIndex;
        QItemSelection selection(noIndex, noIndex);
        selectionModel()->select(selection, command);
    }

    update();
}

void lds_chart_barview::mousePressEvent(QMouseEvent *event)
{
    QAbstractItemView::mousePressEvent(event);
    origin = event->pos();
    if (!rubberBand)
        rubberBand = new QRubberBand(QRubberBand::Rectangle, viewport());
    rubberBand->setGeometry(QRect(origin, QSize()));
    rubberBand->show();
}

void lds_chart_barview::mouseMoveEvent(QMouseEvent *event)
{
    if (rubberBand)
        rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
    QAbstractItemView::mouseMoveEvent(event);
}

void lds_chart_barview::mouseReleaseEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseReleaseEvent(event);
    if (rubberBand)
        rubberBand->hide();
    viewport()->update();
}

void lds_chart_barview::paintEvent(QPaintEvent *event)
{

    QItemSelectionModel *selections = selectionModel();
    QStyleOptionViewItem option = viewOptions();

    QBrush background = option.palette.base();
    QPen foreground(option.palette.color(QPalette::WindowText));

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(event->rect(), background);
    painter.setPen(foreground);

    // Viewport rectangles
    QRect pieRect = QRect(margin, margin, pieSize, pieSize);

    if (validItems <= 0)
        return;

    painter.save();
    //绘制正负矩形
    painter.translate(pieRect.x() - horizontalScrollBar()->value(),
                      pieRect.y() + pieRect.height()/2.0 - verticalScrollBar()->value());
    painter.setPen(Qt::NoPen);
    int row,  valid_index;
    bar_index_rects_0.clear();
    for (row = 0, valid_index = 0; row < model()->rowCount(rootIndex()); ++row) {
        QModelIndex index = model()->index(row, 1, rootIndex());
        double value = model()->data(index).toDouble();

        if(qAbs(value) != 0.0) {
            //计算rect
            int w=(pieSize*1.0)/validItems /2;  //width
            int h=value/ bar_itemvaluemax * pieRect.height()/2.0 * -1; //height
            int x=(valid_index*1.0)/validItems* pieSize + (w*1.0)/2;//x
            int y=0;
            bar_index_rects_0.insert(row, QRect(x, y, w, h).normalized());

            QModelIndex colorIndex = model()->index(row, 0, rootIndex());
            QColor color = QColor(model()->data(colorIndex, Qt::DecorationRole).toString());

            if (currentIndex() == index)
                painter.setBrush(QBrush(color, Qt::Dense4Pattern));
            else if (selections->isSelected(index))
                painter.setBrush(QBrush(color, Qt::Dense3Pattern));
            else
                painter.setBrush(QBrush(color));

            painter.drawRect(bar_index_rects_0[row]);

            valid_index++;
        }
    }

    //坐标轴
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.drawLine(QPointF(0, -pieRect.height()/2.0),  QPointF(0, pieRect.height()/2.0));
    painter.drawLine(QPointF(0, 0),  QPointF(pieRect.width(), 0));
    painter.restore();

    //itemtext
    int keyNumber = 0;
    bar_index_rects_1.clear();

    for (row = 0; row < model()->rowCount(rootIndex()); ++row) {
        QModelIndex index = model()->index(row, 1, rootIndex());
        double value = model()->data(index).toDouble();
        if (qAbs(value) != 0.0) {
            QModelIndex labelIndex = model()->index(row, 0, rootIndex());

            QStyleOptionViewItem option = viewOptions();
            option.rect = visualRect(labelIndex);
            if (selections->isSelected(labelIndex))
                option.state |= QStyle::State_Selected;
            if (currentIndex() == labelIndex)
                option.state |= QStyle::State_HasFocus;
            itemDelegate()->paint(&painter, option, labelIndex);
            bar_index_rects_1.insert(row, option.rect);
            ++keyNumber;
        }
    }
}

void lds_chart_barview::resizeEvent(QResizeEvent *event)
{
    updateGeometries();
}

void lds_chart_barview::scrollContentsBy(int dx, int dy)
{
    viewport()->scroll(dx, dy);
}

QRegion lds_chart_barview::visualRegionForSelection(const QItemSelection &selection) const
{
    int ranges = selection.count();

    if (ranges == 0)
        return QRect();

    QRegion region;
    for (int i = 0; i < ranges; ++i) {
        QItemSelectionRange range = selection.value(i);
        for (int row = range.top(); row <= range.bottom(); ++row) {
            for (int col = range.left(); col <= range.right(); ++col) {
                QModelIndex index = model()->index(row, col, rootIndex());
                region += visualRect(index);
            }
        }
    }
    return region;
}

QRect lds_chart_barview::itemRect(const QModelIndex &index) const
{
    if (!index.isValid())
        return QRect();

    // Check whether the index's row is in the list of rows represented
    // by slices.
    QModelIndex valueIndex;

    if (index.column() != 1)
        valueIndex = model()->index(index.row(), 1, rootIndex());
    else
        valueIndex = index;

    int listItem = 0;
    for (int row = index.row()-1; row >= 0; --row) {
        double value = model()->data(model()->index(row, 1, rootIndex())).toDouble();
        if (qAbs(value) != 0.0)
            listItem++;
    }

    double itemHeight;

    switch (index.column()) {
    case 0:
        itemHeight = QFontMetrics(viewOptions().font).height();

        return QRect(totalSize,
                     int(margin + listItem*itemHeight),
                     totalSize - margin, int(itemHeight));
    case 1:
        return viewport()->rect();
    }
    return QRect();
}

QRegion lds_chart_barview::itemRegion(const QModelIndex &index) const
{

    if (!index.isValid())
        return QRegion();

    if (index.column() != 1)
        return itemRect(index);

    if(bar_index_rects_0.find(index.row()) == bar_index_rects_0.end()) {

    } else {
        return QRect(bar_index_rects_0[index.row()].topLeft()+QPoint(margin, pieSize/2+margin),
                QSize(bar_index_rects_0[index.row()].size())
                );
    }

    return QRegion();
}

int lds_chart_barview::rows(const QModelIndex &index) const
{
    return model()->rowCount(model()->parent(index));
}

void lds_chart_barview::updateGeometries()
{
    int itemHeight = QFontMetrics(viewOptions().font).height();

    int heightcount=int(margin*2 + validItems*itemHeight);

    horizontalScrollBar()->setPageStep(viewport()->width());
    horizontalScrollBar()->setRange(0, qMax(0, totalSize+totalSize/5 - viewport()->width()));
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setRange(0, qMax(0, qMax(heightcount, totalSize) - viewport()->height()));
}

void lds_chart_barview::update_baritemvaluemax()
{
    double cmptmp=0.0;
    bar_itemvaluemax=0.0;
    for (int row = rows(); row >= 0; --row) {
        cmptmp=model()->data(model()->index(row, 1, rootIndex())).toDouble();
        if(bar_itemvaluemax < qAbs(cmptmp))
            bar_itemvaluemax=qAbs(cmptmp);
    }
}

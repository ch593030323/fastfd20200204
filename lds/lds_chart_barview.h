#ifndef LDS_CHART_BARVIEW_H
#define LDS_CHART_BARVIEW_H

#include <QAbstractItemView>
#include <QRect>
//! [0]
class lds_chart_barview : public QAbstractItemView
{
    Q_OBJECT

public:
    lds_chart_barview(QWidget *parent = 0);

    QRect visualRect(const QModelIndex &index) const ;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) ;
    QModelIndex indexAt(const QPoint &point) const ;

protected slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                     const QVector<int> &roles = QVector<int>()) ;
    void rowsInserted(const QModelIndex &parent, int start, int end) ;
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) ;

protected:
    bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event) ;
    QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction,
                           Qt::KeyboardModifiers modifiers) ;

    int horizontalOffset() const ;
    int verticalOffset() const ;

    bool isIndexHidden(const QModelIndex &index) const ;

    void setSelection(const QRect&rect, QItemSelectionModel::SelectionFlags command) ;

    void mousePressEvent(QMouseEvent *event) ;

    void mouseMoveEvent(QMouseEvent *event) ;
    void mouseReleaseEvent(QMouseEvent *event) ;

    void paintEvent(QPaintEvent *event) ;
    void resizeEvent(QResizeEvent *event) ;
    void scrollContentsBy(int dx, int dy) ;

    QRegion visualRegionForSelection(const QItemSelection &selection) const ;

private:
    QRect itemRect(const QModelIndex &index) const;
    QRegion itemRegion(const QModelIndex &index) const;
    int rows(const QModelIndex &index = QModelIndex()) const;
    void updateGeometries() ;
    void update_baritemvaluemax();
    int margin;
    int totalSize;
    int pieSize;
    int validItems;

    double bar_itemvaluemax;
    QMap<int, QRect> bar_index_rects_0;
    QMap<int, QRect> bar_index_rects_1;

    double totalValue;
    QPoint origin;
    QRubberBand *rubberBand;
};
//! [0]

#endif // LDS_CHART_BARVIEW_H

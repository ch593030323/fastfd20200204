#ifndef LDS_TABLEVIEW_HEADER_H
#define LDS_TABLEVIEW_HEADER_H

#include <QHeaderView>

class lds_tableview_header : public QHeaderView {
    Q_OBJECT
public:
    lds_tableview_header(Qt::Orientation orientation, QWidget * parent = 0);

    virtual QSize sectionSizeFromContents(int logicalIndex) const;
    virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

    virtual QString paint_transfer(int logicalIndex) const ;

};

#endif // LDS_TABLEVIEW_HEADER_H

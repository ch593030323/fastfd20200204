#ifndef RESTRAURANTDELEGATE_LIST_TABLE_H
#define RESTRAURANTDELEGATE_LIST_TABLE_H

#include <QStyledItemDelegate>
#include "lds_model_sqltablemodel.h"
#include "lds_tableview_delegate_virtual.h"

class restraurantDelegate_list_table : public lds_tableview_delegate_virtual
{
    Q_OBJECT
public:
    restraurantDelegate_list_table(QObject *parent = 0);
    // painting
    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawTable(QPainter *painter, const QRect &rect, const QModelIndex &index, qreal xyRadius) const;

};

class restraurantDelegate_list_dish : public lds_tableview_delegate_virtual
{
    Q_OBJECT
public:
    restraurantDelegate_list_dish(QObject *parent = 0);
    virtual void drawText(QPainter *painter,
                          QStyle::State state,
                          const QRect &painter_text_rect, Qt::Alignment displayAlignment, const QString &painter_text) const;

};

#endif // RESTRAURANTDELEGATE_LIST_TABLE_H

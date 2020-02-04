#ifndef LDS_TABLEVIEW_DELEGATE_RADIO_H
#define LDS_TABLEVIEW_DELEGATE_RADIO_H

#include <QItemDelegate>
#include <QStyledItemDelegate>
#include "lds_tableview_delegate_virtual.h"

class lds_tableview_delegate_radio : public QItemDelegate
{
    Q_OBJECT
public:
    lds_tableview_delegate_radio(QObject *parent = 0);
    ~lds_tableview_delegate_radio();

    virtual void paint(QPainter * painter,const QStyleOptionViewItem & option, const QModelIndex & index) const;
    QWidget *createEditor(QWidget */*aParent*/, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const;

};

#endif // LDS_TABLEVIEW_DELEGATE_RADIO_H

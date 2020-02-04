#ifndef LDS_TABLEVIEW_DELEGATE_DOUBLESPINBOX_H
#define LDS_TABLEVIEW_DELEGATE_DOUBLESPINBOX_H

#include "lds_tableview_delegate_virtual.h"

class lds_tableview_delegate_doublespinbox : public lds_tableview_delegate_virtual
{
    Q_OBJECT
public:
    explicit lds_tableview_delegate_doublespinbox( QObject *parent);
    lds_tableview_delegate_doublespinbox( int decimal, QObject *parent);

    QWidget *createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    void setDecimals(int prec);
    int prec;
};

#endif // LDS_TABLEVIEW_DELEGATE_DOUBLESPINBOX_H

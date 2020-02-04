#ifndef LDS_TABLEVIEW_DELEGATE_COM_H
#define LDS_TABLEVIEW_DELEGATE_COM_H

#include <QItemDelegate>
#include <QMap>
#include <QStandardItemModel>
#include "lds_tableview_delegate_virtual.h"

//m必须有，并且为一列， Qt::UserRole+1为编号Qt::EditRole为显示的内容
class lds_tableview_delegate_com : public lds_tableview_delegate_virtual
{
    Q_OBJECT
public:
    explicit lds_tableview_delegate_com(QStandardItemModel *m, int c, QObject *parent = 0);
    explicit lds_tableview_delegate_com(const QMap<QString/*key*/, QVariant/*value*/> & map, QObject *parent = 0);

    QWidget *createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void paint(QPainter * painter,const QStyleOptionViewItem & option, const QModelIndex & index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    QStandardItemModel *typemodel;
    QMap<QString/*key*/, QVariant/*value*/> map;
    void setonlyk(bool f);
private:
    int column;
    bool _onlyk;
};

#endif // LDS_TABLEVIEW_DELEGATE_COM_H

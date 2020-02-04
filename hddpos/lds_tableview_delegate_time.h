#ifndef LDS_TABLEVIEW_DELEGATE_TIME_H
#define LDS_TABLEVIEW_DELEGATE_TIME_H
#include <QItemDelegate>
#include "lds_tableview_delegate_virtual.h"

//    tablemodel->setData(tablemodel->index(rowcount, 2), "00:00"); 时间需要手动指定
class lds_tableview_delegate_time:public lds_tableview_delegate_virtual
{
    Q_OBJECT
public:
    explicit lds_tableview_delegate_time(QVariant::Type type = QVariant::DateTime , QObject *parent = 0);
    void setDisplayFormat(QVariant::Type type);
protected:
    QWidget *createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;

private:
    QVariant dateTime2Varint(const QDateTime &dt) const;
    QString dataTime2String(const QDateTime &dt) const;
    QString dataTime2Format() const;
private:
    QVariant::Type _type;
};

#endif // LDS_TABLEVIEW_DELEGATE_TIME_H

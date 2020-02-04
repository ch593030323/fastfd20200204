#ifndef LDS_TABLEVIEW_DELEGATE_CHECK_H
#define LDS_TABLEVIEW_DELEGATE_CHECK_H
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include "lds_tableview_delegate_virtual.h"




//tableview 的代理，实现checkbox的显示






class lds_tableview_delegate_check : public lds_tableview_delegate_virtual
{
    Q_OBJECT
public:
    enum delegateType{
        just_for_column = 0,
        just_for_item
    };

public:
    explicit lds_tableview_delegate_check(QVector<int> &checkcolumn_vector, QObject *tableview, bool ischeckable=false);
    lds_tableview_delegate_check(int checkrow, int checkcolumn, QObject *tableview, bool ischeckable=false);//aftercolumn后的列都为checkbox
    lds_tableview_delegate_check(int checkcolumn, QObject *tableview, bool ischeckable=false);//aftercolumn后的列都为checkbox

    ~lds_tableview_delegate_check();
    virtual void paint(QPainter * painter,const QStyleOptionViewItem & option, const QModelIndex & index) const;
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void setClickable();
    bool clickable();
signals:
    void checkclick(const QModelIndex& index);
private slots:
    void inter_clicked(const QModelIndex& index);
private:
    QVector<int> checkcolumn_vector;
    bool ischeckable;
    int checkrow;
    int checkcolumn;
    delegateType dtype;
protected:
    QString checkkey;
    QString uncheckkey;
};

#endif // LDS_TABLEVIEW_DELEGATE_CHECK_H

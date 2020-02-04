#ifndef LDS_TABLEVIEW_SUM_H
#define LDS_TABLEVIEW_SUM_H
#include <QWidget>
#include <QStandardItemModel>
#include <QItemSelection>
#include <QItemDelegate>
#include <QPainter>
#include <QtGlobal>
#include <QAbstractItemView>
#include "lds_model_sqlstandardmodel.h"
#include "lds_tableview_delegate_virtual.h"
#include "lds_tableview_header.h"

//有汇总的tableview





class QTableView;
class lds_tableView;
class QLineEdit;
class QAbstractItemModel;
class QScrollBar;
class QHeaderView;

class lds_tableview_sum : public QWidget
{
    Q_OBJECT
public:
    explicit lds_tableview_sum(QWidget *parent = 0);
    void setModel(QAbstractItemModel *model);
    lds_tableView *tableView;//tableview 本身的函数在这里操作
    lds_tableView *lineView;

    QStringList updateSumRange(int from, int to);
    QString updateSum(int column);//用的是普通model，即model和linemodel的列数一致
    QString updateSum(int column, const QVariant &value);//用的是普通model，即model和linemodel的列数一致

   QList<int> rePecifyHeader(const QStringList &tos);
   QList<int> rePecifyHeader(const QList<int> &tos);

   void setEditTriggers(QAbstractItemView::EditTriggers triggers);
    void setSelectionBehavior(QAbstractItemView::SelectionBehavior behavior);
    void setItemDelegateForColumn(int column, QAbstractItemDelegate *delegate);
    void setSelectionMode(QAbstractItemView::SelectionMode mode);
    void setItemDelegate(QAbstractItemDelegate *delegate);
    void resizeColumnsToContents();
    QHeaderView *horizontalHeader() const;
    QHeaderView *verticalHeader() const;
    QModelIndex currentIndex() const;
    QAbstractItemDelegate *itemDelegateForColumn(int column) const;

    void setTransferHheader();
    void restore(int defrow = -1);
    void lineView_show();
private slots:
    void linemodel_remove_and_filldata();
    void updateVerticalScrollBarRange(int min, int max);
    void updateHorizontalScrollBarRange(int min, int max);

private:
    void updatelinemodel();
    void update_total_str();

    lds_model_sqlstandardmodel *linemodel;
    QAbstractItemModel *parentmodel;
    QScrollBar *horizontalScrollBar;
    QScrollBar *verticalScrollBar;

};
#endif // LDS_TABLEVIEW_SUM_H

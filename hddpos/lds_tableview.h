#ifndef LDS_TABLEVIEW_H
#define LDS_TABLEVIEW_H

#include "lds_tableview_virtual.h"
#include <QStack>
#include "lds_record_do_longpress.h"

class lds_tableview_delegate_virtual;

class lds_tableView : public lds_tableview_virtual
{
    Q_OBJECT
public:
    explicit lds_tableView(QWidget *parent = 0);
    ~lds_tableView();
    void restore(int defrow = -1);

    void select_beforerow();
    virtual void setModel(QAbstractItemModel *model);

    QString dg_paint_transfer(const QVariant &dd, const QString & column);
    QString dg_paint_transfer(int row, int column);

    QString hh_paint_transfer(int logicalIndex);
    lds_tableview_delegate_virtual *cur_item_delegate(int column);
    void setTransferHheader();

    int maxDecimals(int column);
    int maxDecimals(lds_tableview_delegate_virtual *d, QAbstractItemModel *m, int column);
    int maxDecimalFontWidth(QFontMetrics&fontM, int decimals);
    void resize_decimal(int column);
    void resize_decimal_evencolumn();

    void resize_decimal_enable();
    void resizeColumnsToContentsDelay(int mesc = 10);
public slots:
    void clearSelection();
    int gonext();
    int goprevious();

    void resizeColumnsToContentsOrigin();
    void resizeColumnsToContents(bool scan_all_hhead = true);
    void resizeColumnsToContents2(bool scan_all_hhead = true);
signals:
    void selectChanged(int crow);
    void selectChanged(const QModelIndex &index);
    //longpress
    void timeout();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
private slots:
    void toselectrow(const QModelIndex &current, const QModelIndex &previous);
private:
    int currow;
    QStack<int> stack_row;
private:
    lds_record_do_double_clicked do_double_clicked;
    lds_record_do_longpress *do_longpress;
};

#endif // LDS_TABLEVIEW_H

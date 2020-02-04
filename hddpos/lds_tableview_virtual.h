#ifndef LDS_TABLEVIEW_VIRTUAL_H
#define LDS_TABLEVIEW_VIRTUAL_H

#include <QTableView>
#include <QHeaderView>
#include <QAbstractButton>
#include "lds.h"

class lds_tableview_virtual : public QTableView
{
    Q_OBJECT
public:
    static void tableviewSetpan(QTableView *tableview, int cmp_column, const QList<int> &span_column_list, bool clearSpans = true);
    static int get_hheight();
    static int get_hscrollheight();
private:
    static int hheight;
    static int hscrollheight;
public:
    lds_tableview_virtual(QWidget *parent = 0);

    QList<int> getSelectedRowList();
    QList<int> getSelectedRowListDesc();
    QList<int> getSelectedRowListAsc();

    void setHeaderResizeMode(Qt::Orientation o, QHeaderView::ResizeMode m);

    QList<int> rePecifyHeader(const QStringList &tos);
    QList<int> rePecifyHeader(const QList<int> &tos);
    QList<int> rePecifyHeaderList;
    QList<int> horizontalShowedHeaderList();

    void setCornerText(const QString &text);
    QAbstractButton* cornerButton();

    void justShowColumns(const QList<int> &column_list);
    void justShowColumn(int column);

    void setShowNest(bool flag);

    QModelIndex model_index(int row, int column);
    QVariant model_data(int row, int column);
signals:
    void double_clicked(const QModelIndex &index);

public slots:
    void toPageUp();
    void toPageDown();
    void toSelectAll();
    void toClearAll();

protected:
    bool eventFilter(QObject *o, QEvent *e);
    void paintEvent(QPaintEvent *e);
    QAbstractButton *corner_button;

private:
    lds_record_do_double_clicked do_double_clicked;
    bool is_show_nest;

};


#endif // LDS_TABLEVIEW_VIRTUAL_H

#ifndef LDS_TABLEVIEW_DELEGATE_VIRTUAL_H
#define LDS_TABLEVIEW_DELEGATE_VIRTUAL_H

#include <QObject>
#include <QStyledItemDelegate>
#include "lds_tableview.h"

class QTableView;

class lds_tableview_delegate_virtual : public QStyledItemDelegate
{
    Q_OBJECT
public:
    static void delay_tableview_resize_column_to_content(lds_tableView *tableview, int column);
    static void delay_tableview_resize_column_to_content(lds_tableView *tableview, const QList<int> rePecifyHeaderList= QList<int>(), bool scan_all_hhead = false);
public:
    explicit lds_tableview_delegate_virtual(QObject *parent = 0);

    // painting
    void paintText(QPainter *painter,
                   const QStyleOptionViewItem &option, const QModelIndex &index, const QVariant &dd) const;
    virtual void drawText(QPainter *painter,
                          QStyle::State state,
                          const QRect &painter_text_rect, Qt::Alignment displayAlignment, const QString &painter_text) const;
    // painting
    void paintProgressBar(QPainter *painter,
                          const QStyleOptionViewItem &option, const QModelIndex &index, const QString *text = 0) const;
    // painting
    void paintBtn(QPainter *painter,
                  const QStyleOptionViewItem &option, const QModelIndex &index, const QVariant &dd, int text_color = 0) const;
    // painting
    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &option, const QModelIndex &index) const;

    virtual QWidget *createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void setModelData(const QVariant &d, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

    virtual QString paint_transfer(const QVariant &dd, const QString &field_name) const ;
    QString fieldName(const QModelIndex &index) const;

    int resize_decimal_field(const QString &field_name) const;
    void resize_decimal_enable();
    void resize_decimal(int column, int maxdecimals);
    int resize_decimal_max(int column) const;

    void disable_paint_transfer();
signals:
    void signal_edit_commit(const QModelIndex &index) const;
protected:
    bool b_paint_transfer; //直接返回key
private:
    int resize_decimal_mode;
    QMap<int, int> resize_decimal_map;

    int bp_fmt_price_dec;
    int bp_fmt_num_dec;
    int bp_fmt_amount_dec;

private:
    class LayoutDirectionSaver
    {
    public:
        LayoutDirectionSaver(QPainter *painter, Qt::LayoutDirection will_dir);
        LayoutDirectionSaver();
        ~LayoutDirectionSaver();
        void keep(QPainter *painter, Qt::LayoutDirection will_dir);
    private:
        QPainter*painter;
        Qt::LayoutDirection old_dir;
    };
};

#endif // LDS_TABLEVIEW_DELEGATE_VIRTUAL_H

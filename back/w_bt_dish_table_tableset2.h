#ifndef W_BT_DISH_TABLE_TABLESET2_H
#define W_BT_DISH_TABLE_TABLESET2_H

#include <QDialog>
#include <QPushButton>
#include <QMap>
#include <QVariant>
#include "lds_tableview_delegate_virtual.h"
class lds_model_sqltablemodel;

namespace Ui {
class w_bt_dish_table_tableset2;
}

class w_bt_dish_table_tableset2 : public QDialog
{
    Q_OBJECT
public:
    class restraurantDelegate : public QStyledItemDelegate
    {
    public:
        restraurantDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}
    protected:
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option, const QModelIndex &index) const;

        //
        QWidget *createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;

        //
        QString curText(const QModelIndex &index) const;

        //
        void drawButton(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void drawText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };

public:
    explicit w_bt_dish_table_tableset2(QWidget *parent = 0);
    ~w_bt_dish_table_tableset2();

private slots:
    void took();
    void toexit();
    void todel();
    void tonew();
    void toServiceLowMode(const QModelIndex &index);
    void torefresh();
private:
    bool saveData();
private:
    Ui::w_bt_dish_table_tableset2 *ui;
    lds_model_sqltablemodel *tablemodel;
};

#endif // W_BT_DISH_TABLE_TABLESET2_H

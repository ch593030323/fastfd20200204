#ifndef W_BT_DISH_TABLE_TABLESET2_SET_H
#define W_BT_DISH_TABLE_TABLESET2_SET_H

#include <QDialog>
#include <QMap>
#include <QVariant>

namespace Ui {
class w_bt_dish_table_tableset2_set;
}

class w_bt_dish_table_tableset2_set : public QDialog
{
    Q_OBJECT

public:
    explicit w_bt_dish_table_tableset2_set(const QMap<QString, QVariant> &map, QWidget *parent = 0);
    ~w_bt_dish_table_tableset2_set();

    QMap<QString, QVariant> map;
private slots:
    void toUpdateLowMode();
    void toUpdateServiceMode();

    void took();
    void toexit();

    void showRow(const QWidget *widget, bool show);
private:
    void setWidgetValue(const QString &key, const QVariant &value);
private:
    Ui::w_bt_dish_table_tableset2_set *ui;
};

#endif // W_BT_DISH_TABLE_TABLESET2_SET_H

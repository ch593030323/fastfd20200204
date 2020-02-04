#ifndef FRONTVIEWDIALOG_VIRTUAL_H
#define FRONTVIEWDIALOG_VIRTUAL_H

#include <QDialog>
#include <QStandardItemModel>
#include <QDateTime>
#include <QTimer>
#include <QItemSelectionModel>
#include <QTcpSocket>
#include "lds.h"

class frontViewDialog_virtual : public QObject
{
    Q_OBJECT
public:
    explicit frontViewDialog_virtual(QObject *parent = 0);

    ~frontViewDialog_virtual();
    /**
     * 客显--------------------------begin*/
public:

    static void static_display_dish(const QString &dishname, double num, double price, double total);
    static void static_display_cash(double value);
    static void static_display_change(double value);
    static void static_display_clear();
private:
    static void static_display_num(double value);
    static void static_display_price(double value, const QString &vch_dishname);
    static void static_display_total(double value);

signals:
    void signal_payfor_info(const QString &ch_payno,int paytype);          //结账单信息
public:
    /**
     * 点菜--------------------------end*/
};

#endif // FRONTVIEWDIALOG_VIRTUAL_H

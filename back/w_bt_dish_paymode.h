#ifndef W_BT_DISH_PAYMODE_H
#define W_BT_DISH_PAYMODE_H

#include <QDialog>
#include "w_bt_dish.h"
#include "lds_model_sqltablemodel.h"
#include "mobile_pay_help.h"
#include "testwx_help.h"

class Ui_w_bt_dish_paymode_dialog;
class lds_tableView;
class QDataWidgetMapper;
class Ui_w_bt_dish_paymode_set;

class w_bt_dish_paymode : public QDialog
{
    Q_OBJECT
public:
    class paymodetablemodel : public lds_model_sqltablemodel{
    public:
        paymodetablemodel(QObject *parent = 0);
        virtual ~paymodetablemodel();
        QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    };

    struct PayModeData{
        QString ch_paymodeno;
        QString ch_IDflag;
        QString ch_faceflag;
        QString ch_changeflag;
        QString ch_incomeflag;
        QString vch_arg4;
        QString vch_paymodename;
        QString vch_english;
    };

    static void update_vch_english();
    static void update_tanslate();
    static void fille_paylist(QList<PayModeData> &paylist);
public:
    explicit w_bt_dish_paymode(QWidget *parent = 0);

private slots:
    void tonew2();
    void tonew();
    void todel();
    void torefresh();
    bool tosave();
    void toexit();
    void toarg();
    void mapper_setcurindex(int index);
    void todef();
    void tosave_unitprefix(const QString &text);
private:
    Ui_w_bt_dish_paymode_dialog *ui;
    paymodetablemodel *tablemodel;
    QDataWidgetMapper *mapper;

};

class Ui_w_bt_dish_unitset_Dialog;
class lds_menu;
class w_bt_dish_paymode_set : public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_paymode_set(const QString &ch_paymodeno, QWidget *parent = 0);
    ~w_bt_dish_paymode_set();
private slots:
    void togetpath();
    void togetpathData();
    void totest();
    void tosave();
    void toexit();
    void todef();
    void towxzfb_paste_public_key();
    void toShowRSA2(int index);
private:
    void wxzfb_test(OnlinePayType type);
    QString modelFieldString(const QString &field_name);
    void setModelFieldString(const QString &field_name, const QString &value);
private:
    Ui_w_bt_dish_paymode_set *ui;
    QString _ch_paymodeno;
    int rowcount;
    class paymode_model : public QStandardItemModel{
    public:
        paymode_model(QObject *parent) : QStandardItemModel(parent) {}
        Qt::ItemFlags flags(const QModelIndex &index) const;
    };
    paymode_model*standmodel;
};

class w_bt_dish_paymode_teampay : public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_paymode_teampay(QWidget *parent = 0);
    static lds_tableView *creatpopView(int rowcount);

    void set_ch_paymodeno(const QString &ch_paymodeno);
private:
    Ui_w_bt_dish_unitset_Dialog *ui;
    w_bt_dish_paymode::paymodetablemodel *tablemodel;
    lds_menu *menu;
private slots:
    void tonew();
    void todel();
    void torefresh();
    void tonettest();
    bool tosave();
    void toexit();

    void newpaymode_team(const QModelIndex &index);
};

#endif // W_BT_DISH_PAYMODE_H

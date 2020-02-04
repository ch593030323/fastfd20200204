#ifndef W_BT_DISH_KITCHENPRINTER_PRINTERSET_H
#define W_BT_DISH_KITCHENPRINTER_PRINTERSET_H

#include <QDialog>
#include "lds_thread.h"
#include <QMutex>
#include "public_printby_ch_billno.h"
class Ui_w_bt_dish_Kitchenprinter_printerSet;
class w_bt_dish_kitchenprinter_printerset_port;

class w_bt_dish_Kitchenprinter_printerSet0 : public QDialog{
    Q_OBJECT
public:
    explicit w_bt_dish_Kitchenprinter_printerSet0(QWidget *parent = 0);
    ~w_bt_dish_Kitchenprinter_printerSet0();
    Ui_w_bt_dish_Kitchenprinter_printerSet *ui;

    void initData(const QString &no,const QString &name,
                  const QString &ch_kitchen_view = "N",
                  const QString &printtype=0, int instype=0,
                  const QString &port="",
                  int paperwidth = 0,
                  const QString &ch_areano = "",
                  int xinye_beep = 4);
    QString getno();
    QString getname();
    QString getprinttype();
    int getinstype();
    QString getport();//会在保存时记录
    int getpaperwidth();//会在保存时记录
    QString get_ch_areano();
    int get_xinye_beep();//会在保存时记录
    QString get_ch_kitchen_view();

    void setPrinterport(const QString &port);
protected:
    QString printerport();
protected slots:
    void toprinttype(int index);
    void took();
    void toexit();
    void totest();
    void tonextpage();
    void touppage();
    void tochkitchenview(bool checked);
    void totool();

    virtual void toSaaSCheck(int index);
protected:
    QString port;
    virtual bool trysave(const QString &int_id, const QString &vch_printname, const QString &vch_printtype, int vch_instype, const QString &vch_printip);
private:
    QString ch_kitchen_view;

    w_bt_dish_kitchenprinter_printerset_port *print_help;

};

class w_bt_dish_Kitchenprinter_printerSet : public w_bt_dish_Kitchenprinter_printerSet0
{
    Q_OBJECT
public:
    explicit w_bt_dish_Kitchenprinter_printerSet(QWidget *parent = 0);
protected slots:
    virtual void toSaaSCheck(int index);
protected:
    virtual bool trysave(const QString &int_id, const QString &vch_printname, const QString &vch_printtype, int vch_instype, const QString &vch_printip);
};


class w_bt_dish_Kitchenprinter_printer_thread_event : public lds_thread_event
{
    Q_OBJECT
public:
    static QMutex mutex;
public:
    w_bt_dish_Kitchenprinter_printer_thread_event(QObject *parent = 0);
public slots:
    void toprintBill(const QString &bp_ch_billno, const QString &bp_languageKey, const QStringList & label_insert_int_flowid_list, int pay_print_mode);
    void toprintBill(const QString &bp_ch_billno, const QString &bp_languageKey, const QStringList & label_insert_int_flowid_list, int pay_print_mode,
                     const QImage &image);
    //from lds_thread_event
public slots:
    virtual void toExec() ;
};

class w_bt_dish_Kitchenprinter_printer_thread : public lds_thread
{
    Q_OBJECT
public:
    w_bt_dish_Kitchenprinter_printer_thread(QObject *parent = 0);
signals:
    void signalTrigger(const QString &bp_ch_billno, const QString &bp_languageKey, const QStringList & label_insert_int_flowid_list, int pay_print_mode);
    void signalTrigger(const QString &bp_ch_billno, const QString &bp_languageKey, const QStringList & label_insert_int_flowid_list, int pay_print_mode, const QImage &img);
public:
    void emitsignalTrigger(const QString &bp_ch_billno, const QString &bp_languageKey, const QStringList & label_insert_int_flowid_list, int pay_print_mode);
    void emitsignalTrigger(const QString &bp_ch_billno, const QString &bp_languageKey, const QStringList & label_insert_int_flowid_list, int pay_print_mode, const QImage &img);
    //from QThread
protected:
    virtual void run();
};


class w_bt_dish_Kitchenprinter_display_thread_event : public lds_thread_event
{
    Q_OBJECT
public:
    w_bt_dish_Kitchenprinter_display_thread_event(QObject *parent = 0);
public slots:
    void toDisplay(const QByteArray &json);
    //from lds_thread_event
public slots:
    virtual void toExec();
};

class w_bt_dish_Kitchenprinter_display_thread : public lds_thread
{
    Q_OBJECT
public:
    w_bt_dish_Kitchenprinter_display_thread(QObject *parent = 0);
signals:
    void signalTrigger(const QByteArray &json);
public slots:
    void emitsignalTrigger(const QByteArray &json);
    void emitsignalTrigger(int sqr, double value, const QString &name);
    void emitsignalTrigger(int sqr, double value);
    //from QThread
protected:
    virtual void run();
};


#endif // W_BT_DISH_KITCHENPRINTER_PRINTERSET_H

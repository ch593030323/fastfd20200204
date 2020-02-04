#ifndef qt_retail_scale_scpos_keyboard_thread_THREAD_H
#define qt_retail_scale_scpos_keyboard_thread_THREAD_H


#include <QThread>
#include <QMutex>
#include "printer_pos_serial.h"
#include "electroniccountprice_resolve.h"

class qt_retail_scale_scpos_keyboard_event : public QObject {

    Q_OBJECT
public:
    static void reslove(const QByteArray &scale_data,
                        ElectronicCountPrice_resolve::resolveData &qupi,
                        ElectronicCountPrice_resolve::resolveData &lingwei,
                        ElectronicCountPrice_resolve::resolveData &scale_weight,
                        QString &key
                        );

public:
    qt_retail_scale_scpos_keyboard_event(QObject *parent = 0);

    ~qt_retail_scale_scpos_keyboard_event();

    Printer_POS_SERIAL printer;
    QByteArray printerd;
    bool _scale_loop_has_next;
    QMutex handled_m;
    int handled_count;
public slots:
    void zhiling();
    void qupi();

    void handled();//数据处理
    void scale_open(const QString &port, bool enable);
signals:
    //send
    void dataComing(const QString &d);
    void signalNumKey(const QString &);
    void weighComming(double);
    void signalzhiling(bool);
    void signalqupi(bool);
};

class qt_retail_scale_scpos_keyboard_thread : public QThread
{
    Q_OBJECT
public:
    qt_retail_scale_scpos_keyboard_thread(QObject *parent = 0);
    ~qt_retail_scale_scpos_keyboard_thread();

public slots:
    void qupi();
    void zhiling();

    void scale_open(const QString &port, bool enable);
    //send
signals:
    void signal_qupi();
    void signal_zhiling();
    void signal_scale_loop_has_next(const QString &, bool);
    //receive
signals:
    void dataComing(const QString &d);
    void weighComming(double);
    void signalzhiling(bool);
    void signalqupi(bool);
private slots:
    void toNumKey(const QString &key);
protected:
    void run();
};

#endif // qt_retail_scale_scpos_keyboard_thread_THREAD_H

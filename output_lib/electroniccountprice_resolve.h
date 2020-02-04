#ifndef ELECTRONICCOUNTPRICE_RESOLVE_H
#define ELECTRONICCOUNTPRICE_RESOLVE_H

#include <QObject>
#include "printer_pos_serial.h"
#include <QTimer>

class ElectronicCountPrice_resolve : public QObject
{
    Q_OBJECT
private:
    static QString jieshi(uchar a);
public :
    struct resolveData{
        double doubleValue;
        bool boolValud;
        bool isValid;
    };

public:
    explicit ElectronicCountPrice_resolve(QObject *parent = 0);
    bool scaleOpen(const QString &type_name, const QString &path, int baudRate, QString *errstring = 0);
    void setTimeOut(int  sec);

private:
    Printer_POS_SERIAL printer;
    QString type_name;
    int timeout;
    bool timeoutRunning;
signals:
    void weighComming(const QString &w);
    void weighComming(double w);
    void tareComing(const QString &w);
    void tareComing(double w);
    void qupiComming(bool flag);
    void lingweiComming(bool flag);
private slots:
    void toResolveData();
public slots:
    void qupi();
    void zhiling();

    //////////////////
public:
    static void  resolve(const QString &type_name, const QByteArray &scale_data, resolveData &qupi, resolveData &lingwei, resolveData &scale_weight, resolveData &scale_tare);
    static void  open(const QString &type_name, Printer_POS_SERIAL *printer, const QString &path, int baudRate);
    static QByteArray ins_zhiling(const QString &type_name);
    static QByteArray ins_qupi(const QString &type_name);
    static QByteArray ins_quzhong(const QString &type_name);
    static QStringList creat_pricecount_scaletypes();

};

#endif // ELECTRONICCOUNTPRICE_RESOLVE_H

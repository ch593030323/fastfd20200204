#ifndef BUSINESS_INTERFACE_MALAYISA_HOURLY_POS_H
#define BUSINESS_INTERFACE_MALAYISA_HOURLY_POS_H

#include <QDialog>
#include <QDate>
#include <QNetworkReply>
#include "lds_thread.h"
#include <QSharedPointer>
#include <QMutex>

class lds_pushbutton;
namespace Ui {
class business_interface_malayisa_hourly_pos;
}

class business_interface_malayisa_hourly_pos : public QDialog
{
    Q_OBJECT

public:
    explicit business_interface_malayisa_hourly_pos(QWidget *parent = 0);
    ~business_interface_malayisa_hourly_pos();

private slots:
    void took();
    void tocancel();
    void tomachineCheck();
    void tomanual_upload();

private:
    void setbuttonChecked(lds_pushbutton *b, bool check);
    void saveData();

private:
    Ui::business_interface_malayisa_hourly_pos *ui;
};

class business_interface_malayisa_hourly_pos_event : public lds_thread_event{

    Q_OBJECT
public:
    struct PosData {
        double GTO;
        double SST;
        double Discount;
        double Service;

        int Pax;
        double Cash;
        double TNG;
        double Visa;
        double MasterCard;

        double Amex;
        double Voucher;
        double Other;
        QStringList ch_payno_list;

        void init();
        void init(const QString &ch_payno);
        void add(const PosData &d);
        QString lineData(const QDate &date, int hour);
    };

    class NetworkReplyDeleteLater {
    public:
        NetworkReplyDeleteLater(QNetworkReply *reply);
        ~NetworkReplyDeleteLater();
        QNetworkReply::NetworkError error();
        QString errorString();
    private:
        QNetworkReply *reply;
    };

public:
    business_interface_malayisa_hourly_pos_event(QObject *parent = 0)
        : lds_thread_event(parent){}
signals:
    void signal_state(const QString &string);
public slots:
    //from lds_thread_event
    virtual void toExec();
public:
    static QString dayContent(const QDate &date);
private:
    static QMutex mutex;
};

class business_interface_malayisa_hourly_pos_thread : public lds_thread{
    Q_OBJECT
public:
    business_interface_malayisa_hourly_pos_thread(QObject *parent)
        :lds_thread("business_interface_malayisa_hourly_pos", parent) {}
protected:
    //from QThread
    virtual void run();
};
#endif // BUSINESS_INTERFACE_MALAYISA_HOURLY_POS_H

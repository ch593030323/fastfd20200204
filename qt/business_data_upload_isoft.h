#ifndef BUSINESS_DATA_UPLOAD_ISOFT_H
#define BUSINESS_DATA_UPLOAD_ISOFT_H

#include <QString>
#include "lds_thread.h"
#include "lds_bool.h"
#include <QObject>

//星摩尔接口

class business_data_upload_isoft
{
public:
    business_data_upload_isoft();
    static void uploadBill();
    static void genrateBill(const QString &ch_payno);
};

class business_data_upload_isoft_thread : public lds_thread{
    Q_OBJECT
public:
    business_data_upload_isoft_thread(QObject *parent):lds_thread("isoft", parent) {}
protected:
    //from QThread
    virtual void run();
};

class business_data_upload_isoft_thread_event : public lds_thread_event {
    Q_OBJECT
public:
    business_data_upload_isoft_thread_event(QObject *parent = 0)
        : lds_thread_event(parent) {}
public slots:
    //from lds_thread_event
    virtual void toExec();
};


#endif // BUSINESS_DATA_UPLOAD_ISOFT_H

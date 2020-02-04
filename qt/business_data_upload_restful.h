#ifndef BUSINESS_DATA_UPLOAD_RESTFUL_H
#define BUSINESS_DATA_UPLOAD_RESTFUL_H

#include <QDialog>
#include "lds_thread.h"
#include "lds_bool.h"
#include <QObject>

namespace Ui {
class business_data_upload_restful;
}

class business_data_upload_restful : public QDialog
{
    Q_OBJECT

public:
    static void uploadBill();
    static void genrateBill(const QString &ch_payno);

public:
    explicit business_data_upload_restful(QWidget *parent = 0);
    ~business_data_upload_restful();

private slots:
    void took();
    void tocancel();
private:
    Ui::business_data_upload_restful *ui;
};
class business_data_upload_restful_thread : public lds_thread{
    Q_OBJECT
public:
    business_data_upload_restful_thread(QObject *parent):lds_thread("restful", parent) {}
protected:
    //from QThread
    virtual void run();
};

class business_data_upload_restful_thread_event : public lds_thread_event {
    Q_OBJECT
public:
    business_data_upload_restful_thread_event(QObject *parent = 0)
        : lds_thread_event(parent) {}
public slots:
    //from lds_thread_event
    virtual void toExec();
};

#endif // BUSINESS_DATA_UPLOAD_RESTFUL_H

#ifndef PRINTER_POS_VIDPID_H
#define PRINTER_POS_VIDPID_H

#include <QObject>
#include "printer_pos.h"
#ifdef Q_OS_UNIX
#include <sys/types.h>
#include "libusb-1.0/libusb.h"
#include <linux/fs.h>
#include <stdio.h>
#endif
#include <stdint.h>
#include <QStringList>
class Printer_POS_VIDPID : public Printer_POS
{
public:
    struct vidpid_Data{
        uint16_t idVendor;
        uint16_t idProduct;
        uint8_t device_address;
        void print();
    };

public:
    explicit Printer_POS_VIDPID();
    virtual ~Printer_POS_VIDPID();
    //virtual
    virtual qint64 writedirect(const QByteArray & byte);
    virtual qint64 write(const QString & str, QString languageKey);//if n have write then n is return; if n == -1 then error
    virtual qint64 writeHex(const QString &hex);//if n have write then n is return; if n == -1 then error
    virtual bool tryOpen(const QString &port);
    virtual bool isOpen();
    virtual void close();
    virtual QString lastError();
    virtual QString port();
    virtual bool waitForReadyReadv(int);//+v做区分
    virtual QByteArray readAll();
    //~virtual


private:
    bool open(const QString & hex_vid, const QString & hex_pid);
    bool open(uint16_t vid, uint16_t pid);
private:
    int scanall();
#ifdef Q_OS_UNIX
    uint8_t get_endpoint_out(libusb_device_handle *handle);
#endif
private:
#ifdef Q_OS_UNIX
    libusb_device_handle *dev_handle;//usb
    libusb_context *ctx;//usb context
#endif
    QString otherError;
    bool _open;
    QString _port;
    uint8_t endpoint_out;

    ///////////////
public:
    static QList<vidpid_Data> get_vidpid_print_type_list();
    static QStringList get_vidpid_print_type_list_str();
};

#endif // PRINTER_POS_VIDPID_H

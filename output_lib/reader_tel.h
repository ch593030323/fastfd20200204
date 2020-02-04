#ifndef READER_TEL_H
#define READER_TEL_H

#include <QObject>
#include <QByteArray>
#include <QStringList>
#ifdef Q_OS_UNIX
#include "libusb-1.0/libusb.h"
#endif
#include <QMutex>
#include <QThread>

//#include <linux/fs.h>//文件系统头文件,定义文件表结构(file,buffer_head,m_inode等)

struct info_tel{
    info_tel() : isvalid(false) {}
    bool isvalid;
    QString errstring;

    QString datetime;
    QString callerID;
    QString selfID;
    QString callerName;
};

class Reader_Tel : public QObject
{
    Q_OBJECT
public:
    explicit Reader_Tel(QObject *parent = 0);
    ~Reader_Tel();
    bool open(QString *errstring = 0);
    void close();

    info_tel getTelInfo();

#ifdef Q_OS_UNIX
private:
    libusb_device_handle *dev_handle;//usb
    libusb_context *ctx;//usb context
    QMutex mutex;
#endif
    bool isopen;//判断是否打开usb
private:
    QString telInfoTrans(const QString &str1);
};


#endif // READER_TEL_H

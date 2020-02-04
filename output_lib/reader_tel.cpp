#include "reader_tel.h"
#include <QDebug>

Reader_Tel::Reader_Tel(QObject *parent) :
    QObject(parent)
{
#ifdef Q_OS_UNIX
    dev_handle = NULL;//libusb句柄
    ctx = NULL ;//USB连接
#endif
}

Reader_Tel::~Reader_Tel() {
    close();
}

//连接USB
bool Reader_Tel::open(QString *errstring) {

#ifdef Q_OS_UNIX
    int r;
    isopen = false;
    //1初始化 libusb的初始化，必须最先调用
    if ((r = libusb_init(&ctx)) < 0) {//connect usb连接返值小于0表示连接失败
        if(errstring) *errstring = "libusb_init error";
        goto failure;
    }
    //2启动调试
    libusb_set_debug(ctx, 3);    //确保应用程序不关闭文件描述符 会stderr输出错误
    //3按vid,pid打开 根据 VID UID 找到相应的设备并打开它，将设备句柄返回
    if((dev_handle = libusb_open_device_with_vid_pid(ctx,0x483,0x5750)) == NULL) {//打开失败则 退出 ->dev_handle == NULL
        if(errstring) *errstring = "libusb_open_device_with_vid_pid error!";
        goto failure;
    }
    //4声明前判断
    r = libusb_kernel_driver_active(dev_handle, 0);//判断 该接口是否已经被内核声明
    if (r == 1) {//若声明一个已经被内核声明过的接口，则脱离该接口的内核驱动程序
        r = libusb_detach_kernel_driver(dev_handle, 0);//同一次 只会进入一次，及 只会脱离一次
    }
    //5声明
    if((r=libusb_claim_interface(dev_handle,0)) < 0) {//请求接口（需要你知道设备的接口数值dev_handle） 如果请求失败
        if(errstring) *errstring = "libusb_claim_interface error!";
        goto failure;
    }
    isopen = true;
    goto success;
failure:
    if(ctx != NULL) {
        libusb_exit(ctx);
        ctx = NULL;
    }
success:
#endif
    return isopen;
}

void Reader_Tel::close() {
#ifdef Q_OS_UNIX
    mutex.lock();

    if(dev_handle != NULL) {
        libusb_release_interface(dev_handle,0);
        libusb_close(dev_handle); //close the device we opened
        libusb_exit(ctx); //needs to be called to end the
        dev_handle = NULL;
        ctx = NULL;
    }
    isopen=false;

    mutex.unlock();
#endif
}

info_tel Reader_Tel::getTelInfo()
{
    info_tel tel1;
#ifdef Q_OS_UNIX
    QByteArray byte2;
    byte2.resize(128);//接受的数据
    if(!isopen) {
        tel1.errstring = "usb is not open!";
        return tel1;
    }
    int actual = -1; // 实际读到的
    mutex.lock();
    libusb_bulk_transfer(dev_handle,(0x81),(uchar*)byte2.data(),byte2.length(),&actual,1000);
    mutex.unlock();
    if(actual == 0) {
        tel1.errstring = "read none data!";
        return tel1;;
    } else {
        byte2 = byte2.left(actual);
    }
    if(byte2.length() >= 64) {//数据长度 硬件如果有数据 会固定发出64字节的数据包
        if(byte2[0] == char(100)) {//表示开始震铃
            if(byte2[1] == char(3)) {
                tel1.datetime = byte2.mid(2+16*0,16);
                tel1.callerID = byte2.mid(2+16*1,16);// 手机号
                tel1.selfID   = byte2.mid(2+16*2,16);
                tel1.callerName=byte2.mid(2+16*3,16);
                tel1.callerID = telInfoTrans(tel1.callerID);

                //                qDebug() << "datetime:" << tel1.datetime;
                //                qDebug() << "callerID:" << telInfoTrans(tel1.callerID)<<"2";
                //                qDebug() << "selfID:"   << tel1.selfID;
                //                qDebug() << "callerName:"<< tel1.callerName;
            } else {//表示为电话按键
                QString str = QString().append(byte2);
                QString anJian=telInfoTrans(str);
                if(!anJian.isEmpty()) {
                    //                    qDebug() << "正在按键:" << anJian;//转换的话只能转数字 其他键待做
                    tel1.errstring = "press digit button";
                    tel1.isvalid = false;
                    return tel1;
                }else{// arm上目前有bug 在这里
                    tel1.datetime = byte2.mid(2+16*0,16);
                    tel1.callerID = byte2.mid(2+16*1,16);// 手机号
                    tel1.selfID   = byte2.mid(2+16*2,16);
                    tel1.callerName=byte2.mid(2+16*3,16);
                    tel1.callerID = telInfoTrans(tel1.callerID);

                    if(tel1.callerID.isEmpty()) {
                        tel1.errstring = "callerID is empty";
                        return tel1;
                    }

                    //                    qDebug() << "datetime:" << tel1.datetime;
                    //                    qDebug() << "callerID:" << telInfoTrans(tel1.callerID)<< "3";
                    //                    qDebug() << "selfID:"   << tel1.selfID;
                    //                    qDebug() << "callerName:"<< tel1.callerName;
                }
            }
        }
    } else{
        tel1.errstring = "read error!";
        return tel1;;
    }
    //
    tel1.isvalid = true;
#endif
    return tel1;
}

QString Reader_Tel::telInfoTrans(const QString &str)
{
    if(str.isEmpty())return "";
    QString newNum="";
    for (int i = 0; i < str.size(); ++i)
    {
        ushort tmp = str[i].unicode();
        if (tmp >= 48 && tmp <= 57)
        {
            newNum+=str[i];
        }
    }
    return newNum;
}




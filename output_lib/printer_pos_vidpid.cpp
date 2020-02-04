#include "printer_pos_vidpid.h"
#include "printer_0.h"
#include <QtDebug>


Printer_POS_VIDPID::Printer_POS_VIDPID() :
    _open(false)
{
#ifdef Q_OS_UNIX
    dev_handle = NULL;//usb
    ctx = NULL;//usb context
#endif

    //    scanall();
#ifdef QT_DEBUG
    QList<vidpid_Data>  dlist = get_vidpid_print_type_list();
    foreach(vidpid_Data d, dlist) {
        d.print();
    }
#endif
}

Printer_POS_VIDPID::~Printer_POS_VIDPID()
{
    qDebug() << __FUNCTION__;
    close();
}

qint64 Printer_POS_VIDPID::writedirect(const QByteArray &byte)
{
    if(!_open || byte.isEmpty())return 0;
    otherError.clear();
    int write_actual = 0;
#ifdef Q_OS_UNIX
    int write_count = byte.length()/5000 + (byte.length()%5000 == 0 ? 0 : 1);
    for(int k = 0;k < write_count; k ++) {
        int r;
        QByteArray data=byte.mid(k*5000, 5000);
        int actual;
        r = libusb_bulk_transfer(dev_handle, (endpoint_out | LIBUSB_ENDPOINT_OUT), (uchar*)data.data(), data.length(), &actual, 200);
        if(r == 0 && actual == data.count()) {
            write_actual += actual;
            continue;
        }
        otherError = libusb_error_name(r);
        return 0;
    }
#endif
    return write_actual;
}

qint64 Printer_POS_VIDPID::write(const QString & str, QString languageKey)
{
    return writedirect(printer_0::trans_u2codepage(str, languageKey, "GBK"));
}

qint64 Printer_POS_VIDPID::writeHex(const QString &hex)
{
    return writedirect(QByteArray::fromHex(QByteArray().append(hex)));
}

bool Printer_POS_VIDPID::tryOpen(const QString &port)
{
    QStringList vidpid=port.split("&");
    if(vidpid.value(0).isEmpty()
            ||vidpid.value(1).isEmpty()) {
        otherError=QObject::tr("路径错误");
        return false;
    }
    return open( qint16(vidpid.value(0).toInt()), qint16(vidpid.value(1).toInt()) );
}

bool Printer_POS_VIDPID::isOpen()
{
    return _open;
}

void Printer_POS_VIDPID::close()
{
#ifdef Q_OS_UNIX
    if(dev_handle != NULL) {
        libusb_release_interface(dev_handle,0);
        libusb_close(dev_handle); //close the device we opened
        libusb_exit(ctx); //needs to be called to end the
        dev_handle = NULL;
        ctx = NULL;
    }
#endif
    _open=false;
}

QString Printer_POS_VIDPID::lastError()
{
    return otherError;
}

QString Printer_POS_VIDPID::port()
{
    return _port;
}

bool Printer_POS_VIDPID::waitForReadyReadv(int /*msecs*/)
{
    return false;
}

QByteArray Printer_POS_VIDPID::readAll()
{
    QByteArray d;
#ifdef Q_OS_UNIX
    d.resize(128);
    int actual =-1;
    int r;

    r = libusb_bulk_transfer(dev_handle, (0x81), (uchar *)d.data(), d.length(), &actual, 1000);
    otherError = libusb_error_name(r);
    if(actual == 0) return "";
    d = d.left(actual);
    //


    qDebug() << __LINE__ << d;


    //    64050000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
    //    64060000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000

    //    r = libusb_interrupt_transfer(dev_handle, 0x81, (uchar *)d.data(), d.length(), &actual, 1000);
    //    otherError = libusb_error_name(r);
    //    qDebug() << __LINE__ << otherError;
    //    qDebug() << __LINE__ << d;

    //    /*
    //    //                                LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
    //    //                                LIBUSB_REQUEST_SYNCH_FRAME, wValue, 0x00, (unsigned char *)byte2.data(), 0xff, 200);
    //*/
    //    r = libusb_control_transfer(dev_handle,
    //                                LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
    //                                LIBUSB_REQUEST_SYNCH_FRAME, 0x306, 0x01, (uchar *)d.data(), 0xff, 200);
    //    otherError = libusb_error_name(r);
    //    qDebug() << __LINE__ << otherError;
    //    qDebug() << __LINE__ << d;

    //    qDebug() << "----------------------------";
#endif
    return "";


    //    if(!_open)return 0;
    //    int endpoint_in = 1;
    //    static unsigned char endpoint_out = 0x0;
    //    QByteArray data;
    //    data.resize(100);
    //    otherError.clear();
    //    int actual;
    //    QByteArray byteOut;
    //    byteOut = QByteArray::fromHex("01e443");

    //    unsigned char *my_string1;
    //    my_string1 = (unsigned char *)malloc(256 + 1);
    //    memset(my_string1, '\0', 64);

    //#ifdef Q_OS_UNIX
    //    int r;
    //    r = libusb_interrupt_transfer(dev_handle, (endpoint_in | LIBUSB_ENDPOINT_IN), my_string1, 64, &actual, 200);
    //    otherError = libusb_error_name(r);
    //    qDebug() << __LINE__ << otherError;

    //    qDebug("endpoint_out:0x%x", endpoint_out);
    //    r = libusb_bulk_transfer(dev_handle, (endpoint_out | LIBUSB_ENDPOINT_OUT), (uchar *)byteOut.data(), byteOut.length(), &actual, 200);
    //    otherError = libusb_error_name(r);
    //    if(r >= 0) {
    //        return QByteArray("dfsf");
    //    }
    //    qDebug() << __LINE__ << otherError;

    //    r = libusb_bulk_transfer(dev_handle, (0x01 | LIBUSB_ENDPOINT_IN), my_string1, 64, &actual, 200);
    //    otherError = libusb_error_name(r);
    //    qDebug() << __LINE__ << otherError;

    //    //////////////
    //    QByteArray byte2;//
    //    QByteArray tmp;//
    //    tmp.append(0x01e4);
    //    byte2.resize(64);
    //    static uint16_t wValue = 0x306;
    //    r = libusb_control_transfer(dev_handle, 0x21, 0x09, wValue, 0x00, (unsigned char *)tmp.data(), 0xff, 200);
    //    otherError = libusb_error_name(r);
    //    qDebug() << __LINE__ << otherError;


    //    r = libusb_control_transfer(dev_handle,
    //                                LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
    //                                LIBUSB_REQUEST_SYNCH_FRAME, wValue, 0x00, (unsigned char *)byte2.data(), 0xff, 200);
    //    otherError = libusb_error_name(r);
    //    qDebug() << __LINE__ << otherError;

    //    qDebug("wValue:0x%x", wValue++) ;
    //    qDebug() <<  (r != LIBUSB_ERROR_PIPE);
    //    if(r != LIBUSB_ERROR_PIPE) {
    //        return QByteArray("dfsf");
    //    }

    //    endpoint_out ++;
    //#endif
    //    return QByteArray();
}

bool Printer_POS_VIDPID::open(const QString &hex_vid, const QString &hex_pid)
{
    return open(hex_vid.toInt(0, 16), hex_pid.toInt(0, 16));
}

bool Printer_POS_VIDPID::open(uint16_t vid, uint16_t pid)
{
    if(_open) {
        return true;
    }
#ifdef Q_OS_UNIX
    int r;
    _port=QString().sprintf("%d&%d", vid, pid);
    _open=false;
    otherError.clear();
    dev_handle = NULL;
    ctx=NULL;

    //初始化
    if ((r = libusb_init(&ctx)) < 0) {
        otherError="libusb_init failure!";
        goto failure;
    }
    //启动调试
    libusb_set_debug(ctx, 3);    //确保应用程序不关闭文件描述符 stderr输出。
    //按vid,pid打开
    if((dev_handle = libusb_open_device_with_vid_pid( ctx,vid,pid )) == NULL) {//打开失败则 退出 dev_handle == NULL
        //qDebug( "libusb_open_device_with_vid_pid = %d",dev_handle);                            //该函数 包含了libusb_get_device_list的判断
        otherError="libusb_open_device_with_vid_pid failure!";
        goto failure;
    }
    //声明前判断
    r = libusb_kernel_driver_active(dev_handle, 0);           //判断 该接口是否已经被内核声明
    if (r == 1) {                                                                 //若声明一个已经被内核声明过的接口，则脱离该接口的内核驱动程序
        r = libusb_detach_kernel_driver(dev_handle, 0);     //同一次 只会进入一次，及 只会脱离一次
        //qDebug("libusb_detach_kernel_driver = %d\n", r);
    }
    //声明
    if((r=libusb_claim_interface(dev_handle,0)) < 0) {
        otherError="libusb_claim_interface failure!";
        goto failure;
    }

    _open=true;
    endpoint_out = get_endpoint_out(dev_handle);
    goto success;
failure:
    if(ctx != NULL) {
        libusb_exit(ctx);
        ctx = NULL;
    }
success:
#endif
    return _open;
}

int Printer_POS_VIDPID::scanall()
{

#ifdef Q_OS_UNIX
    libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
    libusb_context *ctx = NULL; //a libusb session
    int r; //for return values
    ssize_t cnt; //holding number of devices in list
    r = libusb_init(&ctx); //initialize the library for the session we just declared
    if(r < 0) {
        qDebug() <<"Init Error "<<r<<endl; //there was an error
        return -1;
    }
    libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if(cnt < 0) {
        qDebug() <<"Get Device Error"<<endl; //there was an error
        return -1;
    }
    qDebug() <<cnt<<" Devices in list."<<endl;

    libusb_free_device_list(devs, 1); //free the list, unref the devices in it

    //!
    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if(cnt >=0) {
        for(ssize_t i = 0; i < cnt; i++) {
            libusb_device *dev = devs[i];
            qDebug() << "路径：/dev/bus/usb/"+  QString().sprintf("%03d", libusb_get_bus_number(dev)) + "/" +QString().sprintf("%03d", libusb_get_device_address(dev));

            libusb_device_descriptor desc;
            int r = libusb_get_device_descriptor(dev, &desc);
            if (r < 0) {
                qDebug() <<"failed to get device descriptor";
            } else {
                //                qDebug() << "find device info :"
                //                         << desc.bLength
                //                         << desc.bDescriptorType
                //                         << QString().sprintf("bcdUSB:0x%x", desc.bcdUSB)
                //                         << QString("bDeviceClass:%1").arg(desc.bDeviceClass)
                //                         << desc.bDeviceSubClass
                //                         << desc.bDeviceProtocol
                //                         << QString().sprintf("idVendor:0x%x", desc.idVendor)
                //                         << desc.bMaxPacketSize0
                //                         << QString().sprintf("idProduct:0x%x", desc.idProduct)
                //                         << desc.bcdDevice
                //                         << desc.iManufacturer
                //                         << desc.iProduct
                //                         << desc.iSerialNumber
                //                         << desc.bNumConfigurations
                ;

                libusb_config_descriptor *config;
                libusb_get_config_descriptor(dev, 0, &config);
                const libusb_interface *inter;
                const libusb_interface_descriptor *interdesc;
                const libusb_endpoint_descriptor *epdesc;
                QString msg;
                msg += QString("%1%2\t").arg("bDescriptorType:").arg(desc.bDescriptorType, 0, 16);
                msg += QString("%1%2\t").arg("iManufacturer:").arg(desc.iManufacturer, 0, 16);
                msg += QString("%1%2\t").arg("bDeviceSubClass:").arg(desc.bDeviceSubClass, 0, 16);
                msg += QString("%1%2\t").arg("bDeviceClass:").arg(desc.bDeviceClass, 0, 16);
                msg += QString("%1%2\t").arg("idVendor:").arg(desc.idVendor, 0, 16);
                msg += QString("%1%2\t").arg("idProduct:").arg(desc.idProduct, 0, 16);
                for(int i=0; i<(int)config->bNumInterfaces; i++) {
                    inter = &config->interface[i];
                    msg += QString("%1%2").arg("Number of alternate settings:").arg(inter->num_altsetting);
                    for(int j=0; j<inter->num_altsetting; j++) {
                        interdesc = &inter->altsetting[j];
                        msg += QString("\t%1 0x%2").arg("bInterfaceClass: ").arg(interdesc->bInterfaceClass, 0, 16);
                        msg += QString("\t%1%2").arg("bInterfaceNumber: ").arg(interdesc->bInterfaceNumber);
                        msg += QString("\t%1 0x%2").arg("bNumEndpoints: ").arg(interdesc->bNumEndpoints, 0, 16);
                        for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
                            epdesc = &interdesc->endpoint[k];
                            msg += QString("\t%1%2").arg("Descriptor Type: ").arg(epdesc->bDescriptorType);
                            msg += QString("\t%1 0x%2").arg("EP Address: ").arg(epdesc->bEndpointAddress, 0, 16);
                            msg += QString("\t%1 %2").arg("bmAttributes: ").arg(epdesc->bmAttributes);
                            msg += QString("\t%1 %2").arg("bInterval: ").arg(epdesc->bInterval);
                        }
                    }
                }
                qDebug() << msg;
                libusb_free_config_descriptor(config);
            }
        }
    }
#endif
    return 0;
}

#ifdef Q_OS_UNIX
uint8_t Printer_POS_VIDPID::get_endpoint_out(libusb_device_handle *handle)
{
    uint8_t endpoint_out = 2;
    uint8_t endpoint_in = 0;
    libusb_device *dev = libusb_get_device(handle);
    libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
        qDebug() <<"failed to get device descriptor";
    } else {
        libusb_config_descriptor *config;
        libusb_get_config_descriptor(dev, 0, &config);
        const libusb_interface *inter = config->interface;
        const libusb_interface_descriptor *interdesc= inter->altsetting;
        const libusb_endpoint_descriptor *epdesc;
        for(int k = 0; k < (int)interdesc->bNumEndpoints; k++) {
            epdesc = &interdesc->endpoint[k];
            if(epdesc->bEndpointAddress & LIBUSB_ENDPOINT_IN) {
                endpoint_in = epdesc->bEndpointAddress & 0xf;
            } else {
                endpoint_out = epdesc->bEndpointAddress & 0xf;
            }
        }
    }

    return endpoint_out;
}
#endif

QList<Printer_POS_VIDPID::vidpid_Data> Printer_POS_VIDPID::get_vidpid_print_type_list()
{
    QList<Printer_POS_VIDPID::vidpid_Data> vd_data_list;

#ifdef Q_OS_UNIX
    libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
    libusb_context *ctx = NULL; //a libusb session
    int r; //for return values
    ssize_t cnt; //holding number of devices in list
    r = libusb_init(&ctx); //initialize the library for the session we just declared
    if(r < 0) {
        qDebug() <<"Init Error "<<r<<endl; //there was an error
        goto end;
    }
    libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if(cnt < 0) {
        qDebug() <<"Get Device Error"<<endl; //there was an error
        goto end;
    }
    //    qDebug() <<cnt<<" Devices in list."<<endl;

    libusb_free_device_list(devs, 1); //free the list, unref the devices in it

    //!
    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if(cnt >=0) {
        for(ssize_t i = 0; i < cnt; i++) {
            libusb_device *dev = devs[i];
            qDebug() << "file path : /dev/bus/usb/"+  QString().sprintf("%03d", libusb_get_bus_number(dev)) + "/" +QString().sprintf("%03d", libusb_get_device_address(dev));

            libusb_device_descriptor desc;
            int r = libusb_get_device_descriptor(dev, &desc);
            if (r < 0) {
                qDebug() <<"failed to get device descriptor";
            } else {
                libusb_config_descriptor *config;
                libusb_get_config_descriptor(dev, 0, &config);
                const libusb_endpoint_descriptor *epdesc;
                const libusb_interface *inter;
                const libusb_interface_descriptor *interdesc;
                for(int i=0; i<(int)config->bNumInterfaces; i++) {
                    inter = &config->interface[i];
                    for(int j=0; j<inter->num_altsetting; j++) {
                        interdesc = &inter->altsetting[j];
                        if(LIBUSB_CLASS_PRINTER == interdesc->bInterfaceClass
                                //                                || LIBUSB_CLASS_VENDOR_SPEC == interdesc->bInterfaceClass
                                ) {
                            //
                            Printer_POS_VIDPID::vidpid_Data d;
                            d.idProduct = desc.idProduct;
                            d.idVendor = desc.idVendor;
                            d.device_address = libusb_get_device_address(dev);
                            vd_data_list.append(d);
                            //
                        }
                    }
                }
                libusb_free_config_descriptor(config);
            }
        }
    }
end:
    libusb_exit(ctx); //needs to be called to end the
#endif
    return vd_data_list;
}

QStringList Printer_POS_VIDPID::get_vidpid_print_type_list_str()
{
    QList<vidpid_Data> d = get_vidpid_print_type_list();
    QStringList list;
    foreach(const vidpid_Data &a, d) {
        QString p =  QString().sprintf("%d&%d", a.idVendor, a.idProduct);
        list.append(p);
    }
    return list;
}

void Printer_POS_VIDPID::vidpid_Data::print()
{
    qDebug() << "\nvidpid_Data:";
    qDebug("idProduct:0x%x", idProduct) ;
    qDebug("idVendor:0x%x", idVendor) ;
    qDebug("device_address:%03d", device_address) ;
}

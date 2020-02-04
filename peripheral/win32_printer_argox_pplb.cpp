#include "win32_printer_argox_pplb.h"
#include <lusb0_usb.h>
#include <stdio.h>
#include <qdebug.h>

// Enables this example to work with a device running the
// libusb-win32 PIC Benchmark Firmware.
#define BENCHMARK_DEVICE

//////////////////////////////////////////////////////////////////////////////
// TEST SETUP (User configurable)

// Issues a Set configuration request
#define TEST_SET_CONFIGURATION

// Issues a claim interface request
#define TEST_CLAIM_INTERFACE

// Use the libusb-win32 async transfer functions. see
// transfer_bulk_async() below.
#define TEST_ASYNC

// Attempts one bulk read.
#define TEST_BULK_READ

// Attempts one bulk write.
// #define TEST_BULK_WRITE

//////////////////////////////////////////////////////////////////////////////
// DEVICE SETUP (User configurable)

// Device vendor and product id.
#define MY_VID 0x1664
#define MY_PID 0x081b

// Device configuration and interface id.
#define MY_CONFIG 1
#define MY_INTF 0

// Device endpoint(s)
#define EP_IN 0x81
#define EP_OUT 0x01

// Device of bytes to transfer.
#define BUF_SIZE 64

//////////////////////////////////////////////////////////////////////////////


usb_dev_handle *open_dev(quint16 vid, quint16 pid)
{
    struct usb_bus *bus;
    struct usb_device *dev;

    for (bus = usb_get_busses(); bus; bus = bus->next)
    {
        for (dev = bus->devices; dev; dev = dev->next)
        {
            if (dev->descriptor.idVendor == vid
                    && dev->descriptor.idProduct == pid)
            {
                return usb_open(dev);
            }
        }
    }
    return NULL;
}

win32_printer_argox_pplb::win32_printer_argox_pplb(QObject *parent) : QObject(parent)
{

}

win32_printer_argox_pplb::~win32_printer_argox_pplb()
{

}

bool win32_printer_argox_pplb::send_data(quint16 vid, quint16 pid, char *data)
{
    usb_dev_handle *dev = NULL; /* the device handle */
    char tmp[BUF_SIZE];
    int ret;
    void* async_read_context = NULL;
    void* async_write_context = NULL;
    err="";
    usb_set_debug(4);

    usb_init(); /* initialize the library */
    usb_find_busses(); /* find all busses */
    usb_find_devices(); /* find all connected devices */


    if (!(dev = open_dev(vid, pid)))
    {
        printf("error opening device: \n%s\n", usb_strerror());
        err=QString(usb_strerror());
        return false;
    }
    else
    {
        printf("success: device %04X:%04X opened\n", vid, pid);
    }


    if (usb_set_configuration(dev, MY_CONFIG) < 0)
    {
        printf("error setting config #%d: %s\n", MY_CONFIG, usb_strerror());
        err=QString(usb_strerror());
        usb_close(dev);
        return false;
    }
    else
    {
        printf("success: set configuration #%d\n", MY_CONFIG);
    }


    if (usb_claim_interface(dev, 0) < 0)
    {
        printf("error claiming interface #%d:\n%s\n", MY_INTF, usb_strerror());
        err=QString(usb_strerror());
        usb_close(dev);
        return false;
    }
    else
    {
        printf("success: claim_interface #%d\n", MY_INTF);
    }

//    char data[]=
//            "N\nZB\nA98,49,0,966,1,1,N,\"hello\"\nP1\n";//"N\r\nZB\r\nFS\"standalone\"\r\nT20,20,0,6,1,1,N,\"ABC123hello\"\r\nFE\r\nQ240,40\r\nFR\"standalone\"\r\nW1\r\nFK\"standalone\"\r\n";

    ret = usb_bulk_write(dev, 1|USB_ENDPOINT_OUT, data, sizeof(data), 2000);
    if(ret < 0) {
        printf("usb_bulk_write device: \n%s\n", usb_strerror());
        err=QString(usb_strerror());
        usb_close(dev);
        return false;
    }

    usb_release_interface(dev, 0);

    if (dev)
    {
        usb_close(dev);
    }
    return true;
}

QString win32_printer_argox_pplb::errString()
{
    return err;
}


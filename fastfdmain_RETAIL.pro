DEFINES += QT_NEW_SYSTEM
INCLUDEPATH += inputnew/

QT       += core gui sql network xml concurrent svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
lessThan(QT_MAJOR_VERSION, 5){
 QT += phonon
 DEFINES += QT_PHONON_SUPPORT
}

TARGET = SoftPos

TEMPLATE = app

DESTDIR = .

include(testwx/testwx.pri)

include(lds/lds.pri)

include(output_lib/output_lib.pri)

include(fileviewer/fileviewer.pri)

include(mwrf/mwrf.pri)

include(inputnew/inputnew.pri)

include(hddpos/hddpos.pri)

include(fastfdmain_RETAIL/fastfdmain_RETAIL.pri)

include(fastfd_update/fastfd_update.pri)


RESOURCES += \
    res.qrc

RC_FILE = fastfd.rc


unix{
#openssl
INCLUDEPATH +=/usr/local/openssl/include
LIBS += -L/usr/local/openssl/lib -lssl -lcrypto -ldl -lpthread
#~openssl
#lusb
CONFIG(debug,debug|release):INCLUDEPATH += /usr/local/include
CONFIG(release,debug|release):INCLUDEPATH += /usr/local/usb-arm/include
CONFIG(debug,debug|release):LIBS +=-L/usr/local/lib -lusb-1.0
CONFIG(release,debug|release):LIBS +=  -lusb-1.0  /lib/arm-linux-gnueabihf/libudev.so.1
#~lusb
}

win32:LIBS +=-lwinspool

#查看进程打开文件数：lsof -c SoftPos | wc -l

HEADERS += \
    public/fexpandmain_tableview.h \
    public/label_click.h \
    public/public_scale_led.h \
    back/w_bt_dish_kitchenprinter_printerset_port.h \
    public/public_select_stringlist.h

SOURCES += \
    public/fexpandmain_tableview.cpp \
    public/label_click.cpp \
    public/public_scale_led.cpp \
    back/w_bt_dish_kitchenprinter_printerset_port.cpp \
    public/public_select_stringlist.cpp


FORMS += \
    back/w_bt_dish_kitchenprinter_printerset_port.ui \
    public/public_select_stringlist.ui

INCLUDEPATH += public

INCLUDEPATH += back

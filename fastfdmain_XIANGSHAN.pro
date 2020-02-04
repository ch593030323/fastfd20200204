DEFINES += QT_NEW_SYSTEM
INCLUDEPATH += inputnew/

QT       += core gui sql network xml concurrent svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
lessThan(QT_MAJOR_VERSION, 5){
 QT += phonon
 DEFINES += QT_PHONON_SUPPORT
}

CONFIG(debug,debug|release):TARGET = SoftPos1
CONFIG(release,debug|release):TARGET = SoftPos


TEMPLATE = app

DESTDIR = .

include(testwx/testwx.pri)

include(lds/lds.pri)

include(output_lib/output_lib.pri)

include(fastfdmain_XIANGSHAN/fastfdmain_XIANGSHAN.pri)


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

win32{
LIBS +=-lwinspool

LIBS += -L"openssl/lib" -llibeay32
LIBS += -L"openssl/lib" -lssleay32
INCLUDEPATH += openssl/include
}

INCLUDEPATH += public \
                                back \
                                dscreen


HEADERS += \
    dscreen/tableview_item.h \
    public/public_scale_led.h

SOURCES += \
    dscreen/tableview_item.cpp \
    public/public_scale_led.cpp



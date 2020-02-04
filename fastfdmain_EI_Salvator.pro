#-------------------------------------------------
#
# Project created by QtCreator 2015-12-06T22:56:01
#
#-------------------------------------------------

DEFINES += QT_NEW_SYSTEM
INCLUDEPATH += inputnew/

QT       += core gui sql network xml concurrent svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
lessThan(QT_MAJOR_VERSION, 5){
 QT += phonon
 DEFINES += QT_PHONON_SUPPORT
}

TARGET = SoftPos_EI_Salvator

TEMPLATE = app

#with system\ chinese
#/home/cz/Qt5.2.1/5.2.1/gcc_64/bin/lupdate ~/视频/fastfdmain/fastfdmain_new.pro
# /home/cz/Qt5.2.1/5.2.1/gcc_64/bin/linguist

DESTDIR = .

include(peripheral/peripheral.pri)

include(testwx/testwx.pri)

include(lds/lds.pri)

include(output_lib/output_lib.pri)

include(fileviewer/fileviewer.pri)

include(mwrf/mwrf.pri)

include(inputnew/inputnew.pri)

include(hddpos/hddpos.pri)

include(fastfdmain_EI_Salvator/fastfdmain_EI_Salvator.pri)


RESOURCES += \
    res.qrc

RC_FILE = fastfd.rc


unix{
#libcurl
CONFIG(debug,debug|release):INCLUDEPATH += /usr/local/curl/include
CONFIG(release,debug|release):INCLUDEPATH += /usr/local/libcurl/include
CONFIG(debug,debug|release):LIBS += /usr/local/curl/lib/libcurl.a -lz -lrt
CONFIG(release,debug|release):LIBS +=-lcurl
#~libcurl
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
    public/pushbutton_underline.h

SOURCES += \
    public/pushbutton_underline.cpp

INCLUDEPATH += public/


#-------------------------------------------------
#
# Project created by QtCreator 2015-12-06T22:56:01
#
#-------------------------------------------------

INCLUDEPATH += $$PWD/inputnew/

QT       += core gui sql network xml concurrent svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
lessThan(QT_MAJOR_VERSION, 5) {
 QT += phonon
 DEFINES += QT_PHONON_SUPPORT
}
unix{
contains(QT_VERSION, 4.8.6):DEFINES += LINPOS_DEBUG
contains(QT_VERSION, 4.8.7):DEFINES += LINPOS_OS6
contains(QT_VERSION, 5.11.3):DEFINES += LINPOS_OS7
}
win32:DEFINES += LINPOS_WIN


QMAKE_CXXFLAGS += -std=c++11

win32:TARGET = softpos_patch
#win32:TARGET = softpos_patch
unix:TARGET = SoftPos

TEMPLATE = app

TRANSLATIONS +=ts/EN_fastfd.ts

DESTDIR = .

include(qt/qt.pri)

include(back/back.pri)

include(public/public.pri)

include(peripheral/peripheral.pri)

include(testwx/testwx.pri)

include(lds/lds.pri)

include(fastfd_update/fastfd_update.pri)

include(output_lib/output_lib.pri)

include(fileviewer/fileviewer.pri)

include(mwrf/mwrf.pri)

include(inputnew/inputnew.pri)

include(hddpos/hddpos.pri)

include(saas/saas.pri)

include(sendemailapi/sendemailapi.pri)

include(label_img_design/label_img_design.pri)

SOURCES += main.cpp \
#    ../QtProject/untitled15/mainwindow.cpp

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
#libcurl
#CONFIG(debug,debug|release):INCLUDEPATH += /usr/include
#CONFIG(release,debug|release):INCLUDEPATH += /usr/local/libcurl/include
#LIBS +=-lcurl
#~libcurl
include(qjson4-master/QJson4.pri)
}

win32{
LIBS +=-lwinspool

#openssl
LIBS += -L"openssl/lib" -llibeay32
LIBS += -L"openssl/lib" -lssleay32
INCLUDEPATH += openssl/include
#～openssl

#libcurl
#LIBS += -L"curl/lib/------------------------------" -llibcurl
#LIBS += -L"curl/lib/curl" -llibcurld
#INCLUDEPATH += curl/include
#~libcurl

#quazip
include(mysql_install/mysql_install.pri)

INCLUDEPATH += quazip/include
CONFIG(debug, debug|release) {
    LIBS += -L"quazip/lib" -lquazipd
} else {
    LIBS += -L"quazip/lib" -lquazip
}
#~quazip

}

#查看进程打开文件数：lsof -c SoftPos | wc -l
#window depend on NoxApp.dll、libmysql.dll、mwrf32.dll
#http://download.qt.io/archive/qt/5.8/5.8.0/

#ubuntu mysql
#https://blog.csdn.net/yhf19881015/article/details/9048467
#sudo apt-get download libqt4-sql-mysql

#ubuntu lisusb
#https://blog.csdn.net/desert187/article/details/46619739
#sudo apt-get install libusb*

#ubuntu phonon
#https://blog.csdn.net/stpallas/article/details/8740737
#sudo apt-get install libphonon-dev libphonon4 phonon-backend-gstreamer


#ubuntu linguist
#/home/cz/Qt5.2.1/5.2.1/gcc_64/bin/lupdate ~/文档/fastfdmain/fastfdmain_new.pro
#/home/cz/Qt5.8.0/5.8/gcc_64/bin/lupdate ~/文档/fastfdmain//fastfdmain_new.pro
#/home/cz/Qt5.8.0/5.8/gcc_64/bin/linguist

#ubuntu openssl
#https://www.linuxidc.com/Linux/2015-10/124001.htm
#sudo apt-get install apache2
#sudo apt-get install openssl
#sudo apt-get install libssl-dev

#ubuntu libcurl
#apt-get install libcurl4-openssl-dev
#https://blog.csdn.net/u011641885/article/details/46900771

#打印机设置
#1f1b1fff25,25是code page
#1f1b1ffe01,1 off，0 open （chinese model）

#远程编译说明
#地址:192.168.1.187
#用户名:cz
#密码:123321
#/opt/qt-hddlinuxqt4/bin/qmake
#make -j6

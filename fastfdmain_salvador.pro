#-------------------------------------------------
#
# Project created by QtCreator 2015-12-06T22:56:01
#
#-------------------------------------------------

DEFINES += QT_NEW_SYSTEM
INCLUDEPATH += $$PWD/inputnew/

QT       += core gui sql network xml concurrent svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
lessThan(QT_MAJOR_VERSION, 5){
 QT += phonon
 DEFINES += QT_PHONON_SUPPORT
}
 DEFINES += QT_SALVADOR_SUPPORT

TARGET = SoftPos

TEMPLATE = app

TRANSLATIONS +=ts/EN_fastfd.ts
TRANSLATIONS +=ts/FA_fastfd.ts
TRANSLATIONS +=ts/DE_fastfd.ts

TRANSLATIONS +=ts/FR_fastfd.ts
TRANSLATIONS +=ts/RU_fastfd.ts
TRANSLATIONS +=ts/ES_fastfd.ts
TRANSLATIONS +=ts/TR_fastfd.ts
TRANSLATIONS +=ts/AR_fastfd.ts

TRANSLATIONS +=ts/KO_fastfd.ts
TRANSLATIONS +=ts/JA_fastfd.ts
TRANSLATIONS +=ts/IW_fastfd.ts
TRANSLATIONS +=ts/IT_fastfd.ts
TRANSLATIONS +=ts/PT_fastfd.ts

TRANSLATIONS +=ts/HI_fastfd.ts
TRANSLATIONS +=ts/BN_fastfd.ts
TRANSLATIONS +=ts/PA_fastfd.ts
TRANSLATIONS +=ts/MM_fastfd.ts

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

SOURCES += main.cpp \

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
#/home/cz/Qt5.8.0/5.8/gcc_64/bin/lupdate ~/文档/fastfdmain/fastfdmain_new.pro
#/home/cz/Qt5.8.0/5.8/gcc_64/bin/linguist

#ubuntu openssl
#https://www.linuxidc.com/Linux/2015-10/124001.htm
#sudo apt-get install apache2
#sudo apt-get install openssl
#sudo apt-get install libssl-dev

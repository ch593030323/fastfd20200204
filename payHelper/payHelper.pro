#-------------------------------------------------
#
# Project created by QtCreator 2018-09-07T13:10:17
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = payHelper
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    ../lds/cJSON.c \
    pay_helper_main.cpp \
    pay_helper_input.cpp \
    pay_helper_set.cpp \
    pay_helper_pay.cpp \
    pay_helper_loading.cpp

HEADERS += \
        mainwindow.h \
    ../lds/cJSON.h \
    pay_helper_main.h \
    pay_helper_input.h \
    pay_helper_set.h \
    pay_helper_pay.h \
    pay_helper_loading.h

FORMS += \
        mainwindow.ui \
    pay_helper_main.ui \
    pay_helper_input.ui \
    pay_helper_set.ui \
    pay_helper_pay.ui \
    pay_helper_loading.ui

include(../testwx/testwx.pri)

unix{
#openssl
INCLUDEPATH +=/usr/local/openssl/include
LIBS += -L/usr/local/openssl/lib -lssl -lcrypto -ldl -lpthread
#~openssl
}
win32{
LIBS += -L"openssl/lib" -llibeay32
LIBS += -L"openssl/lib" -lssleay32
}

INCLUDEPATH += openssl/include \
        ../lds/

RESOURCES += \
    res.qrc


RC_FILE = fastfd.rc

#支付助手发布文件中包含：openssl、libeay32.dll、libssl32.dll
#目前支持xp的话，需要Qt5.5.1编译通过并包含：ssleay32.dll、msvcr120.dll

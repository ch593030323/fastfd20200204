#-------------------------------------------------
#
# Project created by QtCreator 2019-11-12T17:51:14
#
#-------------------------------------------------

QT       += core gui sql network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kds_temphost
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        kds_public.cpp \
    ../back/w_bt_dish_kitchen_view_plan.cpp \
    ../hddpos/vch_print_memodata.cpp \
    ../hddpos/w_bt_dish_flag.cpp \
    ../hddpos/lds_query_hddpos.cpp \
    ../hddpos/lds_query_thread.cpp \
    ../hddpos/w_bt_dish_back.cpp \
    ../hddpos/w_bt_dish.cpp \
    ../hddpos/lds_model_sqltablemodel.cpp \
    ../hddpos/lds_model_sqltablemodel_delegate_com.cpp \
    ../hddpos/lds_tableview_delegate_virtual.cpp \
    ../hddpos/lds_tableView.cpp \
    ../hddpos/lds_tableview_virtual.cpp \
    ../hddpos/w_bt_dish_signal_transfer.cpp \
    ../hddpos/lds_tableview_repecify.cpp \
    ../hddpos/lds_tableview_header.cpp \
    kds_dishtype_setting.cpp


HEADERS  += mainwindow.h \
    kds_public.h \
    ../back/w_bt_dish_kitchen_view_plan.h \
    ../hddpos/vch_print_memodata.h \
    ../hddpos/w_bt_dish_flag.h \
    ../hddpos/lds_query_hddpos.h \
    ../hddpos/lds_query_thread.h \
    ../hddpos/w_bt_dish_back.h \
    ../hddpos/w_bt_dish.h \
    ../hddpos/lds_model_sqltablemodel.h \
    ../hddpos/lds_model_sqltablemodel_delegate_com.h \
    ../hddpos/lds_tableview_delegate_virtual.h \
    ../hddpos/lds_tableView.h \
    ../hddpos/lds_tableview_virtual.h \
    ../hddpos/w_bt_dish_signal_transfer.h \
    ../hddpos/lds_tableview_repecify.h \
    ../hddpos/lds_tableview_header.h \
    kds_dishtype_setting.h

include(../lds/lds.pri)

FORMS    += mainwindow.ui \
    ../back/w_bt_dish_kitchen_view_plan.ui \
    kds_dishtype_setting.ui

INCLUDEPATH += ../lds \
    ../hddpos \
    ../back \
    .. \

RESOURCES += \
    ../res.qrc \
    kds_res.qrc

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
}


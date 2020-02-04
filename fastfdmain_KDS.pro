#-------------------------------------------------
#
# Project created by QtCreator 2015-12-06T22:56:01
#
#-------------------------------------------------

DEFINES += QT_NEW_SYSTEM
INCLUDEPATH += $$PWD/inputnew/

QT       += core gui sql network xml concurrent svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
lessThan(QT_MAJOR_VERSION, 5) {
 QT += phonon
 DEFINES += QT_PHONON_SUPPORT
}

QMAKE_CXXFLAGS += -std=c++11

TARGET = kds

TEMPLATE = app

TRANSLATIONS +=/home/cz/文档/kds_en.ts

DESTDIR = .


HEADERS += \
    public/public_select_stringlist.h \
    hddpos/ftableview_standmodel_virtual.h \
    hddpos/ftableview_standmodel_sql_none.h \
    hddpos/ftableview_delegate.h \
    hddpos/ftableview.h \
    hddpos/lds_tableview_timeout.h \
    hddpos/lds_tableview_virtual.h \
    hddpos/lds_tableview_repecify.h \
    hddpos/w_bt_dish_flag.h \
    hddpos/lds_lan_terminal.h \
    hddpos/vch_print_memodata.h \
    lds/lds_toolbar_button.h \
    lds/lds_pushbutton_wrap.h \
    lds/lds_messagebox.h \
    lds/lds_roundeddialog.h \
    lds/lds_roundeddialog_title.h \
    lds/lds_roundeddialog_rect_align.h \
    lds/lds_dialog_drog.h \
    lds/lds_checkbox.h \
    lds/lds_checkbox_wrap.h \
    lds/lds_checkbox_phonestyle.h \
    lds/lds.h \
    lds/lds_dd.h \
    lds/lds_query_db.h \
    lds/lds_ogg_play.h \
    lds/cjson_help.h \
    lds/cJSON.h \
    lds/lds_query_variant.h \
    lds/lds_model_sqltablemodel_m.h \
    lds/lds_record_do_longpress.h \
    lds/lds_expression2rpn.h \
    lds/lds_model_sqltablemodel_d.h \
    lds/lds_tablewidget.h \
    lds/lds_label_circle_num.h \
    lds/lds_pushbutton.h \
    lds/lds_combobox.h \
    lds/label_click.h \
    back/w_bt_dish_kitchenprinter_printerset_port_dialog.h \
    back/w_bt_dish_kitchenprinter_printerset_port.h \

SOURCES += \
    public/public_select_stringlist.cpp \
    hddpos/ftableview_standmodel_virtual.cpp \
    hddpos/ftableview_standmodel_sql_none.cpp \
    hddpos/ftableview_delegate.cpp \
    hddpos/ftableview.cpp \
    hddpos/lds_tableview_timeout.cpp \
    hddpos/lds_tableview_virtual.cpp \
    hddpos/lds_tableview_repecify.cpp \
    hddpos/w_bt_dish_flag.cpp \
    hddpos/lds_lan_terminal.cpp \
    hddpos/vch_print_memodata.cpp \
    lds/lds_toolbar_button.cpp \
    lds/lds_pushbutton_wrap.cpp \
    lds/lds_messagebox.cpp \
    lds/lds_roundeddialog.cpp \
    lds/lds_roundeddialog_title.cpp \
    lds/lds_roundeddialog_rect_align.cpp \
    lds/lds_dialog_drog.cpp \
    lds/lds_checkbox.cpp \
    lds/lds_checkbox_wrap.cpp \
    lds/lds_checkbox_phonestyle.cpp \
    lds/lds.cpp \
    lds/lds_dd.cpp \
    lds/lds_query_db.cpp \
    lds/lds_ogg_play.cpp \
    lds/cjson_help.cpp \
    lds/cJSON.c \
    lds/lds_query_variant.cpp \
    lds/lds_model_sqltablemodel_m.cpp \
    lds/lds_record_do_longpress.cpp \
    lds/lds_expression2rpn.cpp \
    lds/lds_model_sqltablemodel_d.cpp \
    lds/lds_tablewidget.cpp \
    lds/lds_label_circle_num.cpp \
    lds/lds_pushbutton.cpp \
    lds/lds_combobox.cpp \
    lds/label_click.cpp \
    back/w_bt_dish_kitchenprinter_printerset_port_dialog.cpp \
    back/w_bt_dish_kitchenprinter_printerset_port.cpp \

FORMS += \
    public/public_select_stringlist.ui \
    back/w_bt_dish_kitchenprinter_printerset_port_dialog.ui

INCLUDEPATH += hddpos/ \
                               public/ \
                               lds/ \
                               back/ \

include(fastfdmain_KDS/fastfdmain_KDS.pri)
include(output_lib/output_lib.pri)

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
CONFIG(debug,debug|release):INCLUDEPATH += /usr/include
CONFIG(release,debug|release):INCLUDEPATH += /usr/local/libcurl/include
LIBS +=-lcurl
#~libcurl
}

win32{
LIBS +=-lwinspool

#openssl
LIBS += -L"openssl/lib" -llibeay32
LIBS += -L"openssl/lib" -lssleay32
INCLUDEPATH += openssl/include
#～openssl

#libcurl
LIBS += -L"curl/lib/curl" -llibcurl
LIBS += -L"curl/lib/curl" -llibcurld
INCLUDEPATH += curl/include
#~libcurl

#quazip
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
#/home/cz/Qt5.8.0/5.8/gcc_64/bin/lupdate ~/文档/fastfdmain/fastfdmain_KDS.pro
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
#192,168.1.187
#cz
#123321
# /opt/qt-hddlinuxqt4/bin/qmake
#make -4

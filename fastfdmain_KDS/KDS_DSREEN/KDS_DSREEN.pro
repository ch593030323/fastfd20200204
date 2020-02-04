QT       += core gui sql network xml concurrent svg
unix{
QT += dbus
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

unix {
TARGET = kdsdd
}
TEMPLATE = app

DESTDIR = .

SOURCES += \
    main.cpp \
    dialog.cpp \
    ../../hddpos/ftableview_standmodel_virtual.cpp \
    ../../hddpos/ftableview_standmodel_sql_none.cpp \
    ../../hddpos/lds_lan_terminal.cpp \
    ../kds.cpp \
    ../../lds/lds.cpp \
    ../../lds/lds_dd.cpp \
    ../../lds/lds_query_db.cpp \
    ../../lds/lds_ogg_play.cpp \
    ../../lds/cjson_help.cpp \
    ../../lds/cJSON.c \
    ../../lds/lds_query_variant.cpp \
    ../../lds/lds_model_sqltablemodel_d.cpp \
    ../../lds/lds_model_sqltablemodel_m.cpp \
    ../../hddpos/vch_print_memodata.cpp \
    ../../lds/lds_expression2rpn.cpp \
    autofontlabel.cpp

FORMS += \
    dialog.ui \

HEADERS += \
    dialog.h \
    ../../hddpos/ftableview_standmodel_virtual.h \
    ../../hddpos/ftableview_standmodel_sql_none.h \
    ../../hddpos/lds_lan_terminal.h \
    ../kds.h \
    ../../lds/lds.h \
    ../../lds/lds_dd.h \
    ../../lds/lds_query_db.h \
    ../../lds/lds_ogg_play.h \
    ../../lds/cjson_help.h \
    ../../lds/cJSON.h \
    ../../lds/lds_query_variant.h \
    ../../lds/lds_model_sqltablemodel_d.h \
    ../../lds/lds_model_sqltablemodel_m.h \
    ../../hddpos/vch_print_memodata.h \
    ../../lds/lds_expression2rpn.h \
    autofontlabel.h

INCLUDEPATH += ../../public/ \
                             ../../hddpos/ \
                             ../ \
                             ../../lds/ \


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

RESOURCES += \
    res.qrc

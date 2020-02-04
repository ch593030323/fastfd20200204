#-------------------------------------------------
#
# Project created by QtCreator 2015-12-06T22:56:01
#
#-------------------------------------------------

QT       += core gui sql network xml concurrent
unix{
QT += dbus
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
win32 {
TARGET = ../../fastfdd#window下的有debug目录
}
unix {
TARGET = ../fastfdd
}
TEMPLATE = app

DESTDIR = .


HEADERS += \
    doublewidget.h \
    ../lds/cJSON.h \
    ../lds/lds.h \
    ../lds/lds_graphicsview.h \
    ../hddpos/lds_tableview_repecify.h \
    ../testwx/liberweima/bitstream.h \
    ../testwx/liberweima/config.h \
    ../testwx/liberweima/CreatQr.h \
    ../testwx/liberweima/erweima.h \
    ../testwx/liberweima/mask.h \
    ../testwx/liberweima/mmask.h \
    ../testwx/liberweima/mqrspec.h \
    ../testwx/liberweima/qrencode.h \
    ../testwx/liberweima/qrinput.h \
    ../testwx/liberweima/qrspec.h \
    ../testwx/liberweima/rscode.h \
    ../testwx/liberweima/split.h \
    ../lds/lds_dialog_drog.h \
    ../hddpos/lds_tableview_timeout.h \
    ../testwx/testwx_help.h \
    doublewidget_1024x600.h \
    ../hddpos/lds_model_sqltablemodel.h \
    ../lds/lds_expression2rpn.h \
    ../hddpos/lds_tableview_virtual.h \
    ../hddpos/lds_thread.h \
    ../lds/lds_model_sqltablemodel_d.h \
    ../lds/lds_record_do_longpress.h \
    ../lds/lds_bool.h \
    ../lds/lds_query_db.h \
    ../lds/lds_model_sqltablemodel_m.h \
    tableview_item.h \
    doublewidget_normal.h \
    ../hddpos/fexpandmain_table.h \
    doublewidget_scale.h \
    ../public/public_scale_led.h \
    ../lds/lds_dd.h \
    ../lds/lds_const_var.h \
    ../hddpos/lds_query_thread.h \
    ../lds/lds_query_variant.h \
    ../lds/cjson_help.h \
    ../lds/ldsvariantmap.h \
    ../lds/lds_tablewidget.h \
    fexpandmain_paying.h \
    ../lds/lds_label_cell.h \
    ../lds/lds_label_text.h \
    doublewidget_normal_paying.h


SOURCES += \
    main.cpp \
    doublewidget.cpp \
    ../lds/lds.cpp \
    ../lds/cJSON.c \
    ../lds/lds_graphicsview.cpp \
    ../hddpos/lds_tableview_repecify.cpp \
    ../testwx/liberweima/erweima.cpp \
    ../testwx/liberweima/bitstream.c \
    ../testwx/liberweima/CreatQr.c \
    ../testwx/liberweima/mask.c \
    ../testwx/liberweima/mmask.c \
    ../testwx/liberweima/mqrspec.c \
    ../testwx/liberweima/qrencode.c \
    ../testwx/liberweima/qrinput.c \
    ../testwx/liberweima/qrspec.c \
    ../testwx/liberweima/rscode.c \
    ../testwx/liberweima/split.c \
    ../lds/lds_dialog_drog.cpp \
    ../hddpos/lds_tableview_timeout.cpp \
    ../testwx/testwx_help.cpp \
    doublewidget_1024x600.cpp \
    ../hddpos/lds_model_sqltablemodel.cpp \
    ../lds/lds_expression2rpn.cpp \
    ../hddpos/lds_tableview_virtual.cpp \
    ../hddpos/lds_thread.cpp \
    ../lds/lds_model_sqltablemodel_d.cpp \
    ../lds/lds_record_do_longpress.cpp \
    ../lds/lds_bool.cpp \
    ../lds/lds_query_db.cpp \
    ../lds/lds_model_sqltablemodel_m.cpp \
    tableview_item.cpp \
    doublewidget_normal.cpp \
    ../hddpos/fexpandmain_table.cpp \
    doublewidget_scale.cpp \
    ../public/public_scale_led.cpp \
    ../lds/lds_dd.cpp \
    ../lds/lds_const_var.cpp \
    ../hddpos/lds_query_thread.cpp \
    ../lds/lds_query_variant.cpp \
    ../lds/cjson_help.cpp \
    ../lds/ldsvariantmap.cpp \
    ../lds/lds_tablewidget.cpp \
    fexpandmain_paying.cpp \
    ../lds/lds_label_cell.cpp \
    ../lds/lds_label_text.cpp \
    doublewidget_normal_paying.cpp


FORMS += \
    doublewidget.ui \
    ../hddpos/fexpandmain_payover.ui \
    fexpandmain_paying.ui

RESOURCES += \
    res.qrc

UI_DIR += .

RC_FILE = ../fastfd.rc

OTHER_FILES += \

INCLUDEPATH +=  ../lds \
                                ../public \
                                ../testwx/liberweima \
                                ../qt \
                                ../testwx \
                                ../back \
                                ../hddpos \

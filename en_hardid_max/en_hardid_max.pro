#-------------------------------------------------
#
# Project created by QtCreator 2017-04-07T15:46:52
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = en_hardid_max
TEMPLATE = app


SOURCES += main.cpp\
        en_hardid_max.cpp \
    ../lds/lds.cpp \
    ../lds/lds_const_var.cpp \
    ../lds/lds_dd.cpp \
    ../lds/lds_popup_pos.cpp

HEADERS  += en_hardid_max.h \
    ../lds/lds.h \
    ../lds/lds_const_var.h \
    ../lds/lds_dd.h \
    ../lds/lds_popup_pos.h

FORMS    += en_hardid_max.ui

RC_FILE = fastfd.rc

INCLUDEPATH += ../lds \
               ../lds/lds_vir


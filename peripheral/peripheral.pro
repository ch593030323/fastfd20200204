#-------------------------------------------------
#
# Project created by QtCreator 2015-09-09T17:58:30
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = output_set
TEMPLATE = app
TRANSLATIONS += peripheral.ts


SOURCES += main.cpp \
    mainwindow.cpp

HEADERS  += mainwindow.h \
    mainwindow.h

FORMS    += \
    mainwindow.ui

include(peripheral.pri)

include($$PWD/../output_lib/output_lib.pri)

RESOURCES += \
    peripheral_res.qrc


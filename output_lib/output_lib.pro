 #-------------------------------------------------
#
# Project created by QtCreator 2015-12-06T22:56:01
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = printer
TEMPLATE = app
DESTDIR = .

include(output_lib.pri)
RC_FILE = fastfd.rc

SOURCES += \
    main.cpp \
    dialog.cpp \
    mainwindow.cpp

FORMS += \
    dialog.ui \
    mainwindow.ui

HEADERS += \
    dialog.h \
    mainwindow.h

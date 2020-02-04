#-------------------------------------------------
#
# Project created by QtCreator 2016-11-30T14:16:30
#
#-------------------------------------------------

QT       += core network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32 {
TARGET = ../../fastfd_updated#window下的有debug目录
}
unix {
TARGET = ../fastfd_updated
}

TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    fastfd_update.cpp \
    ../lds/lds_const_var.cpp

HEADERS  += mainwindow.h \
    fastfd_update.h \
    ../lds/lds_const_var.h

FORMS    += mainwindow.ui \
    fastfd_update.ui

RC_FILE = fastfd_update.rc

RESOURCES += \
    res.qrc

INCLUDEPATH += ../lds

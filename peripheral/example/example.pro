QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = example
TEMPLATE = app


SOURCES += main.cpp\

LIBS += -L$$PWD/../lib/ -lperipheral_dll
INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

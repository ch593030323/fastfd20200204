INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/liberweima
DEPENDPATH += $$PWD


SOURCES += \
        $$PWD/widget.cpp \
    $$PWD/liberweima/erweima.cpp \
    $$PWD/liberweima/bitstream.c \
    $$PWD/liberweima/CreatQr.c \
    $$PWD/liberweima/mask.c \
    $$PWD/liberweima/mmask.c \
    $$PWD/liberweima/mqrspec.c \
    $$PWD/liberweima/qrencode.c \
    $$PWD/liberweima/qrinput.c \
    $$PWD/liberweima/qrspec.c \
    $$PWD/liberweima/split.c \
    $$PWD/liberweima/rscode.c

HEADERS  += widget.h \
    $$PWD/liberweima/bitstream.h \
    $$PWD/liberweima/config.h \
    $$PWD/liberweima/CreatQr.h \
    $$PWD/liberweima/erweima.h \
    $$PWD/liberweima/mask.h \
    $$PWD/liberweima/mmask.h \
    $$PWD/liberweima/mqrspec.h \
    $$PWD/liberweima/qrencode.h \
    $$PWD/liberweima/qrinput.h \
    $$PWD/liberweima/qrspec.h \
    $$PWD/liberweima/split.h \
    $$PWD/liberweima/rscode.h

FORMS    += widget.ui

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/liberweima
DEPENDPATH += $$PWD

SOURCES += \
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
    $$PWD/liberweima/rscode.c \
    $$PWD/widget.cpp \
    $$PWD/testwx_help.cpp \
    $$PWD/alipay_help.cpp \
    $$PWD/mobile_pay_help.cpp \
    $$PWD/ccb_pay_help.cpp \
    $$PWD/fupay_help.cpp \
    $$PWD/liantuofu_pay.cpp

HEADERS  += $$PWD/widget.h \
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
    $$PWD/liberweima/rscode.h \
    $$PWD/testwx_help.h \
    $$PWD/alipay_help.h \
    $$PWD/mobile_pay_help.h \
    $$PWD/ccb_pay_help.h \
    $$PWD/fupay_help.h \
    $$PWD/liantuofu_pay.h

FORMS    += $$PWD/widget.ui \

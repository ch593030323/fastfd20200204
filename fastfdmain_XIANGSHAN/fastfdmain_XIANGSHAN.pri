INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    $$PWD/qt_xiangshan_main.ui \
    $$PWD/qt_xiangshan_pay.ui \
    $$PWD/qt_xiangshan_pay_info.ui \
    $$PWD/qt_xiangshan_setting.ui

HEADERS += \
    $$PWD/qt_xiangshan.h \
    $$PWD/qt_xiangshan_main.h \
    $$PWD/qt_xiangshan_pay.h \
    $$PWD/qt_xiangshan_pay_info.h \
    $$PWD/qt_xiangshan_setting.h \
    fastfdmain_XIANGSHAN/qt_xiangshan_copy_file.h

SOURCES += \
    $$PWD/qt_xiangshan.cpp \
    $$PWD/qt_xiangshan_main.cpp \
    $$PWD/main.cpp \
    $$PWD/qt_xiangshan_pay.cpp \
    $$PWD/qt_xiangshan_pay_info.cpp \
    $$PWD/qt_xiangshan_setting.cpp \
    fastfdmain_XIANGSHAN/qt_xiangshan_copy_file.cpp

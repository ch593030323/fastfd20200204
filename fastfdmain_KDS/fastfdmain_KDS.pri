INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/main.cpp \
    $$PWD/kds_main_view.cpp \
    $$PWD/kds.cpp \
    $$PWD/kds_main_info_brief.cpp \
    $$PWD/kds_main_info_brief_frame.cpp \
    $$PWD/kds_login.cpp \
    $$PWD/kds_application.cpp \
    $$PWD/kds_quit.cpp \
    $$PWD/kds_quit_reason.cpp \
    $$PWD/kds_standmodel_sql.cpp \
    $$PWD/kds_tableview.cpp\
    $$PWD/kds_main_info_print.cpp

FORMS += \
    $$PWD/kds_main_view.ui \
    $$PWD/kds_main_info_brief.ui \
    $$PWD/kds_login.ui \
    $$PWD/kds_quit.ui \
    $$PWD/kds_quit_reason.ui \
    $$PWD/kds_main_info_print.ui

HEADERS += \
    $$PWD/kds_main_view.h \
    $$PWD/kds.h \
    $$PWD/kds_main_info_brief.h \
    $$PWD/kds_main_info_brief_frame.h \
    $$PWD/kds_login.h \
    $$PWD/kds_application.h \
    $$PWD/kds_quit.h \
    $$PWD/kds_quit_reason.h \
    $$PWD/kds_standmodel_sql.h \
    $$PWD/kds_tableview.h\
    $$PWD/kds_main_info_print.h

RESOURCES += \
    $$PWD/kds.qrc


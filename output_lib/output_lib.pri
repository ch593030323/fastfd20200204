INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/reader_rf.h \
    $$PWD/printer_pos.h \
    $$PWD/printer_pos_vidpid.h \
    $$PWD/printer_pos_serial.h \
    $$PWD/printer_pos_net.h \
    $$PWD/printer_normal.h \
    $$PWD/printer_normal_xinye.h \
    $$PWD/printer_normal_t2us.h \
    $$PWD/printer_normal_kf58.h \
    $$PWD/printer_normal_80_old.h \
    $$PWD/printer_normal_80.h \
    $$PWD/printer_normal_58.h \
    $$PWD/printer_0.h \
    $$PWD/electronicscale_ysheng.h \
    $$PWD/electronicscale_rongta.h \
    $$PWD/electronicscale_kaifeng.h \
    $$PWD/electronicscale_djian.h \
    $$PWD/electronicscale_dahua.h \
    $$PWD/electronicscale_cas.h \
    $$PWD/electroniccountprice_kaifeng.h \
    $$PWD/clientdisplay_no.h \
    $$PWD/clientdisplay.h \
    $$PWD/bmp_info.h \
    $$PWD/electronicscale.h \
    $$PWD/printer_label.h \
    $$PWD/printer_label_argox.h \
    $$PWD/printer_label_argox_pplb.h \
    $$PWD/printer_label_argox_ppla.h \
    $$PWD/printer_label_gainscha.h \
    $$PWD/printer_label_gainscha_1224t.h \
    $$PWD/printer_label_postek.h \
    $$PWD/printer_label_tsc_ttp_244_plus.h \
    $$PWD/printer_pos_parallel.h \
    $$PWD/printer_label_tsc_hprt.h \
    $$PWD/electronicscale_dahua_en.h \
    $$PWD/electronicscale_dahua_07.h \
    $$PWD/printer_pos_win_pos80.h \
    $$PWD/printer_normal_chuanwei.h \
    $$PWD/electroniccountprice_resolve.h \
    $$PWD/printer_normal_weihuang.h \
    $$PWD/reader_tel.h \
    $$PWD/qt_retail_scale_scpos_keyboard_thread.h

SOURCES += \
    $$PWD/reader_rf.cpp \
    $$PWD/printer_pos.cpp \
    $$PWD/printer_pos_vidpid.cpp \
    $$PWD/printer_pos_serial.cpp \
    $$PWD/printer_pos_net.cpp \
    $$PWD/printer_normal.cpp \
    $$PWD/printer_normal_xinye.cpp \
    $$PWD/printer_normal_t2us.cpp \
    $$PWD/printer_normal_kf58.cpp \
    $$PWD/printer_normal_80_old.cpp \
    $$PWD/printer_normal_80.cpp \
    $$PWD/printer_normal_58.cpp \
    $$PWD/printer_0.cpp \
    $$PWD/electronicscale_ysheng.cpp \
    $$PWD/electronicscale_rongta.cpp \
    $$PWD/electronicscale_kaifeng.cpp \
    $$PWD/electronicscale_djian.cpp \
    $$PWD/electronicscale_dahua.cpp \
    $$PWD/electronicscale_cas.cpp \
    $$PWD/electroniccountprice_kaifeng.cpp \
    $$PWD/clientdisplay_no.cpp \
    $$PWD/clientdisplay.cpp \
    $$PWD/bmp_info.cpp \
    $$PWD/electronicscale.cpp \
    $$PWD/printer_label.cpp \
    $$PWD/printer_label_argox.cpp \
    $$PWD/printer_label_argox_pplb.cpp \
    $$PWD/printer_label_argox_ppla.cpp \
    $$PWD/printer_label_gainscha.cpp \
    $$PWD/printer_label_gainscha_1224t.cpp \
    $$PWD/printer_label_postek.cpp \
    $$PWD/printer_label_tsc_ttp_244_plus.cpp \
    $$PWD/printer_pos_parallel.cpp \
    $$PWD/printer_label_tsc_hprt.cpp \
    $$PWD/electronicscale_dahua_en.cpp \
    $$PWD/electronicscale_dahua_07.cpp \
    $$PWD/printer_pos_win_pos80.cpp \
    $$PWD/printer_normal_chuanwei.cpp \
    $$PWD/electroniccountprice_resolve.cpp \
    $$PWD/printer_normal_weihuang.cpp \
    $$PWD/reader_tel.cpp \
    $$PWD/qt_retail_scale_scpos_keyboard_thread.cpp

include(qextserialport/qextserialport.pri)

INCLUDEPATH += $$PWD\
     $$PWD/include

DEPENDPATH += $$PWD\
    $$PWD/include

HEADERS += \
    $$PWD/barscale_num_input.h \
    $$PWD/barscale_downdialog.h \
    $$PWD/barscale_main_dialog.h \
    $$PWD/barscale_net_set_dialog.h \
    $$PWD/commonheader.h \
    $$PWD/frmmain.h \
    $$PWD/labelprint_design_dialog.h \
    $$PWD/labelprint_design_import_dialog.h \
    $$PWD/labelprint_goodinfo_add_dialog.h \
    $$PWD/labelprint_goodinfo_dialog.h \
    $$PWD/labelprint_goodinfo_select_dialog.h \
    $$PWD/labelprint_main_dialog.h \
    $$PWD/labelprint_set_dialog.h \
    $$PWD/labelprint_set_tagbarcode_dialog.h \
    $$PWD/labelprint_set_vidpid_dialog.h \
    $$PWD/labelprint_project_self_dialog.h \
    $$PWD/s_commands.h \
    $$PWD/s_document.h \
    $$PWD/s_mainwindow.h \
    $$PWD/uinormal_message.h \
    $$PWD/myhelper.h

SOURCES += \
    $$PWD/barscale_downdialog.cpp \
    $$PWD/barscale_main_dialog.cpp \
    $$PWD/barscale_net_set_dialog.cpp \
    $$PWD/barscale_num_input.cpp \
    $$PWD/commonheader.cpp \
    $$PWD/frmmain.cpp \
    $$PWD/labelprint_design_dialog.cpp \
    $$PWD/labelprint_design_import_dialog.cpp \
    $$PWD/labelprint_goodinfo_add_dialog.cpp \
    $$PWD/labelprint_goodinfo_dialog.cpp \
    $$PWD/labelprint_goodinfo_select_dialog.cpp \
    $$PWD/labelprint_main_dialog.cpp \
    $$PWD/labelprint_set_dialog.cpp \
    $$PWD/labelprint_set_tagbarcode_dialog.cpp \
    $$PWD/labelprint_set_vidpid_dialog.cpp \
    $$PWD/labelprint_project_self_dialog.cpp \
    $$PWD/s_commands.cpp \
    $$PWD/s_document.cpp \
    $$PWD/s_mainwindow.cpp \
    $$PWD/uinormal_message.cpp \

#串口文件#

win32 {
#HEADERS += \
#    $$PWD/win32_printer_argox_pplb.h

#SOURCES += \
#    $$PWD/win32_printer_argox_pplb.cpp

#http://www.libusb.org/wiki/libusb-win32#Installation
#To use libusb-win32 in your own programs include the supplied header file usb.h, and link against the import library (libraries for GCC, BCC, and MSVC 32bit/64bit are available).
#To avoid any version conflicts, DO NOT include the DLL libusb0.dll in your application's directory. The DLL is part of the driver and installed automatically to the Windows system directory.

#window 下面通过libusb来访问usb接口，或者更新的
#1.下载外部包libusb-win32-bin-1.2.6.0.zip
#2.根据不同的系统选择\bin 目录下的那个文件下exe进行安装
#3.安装主要是将系统上usb进行bind，方便libusb进行访问
#3.1安装时，要指定设备对应的pid,vid,以便之后对设备进行访
#4.安装好后，pro里包
#LIBS += -L$$PWD/libusb-win32-bin-1.2.6.0/lib/gcc/ -lusb
#5.软件好后，pro里包含头文件路径
#INCLUDEPATH += $$PWD/libusb-win32-bin-1.2.6.0/include
}#win32

FORMS += \
    $$PWD/barscale_net_set_dialog.ui \
    $$PWD/barscale_net_set_change_dialog.ui \
    $$PWD/barscale_main_dialog.ui \
    $$PWD/barscale_downdialog.ui \
    $$PWD/s_mainwindow.ui \
    $$PWD/labelprint_set_tagbarcode_dialog.ui \
    $$PWD/labelprint_set_dialog.ui \
    $$PWD/labelprint_main_dialog.ui \
    $$PWD/labelprint_goodinfo_select_dialog.ui \
    $$PWD/labelprint_goodinfo_dialog.ui \
    $$PWD/labelprint_design_save_dialog.ui \
    $$PWD/labelprint_design_import_dialog.ui \
    $$PWD/labelprint_design_dialog.ui \
    $$PWD/labelprint_goodinfo_add_dialog.ui \
    $$PWD/labelprint_set_vidpid_dialog.ui \
    $$PWD/barscale_num_input.ui \
    $$PWD/labelprint_project_self_dialog.ui

OTHER_FILES += \
    peripheral.qm \
    peripheral.ts \

SUBDIRS += \
    peripheral.pro \
    peripheral_dll.pro


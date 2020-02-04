#include <QApplication>
#include <QDialog>
#include <QTextCodec>
#include "lds.h"
#include <QtGui>
#include "lds_messagebox.h"
#include "qt_xiangshan_main.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //lds
    QSettings conf(lds::localdataPath +"/fastfd.ini", QSettings::IniFormat);
    lds::sysconf = &conf;
    lds::systemTextCodec();
    lds::LDS_INIT();
    //~lds
    //font
    QFont font = app.font();
    font.setPointSize(18);
    app.setFont(font);
    //~font
    //stylesheet
    QFile file(QString("userdata/settings/skin/fastfd_%1.qss")
               .arg(lds::sysconf->value("system_setting/skin_name", "obsidian").toString()));
    if(file.open(QFile::ReadOnly)) {
        qApp->setStyleSheet(file.readAll());
        file.close();
    }
    //~stylesheet


//    if(lds::appName() == "SoftPos") {
//        qt_XIANGSHAN_main::reboot();
//        return 0;
//    }
    qt_XIANGSHAN_main dialog;
    return dialog.exec();
}

#include <QApplication>
#include<QTextCodec>
#include<QSettings>
#include<QTranslator>
#include "getfilepath.h"
#include "tabview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //设置编号
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//中文可用//包含头文件
    QTextCodec::setCodecForLocale(codec);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif



    QSettings settings( "/usr/desk/desktop.ini",QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    QString language;
    language=settings.value(QString("desktop/language")).toString();
    QTranslator qtTranslator;
    if (language == "1") {//zh

        qtTranslator.load(":/music_en.qm");
        a.installTranslator(&qtTranslator);

    }

    QApplication::setStyle("plastique");

    return a.exec();
}

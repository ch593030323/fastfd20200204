#include "pay_helper_main.h"
#include <QApplication>
#include <QTextCodec>
#include <QIcon>

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


    a.setWindowIcon(QIcon(":/rec/logo.png"));
    pay_helper_main w;
    w.show();

    return a.exec();
}

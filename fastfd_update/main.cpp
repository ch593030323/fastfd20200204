#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "fastfd_update.h"
#include <QtDebug>
#include <QMessageBox>

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

    if(argc == 2)
        fastfd_update::setSoftVersiontCyPath(argv[1]);
    if(argc == 6) {
        QMap<QString, QString> map;
        for(int k = 1; k < argc; k ++) {
            QString arg = argv[k];
            map.insert(arg.mid(0, 3), arg.mid(3));
        }
        fastfd_update::setSoftVersiontCyPath(map.value("-ve"), map.value("-pa"), map.value("-zn"), map.value("-zp"), map.value("-an"));// argv[1], argv[2], argv[3], argv[4]);
    }
#ifdef QT_DEBUG
    fastfd_update::setSoftVersiontCyPath("V5");
#endif
    fastfd_update w;
    w.setWindowTitle("software update"+QString::number(argc));
    w.setWindowIcon(QPixmap(":/settings.png"));
    w.show();
    QTimer::singleShot(100, &w, SLOT(tocheck_down()));

    return a.exec();
}

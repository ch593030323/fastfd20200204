#include <QApplication>
#include "dialog.h"
#include <QTextCodec>
#include "lds.h"
#include "kds.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //设置编码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
    QSettings conf("userdata/kds/fastfd_kds.ini", QSettings::IniFormat);
    lds::sysconf = &conf;
    lds::LDS_INIT(lds::func_dec_price_def, lds::func_dec_num_def, lds::func_dec_amt_def);
    //
    QMap<QString, QString> paramValueMap;
    paramValueMap.insert("-ss", "1024x768");
    if(argc>1) {
        //1
        QStringList ps;
        for(int index=1;index < argc;index++) {
            ps.append(argv[index]);
        }
        foreach(const QString &p, ps) {
            paramValueMap.insert(p.left(3), p.mid(3));
        }
    }
    //
    Dialog w;
    w.resize(kds::xstringToSize(paramValueMap.value("-ss"), kds::sizeToxstring(a.desktop()->size())));//随主屏
    w.show();

    return a.exec();
}

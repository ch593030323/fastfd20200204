#include <QApplication>
#include "doublewidget_normal.h"
#include <QFile>
#include <QTextCodec>
#include <QProcess>
#include <QDesktopWidget>
#include <QSettings>
#include "lds.h"
#include <QLabel>
#include <QTranslator>
#include "doublewidget_1024x600.h"
#include <qdir.h>
#include <QFontDatabase>
#include "doublewidget_scale.h"
#include "doublewidget_normal_paying.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //工作环境
#if defined(Q_OS_UNIX )
    QDir::setCurrent("/usr/desk");
#endif
#if defined(QT_DEBUG)
    QDir::setCurrent("build-fastfdmain_new-Desktop-Debug");
#endif

    //设置编号
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//中文可用//包含头文件
    QTextCodec::setCodecForLocale(codec);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
    //
    QMap<QString, QString> paramValueMap;
    if(argc>1) {
        //1
        QStringList ps;
        for(int index=1;index < argc;index++) {
            ps.append(argv[index]);
        }
        foreach(const QString &p, ps) {
            paramValueMap.insert(p.left(3), p.mid(3));
        }

        /*
参数  详细      解释                   值
-dp -dishpath 菜品图片路径            /usr/desk
-ap -advpath 广告图片路径             /usr/desk
-ao -advonly 只显示广告不显示菜品      0，1（只显示）
-sn -skinname 皮肤名称                skyblue（正常），obsidian（夜间）
-ps  -primaryscreensize    副屏大小              随主屏（1024x768）
-ss  -secondaryscreensize    副屏大小              随主屏（1024x768）
-ty  -type     副屏类型               0（普通）,1（4.3吋）,2（双屏异显）,3（称重副屏），-1（不启用）
-la  -layout    布局方向              0（水平）,1（垂直）
-ro  -rotate    旋转                  0（任意度数）
*/
    }

    QString xdeskSize = QString("%1x%2").arg(app.desktop()->width()).arg(app.desktop()->height());
    lds::setSystemValue("_dishpath", paramValueMap.value("-dp", "/usr/desk"));
    lds::setSystemValue("_advpath", paramValueMap.value("-ap", "/usr/desk"));
    lds::setSystemValue("_showdish", paramValueMap.value("-sd", "1"));
    lds::setSystemValue("_skinname", paramValueMap.value("-sn", "skyblue"));
    lds::setSystemValue("_primaryscreensize", doublewidget::xstringToSize(paramValueMap.value("-ps"), xdeskSize));
    lds::setSystemValue("_secondaryscreensize", doublewidget::xstringToSize(paramValueMap.value("-ss"), doublewidget::sizeToxstring(doublewidget::_primaryscreensize() )));//随主屏

    lds::setSystemValue("_type", paramValueMap.value("-ty", "0"));
    lds::setSystemValue("_layout", paramValueMap.value("-la", "0"));
    lds::setSystemValue("_secondaryscreenx", paramValueMap.value("-sx", "0"));
    lds::setSystemValue("_fontpath", paramValueMap.value("-fp", ""));


    int FONT_ID = QFontDatabase::addApplicationFont(lds::systemValue("_fontpath").toString());
    if(-1 != FONT_ID) {
        app.setFont(ldsObjFont<QApplication>(&app, QFontDatabase::applicationFontFamilies(FONT_ID).value(0)));
    }
#ifdef QT_DEBUG
    ///home/cz/文档/build-fastfdmain_new-Desktop-Debug/userdata/fastfd_localdata/image_double_screen_dir
    lds::setSystemValue("_advpath", paramValueMap.value("-ap", "/home/cz/文档/build-fastfdmain_new-Desktop-Debug/userdata/fastfd_localdata/image_double_screen_dir"));
    lds::setSystemValue("_primaryscreensize", QSize(800, 600));
    lds::setSystemValue("_secondaryscreensize", QSize(800, 600));
#endif
    qDebug() << __FILE__ << __LINE__ <<  doublewidget::_dishpath();
    qDebug() << __FILE__ << __LINE__ <<  doublewidget::_advpath();
    qDebug() << __FILE__ << __LINE__ <<  doublewidget::_showdish();
    qDebug() << __FILE__ << __LINE__ <<  doublewidget::_skinname();
    qDebug() << __FILE__ << __LINE__ <<  doublewidget::_primaryscreensize();
    qDebug() << __FILE__ << __LINE__ <<  doublewidget::_secondaryscreensize();

    qDebug() << __FILE__ << __LINE__ <<  doublewidget::_type();
    qDebug() << __FILE__ << __LINE__ <<  doublewidget::_layout();
    qDebug() << __FILE__ << __LINE__ <<  doublewidget::_secondaryscreenx();

    //副屏的背景图片 fastfd_fuping_bg_pic

    //! 1-----------------配置文件
    QSettings conf(lds::localdataPath + "/fastfd.ini", QSettings::IniFormat);
    lds::sysconf = &conf;

    lds::               LDS_INIT(lds::func_dec_price, lds::func_dec_num, lds::func_dec_amt);                                 //公共参数里的初始化

    QFile file(":/fastfd_" + doublewidget::_skinname() + ".qss");
    if(file.open(QFile::ReadOnly)) {
        //logo
        app.setStyleSheet(file.readAll());
        file.close();
    } else {
        qDebug() << file.errorString();
    }

    //    QSize dsize = lds::dsize();
    //    if(argc > 1) {
    //        dsize = QSize(QString(argv[1]).toInt(), QString(argv[2]).toInt());
    //    }
    //    QSize dsize = QSize(doublewidget::_resize.split("x").value(0).toInt(), doublewidget::_resize.split("x").value(1).toInt());
    //    int fuping_mode = doublewidget::_type.toInt();
    //    int fuping_mode = lds::sysconf->value("system_setting/fuping_mode", -1).toInt();
    QDialog *dialog = 0;
    switch(doublewidget::_type().toInt()) {
    case 0://普通副屏
        dialog = new doublewidget_normal(doublewidget::_secondaryscreensize());
        dialog->move(0, 0);
        break;
    case 1://1024*600客显
        dialog = new doublewidget_1024x600(doublewidget::_secondaryscreensize());
        dialog->move(0, 0);
        break;
    case 2://双屏异显
        dialog = new doublewidget_normal(doublewidget::_secondaryscreensize());
        dialog->move(doublewidget::_secondaryscreenx().toInt(), 0);
        break;
    case 3://称重副屏
        dialog = new doublewidget_scale(doublewidget::_secondaryscreensize());
        dialog->move(0, 0);
        break;
    case 4://普通副屏+
        dialog = new doublewidget_normal_paying(doublewidget::_secondaryscreensize());
        dialog->move(0, 0);
        break;
    case -1://不启用
    default:
        return 1;
    }
#ifdef QT_DEBUG
    dialog->move(20, 1000);
#endif
    //显示
    bool ret = app.exec();

    if(dialog) delete dialog;
    return ret;
}

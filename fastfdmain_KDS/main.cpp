#include <QtGui/QApplication>
#include <QTextCodec>
#include <QMessageBox>
#include "kds_main_view.h"
#include "lds.h"
#include "kds.h"
#include <QSqlDatabase>
#include <QTranslator>
#include <QFile>
#include "kds_login.h"
#include "lds_messagebox.h"
#include "kds_application.h"
#include "lds_model_sqltablemodel_d.h"
#include "kds_quit.h"
#include "printer_pos.h"

int main(int argc, char *argv[])
{
    kds_application app(argc, argv);

    //设置编码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
    QSettings conf("userdata/kds/kds.ini", QSettings::IniFormat);
    lds::sysconf = &conf;
    app.setStyleSheet(kds::stylesheetData());
    lds::LDS_INIT(kds::func_dec_price, kds::func_dec_num, kds::func_dec_amt);
    lds::arg_forbid_debug(argc, argv);
 #ifdef QT_DEBUG
//    lds::terminalCode.setOther("AA80521164711041D");
//    lds::terminalCode.setOther("AA81206140055513C");
//    lds::terminalCode.setOther("AA80703090245838C");

//    lds::terminalCode.setOther("AA80819114715017D");
    lds::terminalCode.setOther("AA80413164727729D");
#endif
    //
    kds::setTranslator(lds::sysconf->value("kds_manager/language", "zh").toString());
    kds::kds_second_screen_translate();
    if(!kds::softCanStart())
        return 0;
    //
    //mediaPlayer
    lds_ogg_play_thread mediaPlayer;
    kds::mediaPlayer = &mediaPlayer;
    kds::mediaPlayer->start();
    //
    QSqlDatabase::addDatabase("QMYSQL");
    kds_main_view d;
    d.move(0, 0);
#ifdef QT_DEBUG
    d.move(1000, 0);
    lds_messagebox::messge_pos = QPoint(1000, 0);
#endif
    d.show();
    //
    return app.exec();
}

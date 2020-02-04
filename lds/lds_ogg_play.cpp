#include "lds_ogg_play.h"
#include <QEventLoop>
#include <QFile>
#include <QtDebug>

#ifdef QT_PHONON_SUPPORT
Phonon::MediaObject lds_ogg_play::mobject;
#endif
QList<QStringList> lds_ogg_play::current_playlist;
bool lds_ogg_play::isplaying = false;
QString lds_ogg_play::ogg_path;
lds_ogg_play::lds_ogg_play()
{
}

bool lds_ogg_play::play(const QString &ogg_path, double value, int decimals, QString *errstring)
{
    QStringList msource_list = getPlayList(value, decimals);
    //yuan
    msource_list.prepend(("suc1.ogg"));
    msource_list.append(("yuan.ogg"));

    return play(ogg_path, msource_list, errstring);
}

bool lds_ogg_play::play(const QString &ogg_path, const QStringList &playlist, QString *errstring)
{
#ifdef QT_DEBUG
    return true;
#endif

#ifdef QT_PHONON_SUPPORT
    if(false == mobject.isValid())
    {
        if(errstring) *errstring = "MediaObject not is valid";
        return false;
    }
    static bool f = true;
    if(f) {//只有在收款机下才能工作，正常编译下会崩溃,生成一次
        lds_ogg_play::ogg_path = ogg_path;
        Phonon::createPath(&mobject, new Phonon::AudioOutput(Phonon::MusicCategory, &mobject));
        f = false;
    }

    ///
    current_playlist << playlist;
    if(isplaying) {
        return true;
    }
    QEventLoop eloop;
    QObject::connect(&mobject, SIGNAL(finished()), &eloop, SLOT(quit()));
    while(current_playlist.count() > 0) {
        QString filepath = "audio_for_cash_tmp_ogg";
        QStringList content = current_playlist.takeFirst();

        QFile file(filepath);
        file.remove();
        file.open(QFile::WriteOnly | QFile::Append);
        foreach(const QString &d, content) {
            QFile f(ogg_path + d);
            f.open(QFile::ReadOnly);
            file.write(f.readAll());
        }
        file.close();

        isplaying = true;
        mobject.clear();
        mobject.enqueue(Phonon::MediaSource(filepath));
        mobject.play();

        eloop.exec();

        isplaying = false;
    }
#endif
    qDebug() << __LINE__;
    return true;
}

bool lds_ogg_play::isPlaying()
{
    return current_playlist.count() > 0 || isplaying == true;
}

QStringList lds_ogg_play::getPlayList(double value, int decimals)
{
    QStringList msource_list;

    QString num = QString::number(value, 'f', decimals);
    QStringList zhen_dot= num.split(".");
    QString zhen = zhen_dot.value(0);
    QString dot = zhen_dot.value(1);

    //.12648
    while(dot.endsWith("0")) dot.chop(1);
    if(dot.length() > 0) dot.prepend(".");
    for(int k = 0; k < dot.count(); k++) {
        msource_list.append((charOggName(dot[k])));
    }
    //1245
    for(int k = 0; k < zhen.length(); k++) {
        int pos = zhen.length() - k - 1;
        QChar c = zhen.mid(pos, 1)[0];
        if(k % 4 == 0 && k != 0) {//“万”或“个（不读）”
            msource_list.prepend(("ten_thousand.ogg"));
            if(zhen.mid(pos + 1).startsWith("0")) {//千位为0时，不读“零千”，eg：“一万”
                remove(msource_list, 1, 2);//0千
                if(!zhen.mid(pos + 2).startsWith("0")) {//百位不为0时补读：“零”， eg：一万零一百
                    msource_list.insert(1, ("zero.ogg"));
                }
            }
        }
        if(k % 4 == 1) {//十
            msource_list.prepend(("ten.ogg"));
            if(zhen.mid(pos + 1).startsWith("0")) {//个位为0时，不读个位，eg：“二十”
                remove(msource_list, 1, 1);//0
            }
        }
        if(k % 4 == 2) {//百
            msource_list.prepend(("hundred.ogg"));
            if(zhen.mid(pos + 1).startsWith("0")) {//十位为0时，不读“零十”，eg：“一百”
                remove(msource_list, 1, 2);//0十
                if(!zhen.mid(pos + 2).startsWith("0")) {//个位不为0时补读：“零”， eg：一百零一
                    msource_list.insert(1, ("zero.ogg"));
                }
            }
        }
        if(k % 4 == 3) {//千
            msource_list.prepend(("thousand.ogg"));
            if(zhen.mid(pos + 1).startsWith("0")) {//百位为0时不读“零百”，eg：“一千”
                remove(msource_list, 1, 2);//0百
                if(!zhen.mid(pos + 2).startsWith("0")) {//十位为0时补读：“零”，eg：“一千零一”
                    msource_list.insert(1, ("zero.ogg"));
                }
            }
        }
        //"1十"读"十"
        if(zhen.length() == 2 && k % 4 == 1 && c == '1') {//一十读“十”, 110依旧读：一百一十
            continue;
        }
        msource_list.prepend((charOggName(c)));
    }

    //    //yuan
    //    msource_list.prepend(("suc1.ogg"));
    //    msource_list.append(("yuan.ogg"));

    qDebug() << __LINE__ << "msource_list:" << msource_list;
    return msource_list;
}

QString lds_ogg_play::charOggName(const QChar &c)
{
    if(c == QChar('0')) {
        return "zero.ogg";
    } else if(c == QChar('1')) {
        return "one.ogg";
    } else if(c == QChar('2')) {
        return "two.ogg";
    } else if(c == QChar('3')) {
        return "three.ogg";
    } else if(c == QChar('4')) {
        return "four.ogg";

    }  else if(c == QChar('5')) {
        return "five.ogg";
    } else if(c == QChar('6')) {
        return "six.ogg";
    } else if(c == QChar('7')) {
        return "seven.ogg";
    } else if(c == QChar('8')) {
        return "eight.ogg";
    } else if(c == QChar('9')) {
        return "nine.ogg";
    }

    else if(c == QChar('.')) {
        return "dot.ogg";
    }
    qDebug() << "error:" << c;
    return "";
}

void lds_ogg_play::remove(QStringList &list, int start, int length)
{
    for(int k = 0; k < length; k++) {
        list.removeAt(start);
    }
}


lds_ogg_play_thread::~lds_ogg_play_thread()
{
    this->quit();
    this->wait();
}

void lds_ogg_play_thread::play(const QString &ogg_path, double value, int decimals)
{
    emit signal_play(ogg_path, value, decimals);
}

void lds_ogg_play_thread::play(const QString &ogg_path, const QStringList &playlist)
{
    emit signal_play(ogg_path, playlist);
}

void lds_ogg_play_thread::run()
{
    lds_ogg_play_event e;
    connect(this, SIGNAL(signal_play(QString,double,int)), &e, SLOT(toplay(QString,double,int)));
    connect(this, SIGNAL(signal_play(QString,QStringList)), &e, SLOT(toplay(QString,QStringList)));

    connect(&e, SIGNAL(signal_error(QString,QString)), this, SIGNAL(signal_error(QString,QString)));
    this->exec();
}


void lds_ogg_play_event::toplay(const QString &ogg_path, double value, int decimals)
{
    QString errstring;
    if(false == lds_ogg_play::play(ogg_path, value, decimals, &errstring)) {
        emit signal_error(errstring, "ogg play");
    } else {
    }
}

void lds_ogg_play_event::toplay(const QString &ogg_path, const QStringList &playlist)
{
    QString errstring;
    if(false == lds_ogg_play::play(ogg_path, playlist, &errstring)) {
        emit signal_error(errstring, "ogg play");
    } else {
    }
}

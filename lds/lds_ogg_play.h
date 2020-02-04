#ifndef LDS_OGG_PLAY_H
#define LDS_OGG_PLAY_H

#ifdef QT_PHONON_SUPPORT
#include <phonon>
#endif
#include <QStringList>
#include <QThread>

class lds_ogg_play
{
public:
    lds_ogg_play();

    //play(指定音频位置目录,12, 2)
    //播放：收款成功十二点零零元
    static bool play(const QString &ogg_path, double value, int decimals = 2, QString *errstring = 0);
    //play(指定音频位置目录,音频文件list(单号.ogg, P.ogg, 2.ogg,3.ogg))
    //播放：单号P23
    static bool play(const QString &ogg_path, const QStringList &playlist, QString *errstring = 0);

    //拼接数字的音频文件list
    static QStringList getPlayList(double value, int decimals = 2);

    static bool isPlaying();

    static QString charOggName(const QChar &c);

private:
    static void remove(QStringList &list, int start, int length);

#ifdef QT_PHONON_SUPPORT
    static Phonon::MediaObject mobject;
#endif

    static QList<QStringList> current_playlist;
    static bool isplaying;
    static QString ogg_path;
};

class lds_ogg_play_event : public QObject {
    Q_OBJECT
public:
    lds_ogg_play_event(QObject *parent = 0) : QObject(parent) {}
public slots:
    void toplay(const QString &ogg_path, double value, int decimals);
    void toplay(const QString &ogg_path, const QStringList &playlist);
signals:
    void signal_error(const QString &err, const QString &title);
};

class lds_ogg_play_thread : public QThread {
    Q_OBJECT
public:
    lds_ogg_play_thread(QObject *parent = 0) : QThread(parent) {}
    ~lds_ogg_play_thread();
    void play(const QString &ogg_path, double value, int decimals = 2);
    void play(const QString &ogg_path, const QStringList &playlist);
signals:
    void signal_play(const QString &ogg_path, double value, int decimals);
    void signal_play(const QString &ogg_path, const QStringList &playlist);
    void signal_error(const QString &err, const QString &title);
protected:
    void run();
};

#endif // LDS_OGG_PLAY_H

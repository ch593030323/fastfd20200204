#ifndef LDS_QUERY_THREAD_H
#define LDS_QUERY_THREAD_H

#include <QThread>
#include <QSqlDatabase>

class lds_query_thread : public QThread
{
    Q_OBJECT
public:
    explicit lds_query_thread(QObject *parent = 0);

    static QString getThreadDbname();
    static QString getThreadDbnameAndCreat();
    static QSqlDatabase getThreadDbAndCreat();
    static QSqlDatabase getThreadDbAndOpen();
    static const Qt::HANDLE MAIN_THREAD_ID;
    static void msecSleep(unsigned long msec);
    static void secSleep(unsigned long sec);

};

#endif // LDS_QUERY_THREAD_H

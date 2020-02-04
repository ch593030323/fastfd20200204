#ifndef LDS_THREAD_H
#define LDS_THREAD_H

#include <QThread>
#include <QStringList>
#include <QSqlDatabase>

class lds_thread_event : public QObject {
    Q_OBJECT
public:
    lds_thread_event(QObject *parent = 0) : QObject(parent) {}
public slots:
    virtual void toExec() = 0;
    ////////////////////
Q_SIGNALS:
    void signal_busy(bool);
public slots:
    void toExecBusy();
};


//2 能够正常析构
//3 线程不进行统一管理静态函数
class lds_thread : public QThread
{
    Q_OBJECT
public:
    static void msecSleep(unsigned long msec);
    static void secSleep(unsigned long sec);
public:
    explicit lds_thread(const QString &title, QObject *parent = 0);
    ~lds_thread();
    bool tryTrigger();
public slots:
    void ToTryTrigger();
protected:
    QString title;
    ////////////////////////一般run的个数之存在一个
protected:
    int execEvent(lds_thread_event *event);
    int exec();
signals:
    void signalTrigger();
private:
    bool  isBusy();
private slots:
    void toSetBusy(bool flag);
private:
    bool busy_state;
    ////////////////////////一般run的个数之存在一个
};
#endif // LDS_THREAD_H

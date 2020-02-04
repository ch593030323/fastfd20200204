#ifndef ELECTRONICCOUNTPRICE_KAIFENG_H
#define ELECTRONICCOUNTPRICE_KAIFENG_H

//【0】 取重范围：0～15000g
#include <QThread>

class Printer_POS_SERIAL;

class Printer_POS_SERIAL_Thread_scale;
class ElectronicCountPrice_kaifeng : public QObject
{
    Q_OBJECT
public:
    explicit ElectronicCountPrice_kaifeng(QObject *parent = 0);
    ~ElectronicCountPrice_kaifeng();
    void setSendData(const QByteArray &data);
    QByteArray sendData();
    bool tryStart_qs(const QString &path, int baudrate); //尝试计价秤开始读数据，动态模式question answer【应答型】

    void stop();//停止，1.会暂停线程，2.和计时器，3.打开打串口也会关闭
    void restart();
    QString lastError();

    void setBaudrate(const QString &baudrate);
    const Printer_POS_SERIAL_Thread_scale *threader();
public:
    Printer_POS_SERIAL *printer;
    Printer_POS_SERIAL_Thread_scale *t;

};
class  Printer_POS_SERIAL_Thread_scale:public QThread
{
    Q_OBJECT
public:
    enum MODEL{
        QUESTION_ANSWER,
        ANSWER
    };

    Printer_POS_SERIAL_Thread_scale(QObject *parent = 0);
    virtual ~Printer_POS_SERIAL_Thread_scale();
    bool isbreak;
    QByteArray senddata;
    Printer_POS_SERIAL *_printer;
    void tostart();
    void torestart();
    void tostop();
    bool waitForStarted(int msecs = 50*100);

signals:
    void receiveData(const QByteArray &data);
    void timeout();
protected:
    virtual void run();
    //////////////!
private:
    bool isstarted;
private slots:
    void updatestate();
    //////////////~
};
#endif // ELECTRONICCOUNTPRICE_KAIFENG_H

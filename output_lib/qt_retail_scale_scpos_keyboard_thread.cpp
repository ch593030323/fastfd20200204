#include "qt_retail_scale_scpos_keyboard_thread.h"
#include <QTimer>
#include <QKeyEvent>
#include <QApplication>
#include <QMutexLocker>
#include <QWidget>

qt_retail_scale_scpos_keyboard_thread::qt_retail_scale_scpos_keyboard_thread(QObject *parent)
    : QThread(parent)
{
}

qt_retail_scale_scpos_keyboard_thread::~qt_retail_scale_scpos_keyboard_thread()
{
    qDebug() << __FUNCTION__;
    this->quit();
    this->wait();
    qDebug() << __FUNCTION__;
}

void qt_retail_scale_scpos_keyboard_thread::qupi()
{
    emit signal_qupi();
}

void qt_retail_scale_scpos_keyboard_thread::zhiling()
{
    emit signal_zhiling();
}

void qt_retail_scale_scpos_keyboard_thread::scale_open(const QString &port, bool enable)
{
    emit signal_scale_loop_has_next(port, enable);
}

void qt_retail_scale_scpos_keyboard_thread::toNumKey(const QString &key)
{
    int keyUnicode = key.data()->unicode();
    QString keyValue = key;
    if(key == "E") {
        keyUnicode = Qt::Key_Return;
        keyValue = "";
    }
    if(key == "B") {
        keyUnicode = Qt::Key_Backspace;
        keyValue = "";
    }
    QKeyEvent keyPress(QEvent::KeyPress, keyUnicode, Qt::NoModifier, key);

    QWidget *w = qApp->focusWidget();
    if(w) {
        qApp->sendEvent(w, &keyPress);
    }
}

void qt_retail_scale_scpos_keyboard_thread::run()
{
    qt_retail_scale_scpos_keyboard_event event;

    //send
    connect(this, SIGNAL(signal_qupi()), &event, SLOT(qupi()));
    connect(this, SIGNAL(signal_zhiling()), &event, SLOT(zhiling()));
    connect(this, SIGNAL(signal_scale_loop_has_next(QString,bool)), &event, SLOT(scale_open(QString,bool)));

    //receive
    connect(&event, SIGNAL(signalNumKey(QString)), this, SLOT(toNumKey(QString)));
    connect(&event, SIGNAL(signalqupi(bool)), this, SIGNAL(signalqupi(bool)));
    connect(&event, SIGNAL(weighComming(double)), this, SIGNAL(weighComming(double)));
    connect(&event, SIGNAL(signalzhiling(bool)), this, SIGNAL(signalzhiling(bool)));
    connect(&event, SIGNAL(dataComing(QString)), this, SIGNAL(dataComing(QString)));

    this->exec();
}

void qt_retail_scale_scpos_keyboard_event::reslove(const QByteArray &scale_data, ElectronicCountPrice_resolve::resolveData &qupi, ElectronicCountPrice_resolve::resolveData &lingwei, ElectronicCountPrice_resolve::resolveData &scale_weight, QString &key)
{
    qupi.isValid = false;
    lingwei.isValid = false;
    scale_weight.isValid = false;
    key = "";

    if(scale_data.length() < 20) {
        return;
    }

    QByteArray flag   = scale_data.mid(0, 1);// +-
    QByteArray weight = scale_data.mid(1, 7);//weight
    QByteArray kg =     scale_data.mid(8, 2);//kg
    scale_data.mid(10, 1);//,
    QByteArray zero   = scale_data.mid(11, 1);//N非零位、Z零位
    scale_data.mid(12, 1);//,
    QByteArray tare   = scale_data.mid(13, 1);//G非去皮、N去皮
    scale_data.mid(14, 1);//,
    QByteArray stable = scale_data.mid(15, 1);//S稳定、N不稳定
    scale_data.mid(16, 1);//,
    key    = scale_data.mid(17, 1);//0~9数字键，.小数点、E回车键、B退格键、S功能键、T去皮键、Z置零键
    scale_data.mid(18, 1).toHex();//0d
    scale_data.mid(19, 1).toHex();//0a


    scale_weight.doubleValue = weight.toDouble(&scale_weight.isValid);
    if("-" == flag)
        scale_weight.doubleValue = -1 * scale_weight.doubleValue;

    lingwei.isValid = true;
    lingwei.boolValud = ("Z" == zero);

    qupi.isValid = true;
    qupi.boolValud = ("N" == tare);
}

qt_retail_scale_scpos_keyboard_event::qt_retail_scale_scpos_keyboard_event(QObject *parent)
    : QObject(parent)
    , _scale_loop_has_next(false)
    , handled_count(0)
{
    //    printer.setBaudRate(9600);
    //#ifdef QT_DEBUG
    //    printer.tryOpen("/dev/ttyS0");
    //#else
    //    printer.tryOpen(lds::sysconf->value("system_setting/dianzichengduankou").toString());
    //#endif
}

qt_retail_scale_scpos_keyboard_event::~qt_retail_scale_scpos_keyboard_event()
{
}

void qt_retail_scale_scpos_keyboard_event::zhiling()
{
    printer.writeHex("540d0a");
}

void qt_retail_scale_scpos_keyboard_event::qupi()
{
    printer.writeHex("5a0d0a");
}

void qt_retail_scale_scpos_keyboard_event::handled()
{
    //
    handled_m.lock();
    //
    ElectronicCountPrice_resolve::resolveData qupi;
    ElectronicCountPrice_resolve::resolveData lingwei;
    ElectronicCountPrice_resolve::resolveData scale_weight;
    QString key;
    //
    if(printer.bytesAvailable()) {
        printerd += printer.readAll();
        forever {
            int begin = printerd.indexOf("\x0d\x0a") - 18;
            if(begin < 0) {
                break;
            }
            QByteArray scale_d = printerd.mid(begin, 20);
            printerd.remove(begin, 20);

            reslove(scale_d, qupi, lingwei, scale_weight, key);

            emit dataComing(scale_d);
            if(key != " ") {
                qDebug() << "key:" << key;
                emit signalNumKey(QString().append(key));
            }

            if(scale_weight.isValid)
                emit weighComming(scale_weight.doubleValue);

            if(lingwei.isValid)
                emit signalzhiling(lingwei.doubleValue);

            if(qupi.isValid)
                emit signalqupi(qupi.doubleValue);

        } //forever
    }
    if(printerd.size() > 100) {
        qWarning() << "d size more than 1000";
        printerd = "";
    }
    if(_scale_loop_has_next)
        QTimer::singleShot(100, this, SLOT(handled()));
    //
    handled_m.unlock();
}

void qt_retail_scale_scpos_keyboard_event::scale_open(const QString &port, bool enable)
{
    bool loop_need_start = false;
    //
    handled_m.lock();
    bool loop_cur_has_next = _scale_loop_has_next;
    _scale_loop_has_next = enable;
    if(enable) {
        printer.setBaudRate(9600);
        printer.tryOpen(port);
    }
    loop_need_start = (_scale_loop_has_next && false == loop_cur_has_next);
    handled_m.unlock();
    //
    if(loop_need_start)
        handled();
}

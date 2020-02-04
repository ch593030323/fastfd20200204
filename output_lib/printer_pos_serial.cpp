#include "printer_pos_serial.h"
#include "qextserialport.h"
#include "printer_0.h"
#include <QDebug>
#include <QTimer>
#include <fcntl.h>

static const int FLOW_XONXOFF_serial_loop_count = 5;

Printer_POS_SERIAL::SERIAL_PORT_MUTEX_COM_MAP  Printer_POS_SERIAL::SERIAL_MAP;

QextSerialPort *Printer_POS_SERIAL::myCom(const QString &port)
{
    if(SERIAL_MAP.map.find(port) == SERIAL_MAP.map.end()) {
        SERIAL_MAP.map[port] = PortThreadData::create(port);
    }
    //
    if(SERIAL_MAP.map[port].id != QThread::currentThreadId()) {
        SERIAL_MAP.map[port].p->deleteLater();
        SERIAL_MAP.map[port] = PortThreadData::create(port);
    }

    return SERIAL_MAP.map[port].p;
}



void Printer_POS_SERIAL::SERIAL_PORT_MUTEX_COM_MAP_CLOSE_ALL()
{
    for(QMap<QString, PortThreadData>::iterator k = SERIAL_MAP.map.begin(); k != SERIAL_MAP.map.end(); k++) {
        PortThreadData d = k.value();
        d.p->close();
    }
}

Printer_POS_SERIAL::SERIAL_PORT_MUTEX_COM_MAP::SERIAL_PORT_MUTEX_COM_MAP()
{

}

Printer_POS_SERIAL::SERIAL_PORT_MUTEX_COM_MAP::~SERIAL_PORT_MUTEX_COM_MAP()
{
    for(QMap<QString, PortThreadData>::iterator k = map.begin(); k != map.end(); k++) {
        PortThreadData d = k.value();
        delete d.p;
    }
    map.clear();
}


Printer_POS_SERIAL::PortThreadData Printer_POS_SERIAL::PortThreadData::create(const QString &port)
{
    QStringList list = port.split(",");
    QString path = list.value(0).trimmed();
    PortThreadData d;
    d.p = new QextSerialPort;
    d.id = QThread::currentThreadId();
    d.p->setPortName(path);
    return d;
}

Printer_POS_SERIAL::Printer_POS_SERIAL() :
    _type(SERIAL)
{
    //定义串口对象，指定串口名和查询模式，这里使用Polling
    psettings.BaudRate=BAUD115200;
    psettings.DataBits=DATA_8;
    psettings.FlowControl=FLOW_OFF;
    psettings.Parity=PAR_NONE;
    psettings.StopBits=STOP_1;
    psettings.Timeout_Millisec = 10;//串口读写的延时

    FLOW_XONXOFF_enable = true;
}

Printer_POS_SERIAL::~Printer_POS_SERIAL()
{
    close();
}

qint64 Printer_POS_SERIAL::writedirect(const QByteArray &byte)
{
    qint64 ret = 0;
    if(myCom(_port)->flowControl() == FLOW_XONXOFF ) {//软件开启软流控
        if(byte.size() > 6) {//一次写入的内容大于6byte才会执行软流控
            //!流控打印
            if(FLOW_XONXOFF_enable) {//软件允许软流控写，若第一次写入失败，则FLOW_XONXOFF_enable = false
                ret = myCom(_port)->write(byte);
                myCom(_port)->flush();
                if(!FLOW_XONXOFF_wait_for()) {//等待串口返回允许继续写的信号
                    FLOW_XONXOFF_enable = false;
                }
            }
            //!~流控打印
            goto end;
        }
    }
    ret = myCom(_port)->write(byte);
end:
    return ret;
}

//in 你好
qint64 Printer_POS_SERIAL::write(const QString & str, QString languageKey)
{
    return writedirect(printer_0::trans_u2codepage(str, languageKey, "GBK"));
}

//in ef 02
qint64 Printer_POS_SERIAL::writeHex(const QString &hex)
{
    return writedirect(QByteArray::fromHex(QByteArray().append(hex)));
}

QString Printer_POS_SERIAL::lastError()
{
    return ErrString;
}

QString Printer_POS_SERIAL::port()
{
    return _port;
}

bool Printer_POS_SERIAL::waitForReadyReadv(int msecs)
{
    //u口不判断回读，部支持
    if(_type == USB) return false;

    //
    int count = msecs/50;
    while(count>=1
          && count--
          &&!myCom(_port)->bytesAvailable()) {
        printer_0_thread::msecSleep(50);
    }
    return count != 0;
}

void Printer_POS_SERIAL::close()
{
    if(_type == SERIAL) {

    } else {
        myCom(_port)->close();
    }
}

bool Printer_POS_SERIAL::devOpen()//open and new
{
    switch(_type) {
    case SERIAL:
        //波特率设置
        myCom(_port)->setBaudRate(psettings.BaudRate);

        //奇偶校验设置
        myCom(_port)->setParity(psettings.Parity);

        //数据位设置
        myCom(_port)->setDataBits(psettings.DataBits);

        //停止位设置
        myCom(_port)->setStopBits(psettings.StopBits);

        //数据流控制设置
        myCom(_port)->setFlowControl(psettings.FlowControl);

        //延时设置
        myCom(_port)->setTimeout(psettings.Timeout_Millisec);
        break;
    case USB:break;
    }
    //
    if(false == myCom(_port)->isOpen()) {
        if(false == myCom(_port) ->open(QIODevice::ReadWrite)) {
            ErrString = myCom(_port)->errorString();//QObject::tr("设备打开失败.没有权限");
            return false;
        }
    }
    return true;
}
/**
port,9600      , 0       , 8           , 0            , 0           , 100
port,baudrate,parity,databits,stopbits, flowtype,timeout
0-port             :/dev/ttyS0
1-baudrate     :50 - 115200；
2-parity           :PAR_NONE = 0,PAR_ODD,PAR_EVEN,PAR_SPACE
3-databits       :DATA_5=5, DATA_6, DATA_7, DATA_8
4-stopbits       :STOP_1=0,STOP_2
5-flowtype      :FLOW_OFF=0,FLOW_HARDWARE,FLOW_XONXOFF
6-timeout       : 100

eg. COM1,9600,0,8,1,0,100
*/
bool Printer_POS_SERIAL::tryOpen(const QString &port)
{
    FLOW_XONXOFF_enable = true;
    ErrString.clear();
    _port = port;

    QStringList list = port.split(",");
    QString path = list.value(0).trimmed();
    QString tmp;
    tmp=list.value(1).trimmed();if(!tmp.isEmpty()) psettings.BaudRate = BaudRateType(tmp.toInt());
    tmp=list.value(2).trimmed();if(!tmp.isEmpty()) psettings.Parity = ParityType(tmp.toInt());
    tmp=list.value(3).trimmed();if(!tmp.isEmpty()) psettings.DataBits = DataBitsType(tmp.toInt());
    tmp=list.value(4).trimmed();if(!tmp.isEmpty()) psettings.StopBits = StopBitsType(tmp.toInt());
    tmp=list.value(5).trimmed();if(!tmp.isEmpty()) psettings.FlowControl = FlowType(tmp.toInt());
    tmp=list.value(6).trimmed();if(!tmp.isEmpty()) psettings.Timeout_Millisec = tmp.toInt();

#ifdef Q_OS_UNIX
    if(false == QFile::exists(path)) {
        ErrString = QString("port doesn't exists").arg(_port);
        return false;
    }
#endif
    return devOpen();
}

void Printer_POS_SERIAL::setType(PORT_TYPE type)
{
    _type = type;
}

QStringList Printer_POS_SERIAL::fixedPorts()
{
    return QStringList()
        #ifdef Q_OS_UNIX
            <<("/dev/usb/lp0")
           <<("/dev/usb/lp1")
          <<("/dev/usb/lp2")
         <<("/dev/usb/lp3")
        <<("/dev/ttyS0")
       <<("/dev/ttyS1")
      <<("/dev/ttyS2")
     <<("/dev/ttyS3")
    <<("/dev/ttyS4")
    <<("/dev/ttyS5")
    <<("/dev/ttyS6");
#elif defined (Q_OS_WIN)
            <<("COM0")
           <<("COM1")
          <<("COM2")
         <<("COM3")
        <<("COM4")
       <<("COM5")
      <<("COM6")
     <<("COM7")
    <<("COM8")
    <<("COM9")
    <<("COM10")
    <<("COM11")
    <<("COM12")
    <<("COM13")
    <<("COM14")
    <<("COM15")

  #endif
      ;
}
///port,baudrate,parity,databits,stopbits, flowtype,timeout
QString Printer_POS_SERIAL::pathBaudRate(const QString &path, int baudRate)
{
    int dot_count = path.count(",");
    if(dot_count > 0) {
        QStringList k_list = path.split(",");
        for(; dot_count < 6; dot_count ++) {
            k_list << "";
        }
        k_list.replace(1, QString::number(baudRate));
        return k_list.join(",");
    }

    return QString("%1,%2").arg(path).arg(baudRate);
}

QMap<int, QString> &Printer_POS_SERIAL::creat_baudrates()
{
    static QMap<int, QString> map;
    if(map.isEmpty()) {
        map.insert(BAUD115200, "115200");
        map.insert(BAUD9600, "9600");
        map.insert(BAUD19200, "19200");
        map.insert(BAUD38400, "38400");
        map.insert(BAUD57600, "57600");
        map.insert(BAUD4800, "4800");
        map.insert(BAUD2400, "2400");
        map.insert(BAUD1200, "1200");
        map.insert(BAUD600, "600");
        map.insert(BAUD300, "300");
        map.insert(BAUD110, "110");
    }
    return map;
}

QStringList &Printer_POS_SERIAL::creat_baudrate_list()
{
    static QStringList list;
    if(list.isEmpty()) {
        list << QString::number(BAUD115200);
        list << QString::number(BAUD9600);
        list << QString::number(BAUD2400);
        list << QString::number(BAUD19200);
        list << QString::number(BAUD38400);
        list << QString::number(BAUD57600);
        list << QString::number(BAUD4800);
        list << QString::number(BAUD1200);
        list << QString::number(BAUD600);
        list << QString::number(BAUD300);
        list << QString::number(BAUD110);
    }
    return list;
}

QByteArray Printer_POS_SERIAL::readAll()
{return myCom(_port)->readAll();}

qint64 Printer_POS_SERIAL::bytesAvailable()
{return myCom(_port)->bytesAvailable();}
void Printer_POS_SERIAL::flush()
{myCom(_port)->flush();}
qint64 Printer_POS_SERIAL::read(char * data, qint64 maxSize)
{return myCom(_port)->read(data, maxSize);}

bool Printer_POS_SERIAL::isOpen()
{
    return myCom(_port)->isOpen();
}

void Printer_POS_SERIAL::setBaudRate(int baudRate)
{
    if(myCom(_port)->baudRate() != baudRate) {
        myCom(_port)->setBaudRate(BaudRateType(baudRate));
    }
    psettings.BaudRate = (BaudRateType)baudRate;
}


/*前提，打印机开启流控后，每发一次数据必定有回读，回读数据
 * 若空，继续读
 * 若是0x13，继续读，当低于设定的低位时，还会发送0x11，
 * 若是0x11，继续写
 * 数据回读至少一个char
 * 地址：http://blog.csdn.net/robertsong2004/article/details/38540507
 *当接收端的输入缓冲区内数据量超过设定的高位时,就向数据发送端发出XOFF字符（十进制的19或Control-S,设备编程说明书应该有详细阐述）
 * */

bool Printer_POS_SERIAL::FLOW_XONXOFF_readydata()
{
    //
    QByteArray readdata;
    int msecs = 1000;
    int mescs_piece = 10;
    int count = msecs/mescs_piece;

    //
    while(count > 0
          && count--
          && myCom(_port)->bytesAvailable() == 0) {
        printer_0_thread::msecSleep(mescs_piece);
    }

    //
    if(count != 0) {
        readdata = myCom(_port)->readAll();
        if(readdata.indexOf(0x13) < readdata.indexOf(0x11)) return true;//130011
        if(readdata.contains(0x13)) return false;//13
        return true;//11
    }
    return false;
}

bool Printer_POS_SERIAL::FLOW_XONXOFF_wait_for()
{
    for(int k = 0; k < FLOW_XONXOFF_serial_loop_count; k++) {
        if(FLOW_XONXOFF_readydata()) return true;
    }
    return false;
}

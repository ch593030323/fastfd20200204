#include "readcard_mwrf32.h"
#include <QDebug>

HANDLE ReadCard_mwrf32::handle = NULL;
ReadCard_mwrf32::ReadCard_mwrf32(QObject *parent) : QObject(parent)
{
    m_pwdMode = 1;
    m_sec = 1;

    init();
}

bool ReadCard_mwrf32::init()
{
    __int16 port = 1080;    //端口号
    long baud = 9600;       //波特率
    __int16 st;             //返回

    unsigned char _Status[30];
    memset(_Status,0,30);

    rf_exit(handle);
    handle = rf_init(port,baud);
    st = rf_get_status(handle,_Status);

    if((handle<0) || st)
        return false;
    else
        return true;
}

bool ReadCard_mwrf32::writeSqr(int block_id, const QString &part)
{
    if(!cardisready())
        return false;

    __int16 st;             //返回
    unsigned char data[16];
    memset(data,0,16);
    st = rf_write(handle,m_sec*4 + block_id,(unsigned char*)QByteArray().append(part).data());
    if(st)
    {
        qDebug() << ("写卡失败!");
        return false;
    }

    beep();
    return true;
}

bool ReadCard_mwrf32::writecard(const QString &part0, const QString &part1, const QString &part2)
{
    if(!cardisready())
        return false;

    __int16 st;             //返回
    unsigned char data[16];
    memset(data,0,16);
    st = rf_write(handle,m_sec*4 + 0,(unsigned char*)QByteArray().append(part0).data());
    if(st)
    {
        qDebug() << ("写卡失败!");
        return false;
    }
    st = rf_write(handle,m_sec*4 + 1,(unsigned char*)QByteArray().append(part1).data());
    if(st)
    {
        qDebug() << ("写卡失败!");
        return false;
    }
    st = rf_write(handle,m_sec*4 + 2,(unsigned char*)QByteArray().append(part2).data());
    if(st)
    {
        qDebug() << ("写卡失败!");
        return false;
    }

    beep();
    return true;
}

bool ReadCard_mwrf32::readcard(QString &part0, QString &part1, QString &part2)
{
    if(!cardisready())
        return false;

    __int16 st;             //返回
    unsigned char data[16];

    memset(data,0,16);
    st = rf_read(handle,m_sec*4 + 0,data);
    if(st)
    {
        qDebug() << ("读数据失败!");
        return false;
    }
    part0 = QByteArray::fromRawData((char*)data,16);

    memset(data,0,16);
    st = rf_read(handle,m_sec*4 + 1,data);
    if(st)
    {
        qDebug() << ("读数据失败!");
        return false;
    }
    part1 = QByteArray::fromRawData((char*)data,16);

    memset(data,0,16);
    st = rf_read(handle,m_sec*4 + 2,data);
    if(st)
    {
        qDebug() << ("读数据失败!");
        return false;
    }
    part2 = QByteArray::fromRawData((char*)data,16);

    beep();
    return true;
}

bool ReadCard_mwrf32::setDefPwd(const QString &defPwd)
{
    __int16 st;             //返回
    st = rf_load_key(handle,m_pwdMode,m_sec,
                     (unsigned char*)QByteArray::fromHex(QByteArray().append(defPwd)).data());
    if(st)
    {
        qDebug() << "载入密码失败";
        return false;
    }
    return true;
}

bool ReadCard_mwrf32::pwdChange(const QString &oldPwd, const QString &newPwd)
{
    if(!cardisready())
        return false;

    if(newPwd.count() != 12)
        return false;
    __int16 st;             //返回

    st = rf_changeb3(handle,m_sec,(unsigned char*)QByteArray::fromHex(QByteArray().append(newPwd)).data(),
                     0,0,0,1,0,(unsigned char*)QByteArray::fromHex(QByteArray().append(oldPwd)).data());
    if(st)
    {
        return false;
    }
    return true;
}

bool ReadCard_mwrf32::cardisready()
{
    __int16 st;             //返回

    st = rf_reset(handle,5);

    unsigned __int16 TagType;
    st = rf_request(handle,1,&TagType);
    if(st)
    {
        qDebug() << ("寻卡失败!");
        return false;
    }

    unsigned long Snr;
    st = rf_anticoll(handle,0, &Snr);
    if(st)
    {
        qDebug() << ("防冲突失败!");
        return false;
    }

    unsigned char Size;
    st = rf_select(handle,Snr,&Size);
    if(st)
    {
        qDebug() << ("选卡失败!");
        return false;
    }

    st = rf_authentication(handle,m_pwdMode,m_sec);
    if(st)
    {
        qDebug() << ("认证失败");
        return false;
    }

    return true;
}

void ReadCard_mwrf32::beep()
{
    rf_beep(handle,10);
}

QString ReadCard_mwrf32::lastError()
{
    return QObject::tr("读卡失败");
}

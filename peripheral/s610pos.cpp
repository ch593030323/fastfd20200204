#include "s610pos.h"
#include <QDebug>

#define start  0x02
#define send 0x81
#define get 0x01
#define zhenhao 0x01

#define end 0x03
s610pos::s610pos()
{
    CardCount = "";
    PortSettings myComSetting = {BAUD115200,DATA_8,PAR_NONE,STOP_1,FLOW_OFF,500};
    myCom = new QextSerialPort("/dev/ttyUSB0",myComSetting);
    myCom->setTimeout(5000); //设置延时
    //信号和槽函数关联，当串口缓冲区有数据时，进行读串口操作
    connect(myCom,SIGNAL(readyRead()),this,SLOT(readMyCom()));
    //定义串口对象，并传递参数，在构造函数里对其进行初始化
    //以可读写方式打开串口
    bool re = myCom ->open(QIODevice::ReadWrite);
    qDebug()<<re;
}

s610pos::~s610pos()
{

}

void s610pos::CRC16(const CHAR *pDataIn, int iLenIn, WORD *pCRCOut)
{
    WORD wResult = 0;
    WORD wTableNo = 0;

    for(int i = 0; i < iLenIn; i++)
    {
        wTableNo = ((wResult & 0xff) ^ (pDataIn[i] & 0xff));
        wResult = ((wResult >> 8) & 0xff) ^ wCRC16Table[wTableNo];
    }

    *pCRCOut = wResult;
    qDebug()<<*pCRCOut;
}

bool s610pos::readMyCom()
{
    QByteArray temp = myCom->readAll();
    array += temp;
    //qDebug()<<temp.at(0) <<temp.at(temp.length()-1);
    if(array.startsWith(0x02)&&array.endsWith(0x03))
    {
        //读取串口缓冲区的所有数据给临时变量temp
        //ui->textBrowser->insertPlainText(array.toHex());
        CardCount = array.mid(30,19);
        qDebug()<<"cardcount: "<<CardCount;

        return true;
    }
    else
        return false;
    //将串口的数据显示在窗口的文本浏览器中

}

void s610pos::readmagnetic()
{
    isReadCount = true;
    array = "";
    uchar data[] ={send,get,zhenhao,0x03,0x01,0x00,0x19,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00};

    WORD temp = 0x0000;
    WORD* crcout = &temp;
    CRC16((char*)data,26,crcout);
    QString CRC = QString::number(*crcout, 16).toUpper();

    CRCL = CRC.right(2).toInt(0,16);
    CRCH = CRC.remove(CRC.length()-2,2).toInt(0,16);
    QByteArray ICCzhen =start+QByteArray((char*)data,sizeof(data))+CRCL+CRCH+end;
    qDebug()<<myCom->write(ICCzhen);
    qDebug()<<*crcout;
    qDebug()<< QString::number(*crcout, 16).toUpper();
    qDebug()<<CRCL;
    qDebug()<<CRCH;
    qDebug()<<ICCzhen;
}


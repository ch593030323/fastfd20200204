#include "frmmain.h"
#include "myhelper.h"
#include "qextserialport.h"

frmMain::frmMain(QWidget *parent) :
    QWidget(parent)
{
    InitForm();
}

frmMain::~frmMain()
{    
}

void frmMain::InitForm()
{    
    ReceiveCount=0;
    SendCount=0;
    IsShow=true;
    IsAutoclear=false;
    IsHexSend=true;
    IsHexReceive=true;
    IsDebug=false;

    //读取数据(采用定时器读取数据，不采用事件，方便移植到linux)
    myReadTimer=new QTimer(this);
    myReadTimer->setInterval(1);
    connect(myReadTimer,SIGNAL(timeout()),this,SLOT(ReadMyCom()));
}

QString frmMain::jieshi(int a)
{
    QString ret;
    switch(a) {
    case 0xee:
        ret="0";
        break;
    case 0xfe:
        ret="0.";
        break;
    case 0x0a:
        ret="1";
        break;
    case 0x1a:
        ret="1.";
        break;
    case 0x67:
        ret="2";
        break;
    case 0x77:
        ret="2.";
        break;
    case 0x4f:
        ret="3";
        break;
    case 0x5f:
        ret="3.";
        break;
    case 0x8b:
        ret="4";
        break;
    case 0x9b:
        ret="4.";
        break;
    case 0xcd:
        ret="5";
        break;
    case 0xdd:
        ret="5.";
        break;
    case 0xed:
        ret="6";
        break;
    case 0xfd:
        ret="6.";
        break;
    case 0x0e:
        ret="7";
        break;
    case 0x1e:
        ret="7.";
        break;
    case 0xef:
        ret="8";
        break;
    case 0xff:
        ret="8.";
        break;
    case 0xcf:
        ret="9";
        break;
    case 0xdf:
        ret="9.";
        break;
    case 0xaf:
        ret="A";
        break;
    case 0xe9:
        ret="B";
        break;
    case 0xe4:
        ret="C";
        break;
    case 0x6b:
        ret="D";
        break;
    case 0xe5:
        ret="E";
        break;
    case 0xa5:
        ret="F";
        break;
    case 0x68:
        ret="U";
        break;
    case 0x69:
        ret="O";
        break;
    case 0x01:
        ret="-";
        break;
    case 0x11:
        ret="-.";
        break;
    case 0x29:
        ret="N";
        break;
    case 0xe0:
        ret="L";
        break;
    case 0x41:
        ret="=";
        break;
    case 0x00:
        ret=" ";
        break;
    case 0x10:
        ret=" .";
        break;
    default:
        ret="X";
        break;
    }
    return ret;
}

bool frmMain::openFrm(QString path,int rate)
{
    myCom = new QextSerialPort(path);

    if (myCom->open(QIODevice::ReadWrite))
    {
        //清空缓冲区
        myCom->flush();
        //设置波特率
        myCom->setBaudRate((BaudRateType)rate);
        //设置数据位
        myCom->setDataBits((DataBitsType)8);
        //设置校验位
        myCom->setParity((ParityType)0);
        //设置停止位
        myCom->setStopBits((StopBitsType)0);
        myCom->setFlowControl(FLOW_OFF);
        myCom->setTimeout(10);

        this->myReadTimer->start();
        return true;
    }
    else
        return false;
}

void frmMain::zhiling()
{
    QByteArray buf;
    buf.clear();

    buf[0] = 0x8D;
    myCom->write(buf);
}

void frmMain::qupi()
{
    QByteArray buf;
    buf.clear();

    buf[0] = 0x8C;
    myCom->write(buf);
}

void frmMain::ReadMyCom()
{
    //这个判断尤为重要,否则的话直接延时再接收数据,空闲时和会出现高内存占用
    if (myCom->bytesAvailable()<=17) {return;}

    // myHelper::Sleep(100);//延时100毫秒保证接收到的是一条完整的数据,而不是脱节的
    QByteArray temp=myCom->readAll();

    if (temp.size()>=24) {
        if (temp[0]==0x04 && temp[1]==0x02 && temp[2]==0x08 && temp[3]==0x20 && temp[20]==0x06&& temp[21]==0x82&& temp[22]==0x48&& temp[23]==0x28 ) {

            QString s;
            QString zhongliang;
//            QString danjia;
//            QString heji;
            for(int i = 3; i < 20; i++) {
                s.sprintf("%02x", (unsigned char)temp.at(i));
                switch(i) {
                case 4:
                    zhongliang=jieshi(myHelper::StrHexToDecimal(s));
                    if (zhongliang.indexOf(".")>0)
                    {
                        zhongliang=zhongliang.left(1);
                        //  ui->led_qupi->turnOn();
                        emit qupiComming(true);
                    }
                    else
                    {
                        //  ui->led_qupi->turnOff();
                        emit qupiComming(false);
                    }
                    break;
                case 5:

                    zhongliang+=jieshi(myHelper::StrHexToDecimal(s));
                    break;
                case 6:

                    zhongliang+=jieshi(myHelper::StrHexToDecimal(s));
                    break;
                case 7:

                    zhongliang+=jieshi(myHelper::StrHexToDecimal(s));
                    break;
                case 8:
                    if (jieshi(myHelper::StrHexToDecimal(s)).indexOf(".")>0)
                    {

                        //ui->led_lingwei->turnOn();
                        lingweiComming(true);
                        zhongliang+=jieshi(myHelper::StrHexToDecimal(s)).left(1);
                    }
                    else
                    {
                        //ui->led_lingwei->turnOff();
                        lingweiComming(false);
                        zhongliang+=jieshi(myHelper::StrHexToDecimal(s));
                    }

                    //ui->label_10->setText(zhongliang);
                    emit weighComming(zhongliang);
                    break;

                default:
                    break;
                }
            }
        }
    }

}
